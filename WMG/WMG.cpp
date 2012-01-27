/** 
 * @file
 * @author Alexander Sherikov
 * @date 27.09.2011 18:59:03 MSD
 */


#include <stdio.h>
#include <math.h> // cos, sin


#include "WMG.h"
#include "point2d.h"
#include "footstep.h"


/** \brief Default constructor. */
WMG::WMG()
{
    X = NULL;

    T = NULL;
    h = NULL;

    angle = NULL;
    zref_x = NULL;
    zref_y = NULL;
    fp_x = NULL;
    fp_y = NULL;
    lb = NULL;
    ub = NULL;

    A = NULL;
    B = NULL;
}


/** \brief Default destructor. */
WMG::~WMG()
{
    if (X != NULL)
    {
        delete [] X;
        X = NULL;
    }

    if (T != NULL)
    {
        delete T;
    }
    if (h != NULL)
    {
        delete h;
    }

    if (angle != NULL)
    {
        delete angle;
    }
    if (zref_x != NULL)
    {
        delete zref_x;
    }
    if (zref_y != NULL)
    {
        delete zref_y;
    }
    if (fp_x != NULL)
    {
        delete fp_x;
    }
    if (fp_y != NULL)
    {
        delete fp_y;
    }
    if (lb != NULL)
    {
        delete lb;
    }
    if (ub != NULL)
    {
        delete ub;
    }


    if (A != NULL)
    {
        delete A;
    }
    if (B != NULL)
    {
        delete B;
    }
}



/** \brief Initializes a WMG object.

    \param[in] _N Number of sampling times in a preview window
 */
void WMG::init(const int _N)
{
    current_step_number = 0;
    gravity = 9.81; // hard-coded

    N = _N;


    X = new double[SMPC_NUM_VAR*N];

    T = new double[N];
    h = new double[N];

    angle = new double[N];
    zref_x = new double[N];
    zref_y = new double[N];
    fp_x = new double[N];
    fp_y = new double[N];
    lb = new double[2*N];
    ub = new double[2*N];



    /// NAO constraint with safety margin.
    def_constraint[0] = 0.09;
    def_constraint[1] = 0.025;
    def_constraint[2] = 0.03;
    def_constraint[3] = 0.025;
    def_repeat_times = 4;

    def_ds_constraint[0] = 0.07;
    def_ds_constraint[1] = 0.025;
    def_ds_constraint[2] = 0.025;
    def_ds_constraint[3] = 0.025;
    def_ds_num = 0;
}


/** 
 * @brief Initializes a WMG object.
 *
 * @param[in] T_ Sampling time (for the moment it is assumed to be constant) [sec.]
 * @param[in] hCoM_ Height of the Center of Mass [meter]
 * @param[in] step_height_ step hight (for interpolation of feet movements)
 */
void WMG::init_param(
        const double T_, 
        const double hCoM_,
        const double step_height_)
{
    hCoM = hCoM_;      

    for (int i = 0; i < N; i++)
    {
        T[i] = T_;
        h[i] = hCoM/gravity;
    }

    step_height = step_height_;
}


/**
 * @brief Adds a footstep to FS; sets the default constraints, the total number of 
 * iterations and the number of iterations in single support.
 *
 * @param[in] x_relative x_relative X position [meter] relative to the previous footstep.
 * @param[in] y_relative y_relative Y position [meter] relative to the previous footstep.
 * @param[in] angle_relative angle_relative Angle [rad.] relative to the previous footstep.
 * @param[in] n_this Number of (preview window) iterations in the added step.
 * @param[in] n Total number of (preview window) iterations, i.e., nSS + nDS.
 * @param[in] d Vector of the PoS constraints (assumed to be [4 x 1]).
 * @param[in] type (optional) type of the footstep.
 *
 * @note Coordinates and angle are treated as absolute for the first step in the preview window.
 */
void WMG::AddFootstep(
        const double x_relative, 
        const double y_relative, 
        const double angle_relative, 
        const int n_this, 
        const int n, 
        const double *d, 
        const fs_type type)
{
    def_constraint[0] = d[0];
    def_constraint[1] = d[1];
    def_constraint[2] = d[2];
    def_constraint[3] = d[3];
    def_repeat_times = n_this;
    def_ds_num = n - n_this;
    AddFootstep(x_relative, y_relative, angle_relative, type);
}



