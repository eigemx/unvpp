include_directories(${PROJECT_SOURCE_DIR}/include/)

add_executable(unv-report
    unv-report.cpp
)

target_link_libraries(unv-report Unvpp::unvpp)