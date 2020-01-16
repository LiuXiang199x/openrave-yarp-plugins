// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OYW_PORT_READER_HPP__
#define __OYW_PORT_READER_HPP__

#include <yarp/os/Bottle.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/Vocab.h>

#include <openrave/openrave.h>

/**
 * @ingroup OpenraveYarpWorld
 * @brief Processes RPC requests.
 */
class OywPortReader : public yarp::os::PortReader
{
public:

    void setEnvironment(OpenRAVE::EnvironmentBasePtr value) { pEnv = value; }
    void setRobot(OpenRAVE::RobotBasePtr value) { pRobot = value; }
    void setRobotManip(OpenRAVE::RobotBase::ManipulatorPtr value) { pRobotManip = value; }

private:
    // -- variables
    int robotDraw;
    double drawRadius, drawR, drawG, drawB;

    // box/sbox/cyl/scyl/sph/ssph
    std::vector<OpenRAVE::KinBodyPtr> boxKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> sboxKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> cylKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> scylKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> sphKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> ssphKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> meshKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> objKinBodyPtrs;

    OpenRAVE::EnvironmentBasePtr pEnv;  // set in setEnvironment
    OpenRAVE::RobotBasePtr pRobot;  // set in setRobot
    OpenRAVE::RobotBase::ManipulatorPtr pRobotManip;  // set in setRobot

    // Implement the actual responder (callback on RPC).
    virtual bool read(yarp::os::ConnectionReader& in);

    bool checkIfString(yarp::os::Bottle& request, int index, yarp::os::Bottle& response);

    static const yarp::conf::vocab32_t VOCAB_OK;
    static const yarp::conf::vocab32_t VOCAB_FAILED;
};

#endif // __OYW_PORT_READER_HPP__
