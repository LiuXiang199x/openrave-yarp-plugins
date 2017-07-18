include(YCMEPHelper)

ycm_ep_helper(COLOR_DEBUG TYPE GIT
              STYLE GITHUB
              REPOSITORY roboticslab-uc3m/color-debug.git
              TAG master)

# Include path to ColorDebug.hpp.
ExternalProject_Get_Property(COLOR_DEBUG INSTALL_DIR)
include_directories(${INSTALL_DIR}/${CMAKE_INSTALL_INCLUDEDIR})

if(NOT COLOR_DEBUG_FOUND)
    message(STATUS "Build COLOR_DEBUG target and configure project again to make advanced CD options available on UI.")
endif()

# CMake has downloaded color-debug (build step).
if(COLOR_DEBUG_FOUND)
    # Load COLOR_DEBUGConfig.cmake, which in turn includes ColorDebugOptions.cmake.
    find_package(COLOR_DEBUG QUIET)
endif()
