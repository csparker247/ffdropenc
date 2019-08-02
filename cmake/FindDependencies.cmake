########
# Core #
########

find_package(Qt5 5.7 QUIET REQUIRED COMPONENTS Core Widgets Gui)
find_package(nlohmann_json 3.2.0 REQUIRED)

############
# Optional #
############

# If this option is set, then use all optional dependencies
option(PROJ_USE_ALL "Use all optional third-party libs" off)
if(PROJ_USE_ALL)
    message(STATUS "All optional third-party libraries enabled. Individual \
preferences will be ignored.")
endif()
