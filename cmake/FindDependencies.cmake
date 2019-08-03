include(FetchContent)

# QT
find_package(Qt5 5.7 QUIET REQUIRED COMPONENTS Core Widgets Gui)

if((APPLE OR WIN32) AND NOT DEPLOYQT_EXECUTABLE)
    get_target_property(_qmake_bin Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_bin}" DIRECTORY)
    find_program(DEPLOYQT_EXECUTABLE NAMES windeployqt macdeployqt HINTS "${_qt_bin_dir}")
    if(DEPLOYQT_EXECUTABLE)
        message(STATUS "Found Qt deploy tool: ${DEPLOYQT_EXECUTABLE}")
    else()
        message(SEND_ERROR "Qt deploy tool not found. Modify your CMAKE_PREFIX_PATH "
                "or manually set DEPLOYQT_EXECUTABLE to the location of the executable.")
    endif()
endif()

# JSON
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
