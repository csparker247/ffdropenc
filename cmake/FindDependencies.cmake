########
# Core #
########

# For compiler sanitizers. Taken from:
# https://github.com/arsenm/sanitizers-cmake/blob/master/README.md
find_package(Sanitizers)

### Boost ###
find_package(Boost REQUIRED COMPONENTS system filesystem)


############
# Optional #
############

# If this option is set, then use all optional dependencies
option(PROJ_USE_ALL "Use all optional third-party libs" off)
if(PROJ_USE_ALL)
    message(STATUS "All optional third-party libraries enabled. Individual \
preferences will be ignored.")
endif()
