# QT
find_package(Qt5 5.7 QUIET REQUIRED COMPONENTS Core Widgets Gui)
if(APPLE OR WIN32)
    find_package(DeployQt5 REQUIRED)
endif()

# JSON
option(FFDE_BUILD_JSON "Build in-source JSON library" ON)
if(FFDE_BUILD_JSON)
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/archive/v3.9.1.tar.gz
        CMAKE_CACHE_ARGS
            -DJSON_BuildTests:BOOL=OFF
    )

    FetchContent_GetProperties(json)
    if(NOT json_POPULATED)
        set(JSON_BuildTests OFF CACHE INTERNAL "")
        FetchContent_Populate(json)
        add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
else()
    find_package(nlohmann_json 3.9.1 REQUIRED)
endif()