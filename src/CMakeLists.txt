add_library(unvpp SHARED
    src/reader.cpp
    src/stream.cpp
    src/unvpp.cpp
)

set_target_properties(unvpp PROPERTIES VERSION ${PROJECT_VERSION})
add_library(${PROJECT_NAME}::unvpp ALIAS unvpp)
