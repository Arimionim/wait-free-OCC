//
// Created by arimionim on 08.12.2021.
//

#ifndef OCC_WAITFREE_SERVER_H
#define OCC_WAITFREE_SERVER_H


#include <unordered_map>
#include <cstddef>
#include <mutex>

class server {
public:
    void write_data(size_t idx, int val) {
        data[idx] = val;
    }

    int read_data(size_t idx) {
        return data[idx];
    }

private:
    std::unordered_map<size_t, int> data;
};


#endif //OCC_WAITFREE_SERVER_H
