# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveGrabber
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveGrabber
                    INCLUDE YarpOpenraveGrabber.hpp
                    WRAPPER grabber)

IF (NOT SKIP_YarpOpenraveGrabber)

SET(CMAKE_MODULE_PATH ${TEO_MODULE_PATH} ${CMAKE_MODULE_PATH})
FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(OpenRAVE REQUIRED)
find_package(Boost COMPONENTS iostreams python thread system)

if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
  add_definitions("-fno-strict-aliasing -Wall")
endif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${OpenRAVE_INCLUDE_DIRS} ${OPENRAVE_YARP_PLUGINS_INCLUDE_DIRS})
if( Boost_INCLUDE_DIRS )
  include_directories(${Boost_INCLUDE_DIRS})
endif()

LINK_DIRECTORIES(${OPENRAVE_YARP_PLUGINS_LINK_DIRS})

YARP_ADD_PLUGIN(YarpOpenraveGrabber YarpOpenraveGrabber.hpp DeviceDriverImpl.cpp IFrameGrabberImageImpl.cpp)
add_dependencies(YarpOpenraveGrabber COLOR_DEBUG YarpOpenraveBase)
set_target_properties(YarpOpenraveGrabber PROPERTIES COMPILE_FLAGS "${OpenRAVE_CXXFLAGS}")
set_target_properties(YarpOpenraveGrabber PROPERTIES LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")
TARGET_LINK_LIBRARIES(YarpOpenraveGrabber ${OpenRAVE_LIBRARIES} ${OpenRAVE_CORE_LIBRARIES} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${YARP_LIBRARIES} YarpOpenraveBase)

# Exporting dependencies for ...Config.cmake quite manually for now...
set(OPENRAVE_YARP_PLUGINS_INCLUDE_DIRS ${OPENRAVE_YARP_PLUGINS_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "appended header dirs")
#set(OPENRAVE_YARP_PLUGINS_LIBRARIES ${OPENRAVE_YARP_PLUGINS_LIBRARIES} YarpOpenraveGrabber CACHE INTERNAL "appended libraries")

  #install(TARGETS YarpOpenraveGrabber DESTINATION lib)
  yarp_install(TARGETS YarpOpenraveGrabber
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  yarp_install(FILES YarpOpenraveGrabber.ini
              COMPONENT runtime
              DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

ENDIF (NOT SKIP_YarpOpenraveGrabber)
