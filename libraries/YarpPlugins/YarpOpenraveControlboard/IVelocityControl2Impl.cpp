// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <ColorDebug.h>

// ------------------ IVelocity2 Related ----------------------------------------

bool roboticslab::YarpOpenraveControlboard::velocityMove(const int n_joint, const int *joints, const double *spds)
{
    CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
        ok &= velocityMove(joints[i],spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefVelocity(const int joint, double *vel)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefVelocities(double *vels)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefVelocities(const int n_joint, const int *joints, double *vels)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------
