// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRGBDSensor.hpp"

#include <stdint.h> // uint8_t

#include <cmath>

#include <vector>

#include <yarp/os/Time.h>
#include <yarp/sig/Image.h>

#include <ColorDebug.h>

namespace roboticslab
{

// ------------------ IRGBDSensor Related ----------------------------------------

int YarpOpenraveRGBDSensor::getDepthHeight()
{
    return depthHeight;
}

// ----------------------------------------------------------------------------

int YarpOpenraveRGBDSensor::getDepthWidth()
{
    return depthWidth;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveRGBDSensor::setDepthResolution(int width, int height)
{
    return false;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveRGBDSensor::getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp)
{
    //CD_DEBUG("\n");

    timeStamp->update( yarp::os::Time::now() );

    if(!rgb) //-- treat special no RGB case
    {
        yarp::sig::ImageOf<yarp::sig::PixelRgb> tmpImage;
        rgbImage.copy(tmpImage);
        return true;
    }

    if(!rgbSensorBasePtr->GetSensorData(rgbSensorDataPtr))
    {
        CD_DEBUG("RGB: GetSensorData fail...\n");
        return false;
    }

    std::vector<uint8_t> currentFrame = rgbSensorDataPtr->vimagedata;
    //CD_DEBUG("Vector size: %d\n",currentFrame.size()); // i.e. 480 * 640 * 3 = 921600;
    if(0 == currentFrame.size())
    {
        CD_DEBUG("RGB: 0 == currentFrame.size()...\n");
        return false;
    }

    yarp::sig::ImageOf<yarp::sig::PixelRgb> tmpImage;
    tmpImage.setExternal(currentFrame.data(),rgbWidth,rgbHeight);

    //-- Similar to YarpOpenraveGrabber IFrameGrabberImageImpl.cpp, make copy to avoid glitch.
    rgbImage.copy(tmpImage);

    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveRGBDSensor::getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp)
{
    //CD_DEBUG("\n");

    timeStamp->update( yarp::os::Time::now() );

    if(!depthSensorBasePtr->GetSensorData(depthSensorDataPtr))
    {
        CD_DEBUG("Depth: GetSensorData fail...\n");
        return false;
    }

    std::vector<OpenRAVE::Vector> sensorRanges = depthSensorDataPtr->ranges;

    if( (depthHeight == 0) || (depthWidth == 0) )
    {
        if (sensorRanges.size()==3072)
        {
            depthWidth  = 64;
            depthHeight = 48;
        }
        else if (sensorRanges.size()==12288)
        {
            depthWidth  = 128;
            depthHeight = 96;
        }
        else if (sensorRanges.size()==49152)
        {
            depthWidth  = 256;
            depthHeight = 192;
        }
        else if (sensorRanges.size()==307200)
        {
            depthWidth  = 640;
            depthHeight = 480;
        }
        else if (sensorRanges.size()==4)
        {
            depthWidth  = 2;
            depthHeight = 2;
        }
        else if (sensorRanges.size()==0)
        {
            CD_WARNING("sensorRanges.size()==0\n");
        }
        else
        {
            depthWidth  = sensorRanges.size();
            depthHeight = 1;
        }
    }

    depthImage.resize(depthWidth, depthHeight);

    for (int i_y = 0; i_y < depthImage.height(); ++i_y)
    {
        for (int i_x = 0; i_x < depthImage.width(); ++i_x)
        {
            OpenRAVE::Vector ranges = sensorRanges[i_y + (i_x * depthImage.height())];
            double distance = std::sqrt(ranges.lengthsqr3());
            depthImage(i_x, i_y) = distance;  // [meters]
        }
    }

    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveRGBDSensor::getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp *colorStamp, yarp::os::Stamp *depthStamp)
{
    bool ok = true;
    ok &= getRgbImage(colorFrame,colorStamp);
    ok &= getDepthImage(depthFrame,depthStamp);
    return ok;
}

// ----------------------------------------------------------------------------

yarp::dev::IRGBDSensor::RGBDSensor_status YarpOpenraveRGBDSensor::getSensorStatus()
{
    if ((!rgbReady)||(!depthReady))
        return RGBDSensor_status::RGBD_SENSOR_NOT_READY;

    return RGBDSensor_status::RGBD_SENSOR_OK_IN_USE;
}

// ----------------------------------------------------------------------------

}
