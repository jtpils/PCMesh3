// Author: Abhilash Verma <vermaabhilash70@gmail.com>
// Created at Sep 1, 2019 at 07:19 AM
#pragma once

#include <vector>
#include <map>
#include <morton.h>
#include <fstream>

#ifdef DEBUG
#include <debugger.h>
#endif


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
// Stored as std::map
//      Z-order aka Morton encoding of cell -> list of points in cell

class Octree {

    uint8_t resolution;
    uint32_t width;

    class Octant {

    public:
        Octant() {};
        Octant(Vector min, Vector max) : min(min), max(max) {
            #ifdef DEBUG
            debugger::print("Initialized octant with corners ", min, " and ", max);
            #endif
        };
        Vector min;     // xmin, ymin, zmin
        Vector max;     // xmax, ymax, zmax
        // bool intersects(Segment seg);
        // bool intersects(Ray ray);
    };

    class Cell {

        std::vector<Point> points;

    public:
        Cell(Point point) {
            add(point);
        }

        void add(Point point) {
            points.push_back(point);
        };

        std::vector<Point> getPoints() {
            return points;
        }
    };

    typedef std::map<uint64_t, Cell> map;

    Octant boundary;
    map cells;

public:
    Octree(uint8_t resolution, Vector min, Vector max) : resolution(resolution) {
        Vector diagonal = max - min;
        double longest = std::max(std::max(diagonal.x(), diagonal.y()), diagonal.z());
        width = longest * resolution;
        boundary = Octant(min, min + Vector(longest, longest, longest));
    };

    ~Octree() {};
    void add(std::istream_iterator<Point> begin, std::istream_iterator<Point> end);
    void add(Point point);
    Point getCellOrigin(Point point);
};

void Octree::add(std::istream_iterator<Point> begin, std::istream_iterator<Point> end) {
    for(auto it = begin; it != end; it++) {
        add(*it);
    }
    #ifdef DEBUG
    debugger::success(cells.size(), " points added to tree");
    #endif
}

void Octree::add(Point point) {
    Point origin = getCellOrigin(point);
    uint64_t encoding;
    try {
        encoding = morton::encode(origin.x(), origin.y(), origin.z());
    } catch(const std::invalid_argument&) {
        debugger::error("Faulted at ", point);
        debugger::error("Make sure the point lies within the prescribed boundary");
        throw;
    }

    map::iterator lb = cells.lower_bound(encoding);

    if(lb != cells.end() && encoding == lb->first) {
        (lb->second).add(point);

        #ifdef DEBUG
        if(debugger::LOGLEVEL > 1) {
            debugger::print("The following points: ", lb->second.getPoints(), "were added to the same cell");
        }
        #endif

    } else {
        //TODO: Recheck for edge cases
        cells.insert(lb, map::value_type(encoding, Cell(point)));
    }
}

Point Octree::getCellOrigin(Point point) {

    //TODO: Exception Handling for noise samples
    //      lying outside boundary

    Vector p(point.x(), point.y(), point.z());
    Vector pos = p - boundary.min;
    uint32_t x = pos.x() * resolution;
    uint32_t y = pos.y() * resolution;
    uint32_t z = pos.z() * resolution;
    //min           pos                                   max
    // |-------------|-------------------------------------|
    // 0           mapped                             max * resolution
    return Point(x, y, z);
}