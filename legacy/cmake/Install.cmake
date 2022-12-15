# Install resources (e.g. README, LICENSE, etc.)
install(
  FILES "LICENSE"
  DESTINATION "${share_install_dir}"
  COMPONENT "Resources"
)

# What components to install
set(install_components "Programs")
if(FFDE_INSTALL_DOCS)
    list(APPEND install_components "Documentation")
endif()

# Configure Cpack
set(CPACK_GENERATOR "TGZ;ZIP")
set(CPACK_COMPONENTS_ALL ${install_components})
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "C. Seth Parker")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ffdropenc")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

if(APPLE)
   set(CPACK_GENERATOR "DragNDrop")
   set(CPACK_DMG_FORMAT "UDBZ")
   set(CPACK_DMG_VOLUME_NAME "ffdropenc")
   set(CPACK_DMG_BACKGROUND_IMAGE ${PROJECT_SOURCE_DIR}/package/background.png)
   set(CPACK_DMG_DS_STORE ${PROJECT_SOURCE_DIR}/package/DS_Store.template)
   set(CPACK_SYSTEM_NAME "macOS")
endif()

include(CPack)