/**
 * @brief Adds a footstep to FS; sets the default total number of iterations and the 
 * number of iterations in single support.
 *
 * @param[in] x_relative x_relative X position [meter] relative to the previous footstep.
 * @param[in] y_relative y_relative Y position [meter] relative to the previous footstep.
 * @param[in] angle_relative angle_relative Angle [rad.] relative to the previous footstep.
 * @param[in] n_this Number of (preview window) iterations in the added step.
 * @param[in] n Total number of (preview window) iterations, i.e., nSS + nDS.
 * @param[in] type (optional) type of the footstep.
 *
 * @note Coordinates and angle are treated as absolute for the first step in the preview window.
 * @note Default vector of the PoS constraints is used.
 */
void WMG::AddFootstep(
        const double x_relative, 
        const double y_relative, 
        const double angle_relative, 
        const int n_this, 
        const int n, 
        const fs_type type)
{
    def_repeat_times = n_this;
    def_ds_num = n - n_this;
    AddFootstep(x_relative, y_relative, angle_relative, type);
}



/**
 * @brief Adds a footstep to FS.
 *
 * @param[in] x_relative x_relative X position [meter] relative to the previous footstep.
 * @param[in] y_relative y_relative Y position [meter] relative to the previous footstep.
 * @param[in] angle_relative angle_relative Angle [rad.] relative to the previous footstep.
 * @param[in] type (optional) type of the footstep.
 *
 * @note Coordinates and angle are treated as absolute for the first step in the preview window.
 * @note Default vector of the PoS constraintsi, number of iterations in single support and
 * total number of iterations are used.
 */
void WMG::AddFootstep(
        const double x_relative, 
        const double y_relative, 
        const double angle_relative, 
        fs_type type)
{
    if (FS.size() == 0)
    {
        // this is the first ("virtual") step.
        if (type == FS_TYPE_AUTO)
        {
            type = FS_TYPE_DS;
        }

        Point2D zref_rel ((def_constraint[0] - def_constraint[2])/2, 0);
        Point2D zref_abs (
                x_relative + zref_rel.x*cos(angle_relative) + zref_rel.y*sin(angle_relative),
                y_relative - zref_rel.x*sin(angle_relative) + zref_rel.y*cos(angle_relative));

        FS.push_back(
                FootStep(
                    angle_relative, 
                    Point2D(x_relative, y_relative),
                    zref_abs,
                    def_repeat_times, 
                    type,
                    def_constraint));
    }
    else
    {
        // Angle and position of the previous step
        double prev_a = FS.back().angle;
        Point2D prev_p(FS.back());        

        // determine type of the step
        if (type == FS_TYPE_AUTO)
        {
            switch (FS.back().type)
            {
                case FS_TYPE_SS_L:
                    type = FS_TYPE_SS_R;
                    break;
                case FS_TYPE_SS_R:
                    type = FS_TYPE_SS_L;
                    break;
                case FS_TYPE_DS:
                default:
                    type = FS_TYPE_SS_R;
                    break;
            }
        }

        // Position of the next step
        Point2D next_p(prev_p);        
        next_p.x += FS.back().ca*x_relative - FS.back().sa*y_relative;
        next_p.y += FS.back().sa*x_relative + FS.back().ca*y_relative;

        double next_angle = prev_a + angle_relative;

        Point2D zref_rel ((def_constraint[0] - def_constraint[2])/2, 0);
        Point2D next_zref (
                next_p.x + zref_rel.x*cos(next_angle) + zref_rel.y*sin(next_angle),
                next_p.y - zref_rel.x*sin(next_angle) + zref_rel.y*cos(next_angle));

        // Add double support constraints that lie between the
        // newly added step and the previous step
        for (int i = 0; i < def_ds_num; i++)
        {
            double theta = (double) (i+1)/(def_ds_num + 1);
            double ds_a = prev_a + angle_relative * theta;

            Point2D position ((1-theta)*prev_p.x + theta*next_p.x, (1-theta)*prev_p.y + theta*next_p.y);
            if (i < def_ds_num/2)
            {
                FS.push_back(
                        FootStep(
                            ds_a, 
                            position,
                            FS.back().ZMPref,
                            1, 
                            FS_TYPE_DS,
                            def_ds_constraint));
            }
            else
            {
                FS.push_back(
                        FootStep(
                            ds_a, 
                            position,
                            next_zref,
                            1, 
                            FS_TYPE_DS,
                            def_ds_constraint));
            }
        }


        // add the new step
        FS.push_back(
                FootStep(
                    next_angle, 
                    next_p, 
                    next_zref,
                    def_repeat_times, 
                    type,
                    def_constraint));
    }    
}



/**
 * @brief Returns index of the next SS.
 *
 * @param[in] start_ind start search from this index.
 * @param[in] type search for a footstep of certain type,
 *                 by default (FS_TYPE_AUTO) both left and right
 *                 are searched.
 *
 * @return index of the next SS.
 */
