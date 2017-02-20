#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpControlboard'):
        raveLogError("Plugin not correctly loaded")

    if not RaveLoadPlugin('OpenraveYarpPaintSquares'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/usr/local/share/xgnitive/contexts/models/teo_cgda_iros.env.xml')  # assumes XGNITIVE installed

    OpenraveYarpControlboard = RaveCreateModule(env,'OpenraveYarpControlboard')
    print OpenraveYarpControlboard.SendCommand('open')

    OpenraveYarpPaintSquares = RaveCreateModule(env,'OpenraveYarpPaintSquares')
    print OpenraveYarpPaintSquares.SendCommand('open')
    #print OpenraveYarpPaintSquares.SendCommand('open /altPortName')

    while 1:
        pass

finally:
    RaveDestroy()

