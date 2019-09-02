#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>

#include <context.h>
#include <octree.h>

#ifdef DEBUG
#include <debugger.h>
short debugger::LOGLEVEL = 0;
#endif

int main(int argc, char* argv[]) {
    #ifdef DEBUG
    int c = 0;
    while((c = getopt(argc, argv, "v")) != -1)
        if (c == 'v') debugger::LOGLEVEL++;

	std::clock_t start;
    double duration;
    #endif

    std::ifstream file("data/teeth.xyz");
    std::istream_iterator<Point> begin(file);
    std::istream_iterator<Point> end;

    #ifdef DEBUG
    start = std::clock();
    #endif

    Octree tree(100, Vector(-16,-16,-16), Vector(17,16,16));
    tree.add(begin, end);

    #ifdef DEBUG
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    std::cout << "Execution took " << duration << " seconds" << std::endl;
    #endif

	return 0;
}