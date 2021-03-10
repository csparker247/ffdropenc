# QT
find_package(Qt5 5.13 QUIET COMPONENTS Core Widgets Gui)
if(NOT Qt5_FOUND)
    find_package(Qt6 6.0 QUIET REQUIRED COMPONENTS Core Widgets Gui)
endif()

if(Qt5_FOUND AND (APPLE OR WIN32))
    set(QT_CORE Qt5::Core)
    set(QT_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)
    find_package(DeployQt5 REQUIRED)
    message(STATUS "Using Qt5")
else()
    set(QT_CORE Qt6::Core)
    set(QT_LIBS Qt6::Core Qt6::Gui Qt6::Widgets)
    find_package(DeployQt6 REQUIRED)
    message(STATUS "Using Qt6")
endif()

# JSON
option(FFDE_BUILD_JSON "Build in-source JSON library" ON)
if(FFDE_BUILD_JSON)
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/archive/v3.9.1.tar.gz
        CMAKE_CACHE_ARGS
            -DJSON_BuildTests:BOOL=OFF
            -DJSON_Install:BOOL=OFF
    )

    FetchContent_GetProperties(json)
    if(NOT json_POPULATED)
        set(JSON_BuildTests OFF CACHE INTERNAL "")
        set(JSON_Install OFF CACHE INTERNAL "")
        FetchContent_Populate(json)
        add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
else()
    find_package(nlohmann_json 3.9.1 REQUIRED)
endif()

FetchContent_Declare(
    ffmpeg
    UPDATE_DISCONNECTED true
    URL ${CMAKE_SOURCE_DIR}/3rdparty/ffmpeg_macos.zip
    URL_HASH SHA256=8f3e9fca63a76d9f69bc60daf5aa8b524178de0e425452a2ecd872a9fd2eb9f6
    SOURCE_DIR ${EXECUTABLE_OUTPUT_PATH}
)
FetchContent_MakeAvailable(ffmpeg)
