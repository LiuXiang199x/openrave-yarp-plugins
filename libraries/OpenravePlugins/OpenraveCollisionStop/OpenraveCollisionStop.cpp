// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <openrave/openrave.h>
#include <openrave/plugin.h>
#include <boost/bind.hpp>

#include <yarp/os/RateThread.h>
#include <yarp/os/Network.h>  // [2018/03/15] Required by yarp::os::RateThread

#include "ColorDebug.hpp"

/**
 * @ingroup OpenravePlugins
 * \defgroup OpenraveCollisionStop
 *
 * @brief Contains roboticslab::OpenraveCollisionStop.
 */

/**
 * @ingroup OpenraveCollisionStop
 * @brief Loads one or several YARP Plugin, passing environment pointer.
 */
class OpenraveCollisionStop : public OpenRAVE::ModuleBase, yarp::os::RateThread
{
public:
    OpenraveCollisionStop(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv), yarp::os::RateThread(20.0)
    {
        __description = "OpenraveCollisionStop plugin.";
        RegisterCommand("open",boost::bind(&OpenraveCollisionStop::Open, this,_1,_2),"opens port");
    }

    virtual ~OpenraveCollisionStop()
    {
    }

    virtual void Destroy()
    {
        RAVELOG_INFO("module unloaded from environment (1/2)\n");
        this->stop();
        RAVELOG_INFO("module unloaded from environment (2/2)\n");
    }

    /**
     * Loop function. This is the thread itself.
     * The thread calls the run() function every <period> ms.
     * At the end of each run, the thread will sleep the amounth of time
     * required, taking into account the time spent inside the loop function.
     * Example:  requested period is 10ms, the run() function take 3ms to
     * be executed, the thread will sleep for 7ms.
     *
     * Note: after each run is completed, the thread will call a yield()
     * in order to facilitate other threads to run.
     */
    virtual void run()
    {
        //CD_DEBUG("\n");

        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment

        /*if( penv->CheckCollision(OpenRAVE::KinBodyConstPtr(vectorOfRobotPtr[0]),_report) )
        {
            CD_WARNING("collsion in trajectory: %s\n",_report->__str__().c_str());
        }*/

        /*if( vectorOfRobotPtr[0]->CheckSelfCollision(_report) )
        {
            CD_WARNING("collsion in trajectory: %s\n",_report->__str__().c_str());
        }*/

        return;
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
            if( ! SendCommand(sout,sinput) )
                return 1;
        }
        return 0;
    }

    bool Open(std::ostream& sout, std::istream& sinput)
    {
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
            strcpy(cstr, str.c_str());
            argv.push_back(cstr);
        }

        for(size_t i=0;i<argv.size();i++)
            CD_DEBUG("argv[%d] is [%s]\n",i,argv[i]);

        yarp::os::Property options;
        options.fromCommand(argv.size(),argv.data());

        CD_DEBUG("config: %s\n", options.toString().c_str());

        //-- Get and put pointer to environment
        CD_INFO("penv: %p\n",GetEnv().get());
        penv = GetEnv();

        penv->GetRobots(vectorOfRobotPtr);

        OpenRAVE::RobotBasePtr probot = vectorOfRobotPtr[ 0 ];

        std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = probot->GetManipulators();

        for(int manipulatorIndex=0;manipulatorIndex<vectorOfManipulatorPtr.size();manipulatorIndex++)
        {
            OpenRAVE::RobotBase::ManipulatorPtr manipulatorPtr = vectorOfManipulatorPtr[manipulatorIndex];

            CD_DEBUG("* Manipulator [%d]: %s\n",manipulatorIndex,manipulatorPtr->GetName().c_str());

            std::vector<int> manipulatorJointIDs = vectorOfManipulatorPtr[manipulatorIndex]->GetArmIndices();

            std::vector<OpenRAVE::RobotBase::LinkPtr> vectorOfLinkPtr = probot->GetLinks();

            for(int jointIndex=0;jointIndex<manipulatorJointIDs.size();jointIndex++)
            {
                CD_DEBUG("** Joint [%d]: %s\n",jointIndex,probot->GetJointFromDOFIndex(manipulatorJointIDs[jointIndex])->GetName().c_str());

                for(int linkIndex=0;linkIndex<vectorOfLinkPtr.size();linkIndex++)
                {
                    OpenRAVE::RobotBase::LinkPtr linkPtr = vectorOfLinkPtr[linkIndex];
                    if( probot->DoesAffect(manipulatorJointIDs[jointIndex],linkPtr->GetIndex()) )
                    {
                        CD_DEBUG("*** Affects: %s\n", linkPtr->GetName().c_str());
                    }
                }

                //{
                    //CD_INFO("in [%d, %d]: %s\n",i,j,vlinks[j]->GetName().c_str());
                    //if( _report->plink1 == vlinks[j] )
                    //{
                    //    //CD_WARNING("collsion in [%d, %d]: %s\n",i,j,_report->__str__().c_str());
                    //}
                //}
            }

            /*std::vector<OpenRAVE::RobotBase::LinkPtr> vectorOfLinkPtr;

            for(int linkIndex=0;linkIndex<vectorOfLinkPtr.size();linkIndex++)
            {
                CD_DEBUG("** Link [%d]: %s\n",linkIndex,vectorOfLinkPtr[linkIndex]->GetName().c_str());

                //{
                    //CD_INFO("in [%d, %d]: %s\n",i,j,vlinks[j]->GetName().c_str());
                    //if( _report->plink1 == vlinks[j] )
                    //{
                    //    //CD_WARNING("collsion in [%d, %d]: %s\n",i,j,_report->__str__().c_str());
                    //}
                //}
            }*/

        }

        _report.reset(new OpenRAVE::CollisionReport());

        this->start();

        return true;

        //////////////////////////////////////////////////////////////////////////

        //-- Fill robotIndices from: robotIndex/robotIndices/allRobots
        std::vector<int> robotIndices;

        if( options.check("robotIndex") )
        {
            int robotIndex = options.find("robotIndex").asInt();
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
            CD_ERROR("Not using --robotIndex or --robotIndices or --allRobots parameter.\n");
            return false;
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

            std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = vectorOfRobotPtr[ robotIndex ]->GetManipulators();

            if( options.check("manipulatorIndex") )
            {
                int manipulatorIndex = options.find("manipulatorIndex").asInt();
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
            else
            {
                CD_INFO("Not using --manipulatorIndex or --manipulatorIndices or --allManipulators parameter.\n");

                if( ! options.check("forceName") )
                {
                    options.put("name",robotName);
                }

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
    yarp::os::Network yarp;  // [2018/03/15] Required by yarp::os::RateThread

    OpenRAVE::EnvironmentBasePtr penv;
    std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
    OpenRAVE::CollisionReportPtr _report;

};

OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    if( type == OpenRAVE::PT_Module && interfacename == "openravecollisionstop")
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveCollisionStop(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info)
{
    info.interfacenames[OpenRAVE::PT_Module].push_back("OpenraveCollisionStop");
}

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}
