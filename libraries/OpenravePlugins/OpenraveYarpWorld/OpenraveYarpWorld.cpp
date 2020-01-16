// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <ColorDebug.h>

#include "OywPortReader.hpp"

#include "OpenraveYarpWorld.hpp"

// -----------------------------------------------------------------------------

OpenraveYarpWorld::OpenraveYarpWorld(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
{
    __description = "OpenraveYarpWorld plugin.";
    OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpWorld::Open, this,_1,_2),"opens OpenraveYarpWorld");
}

// -----------------------------------------------------------------------------

OpenraveYarpWorld::~OpenraveYarpWorld()
{
    worldRpcServer.interrupt();
    worldRpcServer.close();
}

// -----------------------------------------------------------------------------

void OpenraveYarpWorld::Destroy()
{

    RAVELOG_INFO("module unloaded from environment\n");
}

// -----------------------------------------------------------------------------

int OpenraveYarpWorld::main(const std::string& cmd)
{
    RAVELOG_INFO("module initialized with \"%s\"\n", cmd.c_str());
    // hard-coding "open", note that actual Open enabled portName selection
    std::istringstream sinput("open");
    std::ostringstream sout;
    if( ! OpenRAVE::InterfaceBase::SendCommand(sout,sinput) )
        return 1;
    return 0;
}

// -----------------------------------------------------------------------------

bool OpenraveYarpWorld::Open(std::ostream& sout, std::istream& sinput)
{
    std::vector<std::string> funcionArgs;
    while(sinput)
    {
        std::string funcionArg;
        sinput >> funcionArg;
        funcionArgs.push_back(funcionArg);
    }

    std::string portName("/OpenraveYarpWorld/rpc:s");

    if (funcionArgs.size() > 0)
    {
        if( funcionArgs[0][0] == '/')
            portName = funcionArgs[0];
    }
    RAVELOG_INFO("portName: %s\n",portName.c_str());

    if ( !yarp.checkNetwork() )
    {
        RAVELOG_INFO("Found no yarp network (try running \"yarpserver &\"), bye!\n");
        return false;
    }


    RAVELOG_INFO("penv: %p\n",GetEnv().get());
    OpenRAVE::EnvironmentBasePtr penv = GetEnv();


    //-- Get the robot
    std::vector<OpenRAVE::RobotBasePtr> robots;
    penv->GetRobots(robots);
    //-- Robot 0
    probot = robots.at(0);  // which is a RobotBasePtr
    CD_INFO("Using robot 0 (%s) as main robot.\n", probot->GetName().c_str());

    //-- processor
    processor.setEnvironment(penv);
    processor.setRobot(probot);

    //-- world rpc server
    worldRpcServer.open(portName);
    worldRpcServer.setReader(processor);

    return true;
}

// -----------------------------------------------------------------------------

OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarpworldrpcresponder" )
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpWorld(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

// -----------------------------------------------------------------------------

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info)
{
    info.interfacenames[OpenRAVE::PT_Module].push_back("OpenraveYarpWorld");
}

// -----------------------------------------------------------------------------

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}

// -----------------------------------------------------------------------------