#### App ####
set(target ffdropenc)

configure_file(include/ffdropenc.hpp.in ffdropenc.hpp)

set(hdrs
    include/MainLayout.hpp
)

set(srcs
    src/ffdropenc.cpp
    src/MainApplication.cpp
    src/MainLayout.cpp
)

set(rccs Resources.qrc)

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
set(copyright "Copyright ${year} Seth Parker")
string(CONCAT info_string
    ${target}${CMAKE_BUILD_TYPE}
    " Version ${PROJECT_VERSION}"
    ", "
    ${copyright}
)
set(MACOSX_BUNDLE_INFO_STRING ${info_string})
set(MACOSX_BUNDLE_GUI_IDENTIFIER "${target}")
set(MACOSX_BUNDLE_LONG_VERSION_STRING
        "${target}-${CMAKE_BUILD_TYPE} Version ${PROJECT_VERSION}")
set(MACOSX_BUNDLE_BUNDLE_NAME ${target})
set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
set(MACOSX_BUNDLE_COPYRIGHT ${copyright})

add_executable(${target}
    MACOSX_BUNDLE
    WIN32
    ${hdrs}
    ${srcs}
    ${rccs}
    ${icon_path}
)
target_include_directories(${target}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${CMAKE_CURRENT_BINARY_DIR}
)
set_target_properties(${target}
    PROPERTIES
        AUTOMOC on
        AUTOUIC on
        AUTORCC on
)
target_link_libraries(${target}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

install(TARGETS ${target}
    BUNDLE DESTINATION . COMPONENT Programs
    RUNTIME DESTINATION bin COMPONENT Programs
)

### App Packaging ###
set(plugins "")
set(extra_libs "")
set(extra_dirs "")
set(request_qt_conf off)
set(plugins_dir "")
set(components Programs)
if (APPLE)
    set(vc_exe "${target}.app")
    list(APPEND plugins qcocoa)
elseif (WIN32)
    set(vc_exe "${target}.exe")
else ()
    set(vc_exe "${target}")
endif()
if(APPLE OR WIN32)
    include(DeployQt5)
    install_qt5_executable(
        "${vc_exe}"
        "${plugins}"
        "${extra_libs}"
        "${extra_dirs}"
        "${plugins_dir}"
        "${request_qt_conf}"
        "${components}"
    )
endif()