include(FetchContent)

# Core
find_package(Qt5 5.7 QUIET REQUIRED COMPONENTS Core Widgets Gui)
find_package(nlohmann_json 3.2.0 REQUIRED)

# sffmpeg
FetchContent_Declare(
    sffmpeg
    GIT_REPOSITORY https://github.com/csparker247/sffmpeg.git
)
if(NOT sffmpeg_POPULATED)
    FetchContent_Populate(sffmpeg)
    add_subdirectory(${sffmpeg_SOURCE_DIR} ${sffmpeg_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
