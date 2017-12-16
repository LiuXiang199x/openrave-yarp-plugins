// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_ROBOT_MANAGER_HPP__
#define __YARP_OPENRAVE_ROBOT_MANAGER_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#include <openrave-core.h>

#include <boost/make_shared.hpp>

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>

#include "YarpOpenraveBase.hpp"

#include "IRobotManager.hpp"

#include "ColorDebug.hpp"

namespace roboticslab
{

/**
 * @ingroup TeoYarp
 * \defgroup YarpOpenraveRobotManager
 *
 * @brief Contains teo::YarpOpenraveRobotManager.
 *
 * @section YarpOpenraveRobotManager_install Installation
 *
 * The plugin is compiled when ENABLE_TeoYarp_YarpOpenraveRobotManager is activated (not default). For further
 * installation steps refer to <a class="el" href="pages.html">your own system installation guidelines</a>.
 */

/**
 * @ingroup YarpOpenraveRobotManager
 * @brief Implements the YARP_dev DeviceDriver, and IRobotManager.
 * interface class member functions.
 */
class YarpOpenraveRobotManager : YarpOpenraveBase, public yarp::dev::DeviceDriver, public rd::IRobotManager {
public:

    YarpOpenraveRobotManager() {}


    // -------- DeviceDriver declarations. Implementation in DeviceDriverImpl.cpp --------

    /**
     * Open the DeviceDriver.
     * @param config is a list of parameters for the device.
     * Which parameters are effective for your device can vary.
     * See \ref dev_examples "device invocation examples".
     * If there is no example for your device,
     * you can run the "yarpdev" program with the verbose flag
     * set to probe what parameters the device is checking.
     * If that fails too,
     * you'll need to read the source code (please nag one of the
     * yarp developers to add documentation for your device).
     * @return true/false upon success/failure
     */
    virtual bool open(yarp::os::Searchable& config);

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    virtual bool close();

    // ------- IRobotManager declarations. Implementation in IRobotManagerImpl.cpp -------

    //-- Robot movement related functions
    virtual bool moveForward(int velocity);
    virtual bool moveBackwards(int velocity);
    virtual bool turnLeft(int velocity);
    virtual bool turnRight(int velocity);
    virtual bool stopMovement();

    //-- Robot camera related functions
    virtual bool tiltUp(int velocity);
    virtual bool tiltDown(int velocity);
    virtual bool panLeft(int velocity);
    virtual bool panRight(int velocity);
    virtual bool stopCameraMovement();

    //-- Robot connection related functions
    /// @brief Connect to the remote robot
    virtual bool connect();

    /// @brief Disconnect from the remote robot
    virtual bool disconnect();

    /// @brief Test connection (not in used yet)
    virtual bool test();

    /// @brief Enable/disable sending commands through the manager
    virtual void setEnabled(bool enabled);

    //-- Other
    virtual void onDestroy();

    // ------------------------------- Private -------------------------------------

private:

    //OpenRAVE//
    OpenRAVE::ControllerBasePtr pcontrol;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_ROBOT_MANAGER_HPP__
