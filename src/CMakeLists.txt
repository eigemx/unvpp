add_library(unvpp
    reader.cpp
    stream.cpp
    unvpp.cpp
)

set_target_properties(unvpp PROPERTIES VERSION ${PROJECT_VERSION})
add_library(${PROJECT_NAME}::unvpp ALIAS unvpp)
