/** 
 * @file
 * @author Alexander Sherikov
 * @brief Performs a full simulation with a dumb emulation of variable
 *  sampling time in the preview window: the differing period repeated
 *  in the preview window in a cycle.
 */


#include "tests_common.h" 

///@addtogroup gTEST
///@{
int main(int argc, char **argv)
{
    //-----------------------------------------------------------
    // initialize
    WMG wmg;
    init_01 (&wmg);
    //-----------------------------------------------------------
  

    test_start (argv[0]);

    smpc_solver solver(wmg.N);
    int nW;

    int j=0;
    for(;;)
    {
        wmg.T[(wmg.N-1) - j] = 0.05;
        if (j != 0)
        {
            wmg.T[(wmg.N-1) - j + 1] = 0.1;
            if (j == wmg.N-1)
            {
                j = 0;
            }
            else
            {
                j++;
            }
        }
        else
        {
            wmg.T[j] = 0.1;
            j++;
        }
        for (int i=0; i < wmg.N; i++)
        {
            cout << wmg.T[i] << "   ";
        }
        cout << endl;

        //------------------------------------------------------
        wmg.FormPreviewWindow();    
        if (wmg.halt)
        {
            cout << "EXIT (halt = 1)" << endl;
            break;
        }
        //------------------------------------------------------


        //------------------------------------------------------
        solver.init(wmg.T, wmg.h, wmg.angle, wmg.zref_x, wmg.zref_y, wmg.lb, wmg.ub, wmg.X_tilde, wmg.X);
        nW = solver.solve();
        solver.get_next_state_tilde (wmg.X_tilde);
        //------------------------------------------------------


        //------------------------------------------------------
        printf("Num. of activated constraints: %d\n", nW);
        for (int i = 0; i < 6; i++)
        {
            printf("value: % 8e\n", wmg.X[i]);
        }
        //------------------------------------------------------
    }

    test_end (argv[0]);
    return 0;
}
///@}
