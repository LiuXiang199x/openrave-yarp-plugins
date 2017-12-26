#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml')

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpPluginLoader.SendCommand('open --device controlboardwrapper2 --subdevice YarpOpenraveControlboardCollision --robotIndex 0 --allManipulators')

    # Convex Decomposition padding
    teo_robot = env.GetRobots()[0]
    cdmodel = databases.convexdecomposition.ConvexDecompositionModel(teo_robot)

    # Load the ConvexDecomposition model, if it does not exit in the database generate it.
    if not cdmodel.load(): # It always load the model with zero padding
        # If not already in the database. Generate:
        cdmodel.generate(padding=0.02)

    cdmodel.save()
    print 'Finished saving'

    print 'Setting robot...'
    cdmodel.setrobot()
    print 'Finish setrobot'

    print 'Starting visualization of the convexdecomposition model'
    cdmodel.show()
    #print 'Finished loading visualization'

    while 1:
        pass

finally:
    RaveDestroy()

