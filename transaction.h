//
// Created by arimionim on 08.12.2021.
//

#ifndef OCC_WAITFREE_TRANSACTION_H
#define OCC_WAITFREE_TRANSACTION_H


#include <utility>
#include <vector>

class transaction {
public:
    transaction() = default;

    class operation {
    public:
        enum type {
            READ, WRITE
        };
        type op;
        size_t idx;
    };

    explicit transaction(std::vector<operation> ops) : ops(std::move(ops)) {}

    std::vector<operation> ops;
};


#endif //OCC_WAITFREE_TRANSACTION_H
