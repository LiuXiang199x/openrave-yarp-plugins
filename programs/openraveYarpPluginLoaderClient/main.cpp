// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 *
 * @ingroup yarp_devices_programs
 * \defgroup dumpCanBus dumpCanBus
 *
 * @brief Creates an instance of roboticslab::DumpCanBus.
 *
 * @section dumpCanBus_legal Legal
 *
 * Copyright: 2013 (C) Universidad Carlos III de Madrid
 *
 * Author: <a href="http://roboticslab.uc3m.es/roboticslab/persona_publ.php?id_pers=72">Juan G. Victores</a>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see license/LGPL.TXT
 *
 * @section dumpCanBus_install Installation
 *
 * The module is compiled when ENABLE_dumpCanBus is activated (default: ON). For further
 * installation steps refer to <a class="el" href="pages.html">your own system installation guidelines</a>.
 *
 * @section dumpCanBus_running Running (assuming correct installation)
 *
 * First we must run a YARP name server if it is not running in our current namespace:
\verbatim
[on terminal 1] yarp server
\endverbatim
 * And then launch the actual module:
\verbatim
[on terminal 2] dumpCanBus
\endverbatim
 *
 * @section dumpCanBus_modify Modify
 *
 * This file can be edited at
 * programs/dumpCanBus/main.cpp
 *
 */

#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>

#include <ColorDebug.h>

#include "OpenraveYarpPluginLoaderClient.hpp"

int main(int argc, char *argv[])
{
    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultContext("openraveYarpPluginLoaderClient");
    rf.setDefaultConfigFile("openraveYarpPluginLoaderClient.ini");
    rf.configure(argc, argv);

    CD_INFO("Checking for yarp network...\n");
    yarp::os::Network yarp;
    if (!yarp.checkNetwork())
    {
        CD_ERROR("Found no yarp network (try running \"yarpserver &\"), bye!\n");
        return 1;
    }
    CD_SUCCESS("Found yarp network.\n");

    roboticslab::OpenraveYarpPluginLoaderClient mod;
    return mod.runModule(rf);
}
