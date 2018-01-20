// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRGBDSensor.hpp"

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

    rgbSensorBasePtr->GetSensorData(rgbSensorDataPtr);

    rgbImage.setPixelCode(VOCAB_PIXEL_RGB);
    rgbImage.resize(rgbWidth,rgbHeight);

    /*yarp::sig::ImageOf<yarp::sig::PixelRgb> image;
    image.resize(rgbWidth,rgbHeight);
    yarp::sig::PixelRgb p;
    for (int i_x = 0; i_x < rgbWidth; ++i_x)
    {
        for (int i_y = 0; i_y < rgbHeight; ++i_y)
        {
            p.r = rgbSensorDataPtr->vimagedata[3*(i_x+(i_y*rgbWidth))];
            p.g = rgbSensorDataPtr->vimagedata[1+3*(i_x+(i_y*rgbWidth))];
            p.b = rgbSensorDataPtr->vimagedata[2+3*(i_x+(i_y*rgbWidth))];
            image.safePixel(i_x,i_y) = p;
        }
    }*/
    //rgbImage.copy(image);

    timeStamp->update( yarp::os::Time::now() );

    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveRGBDSensor::getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp)
{
    //CD_DEBUG("\n");

    depthSensorBasePtr->GetSensorData(depthSensorDataPtr);

    std::vector< OpenRAVE::RaveVector< OpenRAVE::dReal > > sensorRanges = depthSensorDataPtr->ranges;
    std::vector< OpenRAVE::RaveVector< OpenRAVE::dReal > > sensorPositions = depthSensorDataPtr->positions;

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
        else
        {
            depthWidth  = sensorRanges.size();
            depthHeight = 1;
        }
        //else CD_ERROR("unrecognized laser sensor data size.\n");

        tinv = depthSensorDataPtr->__trans.inverse();
    }

    depthImage.resize(depthWidth,depthHeight);

    for (int i_y = 0; i_y < depthImage.height(); ++i_y)
    {  // was y in x before
        for (int i_x = 0; i_x < depthImage.width(); ++i_x)
        {
            //double p = sensorRanges[i_y+(i_x*depthImage.height())].z;
            double p;
            if( sensorPositions.size() > 0 )
            {
                OpenRAVE::Vector v = tinv*(sensorRanges[i_y+(i_x*depthImage.height())] + sensorPositions[0]);
                p = (float)v.z;
            }
            else
            {
                OpenRAVE::Vector v = tinv*(sensorRanges[i_y+(i_x*depthImage.height())]);
                p = (float)v.z;
            }
            depthImage(i_x,i_y) = p*1000.0;  // give mm
            //*(yarp::sig::PixelFloat*)depthImage.getPixelAddress(0, 0) = 0.0;
        }
    }

    timeStamp->update( yarp::os::Time::now() );

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
    return yarp::dev::IRGBDSensor::RGBD_SENSOR_OK_IN_USE;
}

// ----------------------------------------------------------------------------

}
