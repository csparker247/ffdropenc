#### App ####
configure_file(include/ffdropenc.hpp.in ffdropenc.hpp)

set(hdrs
    include/MainApplication.hpp
    include/MainLayout.hpp
    include/SettingsDialog.hpp
    include/EncodingQueue.hpp
    include/CollapsibleGroupBox.hpp
    include/ClickableLabel.hpp
    include/SettingsWindow.hpp
)

set(srcs
    src/ffdropenc.cpp
    src/MainApplication.cpp
    src/MainLayout.cpp
    src/SettingsDialog.cpp
    src/EncodingQueue.cpp
    src/CollapsibleGroupBox.cpp
    src/ClickableLabel.cpp
    src/SettingsWindow.cpp
)

set(rccs Resources.qrc Presets.qrc)

# Setup Icon properties
set(icon_file appicon.icns)
set(icon_path ${CMAKE_CURRENT_SOURCE_DIR}/images/${icon_file})
set_source_files_properties(
    ${icon_path}
    PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
)

# Setup Info.plist
set(MACOSX_BUNDLE_ICON_FILE ${icon_file})
string(TIMESTAMP year "%Y")
set(copyright "Copyright ${year} C. Seth Parker")
string(CONCAT info_string
    ffdropenc${CMAKE_BUILD_TYPE}
    " Version ${PROJECT_VERSION}"
    ", "
    ${copyright}
)
set(MACOSX_BUNDLE_INFO_STRING ${info_string})
set(MACOSX_BUNDLE_GUI_IDENTIFIER "ffdropenc")
set(MACOSX_BUNDLE_LONG_VERSION_STRING
        "ffdropenc-${CMAKE_BUILD_TYPE} Version ${PROJECT_VERSION}")
set(MACOSX_BUNDLE_BUNDLE_NAME ffdropenc)
set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
set(MACOSX_BUNDLE_COPYRIGHT ${copyright})

add_executable(ffdropenc
    MACOSX_BUNDLE
    WIN32
    ${hdrs}
    ${srcs}
    ${rccs}
    ${icon_path}
)
target_include_directories(ffdropenc
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${CMAKE_CURRENT_BINARY_DIR}
)
set_target_properties(ffdropenc
    PROPERTIES
        AUTOMOC on
        AUTOUIC on
        AUTORCC on
)
target_link_libraries(ffdropenc
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    ffde::core
)

install(TARGETS ffdropenc
    BUNDLE DESTINATION . COMPONENT Programs
    RUNTIME DESTINATION bin COMPONENT Programs
)

# ffmpeg binary (macOS)
add_custom_command(TARGET ffdropenc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${EXECUTABLE_OUTPUT_PATH}/ffmpeg $<TARGET_FILE_DIR:ffdropenc>
)
# binary info (macOS)
add_custom_command(TARGET ffdropenc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/3rdparty/ffmpeg_macos.json $<TARGET_FILE_DIR:ffdropenc>/ffmpeg_details.json
)

### App Packaging ###
if(APPLE OR WIN32)
    qt5_deploy_bundle(ffdropenc)
endif()
