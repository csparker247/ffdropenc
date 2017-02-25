project(libffde_external VERSION 0.1 LANGUAGES C CXX)
set(module "external")
set(target "ffde_${module}")

set(srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/json.hpp
)

add_library(${target} INTERFACE)
#target_sources(${target} INTERFACE ${srcs})
add_library("${namespace}${module}" ALIAS "${target}")
target_include_directories(${target} SYSTEM INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${include_install_dir}>
)
#target_link_libraries(${target}
#    PUBLIC
#
#)
set_target_properties(${target} PROPERTIES
    EXPORT_NAME "${module}"
)

install(
    TARGETS ${target}
    EXPORT ${targets_export_name}
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "${include_install_dir}/${module}"
)
install(
    DIRECTORY "include/${module}"
    DESTINATION "${include_install_dir}"
    FILES_MATCHING REGEX ".*\.(h|hpp)$"
)
