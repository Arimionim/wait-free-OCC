//
// Created by arimionim on 08.12.2021.
//

#ifndef OCC_WAITFREE_WF_SCHEDULER_H
#define OCC_WAITFREE_WF_SCHEDULER_H


#include <thread>
#include <queue>
#include <mutex>
#include <utility>
#include <zconf.h>
#include "transaction.h"
#include "server.h"
#include <unordered_map>
#include <iostream>

class wf_scheduler {
public:
    explicit wf_scheduler(size_t workers_number, server *server) : workers(workers_number),
                                                                   ser(server) {
        for (size_t i = 0; i < workers_number; i++) {
            workers[i] = std::thread(proceed_tx, this, i);
        }
    }

    void sendTransaction(const transaction &tx) {
        add_tx(tx);
    }

    ~wf_scheduler() {
        for (auto &worker : workers) {
            worker.join();
        }
    }

private:

    bool get_next_tx(transaction &res) {
        q_lock.lock();
        if (!txs.empty()) {
            res = txs.front();
            txs.pop();
            q_lock.unlock();
            return true;
        } else {
            q_lock.unlock();
            return false;
        }
    }

    void add_tx(const transaction &tx) {
        q_lock.lock();
        txs.push(tx);
        q_lock.unlock();
    }

    static void proceed_tx(wf_scheduler *scheduler, int idx) {
        while (1) {
            transaction tx;
            while (!scheduler->get_next_tx(tx)) {
                usleep(1000);
            }

            bool read_only = true;

            for (auto &op: tx.ops) {
                if (op.op == transaction::operation::WRITE) {
                    read_only = false;
                    break;
                }
            }

            if (read_only) {
                scheduler->send("READONLY", tx);
            } else {
                scheduler->send("READWRITE", tx);
            }
        }
    }

    std::vector<int> send(std::string const & cmd, transaction const & tx) {
        return ser->receive(cmd, tx);
    }

    std::mutex q_lock;

    server *ser;

    std::queue<transaction> txs;
    std::vector<std::thread> workers;
};


#endif //OCC_WAITFREE_WF_SCHEDULER_H
