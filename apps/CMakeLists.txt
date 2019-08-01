add_executable(ffdropenc src/ffdropenc.cpp)
target_link_libraries(ffdropenc
    ffde::core
)