/**
 * @file
 * @author Alexander Sherikov
 * @date 19.07.2011 15:56:18 MSD
 */


#ifndef QP_SOLVER_H
#define QP_SOLVER_H

/****************************************
 * INCLUDES 
 ****************************************/
#include "smpc_common.h"

#include <vector>


/****************************************
 * Defines
 ****************************************/


using namespace std;

/// @addtogroup gINTERNALS
/// @{
/** 
 * @brief Solve a quadratic program with a specific structure. 
 */
class qp_solver
{
    public:
// functions        
        qp_solver(
                const int N_, 
                const double Alpha = 150.0, 
                const double Beta = 2000.0, 
                const double Gamma = 1.0,
                const double regularization = 0.01,
                const double tol_ = 1e-7);

        virtual ~qp_solver()
        {
            if (dX  != NULL)
                delete dX;
        };

        void set_state_parameters(
                const double*, 
                const double*, 
                const double*);

        virtual void set_parameters(
                const double*,
                const double*,
                const double*,
                const double*,
                const double*,
                const double*,
                const double*) {};


        void form_init_fp(const double *, const double *, const double *, double *);
   

// variables
        /// Parameters, which are fed to the methods of #chol_solve class.
        solver_parameters sol_param;

        /** Variables for the QP (contain the states + control variables).
            Initial feasible point with respect to the equality and inequality 
            constraints. */
        double *X;


        /** Number of iterations in a preview window. */
        int N;


    ///@{
    /// Gains used in @ref pPDObj "objective function".
        double gain_alpha;
        double gain_beta;
        double gain_gamma;
    ///@}

        /// tolerance
        double tol;

    // variables and descent direction
     
        /** Feasible descent direction (to be used for updating #X). */
        double *dX;

        /** A number from 0 to 1, which controls depth of descent #X = #X + #alpha*#dX. */
        double alpha;
};

///@}
#endif /*QP_SOLVER_H*/
