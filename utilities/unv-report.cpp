#include <chrono>
#include <iomanip>
#include <iostream>


#include <unvpp/unvpp.h>

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

    std::cout << "Units system: "
              << mesh.unitsSystem.value_or(unv::UnitsSystem()).repr
              << std::endl;
    std::cout << "Vertices count = " << mesh.vertices.size() << std::endl;
    std::cout << "Elements count = "
              << mesh.elements.value_or(std::vector<unv::Element>()).size()
              << std::endl;

    for (auto &group : mesh.groups.value_or(std::vector<unv::Group>())) {
        std::cout << "Group name: " << group.name
                  << " - elements count = " << group.eIndices.size()
                  << std::endl;
    }

    std::cout << std::setprecision(20)
              << "Time of execution: " << duration.count() << " milliseconds"
              << std::endl;
}