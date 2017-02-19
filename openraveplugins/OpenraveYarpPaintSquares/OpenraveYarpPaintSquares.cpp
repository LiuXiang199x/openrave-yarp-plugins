/**
 * thanks Rosen Diankov
   Every plugin contains a bunch of openrave interfaces, the plugincpp plugin creates a simple OpenRAVE::ModuleBase interface named \b mymodule.
   Inside programs, load the plugin using the RaveLoadPlugin, and then create the module the plugin offers using
   \verbatim
   m=RaveCreateModule(env,"mymodule");
   \endverbatim
   To test things through the command line, do:
   \verbatim
   openrave --loadplugin libplugincpp.so --module mymodule "my args"
   \endverbatim
   This will load liboplugincpp.so and startup module "mymodule". From plugincpp, notice that mymodule
   supports some "commands". These are in-process string-based calls invoked through
   interface->SendCommand function.
   If you are using octave or matlab, then can communicate with openrave through tcp/ip, check out: http://openrave.programmingvision.com/wiki/index.php/OctaveMATLAB
   Most openrave users use python to dynamically interact with openrave. For example:
   \verbatim
   openrave.py -i  --loadplugin libplugincpp.so data/lab1.env.xml
   \endverbatim
   drops into the python promp with the plugin loaded and a scene loaded. Then it is possible to execute the following python commands to create the interface and call a command:
   \verbatim
   m=RaveCreateModule(env,'mymodule')
   env.Add(m,true,'my args')
   m.SendCommand('numbodies')
   \endverbatim
   <b>Full Example Code:</b>
 */
#include <openrave/openrave.h>
#include <openrave/plugin.h>
#include <boost/bind.hpp>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#define DEFAULT_RATE_MS 20.0
#define NSQUARES 16


using namespace std;
using namespace OpenRAVE;

YARP_DECLARE_PLUGINS(yarpplugins)

class OpenraveYarpPaintSquares : public ModuleBase, public yarp::os::RateThread
{
public:
    OpenraveYarpPaintSquares(EnvironmentBasePtr penv) : ModuleBase(penv), RateThread(DEFAULT_RATE_MS) {
        YARP_REGISTER_PLUGINS(yarpplugins);
        __description = "OpenraveYarpPaintSquares plugin.";
        RegisterCommand("open",boost::bind(&OpenraveYarpPaintSquares::Open, this,_1,_2),"opens port");
    }

    virtual ~OpenraveYarpPaintSquares() {
        rpcServer.close();
    }

    virtual void Destroy() {

        RAVELOG_INFO("module unloaded from environment\n");
    }

    /*int main(const string& cmd) {
        RAVELOG_INFO("module initialized cmd; %s\n", cmd.c_str());
        return 0;
    }*/

    bool Open(ostream& sout, istream& sinput)
    {
        vector<string> funcionArgs;
        while(sinput)
        {
            string funcionArg;
            sinput >> funcionArg;
            funcionArgs.push_back(funcionArg);
        }

        string portName("/openraveYarpPaintSquares/rpc:s");

        if (funcionArgs.size() > 0)
        {
            if( funcionArgs[0][0] == '/')
                portName = funcionArgs[0];
        }

        if ( !yarp.checkNetwork() )
        {
            RAVELOG_INFO("Found no yarp network (try running \"yarpserver &\"), bye!\n");
            return false;
        }

        RAVELOG_INFO("portName: %s\n",portName.c_str());
        rpcServer.open(portName);

        RAVELOG_INFO("penv: %p\n",GetEnv().get());
        OpenRAVE::EnvironmentBasePtr penv = GetEnv();

        _objPtr = penv->GetKinBody("object");
        if(!_objPtr) {
            fprintf(stderr,"error: object \"object\" does not exist.\n");
        } else printf("sucess: object \"object\" exists.\n");

        _wall = penv->GetKinBody("wall");
        if(!_wall) {
            fprintf(stderr,"error: object \"wall\" does not exist.\n");
        } else printf("sucess: object \"wall\" exists.\n");

        std::vector<RobotBasePtr> robots;
        penv->GetRobots(robots);
        std::cout << "Robot 0: " << robots.at(0)->GetName() << std::endl;  // default: teo
        RobotBasePtr probot = robots.at(0);
        probot->SetActiveManipulator("rightArm");
        probot->Grab(_objPtr);

        psqPainted.resize(NSQUARES);

        this->start();  // start yarp::os::RateThread (calls run periodically)

        return true;
    }

    virtual void run()
    {
        //RAVELOG_INFO("thread\n");

        //Get new object (pen) position
        T_base_object = _objPtr->GetTransform();
        double T_base_object_x = T_base_object.trans.x;
        double T_base_object_y = T_base_object.trans.y;
        double T_base_object_z = T_base_object.trans.z;

        //Update psqpainted to the new values
        for(int i=0; i<(NSQUARES); i++){
            stringstream ss;
            ss << "square" << i;
            Transform pos_square = _wall->GetLink(ss.str())->GetGeometry(0)->GetTransform();

            double pos_square_x = pos_square.trans.x;
            double pos_square_y = pos_square.trans.y;
            double pos_square_z = pos_square.trans.z;
            double dist = sqrt(pow(T_base_object_x-pos_square_x,2)
                                      + pow(T_base_object_y-pos_square_y,2)
                                      + pow(T_base_object_z-pos_square_z,2) );

            if (dist < 0.13){
                _wall->GetLink(ss.str())->GetGeometry(0)->SetDiffuseColor(RaveVector<float>(0.0, 0.0, 1.0));

                //for(int i=0;i<NSQUARES;i++){ //Ugly way of delimiting the size of sqpainted in the loop
                //    std::cout<<psqPainted[i]<<" ";
                //}
             //   std::cout<<"<"<<std::endl;
                psqPainted[i]=1;
    //            for(int i=0;i<NSQUARES;i++){ //Ugly way of delimiting the size of sqpainted in the loop
    //                std::cout<<psqPainted->operator[](i)<<" ";
    //            }
    //            std::cout<<">"<<std::endl;
               // std::cout<<"I have painted a happy little tree \n"<<std::endl;
            }
            else{
              //  std::cout<<"NO HAPPY TREE"<<std::endl;
            }
            ss.str("");
        }

    }

private:
    yarp::os::Network yarp;
    yarp::os::RpcServer rpcServer;

    vector<int> psqPainted;
    Transform T_base_object;
    KinBodyPtr _objPtr;
    KinBodyPtr _wall;
};

InterfaceBasePtr CreateInterfaceValidated(InterfaceType type, const std::string& interfacename, std::istream& sinput, EnvironmentBasePtr penv) {
    if( type == PT_Module && interfacename == "openraveyarppaintsquares" ) {
        return InterfaceBasePtr(new OpenraveYarpPaintSquares(penv));
    }
    return InterfaceBasePtr();
}

void GetPluginAttributesValidated(PLUGININFO& info) {
    info.interfacenames[PT_Module].push_back("OpenraveYarpPaintSquares");
}

OPENRAVE_PLUGIN_API void DestroyPlugin() {
    RAVELOG_INFO("destroying plugin\n");
}
