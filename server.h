//
// Created by arimionim on 08.12.2021.
//

#ifndef OCC_WAITFREE_SERVER_H
#define OCC_WAITFREE_SERVER_H


#include <unordered_map>
#include <cstddef>
#include <atomic>
#include <zconf.h>
#include "transaction.h"

class server {
public:
    std::vector<int> receive(const std::string &cmd, const transaction &tx) {
        std::vector<int> res;
        if (cmd == "READONLY") {
            if (w) {
                ++newR;
                for (auto &op : tx.ops) {
                    if (newdata.find(op.idx) != newdata.end()) {
                        res.push_back(newdata[op.idx]);
                    } else {
                        res.push_back(data[op.idx]);
                    }
                }
                --newR;
            } else {
                ++oldR;
                for (auto &op : tx.ops) {
                    res.push_back(data[op.idx]);
                }
                --oldR;
            }
        } else {
            bool expect = false;
            if (write.compare_exchange_strong(expect, true)) {
                for (auto &op : tx.ops) {
                    if (op.op == transaction::operation::READ) {
                        if (newdata.find(op.idx) != newdata.end()) {
                            res.push_back(newdata[op.idx]);
                        } else {
                            res.push_back(data[op.idx]);
                        }
                    } else {
                        newdata[op.idx] = rand();
                    }
                }

                w = true;
                while (oldR > 0) { usleep(100); }

                for (auto & val : newdata) {
                    data[val.first] = val.second;
                }

                w = false;

                while (newR > 0) { usleep(100); }

                newdata.clear();
                write = false;
            }
        }

        return res;
    }

private:
    std::unordered_map<size_t, int> data;
    std::atomic<bool> w = false;
    std::atomic<int> newR = 0;
    std::atomic<int> oldR = 0;
    std::unordered_map<size_t, int> newdata;
    std::atomic<bool> write = false;
};


#endif //OCC_WAITFREE_SERVER_H
