// Author: Abhilash Verma <vermaabhilash70@gmail.com>
// Created at Sep 1, 2019 at 08:40 AM
#pragma once

#if !_WIN64 && !__x86_64__ && !__ppc64__
#error This program can only run on 64bit machines
#endif

#include <stdexcept>

namespace morton {
    inline uint_fast64_t shift(uint_fast64_t x) {
        if(x > 0x1fffff) {
            throw std::invalid_argument("Values greater than 0x1fffff are not permitted");
        }
        x &= 0x00000000001fffff;                                // x = ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---4 3210 fedc ba98 7654 3210
        x = (x ^ (x << 16) ^ (x << 32)) & 0x001f0000ff0000ff;   // x = ---- ---- ---4 3210 ---- ---- ---- ---- fedc ba98 ---- ---- ---- ---- 7654 3210
        x = (x ^ (x <<  8))             & 0x100f00f00f00f00f;   // x = ---4 ---- ---- 3210 ---- ---- fedc ---- ---- ba98 ---- ---- 7654 ---- ---- 3210
        x = (x ^ (x <<  4))             & 0x10c30c30c30c30c3;   // x = ---4 ---- 32-- --10 ---- fe-- --dc ---- ba-- --98 ---- 76-- --54 ---- 32-- --10
        x = (x ^ (x <<  2))             & 0x1249249249249249;   // x = ---4 --3- -2-- 1--0 --f- -e-- d--c --b- -a-- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
        return x;
    }

    inline uint_fast64_t unshift(uint_fast64_t x) {                                 
        x &= 0x1249249249249249;                                // x = ---4 --3- -2-- 1--0 --f- -e-- d--c --b- -a-- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
        x = (x ^ (x >>  2))             & 0x10c30c30c30c30c3;   // x = ---4 ---- 32-- --10 ---- fe-- --dc ---- ba-- --98 ---- 76-- --54 ---- 32-- --10
        x = (x ^ (x >>  4))             & 0x100f00f00f00f00f;   // x = ---4 ---- ---- 3210 ---- ---- fedc ---- ---- ba98 ---- ---- 7654 ---- ---- 3210
        x = (x ^ (x >>  8))             & 0x001f0000ff0000ff;   // x = ---- ---- ---4 3210 ---- ---- ---- ---- fedc ba98 ---- ---- ---- ---- 7654 3210
        x = (x ^ (x >> 16) ^ (x >> 32)) & 0x00000000001fffff;   // x = ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---4 3210 fedc ba98 7654 3210
        return x;
    }

    inline uint_fast64_t encode(uint_fast32_t x, uint_fast32_t y, uint_fast32_t z) {
        return shift(x) | (shift(y) << 1) | (shift(z) << 2);
    }

    inline void decode(uint_fast64_t code, uint_fast32_t &x, uint_fast32_t &y, uint_fast32_t &z) {
        x = unshift(code);
        y = unshift(code >> 1);
        z = unshift(code >> 2);
    }
}