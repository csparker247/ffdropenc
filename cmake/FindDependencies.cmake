include(FetchContent)

# QT
find_package(Qt5 5.7 QUIET REQUIRED COMPONENTS Core Widgets Gui)
if(APPLE OR WIN32)
    find_package(DeployQt5 REQUIRED)
endif()

# JSON
## Modern JSON ##
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.7.3
)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# sffmpeg
FetchContent_Declare(
    sffmpeg
    GIT_REPOSITORY https://github.com/csparker247/sffmpeg.git
    GIT_TAG update-deps
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(sffmpeg)
