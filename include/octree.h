// Author: Abhilash Verma <vermaabhilash70@gmail.com>
// Created at Sep 1, 2019 at 07:19 AM
#pragma once

#include <vector>
#include <map>

#include "context.h"

#ifdef DEBUG
#include "debugger.h"

inline bool isPow2(unsigned i) {
    return ((i - 1) & i) == 0;
}

inline unsigned log2(unsigned n) {
    assert(n != 0);
    assert(isPow2(n));

    unsigned log = 0;
    while(true)
    {
        n >>= 1;
        if (n == 0) {
            break;
        }
        log++;
    }
    return log;
}

// 3D point set specific implementation of a linear octree
// Supports ray collision detection and nearest neighbour search
// Stored as std::map:
//      Z-order encoding of cell -> list of points in cell

class Octree {

    uint16_t resolution;
    uint32_t width;

    class SuperCell {
        Vector min;     // xmin, ymin, zmin
        Vector max;     // xmax, ymax, zmax

    public:
        SuperCell(Vector min, Vector max) : min(min), max(max) {}
        bool intersects(Segment seg);
        bool intersects(Ray ray);
    }

    class Cell : private SuperCell {
        
        vector<Point> points;
        Cell(Vector min, Vector max, Point point) : SuperCell(min, max);

    public:
        static Cell getCell(Point point);
    }

    SuperCell boundary;
    map<uint64_t, Cell> cells;

public:
    Octree(uint32_t width, Vector min, Vector max) : width(width), boundary(min, max);
    ~Octree();
    void add(Point point);
}