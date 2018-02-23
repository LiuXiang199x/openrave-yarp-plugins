// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_BASE_HPP__
#define __YARP_OPENRAVE_BASE_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#include <openrave-core.h>

#include <boost/make_shared.hpp>

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>

namespace roboticslab
{

// Specific for --env parameter
void SetViewer(OpenRAVE::EnvironmentBasePtr penv, const std::string& viewername);

/**
 * @ingroup openrave-yarp-plugins-libraries
 * \defgroup YarpOpenraveBase
 *
 * @brief Contains roboticslab::YarpOpenraveBase.
 */

/**
 * @ingroup YarpOpenraveBase
 * @brief Implements shared configuration functions.
 */
class YarpOpenraveBase
{
public:
    YarpOpenraveBase();

protected:
    bool configureEnvironment(yarp::os::Searchable& config);
    bool configureOpenravePlugins(yarp::os::Searchable& config);
    bool configureRobot(yarp::os::Searchable& config);
    bool clean();

    //OpenRAVE//
    OpenRAVE::EnvironmentBasePtr penv;
    OpenRAVE::RobotBasePtr probot;
    std::string robotName;

    // Specific for --env parameter with --view
    boost::thread_group openraveThreads;

private:
    static const int NOT_SET;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_BASE_HPP__
