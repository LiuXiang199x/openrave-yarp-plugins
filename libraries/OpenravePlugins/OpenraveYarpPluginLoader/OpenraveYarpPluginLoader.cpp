// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <cstring>

#include <sstream>
#include <string>
#include <vector>

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Value.h>

#include <yarp/dev/PolyDriver.h>

#include <ColorDebug.h>

/**
 * @ingroup OpenravePlugins
 * \defgroup OpenraveYarpPluginLoader
 *
 * @brief Contains roboticslab::OpenraveYarpPluginLoader.
 */

/**
 * @ingroup OpenraveYarpPluginLoader
 * @brief Loads one or several YARP Plugin, passing environment pointer.
 */
class OpenraveYarpPluginLoader : public OpenRAVE::ModuleBase
{
public:
    OpenraveYarpPluginLoader(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
    {
        __description = "OpenraveYarpPluginLoader plugin.";
        OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpPluginLoader::Open, this,_1,_2),"opens OpenraveYarpPluginLoader");
    }

    virtual ~OpenraveYarpPluginLoader()
    {
        for(int i=0;i<yarpPlugins.size();i++)
        {
            yarpPlugins[i]->close();
            delete yarpPlugins[i];
            yarpPlugins[i] = 0;
        }
    }

    virtual void Destroy()
    {
        RAVELOG_INFO("module unloaded from environment\n");
    }

    int main(const std::string& cmd)
    {
        CD_DEBUG("[%s]\n", cmd.c_str());
        std::stringstream ss(cmd);

        //-- Fill openStrings
        std::vector<std::string> openStrings;
        while( ! ss.eof() )
        {
            std::string tmp;
            ss >> tmp;
            if(tmp == "open")
            {
                std::string openString("open");
                openStrings.push_back(openString);
            }
            else
            {
                if(openStrings.size() == 0)
                {
                    CD_ERROR("args must start with open, sorry! Bye!\n");
                    return 1;
                }
                openStrings[openStrings.size()-1].append(" ");
                openStrings[openStrings.size()-1].append(tmp);
            }
        }

        //-- Open each openString
        for(int i=0;i<openStrings.size();i++)
        {
            CD_DEBUG("open[%d]: [%s]\n",i,openStrings[i].c_str());

            std::istringstream sinput( openStrings[i] );
            std::ostringstream sout;
            if( ! OpenRAVE::InterfaceBase::SendCommand(sout,sinput) )
                return 1;
        }
        return 0;
    }

    bool Open(std::ostream& sout, std::istream& sinput)
    {
        CD_INFO("Checking for yarp network...\n");
        if ( ! yarp.checkNetwork() )
        {
            CD_ERROR("Found no yarp network (try running \"yarpserver &\"), bye!\n");
            return false;
        }
        CD_SUCCESS("Found yarp network.\n");

        //-- Given "std::istream& sinput", create equivalent to "int argc, char *argv[]"
        //-- Note that char* != const char* given by std::string::c_str();
        std::vector<char *> argv;
        char* dummyProgramName = "dummyProgramName";
        argv.push_back(dummyProgramName);

        while(sinput)
        {
            std::string str;
            sinput >> str;
            if(str.length() == 0)  //-- Omits empty string that is usually at end via openrave.
                continue;
            char *cstr = new char[str.length() + 1];  // pushed to member argv to be deleted in ~.
            std::strcpy(cstr, str.c_str());
            argv.push_back(cstr);
        }

        for(size_t i=0;i<argv.size();i++)
            CD_DEBUG("argv[%d] is [%s]\n",i,argv[i]);

        yarp::os::Property options;
        options.fromCommand(argv.size(),argv.data());

        CD_DEBUG("config: %s\n", options.toString().c_str());

        //-- Get and put pointer to environment
        CD_INFO("penv: %p\n",GetEnv().get());
        OpenRAVE::EnvironmentBasePtr penv = GetEnv();
        yarp::os::Value v(&penv, sizeof(OpenRAVE::EnvironmentBasePtr));
        options.put("penv",v);

        //-- Fill robotIndices from: robotIndex/robotIndices/allRobots
        std::vector<int> robotIndices;

        std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
        penv->GetRobots(vectorOfRobotPtr);

        if( options.check("robotIndex") )
        {
            int robotIndex = options.find("robotIndex").asInt32();
            robotIndices.push_back(robotIndex);
        }
        else if( options.check("robotIndices") )
        {
            CD_ERROR("robotIndices not implemented yet. Bye!\n");
            return false;
        }
        else if( options.check("allRobots") )
        {
            for(int i=0;i<vectorOfRobotPtr.size();i++)
                robotIndices.push_back(i);
        }
        else
        {
            CD_INFO("Not using --robotIndex or --robotIndices or --allRobots parameter.\n");

            yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
            yarpPlugin->open(options);

            if( ! yarpPlugin->isValid() )
            {
                CD_ERROR("yarp plugin not valid.\n");
                return false;
            }
            CD_SUCCESS("Valid yarp plugin.\n");

            yarpPlugins.push_back(yarpPlugin);

            return true;
        }

        //-- Iterate through robots
        for(int i=0;i<robotIndices.size();i++)
        {
            int robotIndex = robotIndices[i];
            if( robotIndex >= vectorOfRobotPtr.size())
            {
                CD_ERROR("robotIndex %d >= vectorOfRobotPtr.size() %d, not loading yarp plugin. Bye!\n",robotIndex,vectorOfRobotPtr.size());
                return false;
            }
            else if (robotIndex < 0)
            {
                CD_ERROR("robotIndex %d < 0, not loading yarp plugin. Bye!\n",robotIndex);
                return false;
            }
            options.put("robotIndex",robotIndex);

            std::string robotName("/");
            robotName += vectorOfRobotPtr[ robotIndex ]->GetName();

            //-- Fill manipulatorIndices from: manipulatorIndex/manipulatorIndices/allManipulators
            //-- Fill sensorIndices from: sensorIndex/sensorIndices/allSensors
            std::vector<int> manipulatorIndices;
            std::vector<int> sensorIndices;

            std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = vectorOfRobotPtr[ robotIndex ]->GetManipulators();
            std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> vectorOfSensorPtr = vectorOfRobotPtr[ robotIndex ]->GetAttachedSensors();

            if( options.check("manipulatorIndex") )
            {
                int manipulatorIndex = options.find("manipulatorIndex").asInt32();
                manipulatorIndices.push_back(manipulatorIndex);
            }
            else if( options.check("manipulatorIndices") )
            {
                CD_ERROR("manipulatorIndices not implemented yet. Bye!\n");
                return false;
            }
            else if( options.check("allManipulators") )
            {
                for(int i=0;i<vectorOfManipulatorPtr.size();i++)
                    manipulatorIndices.push_back(i);
            }
            else if( options.check("sensorIndex") )
            {
                int sensorIndex = options.find("sensorIndex").asInt32();
                sensorIndices.push_back(sensorIndex);
            }
            else if( options.check("sensorIndices") )
            {
                CD_ERROR("sensorIndices not implemented yet. Bye!\n");
                return false;
            }
            else if( options.check("allSensors") )
            {
                for(int i=0;i<vectorOfSensorPtr.size();i++)
                    sensorIndices.push_back(i);
            }
            else
            {
                CD_INFO("Not using --manipulatorIndex or --manipulatorIndices or --allManipulators parameter.\n");
                CD_INFO("Not using --sensorIndex or --sensorIndices or --allSensors parameter.\n");

                if( ! options.check("forceName") )
                {
                    options.put("name",robotName);
                }

                yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
                yarpPlugin->open(options);

                if( ! yarpPlugin->isValid() )
                {
                    CD_ERROR("yarp plugin not valid.\n");
                    return false;
                }
                CD_SUCCESS("Valid yarp plugin.\n");

                yarpPlugins.push_back(yarpPlugin);
            }

            //-- Iterate through manipulators
            for(int i=0;i<manipulatorIndices.size();i++)
            {
                int manipulatorIndex = manipulatorIndices[i];
                if(manipulatorIndex >= vectorOfManipulatorPtr.size())
                {
                    CD_ERROR("manipulatorIndex %d >= vectorOfManipulatorPtr.size() %d, not loading yarp plugin. Bye!\n",manipulatorIndex,vectorOfManipulatorPtr.size());
                    return false;
                }
                else if (manipulatorIndex < 0)
                {
                    CD_ERROR("manipulatorIndex %d < 0, not loading yarp plugin.\n",manipulatorIndex);
                    return false;
                }
                options.put("manipulatorIndex",manipulatorIndex);

                std::string manipulatorName(robotName);
                manipulatorName += "/";
                manipulatorName += vectorOfManipulatorPtr[ manipulatorIndex ]->GetName();

                if( ! options.check("forceName") )
                {
                    options.put("name",manipulatorName);
                }

                yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
                yarpPlugin->open(options);

                if( ! yarpPlugin->isValid() )
                {
                    CD_ERROR("yarp plugin not valid.\n");
                    return false;
                }
                CD_SUCCESS("Valid yarp plugin.\n");

                yarpPlugins.push_back(yarpPlugin);
            }

            //-- Iterate through sensors
            for(int i=0;i<sensorIndices.size();i++)
            {
                int sensorIndex = sensorIndices[i];
                if(sensorIndex >= vectorOfSensorPtr.size())
                {
                    CD_ERROR("sensorIndex %d >= vectorOfSensorPtr.size() %d, not loading yarp plugin. Bye!\n",sensorIndex,vectorOfSensorPtr.size());
                    return false;
                }
                else if (sensorIndex < 0)
                {
                    CD_ERROR("sensorIndex %d < 0, not loading yarp plugin.\n",sensorIndex);
                    return false;
                }
                options.put("sensorIndex",sensorIndex);

                std::string sensorName(robotName);
                sensorName += "/";
                sensorName += vectorOfSensorPtr[ sensorIndex ]->GetName();

                if( ! options.check("forceName") )
                {
                    options.put("name",sensorName);
                }

                yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
                yarpPlugin->open(options);

                if( ! yarpPlugin->isValid() )
                {
                    CD_ERROR("yarp plugin not valid.\n");
                    return false;
                }
                CD_SUCCESS("Valid yarp plugin.\n");

                yarpPlugins.push_back(yarpPlugin);
            }
        }

        //-- Note that we start on element 1, first elem was not via new!!
        for(size_t i=1;i<argv.size();i++)
        {
            //CD_DEBUG("Deleting [%s]\n",argv[i]);
            delete argv[i];
            argv[i] = 0;
        }

        return true;
    }

private:
    yarp::os::Network yarp;
    std::vector<yarp::dev::PolyDriver*> yarpPlugins;
};

OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarppluginloader")
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpPluginLoader(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info)
{
    info.interfacenames[OpenRAVE::PT_Module].push_back("OpenraveYarpPluginLoader");
}

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}
