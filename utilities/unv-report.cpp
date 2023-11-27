/*
MIT License

Copyright (c) 2022 Mohamed Emara <mae.emara@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <unvpp/unvpp.h>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

auto main(int argc, char* argv[]) -> int {
    // convert argv to vector of strings
    std::vector<std::string> args(argv, argv + argc);

    if (args.size() < 2) {
        std::cerr << "Too few args!" << std::endl;
        std::cerr << "Usage: " << args[0] << " [input]" << std::endl;
        return -1;
    }

    // measure time of execution
    auto start = std::chrono::high_resolution_clock::now();
    auto mesh = unvpp::read(args[1]);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Units system: " << mesh.unit_system().value_or(unvpp::UnitsSystem()).repr
              << std::endl;
    std::cout << "Vertices count = " << mesh.vertices().size() << std::endl;
    std::cout << "Elements count = "
              << mesh.elements().value_or(std::vector<unvpp::Element>()).size() << "\n\n";

    // count elements of each type
    std::vector<std::size_t> elements_count(6, 0);
    for (auto& element : mesh.elements().value_or(std::vector<unvpp::Element>())) {
        elements_count[static_cast<std::size_t>(element.type())]++;
    }

    if (mesh.elements().has_value()) {
        std::cout << "Elements types count:" << std::endl;
        std::cout << "- Lines: " << elements_count[0] << std::endl;
        std::cout << "- Triangles: " << elements_count[1] << std::endl;
        std::cout << "- Quadrangles: " << elements_count[2] << std::endl;
        std::cout << "- Tetrahedrons: " << elements_count[3] << std::endl;
        std::cout << "- Wedges: " << elements_count[4] << std::endl;
        std::cout << "- Hexahedrons: " << elements_count[5] << '\n' << std::endl;
    }

    for (auto& group : mesh.groups().value_or(std::vector<unvpp::Group>())) {
        std::cout << "Group name: " << group.name() << '\n'
                  << " - elements count = " << group.elements_ids().size() << '\n'
                  << " - unique elements types count in group = "
                  << group.unique_element_types().size() << std::endl;

        // print the string representation of each element type in the group
        std::map<unvpp::ElementType, std::string> element_type_to_string;
        element_type_to_string.insert({unvpp::ElementType::Line, "Line"});
        element_type_to_string.insert({unvpp::ElementType::Triangle, "Triangle"});
        element_type_to_string.insert({unvpp::ElementType::Quad, "Quadrangle"});
        element_type_to_string.insert({unvpp::ElementType::Tetra, "Tetrahedron"});
        element_type_to_string.insert({unvpp::ElementType::Wedge, "Wedge"});
        element_type_to_string.insert({unvpp::ElementType::Hex, "Hexahedron"});

        // print the different element types in the group
        for (const auto& element_type : group.unique_element_types()) {
            std::cout << "   * " << element_type_to_string[element_type] << std::endl;
        }
    }

    std::cout << std::setprecision(20) << "Time of execution: " << duration.count()
              << " milliseconds" << std::endl;
}
