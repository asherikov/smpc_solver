## Overview

This package includes two small libraries. The main library is 
located in 'solver' subfolder, it implements a solver of quadratic 
optimization problems defined using sparse model predictive control 
formulation for walking motion generation. The second library ('WMG' 
subfolder) is necessary to perform walking simulation, it produces 
input for the solver on each simulation step.

Tests and demos are placed in 'test' subfolder.


## Requirements:

* GNU make
* Eigen (for WMG)


## The libraries can be compiled in two ways:
1. (Recommended) Using cmake:
    * Type one of the following commands

    >    `make cmake`

            or 

    >    `cd build; cmake .. -DCMAKE_BUILD_TYPE=RELEASE; make`

    * Toolchain can be specified in the folowing way:

    >    `make cmake TOOLCHAIN=< path to a toolchain >`

2. (Does not work on MAC) Using only GNU make:
    * "`make`" to compile libraries
    * "`make test`" to compile libraries and tests


## Further information
        
The newest sources are available on GitHub:

>    <https://github.com/asherikov/smpc_solver/tree/Version_1.x>

The documentation generated by doxygen can be found here:

>    <http://asherikov.github.com/smpc_solver/v1/>
