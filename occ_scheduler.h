//
// Created by arimionim on 08.12.2021.
//

#ifndef OCC_WAITFREE_OCC_SCHEDULER_H
#define OCC_WAITFREE_OCC_SCHEDULER_H


#include <thread>
#include <queue>
#include <mutex>
#include <utility>
#include <zconf.h>
#include "transaction.h"
#include "server.h"
#include <unordered_map>
#include <iostream>

class occ_scheduler {
public:
    explicit occ_scheduler(size_t workers_number, server *server) : workers(workers_number,
                                                                            std::thread(proceed_tx, this)),
                                                                    ser(server) {}

    void sendTransaction(const transaction &tx) {
        add_tx(tx);
    }

    ~occ_scheduler() {
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

    bool seize_storage(size_t idx) {
        busy_lock.lock();
        bool res = busy[idx] == 0;
        busy[idx]++;
        busy_lock.unlock();
        return res;
    }

    void free_storage(size_t idx) {
        busy_lock.lock();
        busy[idx]--;
        if (busy[idx] == 0) {
            busy.erase(idx);
        }
        busy_lock.unlock();
    }

    void write(transaction const & tx) {
        for (auto & op: tx.ops) {
            write_data(op.idx);
        }
    }

    static void proceed_tx(occ_scheduler *scheduler) {
        transaction tx;
        while (!scheduler->get_next_tx(tx)) {
            usleep(1000);
        }

        transaction read_tx;
        transaction write_tx;

        for (auto &op: tx.ops) {
            if (op.op == transaction::operation::READ) {
                read_tx.ops.push_back(op);
            } else {
                write_tx.ops.push_back(op);
            }
        }

        for (auto &op: read_tx.ops) {
            scheduler->seize_storage(op.idx);
        }

        for (auto &op: read_tx.ops) {
            std::cout << "read " << scheduler->read_data(op.idx) << std::endl;
        }

        for (auto &op: read_tx.ops) {
            scheduler->free_storage(op.idx);
        }

        bool all = true;
        for (auto &op: write_tx.ops) {
            all &= scheduler->seize_storage(op.idx);
        }

        if (all) {
            scheduler->write(write_tx);
            std::cout << "write" << std::endl;
        } else {
            std::cout << "discard" << std::endl;
        }


        for (auto &op: write_tx.ops) {
            scheduler->free_storage(op.idx);
        }
    }

    int read_data(size_t idx) {
        return ser->read_data(idx);
    }

    void write_data(size_t idx) {
        ser->write_data(idx, rand() % 100);
    }

    std::mutex q_lock;
    std::mutex busy_lock;

    server* ser;

    std::queue<transaction> txs;
    std::vector<std::thread> workers;
    std::unordered_map<size_t, int> busy;
};


#endif //OCC_WAITFREE_OCC_SCHEDULER_H
