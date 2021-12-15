cmake_minimum_required(VERSION 3.14)
project(occ_waitfree)

set(CMAKE_CXX_STANDARD 17)

add_executable(occ_waitfree main.cpp occ_scheduler.cpp occ_scheduler.h server.cpp server.h client.cpp client.h transaction.cpp transaction.h)