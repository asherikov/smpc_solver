/** 
 * @file
 * @author Alexander Sherikov
 * @brief Comparison of IP and AS.
 */


#include <sys/time.h>
#include <time.h>

#include "tests_common.h" 

///@addtogroup gTEST
///@{

int main(int argc, char **argv)
{
    struct timeval start, end;
    double CurrentCPUTime;
    int NN = 1000;

    ofstream AS_log;
    ofstream IP_log;

    // Circular
    /*
    init_11 AS_test("test_13_as");
    init_11 IP_test("test_13_ip");
    */
    // Straight
    init_10 AS_test("test_13_as");
    init_10 IP_test("test_13_ip");
    
    //-----------------------------------------------------------

    AS_log.open(AS_test.fs_out_filename.c_str(), fstream::app);
    AS_log.precision (numeric_limits<double>::digits10);
    AS_log << endl << endl;
    AS_log << "CoM_ZMP = [";
    IP_log.open(IP_test.fs_out_filename.c_str(), fstream::app);
    IP_log.precision (numeric_limits<double>::digits10);
    IP_log << endl << endl;
    IP_log << "CoM_ZMP = [";



    smpc::solver_ip IP_solver(
            IP_test.wmg->N,          // N
            8000,                    // gain_position
            1.0,                     // gain_velocity
            0.02,                    // gain_acceleration
            1.0,                     // gain_jerk
            1e-3,                    // tol
            1e-2,                    // tol_out | if it is big only one iteration is made.
            1e-1,                    // t
            10,                     // mu
            0.01,                    // bs_alpha
            0.95,                     // bs_beta
            10,                       // max_iter
            smpc::SMPC_IP_BS_LOGBAR, // backtracking search
            true);                   // obj


    smpc::solver_as AS_solver(
            AS_test.wmg->N, // size of the preview window
            8000.0,         // gain_position
            1.0,            // gain_velocity
            0.02,           // gain_acceleration
            1.0,            // gain_jerk
            1e-7,           // tolerance
            0,              // no limit on the number of activated constraints
            true,           // enable constraint removal
            true);          // obj



    for(int counter = 0; /*counter < 10*/; counter++)
    {
        //------------------------------------------------------
        if (IP_test.wmg->formPreviewWindow(*IP_test.par) == WMG_HALT)
        {
            cout << "EXIT (halt = 1)" << endl;
            break;
        }
        if (AS_test.wmg->formPreviewWindow(*AS_test.par) == WMG_HALT)
        {
            cout << "EXIT (halt = 1)" << endl;
            break;
        }
        //------------------------------------------------------


        gettimeofday(&start,0);
        for(int kk=0; kk<NN ;kk++)
        {
            IP_solver.set_parameters (IP_test.par->T, IP_test.par->h, IP_test.par->h0, IP_test.par->angle, IP_test.par->zref_x, IP_test.par->zref_y, IP_test.par->lb, IP_test.par->ub);
            IP_solver.form_init_fp (IP_test.par->fp_x, IP_test.par->fp_y, IP_test.par->init_state, IP_test.par->X);
            IP_solver.solve();
        }
        IP_solver.get_next_state(IP_test.par->init_state);
        gettimeofday(&end,0);             
        CurrentCPUTime = end.tv_sec - start.tv_sec + 0.000001 * (end.tv_usec - start.tv_usec);
        printf("(%3i)  IP: time = % f (ext = %d // int = %d // bs = %d)\n", 
                counter, 
                CurrentCPUTime/NN, 
                IP_solver.ext_loop_iterations,
                IP_solver.int_loop_iterations,
                IP_solver.bt_search_iterations);
        printf ("           OBJ = ");
        for (unsigned int i = 0; i < IP_solver.objective_log.size(); ++i)
        {
            printf ("% 8e ", IP_solver.objective_log[i]);
        }
        printf ("\n");
        IP_solver.get_next_state(IP_test.X_tilde);
        IP_log << endl << IP_test.par->init_state.x() << " " << IP_test.par->init_state.y() << " " << IP_test.X_tilde.x() << " " << IP_test.X_tilde.y() << ";";


        gettimeofday(&start,0);
        for(int kk=0; kk<NN ;kk++)
        {
            AS_solver.set_parameters (AS_test.par->T, AS_test.par->h, AS_test.par->h0, AS_test.par->angle, AS_test.par->zref_x, AS_test.par->zref_y, AS_test.par->lb, AS_test.par->ub);
            AS_solver.form_init_fp (AS_test.par->fp_x, AS_test.par->fp_y, AS_test.par->init_state, AS_test.par->X);
            AS_solver.solve();
        }
        AS_solver.get_next_state(AS_test.par->init_state);
        gettimeofday(&end,0);             
        CurrentCPUTime = end.tv_sec - start.tv_sec + 0.000001 * (end.tv_usec - start.tv_usec);
        printf("       AS: time = % f (AS size = %i)\n", CurrentCPUTime/NN, AS_solver.active_set_size);
        printf("           OBJ = ");
        for (unsigned int i = 0; i < AS_solver.objective_log.size(); ++i)
        {
            printf ("% 8e ", AS_solver.objective_log[i]);
        }
        printf ("\n ---\n");
        AS_solver.get_next_state(AS_test.X_tilde);
        AS_log << endl << AS_test.par->init_state.x() << " " << AS_test.par->init_state.y() << " " << AS_test.X_tilde.x() << " " << AS_test.X_tilde.y() << ";";
        //------------------------------------------------------
    }

    IP_log << "];" << endl;
    IP_log << "plot (CoM_ZMP(:,1), CoM_ZMP(:,2), 'b');" << endl;
    IP_log << "plot (CoM_ZMP(:,3), CoM_ZMP(:,4), 'ks','MarkerSize',5);" << endl;
    IP_log.close();

    AS_log << "];" << endl;
    AS_log << "plot (CoM_ZMP(:,1), CoM_ZMP(:,2), 'b');" << endl;
    AS_log << "plot (CoM_ZMP(:,3), CoM_ZMP(:,4), 'ks','MarkerSize',5);" << endl;
    AS_log.close();

    return 0;
}
///@}
