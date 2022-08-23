#pragma once
#include "common.h"
#include "stream.h"
#include <unordered_map>
#include <utility>

namespace unv {
class Reader {
  public:
    Reader() = delete;
    Reader(FileStream &stream) : _stream(stream){};
    ~Reader() { _stream.~FileStream(); }

    void readTags();
    void readUnits();
    void readVertices();
    void readElements();
    void readGroups();
    void readDOFs();

    UnitsSystem &units();
    std::vector<Vertex> &vertices();
    std::vector<Element> &elements();
    std::vector<Group> &groups();

  private:
    inline void skipTag() {
        while (_stream.readLine(_tempLine) && !isSeparator(_tempLine)) {
        }
    }

    template <typename T = std::pair<std::vector<std::size_t>, GroupType>>
    T readGroupElements(std::size_t n_elements);

    template <typename T = std::pair<std::vector<std::size_t>, GroupType>>
    T readGroupElementsTwoColumns(std::size_t n_elements);

    template <typename T = std::pair<std::vector<std::size_t>, GroupType>>
    T readGroupElementsSingleColumn();

    FileStream &_stream;
    std::string _tempLine;

    UnitsSystem _unitsSystem;
    std::vector<Vertex> _vertices;
    std::vector<Element> _elements;
    std::vector<Group> _groups;

    std::unordered_map<std::size_t, std::size_t> _vertex_id_map;
    std::unordered_map<std::size_t, std::size_t> _element_id_map;
};
} // namespace unv
