#include "WMG.h"

void init_01 (WMG *wmg)
{
    //-----------------------------------------------------------
    // initialize
    wmg->init(15, 0.1, 0.261);

    double d[4] = {0.09 , 0.025, 0.03, 0.075};
    wmg->AddFootstep(0.0, 0.05, 0.0, 3, 3, 1, d);

    double z = 5.0*M_PI/180.0;
    double step_x = 0.035;
    double step_y = 0.1;

    d[3] = 0.025;
    wmg->AddFootstep(0.0   , -step_y, 0.0 , 4,  4, -1, d);
    wmg->AddFootstep(step_x,  step_y, z);
    wmg->AddFootstep(step_x, -step_y, z);
    wmg->AddFootstep(step_x,  step_y, z);
    wmg->AddFootstep(step_x, -step_y, z);
    wmg->AddFootstep(step_x,  step_y, z);
    wmg->AddFootstep(step_x, -step_y, z);
    wmg->AddFootstep(step_x,  step_y, z);
    wmg->AddFootstep(step_x, -step_y, z);
    wmg->AddFootstep(step_x,  step_y, 0.0, 30, 30);
    wmg->AddFootstep(0.0   , -step_y, 0.0);

    //wmg.FS2file(); // output results for later use in Matlab/Octave
    //-----------------------------------------------------------
}


void init_02 (WMG *wmg)
{
    wmg->init(15, 0.1, 0.261);

    double d[4] = {0.09 , 0.025, 0.03, 0.075};
    wmg->AddFootstep(0.0, 0.05, 0.0, 3, 3, 1, d);
    
    double z = 5.0*M_PI/180.0;
    double step_x = 0.035;
    double step_y = 0.1;

    d[3] = 0.025;

    // use this for smaller feet (and we will have more active constraints)
    d[0] = 0.03; d[1] = 0.01; d[2] = 0.01; d[3] = 0.01;

    wmg->AddFootstep(0.0   , -step_y, 0.0 , 4,  4, -1, d);
    wmg->AddFootstep(step_x,  step_y, z);
    wmg->AddFootstep(step_x, -step_y, z);
    wmg->AddFootstep(step_x,  step_y, z);
    wmg->AddFootstep(step_x, -step_y, z);
    wmg->AddFootstep(step_x,  step_y, z);
    wmg->AddFootstep(step_x, -step_y, z);
    wmg->AddFootstep(step_x,  step_y, z);
    wmg->AddFootstep(step_x, -step_y, z);
    wmg->AddFootstep(step_x,  step_y, 0.0, 30, 30);
    wmg->AddFootstep(0.0   , -step_y, 0.0);
}
