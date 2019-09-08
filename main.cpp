#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>

#include <context.h>
#include <octree.h>
#include <random>

#ifdef DEBUG
#include <debugger.h>
short debugger::LOGLEVEL = 0;
#endif

int main(int argc, char* argv[]) {
    #ifdef DEBUG
    int c = 0;
    while((c = getopt(argc, argv, "v")) != -1)
        if (c == 'v') debugger::LOGLEVEL++;

    #endif
	std::clock_t start;
    double duration;

    std::ifstream file("data/teeth.xyz");
    std::istream_iterator<Point> begin(file);
    std::istream_iterator<Point> end;

    start = std::clock();

    Octree tree(7, Point(-16,-16,-16), Point(16,15,16));
    tree.add(begin, end);

    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    debugger::print("Population took ", duration, " seconds");


    start = std::clock();

    Line line(Point(0, 0.5, 0), Point(10,11,10));
    Octree::iterator it(tree);
    std::vector<Point> neighborhood;

    bool SKIP_BRANCH = false;

    while(it.next(SKIP_BRANCH)) {
        SKIP_BRANCH = false; // Reset
        if(tree.neighborhood(line, it.getOctant())) {
            if(it.isLeaf()) {
                std::vector<Point> collection = it.getCell().getPoints();
                neighborhood.insert(neighborhood.end(), collection.begin(), collection.end());
            }
        } else SKIP_BRANCH = true;
    }
    debugger::print("Collected ", neighborhood.size(), " points");

    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    debugger::print("Execution over in ", duration, " seconds");

	return 0;
}