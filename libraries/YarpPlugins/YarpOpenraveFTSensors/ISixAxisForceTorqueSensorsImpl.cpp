// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveFTSensors.hpp"

#include <ColorDebug.h>

namespace roboticslab
{

// ------------------ ISixAxisForceTorqueSensors Related ----------------------------------------

size_t YarpOpenraveFTSensors::getNrOfSixAxisForceTorqueSensors() const
{
    return 1;
}

// ----------------------------------------------------------------------------

yarp::dev::MAS_status YarpOpenraveFTSensors::getSixAxisForceTorqueSensorStatus(size_t sens_index) const
{
    return yarp::dev::MAS_OK;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveFTSensors::getSixAxisForceTorqueSensorName(size_t sens_index, std::string &name) const
{
    name = sensorBasePtr->GetName();
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveFTSensors::getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frameName) const
{
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveFTSensors::getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    out[0] = sensorDataPtr->force[0];
    out[1] = sensorDataPtr->force[1];
    out[2] = sensorDataPtr->force[2];
    out[3] = sensorDataPtr->torque[0];
    out[4] = sensorDataPtr->torque[1];
    out[5] = sensorDataPtr->torque[2];
    return true;
}

// ----------------------------------------------------------------------------

}
