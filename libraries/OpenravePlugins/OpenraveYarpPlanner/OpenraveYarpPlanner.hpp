// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPENRAVE_YARP_PLUGIN_LOADER_HPP__
#define __OPENRAVE_YARP_PLUGIN_LOADER_HPP__

#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

#include <yarp/dev/PolyDriver.h>

#include <openrave/openrave.h>

#include "OypPortReader.hpp"

namespace roboticslab
{

/**
 * @ingroup OpenravePlugins
 * \defgroup OpenraveYarpPlanner
 *
 * @brief Contains roboticslab::OpenraveYarpPlanner.
 */

/**
 * @ingroup OpenraveYarpPlanner
 * @brief Loads one or several YARP Plugin, passing environment pointer.
 */
class OpenraveYarpPlanner : public OpenRAVE::ModuleBase
{
public:
    OpenraveYarpPlanner(OpenRAVE::EnvironmentBasePtr penv);
    virtual ~OpenraveYarpPlanner();
    virtual void Destroy();

    int main(const std::string& cmd);

    bool Open(std::ostream& sout, std::istream& sinput);

private:
    yarp::os::Network yarp;

    OypPortReader oypPortReader;
    yarp::os::RpcServer oypRpcServer;
};

} // namespace roboticslab

#endif // __OPENRAVE_YARP_PLUGIN_LOADER_HPP__
