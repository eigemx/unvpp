add_library(unvpp
    reader.cpp
    stream.cpp
    unvpp.cpp
)

target_include_directories(unvpp PUBLIC ${PROJECT_SOURCE_DIR}/include/)

set_target_properties(unvpp PROPERTIES VERSION ${PROJECT_VERSION})
add_library(${PROJECT_NAME}::unvpp ALIAS unvpp)
