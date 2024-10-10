#include <gtest/gtest.h>
#include <unvpp/unvpp.h>

#include <filesystem>

TEST(ReaderOneCellTest, BasicAssertions) {
    auto path = std::filesystem::path("../../tests/meshes/one_hex_cell.unv");
    auto mesh = unvpp::read(path);

    EXPECT_EQ(mesh.unit_system().has_value(), true);
    EXPECT_EQ(mesh.unit_system().value().code(), 1);

    EXPECT_EQ(mesh.vertices().size(), 8);
    EXPECT_EQ(mesh.elements().has_value(), true);

    // calculate number of hexahedral elements
    const auto& elements = mesh.elements().value();
    auto num_hex_elements =
        std::count_if(elements.begin(), elements.end(), [](const auto& element) {
            return element.type() == unvpp::ElementType::Hex;
        });

    EXPECT_EQ(num_hex_elements, 1);
}
