add_executable(ffdropenc src/ffdropenc.cpp)
target_link_libraries(ffdropenc
    c++fs
    ffde::core
)