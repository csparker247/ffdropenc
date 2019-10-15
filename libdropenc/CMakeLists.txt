project(dropenclib VERSION 0.1 LANGUAGES CXX)
set(module "core")
set(target "ffde_${module}")

set(srcs
    src/Filesystem.cpp
    src/Preset.cpp
    src/QueueItem.cpp
    src/TimeTools.cpp
)

add_library(${target} ${srcs})
add_library("${namespace}${module}" ALIAS "${target}")
target_include_directories(${target}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(${target}
    PUBLIC
        Qt5::Core
        nlohmann_json::nlohmann_json
)
set_target_properties(${target} PROPERTIES
    VERSION "${PROJECT_VERSION}"
    EXPORT_NAME "${module}"
)

install(
    TARGETS "${target}"
    EXPORT "${targets_export_name}"
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    INCLUDES DESTINATION "${include_install_dir}"
    RUNTIME DESTINATION "bin"
)
install(
    DIRECTORY "include/ffdropenc"
    DESTINATION "${include_install_dir}"
    FILES_MATCHING REGEX ".*\.(h|hpp)$"
)
