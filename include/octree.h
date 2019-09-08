// Author: Abhilash Verma <vermaabhilash70@gmail.com>
// Created at Sep 1, 2019 at 07:19 AM
#pragma once

#include <vector>
#include <map>
#include <stack>
#include <fstream>

#include <morton.h>
#include <base2.h>
#include <geometry.h>
#include <debugger.h>

// 3D point set specific implementation of a linear octree
// Supports ray collision detection and nearest neighbour search
// Stored as std::map
//      Z-order aka Morton encoding of cell -> list of points in cell

class Octree {

    uint16_t resolution;
    uint32_t width;
    float threshold = 50;
    Point origin;

    class Octant {

    public:
        Octant() {};
        Octant(Vector min, Vector max) : min(min), max(max) {
            #ifdef DEBUG
            debugger::print("Initialized octant with corners ", min, " and ", max);
            #endif
        };
        Octant child(int i) {
            // Diagonal of new octant is half of the original octant;
            Vector diagonal = (max - min) / 2;
            #ifdef DEBUG
            if(debugger::LOGLEVEL > 1)
                debugger::print("cube diagonal: ", diagonal);
            #endif

            uint_fast32_t xx, yy, zz;
            // Get child origin as cell no
            morton::decode(i, xx, yy, zz);
            Vector origin(xx * diagonal.x(), yy * diagonal.y(), zz * diagonal.z());
            return Octant(min + origin, min + origin + diagonal);
        };
        Vector min;     // xmin, ymin, zmin
        Vector max;     // xmax, ymax, zmax

        uint32_t size() {
            return max.x() - min.x();
        };
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

    class iterator {
    // During depth-first iteration
    //  Path tracks parent and branch # followed
    std::stack<std::pair<Octant, int> > path;
    Octant current;
    Octree &tree;
    int depth = 0;
    int sibling = 0;

    public:

        iterator(Octree &tree) : tree(tree) {
            current = tree.boundary;
        }
        bool isLeaf() {
            return current.size() <= 1.0;
        }
        // Finds first non-empty child
        bool toChild() {
            if(isLeaf())
                return false;
            for(int i = 0; i < 8; i++) {
                Octant oct = current.child(i);
                if(!tree.empty(oct)) {
                    path.push(std::make_pair(current, i));
                    current = oct;
                    sibling = i;
                    depth++;
                    return true;
                }
            }
            return false;
        }
        bool toParent() {
            if(path.empty()) {
                return false;
            }
            current = path.top().first;
            path.pop();
            depth--;
            if(depth != 0)
                sibling = path.top().second;
            else sibling = 0;
            return true;
        }
        bool toSibling() {
            if(path.empty() || sibling >= 7)
                return false;
            for(int i = sibling + 1; i < 8; i++) {
                Octant parent = path.top().first;
                Octant oct = parent.child(i);
                if(!tree.empty(oct)) {
                    current = oct;
                    path.top().second = i;
                    sibling = i;
                    return true;
                }
            }
            return false;
        }
        Octant getOctant() {
            return current;
        }
        Cell getCell() {
            uint64_t encoding = morton::encode(current.min.x(), current.min.y(), current.min.z());
            map::iterator it = tree.cells.find(encoding);
            if(it != tree.cells.end())
                return it->second;
            else
                throw std::logic_error("Cell is not populated in tree");
        }
        bool next(bool skip) {
            // skip => Ignore exploration of branch
            if(!skip && toChild())
                return true;
            if(toSibling())
                return true;
            while(toParent() && !toSibling());
            if(depth == 0)
                return false;
            return true;
        }
    };

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
    bool empty();
    bool empty(Octant octant);
    bool neighborhood(Line line, Octant octant) {
        Vector extension(threshold, threshold, threshold);
        return geometry::intersects(line, octant.min - extension, octant.max + extension);
    }
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
    Vector position = (point - origin) * resolution;
    return Point(position.x(), position.y(), position.z());
}

bool Octree::empty() {
    return empty(boundary);
}

bool Octree::empty(Octant octant) {
    uint64_t first = morton::encode(octant.min.x(), octant.min.y(), octant.min.z());
    uint64_t last = morton::encode(octant.max.x() - 1, octant.max.y() - 1, octant.max.z() - 1);
    map::iterator it = cells.lower_bound(first);
    if(it == cells.end() || it->first > last)
        return true;
    return false;
}