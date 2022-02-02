cmake_minimum_required(VERSION 3.14)
project(occ_waitfree)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(occ_waitfree main.cpp wf_scheduler.h server.h client.h transaction.h)