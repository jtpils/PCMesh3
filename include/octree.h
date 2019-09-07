// Author: Abhilash Verma <vermaabhilash70@gmail.com>
// Created at Sep 1, 2019 at 07:19 AM
#pragma once

#include <vector>
#include <map>
#include <fstream>

#include <morton.h>
#include <base2.h>
#include <debugger.h>

// 3D point set specific implementation of a linear octree
// Supports ray collision detection and nearest neighbour search
// Stored as std::map
//      Z-order aka Morton encoding of cell -> list of points in cell

class Octree {

    uint16_t resolution;
    uint32_t width;
    Point origin;

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

    map cells;
    Octant boundary;

public:


    Octree(uint8_t res, Point min, Point max) {
        // Initialize min and max of octree whilst making sure
        //      the number divisions are a power of 2
        Point floored((int)min.x(), (int)min.y(), (int)min.z());
        Vector diagonal = max - floored;
        uint32_t longest = base2::ceil(                     // Convert to next closest power of 2
                        (int)std::ceil(                     // Convert to next smallest integer
                        std::max({  diagonal.x(),           // Maximize cube to contain everything
                                    diagonal.y(),
                                    diagonal.z()
                                })));

        width = longest << res;
        resolution = 1 << res;
        origin = floored;
        boundary = Octant(Vector(0,0,0), Vector(width, width, width));
    };

    ~Octree() {};
    void add(std::istream_iterator<Point> begin, std::istream_iterator<Point> end);
    void add(Point point);
    Point transform(Point point);
};

void Octree::add(std::istream_iterator<Point> begin, std::istream_iterator<Point> end) {
    int i = 0;
    for(auto it = begin; it != end; it++) {
        add(*it);
        i++;
    }
    #ifdef DEBUG
    debugger::success(i, " points added to ", cells.size(), " cells");
    #endif
}

void Octree::add(Point point) {
    Point cell = transform(point);
    uint64_t encoding;
    try {
        encoding = morton::encode((uint32_t)cell.x(), (uint32_t)cell.y(), (uint32_t)cell.z());
    } catch(std::invalid_argument &e) {
        debugger::error("Faulted at ", point);
        debugger::error("Make sure the point lies within the prescribed boundary");
        throw;
    }

    map::iterator lb = cells.lower_bound(encoding);

    if(lb != cells.end() && encoding == lb->first) {
        (lb->second).add(cell);

        #ifdef DEBUG
        if(debugger::LOGLEVEL > 1) {
            debugger::print("The following points: ", lb->second.getPoints(), "were added to the same cell");
        }
        #endif

    } else {
        //TODO: Recheck for edge cases
        cells.insert(lb, map::value_type(encoding, Cell(cell)));
    }
}

Point Octree::transform(Point point) {

    //TODO: Exception Handling for noise samples
    //      lying outside boundary
    Vector position = (point - origin) * resolution;
    return Point(position.x(), position.y(), position.z());
}