int WMG::getNextSS(const int start_ind, const fs_type type)
{
    int index = start_ind + 1;
    for (; index < (int) FS.size(); index++) 
    {
        if (FS[index].type != FS_TYPE_DS)
        {
            if (type == FS_TYPE_AUTO)
            {
                break;
            }
            else
            {
                if (FS[index].type == type)
                {
                    break;
                }
            }
        }
    }
    return (index);
}



/**
 * @brief Returns index of the previous SS.
 *
 * @param[in] start_ind start search from this index.
 * @param[in] type search for a footstep of certain type,
 *                 by default (FS_TYPE_AUTO) both left and right
 *                 are searched.
 *
 * @return index of the previous SS.
 */
int WMG::getPrevSS(const int start_ind, const fs_type type)
{
    int index = start_ind - 1;
    for (; index >= 0; index--)
    {
        if (FS[index].type != FS_TYPE_DS)
        {
            if (type == FS_TYPE_AUTO)
            {
                break;
            }
            else
            {
                if (FS[index].type == type)
                {
                    break;
                }
            }
        }
    }
    return (index);
}




/**
 * @brief Determine position and orientation of feet
 *
 * @param[in] loops_per_preview_iter number of control loops per preview iteration
 * @param[in] loops_in_current_preview number of control loops passed in the current 
 *                                     preview iteration
 * @param[out] left_foot_pos 3x1 vector of coordinates [x y z] + angle (orientation in x,y plane)
 * @param[out] right_foot_pos 3x1 vector of coordinates [x y z] + angle (orientation in x,y plane)
 *
 * @attention This function requires the walking pattern to be started and finished
 * by single support.
 *
 * @attention Cannot be called on the first or last SS  =>  must be called after 
 * FormPreviewWindow().
 *
 * @note If loops_per_preview_iter is set to 1, then the function returns a position 
 * at the end of preview window with number loops_in_current_preview.
 */
void WMG::getFeetPositions (
        const int loops_per_preview_iter,
        const int loops_in_current_preview,
        double *left_foot_pos,
        double *right_foot_pos)
{
    if (FS[current_step_number].type == FS_TYPE_DS)
    {
        int left_ind, right_ind;

        left_ind = getNextSS (current_step_number);
        if (FS[left_ind].type == FS_TYPE_SS_L)
        {
            right_ind = getPrevSS (current_step_number);
        }
        else
        {
            right_ind = left_ind;
            left_ind = getPrevSS (current_step_number);
        }

        left_foot_pos[0] = FS[left_ind].x;
        left_foot_pos[1] = FS[left_ind].y;
        left_foot_pos[2] = 0.0;
        left_foot_pos[3] = FS[left_ind].angle;

        right_foot_pos[0] = FS[right_ind].x;
        right_foot_pos[1] = FS[right_ind].y;
        right_foot_pos[2] = 0.0;
        right_foot_pos[3] = FS[right_ind].angle;
    }
    else
    {
        double *swing_foot_pos, *ref_foot_pos;
        int next_swing_ind, prev_swing_ind;


        if (FS[current_step_number].type == FS_TYPE_SS_L)
        {
            ref_foot_pos = left_foot_pos;
            swing_foot_pos = right_foot_pos;

            prev_swing_ind = getPrevSS (current_step_number, FS_TYPE_SS_R);
            next_swing_ind = getNextSS (current_step_number, FS_TYPE_SS_R);
        }
        else
        {
            ref_foot_pos = right_foot_pos;
            swing_foot_pos = left_foot_pos;

            prev_swing_ind = getPrevSS (current_step_number, FS_TYPE_SS_L);
            next_swing_ind = getNextSS (current_step_number, FS_TYPE_SS_L);
        }

        ref_foot_pos[0] = FS[current_step_number].x;
        ref_foot_pos[1] = FS[current_step_number].y;
        ref_foot_pos[2] = 0.0;
        ref_foot_pos[3] = FS[current_step_number].angle;


        int num_iter_in_ss = FS[current_step_number].repeat_times;
        int num_iter_in_ss_passed = num_iter_in_ss - FS[current_step_number].repeat_counter;
        double theta =
            (double) (loops_per_preview_iter * num_iter_in_ss_passed + loops_in_current_preview) /
            (loops_per_preview_iter * num_iter_in_ss);


        double x[3] = {
            FS[prev_swing_ind].x,
            (FS[prev_swing_ind].x + FS[next_swing_ind].x)/2,
            FS[next_swing_ind].x};

        double b_coef = - (x[2]*x[2] - x[0]*x[0])/(x[2] - x[0]);
        double a = step_height / (x[1]*x[1] - x[0]*x[0] + b_coef*(x[1] - x[0]));
        double b = a * b_coef;
        double c = - a*x[0]*x[0] - b*x[0];


        swing_foot_pos[0] = (1-theta)*x[0] + theta*x[2]; // linear equation
        swing_foot_pos[1] = FS[next_swing_ind].y;
        swing_foot_pos[2] = a * swing_foot_pos[0] * swing_foot_pos[0] + b * swing_foot_pos[0] + c;
        swing_foot_pos[3] = FS[next_swing_ind].angle;
    }
}



