/**
 * @file
 * @brief This file contains only doxygen definitions and is not to be 
 *  included anywhere.
 *
 * @author Alexander Sherikov
 * @date 15.09.2011 13:24:49 MSD
 *
 *
 * @todo (low priority) Interface with Matlab/Octave.
 * @todo Error processing. Exceptions?
 * @todo Downdate complexity
 */


#ifndef DOXYGEN_H
#define DOXYGEN_H

/**
 * @mainpage A sparse MPC solver for walking motion generation.
 *
 * @par Contents & links
 * - @ref MainIntro
 * - @ref MainLicense
 * - @ref MainOverview
 * - @ref MainHowTo
 * - @ref MainFormulas
 * - @ref MainSrcDocs
 * - @ref MainNotes
 * - @ref MainRef
 * @par
 * - <a href="http://github.com/asherikov/smpc_solver/">Sources on GitHub</a>
 * - <a href="./v1/index.html">Old version of the solver</a>
 * - <a href="http://asherikov.github.io/Projects/naowalk.html">A walking module for Nao using this solver</a>
 *
 * \n
 *
 *
 * @section MainIntro Introduction
 * @verbinclude "README.md"
 * \n
 *
 *
 * @section MainLicense License
 * @verbinclude "LICENSE"
 * \n
 *
 *
 * @section MainOverview Purpose of the library
 * Walk of a robot can be controlled using the following scheme:
 * - (1) determine a desired position of the center of mass (CoM);
 * - (2) determine desired positions of the end-effectors (feet);
 * - (3) using the model of the robot compute necessary changes of joint positions.
 *
 * The smpc_solver library addresses the first step. The position of the CoM is
 * determined using MPC scheme, where it is modeled using inverted pendulum in 3 
 * dimensions. MPC implies, that we have to solve an optimization problem. The 
 * objective function is quadratic, additional constraints are imposed by position 
 * of the support foot/feet: we need to satisfy certain requrements to
 * prevent robot from falling.
 *
 * The WMG library contains functions, which are necessary to define footsteps, 
 * prepare parameters for the solver library, determine positions of the feet.
 *
 * Terms and abbreviations:
 * - CoM -- center of mass.
 * - ZMP -- zero moment point.
 * - MPC -- model predictive control.
 * - SMPC -- sparse model predictive control, this term is introduced by us.
 * - Support foot -- the foot, on which a robot is standing.
 * - Single support -- a situation, when a robot stands on only one foot.
 * - Double support -- a situation, when a robot stands on both feet.
 * - Reference foot -- even when a robot is in double support, we use one foot 
 *   as the reference.
 * - Reference ZMP coordinates -- the objective function contains term, that
 *   tries to minimize the difference between the solution and reference points.
 *   This is allows tuning of the solution.
 * 
 * For more information refer to the papers listed in '@ref MainRef'. Also, the
 * '@ref pProblemDef' section contains more detailed explanations.
 * \n\n
 *
 *
 * @section MainHowTo API & examples
 * @par
 * - @ref gAPI 
 * - @ref gWMG_API
 * - @ref pDemo "A simple demo"
 *
 * \n
 *
 *
 * @section MainFormulas Derivations and algorithms
@anchor pIP
@anchor pIPSMPC
@anchor pIPSchur
@anchor pIPImplementation
@anchor pIPChol
@anchor pAddIC
@anchor pCholUp
@anchor pCholUpAlg
@anchor pAddICz
@anchor pg
@anchor ph
@anchor pz
@anchor pRemoveIC
@anchor pRemoveICz
@anchor pDetails
@anchor pBounds
@anchor pCholDown
@anchor pX_bar
@anchor pProblemDef
@anchor pPDModel
@anchor pPDVarSub
@anchor pX_tilde
@anchor pPDObj
@anchor pGains
@anchor pPD_EC
@anchor pPD_IC
@anchor pKKT
@anchor pInitGuess
@anchor pSchurComplement
@anchor pCholesky
 * Refer to the masters thesis available on the project 
 * <a href="http://asherikov.github.io/Projects/naowalk.html">web-page</a> 
 * for detailed description.
 *
 * \n
 *
 * @section MainSrcDocs Internal implementation of the libraries
 * @par
 * - @ref gINTERNALS
 *      - @ref gAS
 * - @ref gWMG_INTERNALS
 *
 * \n
 *
 * @section MainNotes Important notes
 * @todo The library is not thoroughly tested with variable height of CoM.
 *
 * \n
 *
 * @section MainRef References
 *
 * Dimitar Nikolaev Dimitrov, Alexander Sherikov, and Pierre-Brice Wieber\n
 * <a href="http://www.aass.oru.se/Research/Learning/drdv_dir/publications/iros11/iros11_1.html">
 * A sparse model predictive control formulation for walking motion generation</a>\n
 * IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS),\n
 * September 25-30, 2011, San Francisco, California
 */


/**
 * @defgroup gAPI API of the solver library
 *  
 * @defgroup gINTERNALS Internal classes, functions and definitions of the solver library
 *
 * @defgroup gAS Active set method
 * @ingroup gINTERNALS
 *
 * @defgroup gWMG_API API of the simulation support library
 *
 * @defgroup gWMG_INTERNALS Internal classes, functions and definitions of the simulation support library.
 */


/**
 * @page pDemo A simple demo
 * @dontinclude "test/demo.cpp"
 *
 * @section pDemo1 Include headers:
 * @until -------
 *
 * @section pDemo2 Define footsteps:
 * @until -------
 *
 * @section pDemo3 Setup solver:
 * @until -------
 * The gains are defined @ref pGains "here". The default values of optional
 * parameters can be found here: smpc#solver_as#solver_as.
 *
 * @section pDemo4 Start simulation:
 * @until -------
 *
 * @section pDemo5 Solve QP on each iteration of simulation:
 *
 * @subsection pDemo51 Initialize QP
 * @until -------
 * See also smpc#solver_as#set_parameters. Note, that the current state must be given
 * after @ref pX_tilde "first variable substitution".
 *
 * @subsection pDemo52 Solve QP
 * @until -------
 * See also smpc#solver#solve.
 *
 * @subsection pDemo53 Obtain the next state
 * @until -------
 * See also smpc#solver#get_next_state.
 */
#endif /*DOXYGEN_H*/
 
