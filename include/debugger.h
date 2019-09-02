// Author: Abhilash Verma <vermaabhilash70@gmail.com>
// Created at Sep 1, 2019 at 09:00 AM
#pragma once

#include <iostream>
#include <vector>

namespace debugger {

    template<typename T>
    void print(T t) {
        std::cout << t << std::endl;
    }

    template<typename T, typename A>
    void print(std::vector<T, A> const &t) {
        std::cout << std::endl;
        for(auto it = t.begin(); it != t.end(); it++) {
            std::cout << "\t" << *it << std::endl;
        }
    }

    template<typename T, typename A, typename ...types>
    void print(std::vector<T, A> const &t, types... args) {
        print(t);
        print(args...);
    }

    template<typename T, typename ...types>
    void print(T t, types... args) {
        std::cout << t;
        print(args...);
    }

    template<typename ...types>
    void error(types ...args) {
        print("\x1b[31m", args..., "\x1b[39m");
    }
    #ifdef DEBUG

    extern short LOGLEVEL;

    template<typename ...types>
    void success(types ...args) {
        print("\x1b[32m", args..., "\x1b[39m");
    }
    template<typename ...types>
    void warn(types ...args) {
        print("\x1b[33m", args..., "\x1b[39m");
    }
    template<typename ...types>
    void info(types ...args) {
        print("\x1b[34m", args..., "\x1b[39m");
    }
    #endif
}