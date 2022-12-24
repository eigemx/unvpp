#pragma once
#include "common.h"
#include "stream.h"
#include <sstream>
#include <unordered_map>
#include <utility>


namespace unv {

auto static readFirstScalar(const std::string& line) -> std::size_t;
auto static readScalars(const std::string& line, std::size_t n) -> std::vector<std::size_t>;

class Reader {
public:
    Reader() = delete;
    Reader(FileStream& stream) : _stream(stream) {};
    ~Reader() { _stream.~FileStream(); }

    void readTags();
    void readUnits();
    void readVertices();
    void readElements();
    void readGroups();
    void readDOFs();

    auto units() -> UnitsSystem&;
    auto vertices() -> std::vector<Vertex>&;
    auto elements() -> std::vector<Element>&;
    auto groups() -> std::vector<Group>&;

private:
    inline void skipTag() {
        while (_stream.readLine(_tempLine) && !isSeparator(_tempLine)) {
        }
    }

    template <typename T = std::pair<std::vector<std::size_t>, GroupType>>
    auto readGroupElements(std::size_t n_elements) -> T;

    template <typename T = std::pair<std::vector<std::size_t>, GroupType>>
    auto readGroupElementsTwoColumns(std::size_t n_elements) -> T;

    template <typename T = std::pair<std::vector<std::size_t>, GroupType>>
    auto readGroupElementsSingleColumn() -> T;

    FileStream& _stream;
    std::string _tempLine;

    UnitsSystem _unitsSystem;
    std::vector<Vertex> _vertices;
    std::vector<Element> _elements;
    std::vector<Group> _groups;

    std::unordered_map<std::size_t, std::size_t> _vertex_id_map;
    std::unordered_map<std::size_t, std::size_t> _element_id_map;
};
} // namespace unv
