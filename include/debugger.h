// Author: Abhilash Verma <vermaabhilash70@gmail.com>
// Created at Sep 1, 2019 at 09:00 AM
#pragma once

#include <iostream>

namespace debugger {

    enum Codes {ERROR = 0, SUCCESS = 1, WARNING = 2, INFO = 3, DEFAULT = 4};
    std::string Colors[] = {"\x1b[31m", "\x1b[32m", "\x1b[33m", "\x1b[34m", "\x1b[39m"};

    void print(std::string literal) {
        std::cout << literal << std::endl;
        #ifndef DEBUG
        std::cout << Colors[WARNING] << "\nWARNING: " << "Illegal debugger call!\n" << Colors[DEFAULT] << "  +> Check for definition of DEBUG header before calling debugger functions" << std::endl;
        #endif
    }

    void error(std::string literal) {
        print(Colors[ERROR] + "ERROR: " + literal + Colors[DEFAULT]);
    }
    void success(std::string literal) {
        print(Colors[SUCCESS] + "SUCCESS: " + literal + Colors[DEFAULT]);
    }
    void warn(std::string literal) {
        print(Colors[WARNING] + "WARNING: " + literal + Colors[DEFAULT]);   
    }
    void info(std::string literal) {
        print(Colors[INFO] + "INFO: " + literal + Colors[DEFAULT]); 
    }
}