/**
 * @brief Checks if the support foot switch is needed.
 *
 * @return true if the support foot must be switched. 
 */
bool WMG::isSupportSwitchNeeded ()
{
    // if we are not in the initial support
    if (current_step_number != 0) 
    {
        if (// we are in DS
            (FS[current_step_number].type == FS_TYPE_DS) &&
            // the previous footstep was not DS
            (FS[current_step_number-1].type != FS_TYPE_DS) &&
            // this is the middle of DS
            (FS[current_step_number].repeat_counter == FS[current_step_number].repeat_times%2))
        {
            return (true);
        }
        else if(// from left support to right
                ((FS[current_step_number].type == FS_TYPE_SS_L) && 
                (FS[current_step_number-1].type == FS_TYPE_SS_R)) ||
                // from right support to left
                ((FS[current_step_number].type == FS_TYPE_SS_R) && 
                (FS[current_step_number-1].type == FS_TYPE_SS_L)))
        {
            return (true);
        }
    }

    return (false);
}



/**
 * @brief Forms a preview window.
 *
 * @return WMG_OK or WMG_HALT (simulation must be stopped)
 */
WMGret WMG::formPreviewWindow()
{
    WMGret retval = WMG_OK;
    int win_step_num = current_step_number;
    int step_repeat_times = FS[win_step_num].repeat_counter;


    for (int i = 0; i < N;)
    {
        if (step_repeat_times > 0)
        {
            angle[i] = FS[win_step_num].angle;

            fp_x[i] = FS[win_step_num].x;
            fp_y[i] = FS[win_step_num].y;


            // ZMP reference coordinates
            zref_x[i] = FS[win_step_num].ZMPref.x;
            zref_y[i] = FS[win_step_num].ZMPref.y;


            lb[i*2] = -FS[win_step_num].d[2];
            ub[i*2] = FS[win_step_num].d[0];

            lb[i*2 + 1] = -FS[win_step_num].d[3];
            ub[i*2 + 1] = FS[win_step_num].d[1];

            step_repeat_times--;
            i++;
        }
        else
        {
            win_step_num++;
            if (win_step_num == (int) FS.size())
            {
                retval = WMG_HALT;
                break;
            }
            step_repeat_times = FS[win_step_num].repeat_counter;
        }
    }


    if (FS[current_step_number].repeat_counter == 0)
    {
        current_step_number++;
    }

    FS[current_step_number].repeat_counter--;
    if (FS[current_step_number].repeat_counter == 0)
    {
        current_step_number++;
    }

    return (retval);
}



/**
 * @brief Initialize state (#A) and control (#B) matrices for inverted 
 * pendulum model.
 *
 * @param[in] sampling_time period of time T.
 */
void WMG::initABMatrices (const double sampling_time)
{
    A = new double[9];
    B = new double[3];

    A[0] = A[4] = A[8] = 1;
    A[1] = A[2] = A[5] = 0;
    A[3] = A[7] = sampling_time;
    A[6] = sampling_time * sampling_time/2;

    B[0] = sampling_time * sampling_time * sampling_time / 6;
    B[1] = sampling_time * sampling_time/2;
    B[2] = sampling_time;
}



/**
 * @brief Calculate next state using inverted pendulum model (#A and #B matrices).
 *
 * @param[in] control 1x2 vector of controls
 * @param[in,out] state 1x6 state vector
 *
 * @attention If #A or #B are not initialized, the function does nothing.
 */
void WMG::calculateNextState (smpc::control &control, smpc::state_orig &state)
{
    if ((A == NULL) || (B == NULL))
    {
        return;
    }


    state.x()  = state.x()  * A[0]
               + state.vx() * A[3]
               + state.ax() * A[6]
               + control.jx() * B[0];

    state.vx() = state.vx() * A[4]
               + state.ax() * A[7]
               + control.jx() * B[1];

    state.ax() = state.ax() * A[8]
               + control.jx() * B[2];


    state.y()  = state.y()  * A[0]
               + state.vy() * A[3]
               + state.ay() * A[6]
               + control.jy() * B[0];

    state.vy() = state.vy() * A[4]
               + state.ay() * A[7]
               + control.jy() * B[1];

    state.ay() = state.ay() * A[8]
               + control.jy() * B[2];
}



