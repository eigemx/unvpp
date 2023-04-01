#include <filesystem>
#include <gtest/gtest.h>
#include <map>
#include <unvpp/unvpp.h>


auto count_element_types(const unvpp::Mesh& mesh) -> std::map<unvpp::ElementType, size_t> {
    std::map<unvpp::ElementType, size_t> element_counts;
    for (const auto& element : mesh.elements.value()) {
        if (element_counts.find(element.type()) == element_counts.end()) {
            element_counts[element.type()] = 1;
        } else {
            element_counts[element.type()]++;
        }
    }
    return element_counts;
}

TEST(ReaderElementsTest, ElementsCount) {
    auto path = std::filesystem::path("tests/meshes/cylinderWithGroupsCoarse.unv");
    auto mesh = unvpp::read(path);

    EXPECT_EQ(mesh.vertices.size(), 5207);
    EXPECT_EQ(mesh.elements.has_value(), true);
    EXPECT_EQ(mesh.elements.value().size(), 21984);

    // calculate number of different element types
    const auto& elements = mesh.elements.value();
    auto element_counts = count_element_types(mesh);

    EXPECT_EQ(element_counts[unvpp::ElementType::Line], 141);
    EXPECT_EQ(element_counts[unvpp::ElementType::Triangle], 2786);
    EXPECT_EQ(element_counts[unvpp::ElementType::Quad], 315);
    EXPECT_EQ(element_counts[unvpp::ElementType::Wedge], 3525);
    EXPECT_EQ(element_counts[unvpp::ElementType::Tetra], 15217);
    EXPECT_EQ(element_counts[unvpp::ElementType::Hex], 0);
}