add_executable(ffdropenc src/ffdropenc.cpp)
target_link_libraries(ffdropenc
    Boost::filesystem
    ffde::core
)