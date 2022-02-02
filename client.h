//
// Created by arimionim on 08.12.2021.
//

#ifndef OCC_WAITFREE_CLIENT_H
#define OCC_WAITFREE_CLIENT_H


#include <chrono>
#include <random>
#include "wf_scheduler.h"

class client {
public:

    explicit client(wf_scheduler *scheduler) : scheduler(scheduler) {}

    long sendRandomTransactions(int n) {
        auto start = std::chrono::steady_clock::now();

        for (int i = 0; i < n; i++) {
            scheduler->sendTransaction(genRandTransaction());
        }

        auto end = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

private:

    static transaction genRandTransaction() {
        transaction tx;

        int n = rand() % 5 + 5;
        for (int i = 0; i < n; i++) {
            tx.ops.push_back({(rand() % 2 == 0 ? transaction::operation::READ : transaction::operation::WRITE), static_cast<size_t>(rand() % 10)});
        }

        return tx;
    }


    wf_scheduler *scheduler;
};


#endif //OCC_WAITFREE_CLIENT_H
