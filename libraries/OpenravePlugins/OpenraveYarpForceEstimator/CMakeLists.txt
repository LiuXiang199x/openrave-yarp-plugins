# Generated by Juan G Victores 2013
# Thanks Rosen Diankov for all OpenRAVE developments

option(ENABLE_OpenraveYarpForceEstimator "Enable/disable OpenraveYarpForceEstimator" ON)

if(ENABLE_OpenraveYarpForceEstimator)

include_directories(${OpenRAVE_INCLUDE_DIRS})

link_directories(${OpenRAVE_LIBRARY_DIRS})

add_library(OpenraveYarpForceEstimator MODULE OpenraveYarpForceEstimator.cpp)

set_target_properties(OpenraveYarpForceEstimator PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                            LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

target_link_libraries(OpenraveYarpForceEstimator YARP::YARP_OS
                                                 YARP::YARP_init
                                                 ${OpenRAVE_LIBRARIES}
                                                 Boost::boost
                                                 ROBOTICSLAB::ColorDebug)

include(InstallOpenravePlugin)

install_openrave_plugin(OpenraveYarpForceEstimator)

endif()
