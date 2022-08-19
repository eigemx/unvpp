#include <chrono>
#include <iomanip>
#include <iostream>

#include <unvpp/unvpp.h>
#include "common.h"
#include "reader.h"
#include "stream.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Too few args" << std::endl;
        return -1;
    }

    std::ifstream stream(argv[1]);
    // measure time of execution
    auto start = std::chrono::high_resolution_clock::now();
    auto mesh = unv::read(stream);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Vertices count = " << mesh.vertices.size() << std::endl;


    std::cout << std::setprecision(20)
              << "Time of execution: " << duration.count() << " milliseconds"
              << std::endl;
}