/**
 * @brief Outputs the footsteps in FS to a file, that can be executed in
 * Matlab/Octave to get a figure of the steps.
 *
 * @param[in] filename output file name.
 * @param[in] plot_ds enable/disable plotting of double supports
 */
void WMG::FS2file(const std::string filename, const bool plot_ds)
{
    
    FILE *file_op = fopen(filename.c_str(), "w");
    
    if(!file_op)
    {
        fprintf(stderr, "Cannot open file (for writing)\n");
        return;
    }
    
    fprintf(file_op,"%%\n%% Footsteps generated using the c++ version of the WMG\n%%\n\n");
    fprintf(file_op,"cla;\n");
    fprintf(file_op,"clear FS;\n\n");
    
    int i;
    for (i=0; i< (int) FS.size(); i++ )
    {
        if ((plot_ds) || (FS[i].type != FS_TYPE_DS))
        {
            fprintf(file_op, "FS(%i).a = %f;\nFS(%i).p = [%f;%f];\nFS(%i).d = [%f;%f;%f;%f];\n", 
                    i+1, FS[i].angle, 
                    i+1, FS[i].x, FS[i].y, 
                    i+1, FS[i].d[0], FS[i].d[1], FS[i].d[2], FS[i].d[3]);

            fprintf(file_op, "FS(%i).D = [%f %f;%f %f;%f %f;%f %f];\n", 
                    i+1, FS[i].D[0], FS[i].D[4],
                         FS[i].D[1], FS[i].D[5],
                         FS[i].D[2], FS[i].D[6],
                         FS[i].D[3], FS[i].D[7]); 

            fprintf(file_op, "FS(%i).v = [%f %f; %f %f; %f %f; %f %f; %f %f];\n", 
                    i+1, FS[i].vert[0].x, FS[i].vert[0].y, 
                         FS[i].vert[1].x, FS[i].vert[1].y, 
                         FS[i].vert[2].x, FS[i].vert[2].y, 
                         FS[i].vert[3].x, FS[i].vert[3].y, 
                         FS[i].vert[0].x, FS[i].vert[0].y);

            if (FS[i].type == FS_TYPE_DS)
            {
                fprintf(file_op, "FS(%i).type = 1;\n\n", i+1);
            }
            if ((FS[i].type == FS_TYPE_SS_L) || (FS[i].type == FS_TYPE_SS_R))
            {
                fprintf(file_op, "FS(%i).type = 2;\n\n", i+1);
            }
        }
    }

    fprintf(file_op,"hold on\n");    
    fprintf(file_op,"for i=1:length(FS)\n");
    fprintf(file_op,"    if FS(i).type == 1;\n");
    fprintf(file_op,"        plot (FS(i).p(1),FS(i).p(2),'gs','MarkerFaceColor','r','MarkerSize',2)\n");
    fprintf(file_op,"        plot (FS(i).v(:,1), FS(i).v(:,2), 'c');\n");
    fprintf(file_op,"    end\n");
    fprintf(file_op,"    if FS(i).type == 2;\n");
    fprintf(file_op,"        plot (FS(i).p(1),FS(i).p(2),'gs','MarkerFaceColor','g','MarkerSize',4)\n");
    fprintf(file_op,"        plot (FS(i).v(:,1), FS(i).v(:,2), 'r');\n");
    fprintf(file_op,"    end\n");
    fprintf(file_op,"end\n");
    fprintf(file_op,"grid on; %%axis equal\n");
    fclose(file_op);  
}


/**
 * @brief Return coordinates of footstep reference points and rotation 
 * angles of footsteps (only for SS).
 *
 * @param[out] x_coord x coordinates
 * @param[out] y_coord y coordinates
 * @param[out] angle_rot angles
 */
void WMG::getFootsteps(
        std::vector<double> & x_coord,
        std::vector<double> & y_coord,
        std::vector<double> & angle_rot)
{
    for (unsigned int i = 0; i < FS.size(); i++)
    {
        if ((FS[i].type == FS_TYPE_SS_L) || (FS[i].type == FS_TYPE_SS_R))
        {
            x_coord.push_back(FS[i].x);
            y_coord.push_back(FS[i].y);
            angle_rot.push_back(FS[i].angle);
        }
    }
}
