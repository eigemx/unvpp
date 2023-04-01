#include <filesystem>
#include <gtest/gtest.h>
#include <unvpp/unvpp.h>

TEST(ReaderGroupsTest, GroupsNames) {
    auto path = std::filesystem::path("../../tests/meshes/eight_hex_cube_with_groups.unv");
    auto mesh = unvpp::read(path);

    EXPECT_EQ(mesh.vertices.size(), 27);
    EXPECT_EQ(mesh.elements.has_value(), true);
    EXPECT_EQ(mesh.elements.value().size(), 56);

    // calculate number of hexahedral elements
    const auto& elements = mesh.elements.value();
    auto num_hex_elements =
        std::count_if(elements.begin(), elements.end(), [](const auto& element) {
            return element.type() == unvpp::ElementType::Hex;
        });

    EXPECT_EQ(num_hex_elements, 8);

    // check group names
    EXPECT_EQ(mesh.groups.value().size(), 2);

    const auto& groups = mesh.groups.value();
    EXPECT_EQ(groups[0].name(), "walls");
    EXPECT_EQ(groups[0].elements_ids().size(), 16);
    EXPECT_EQ(groups[0].unique_element_types().size(), 1);

    EXPECT_EQ(groups[1].name(), "inout");
    EXPECT_EQ(groups[1].elements_ids().size(), 8);
    EXPECT_EQ(groups[1].unique_element_types().size(), 1);
}