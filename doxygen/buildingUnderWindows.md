Building the framework and running a simulation under windows {#buildingUnderWindows}
==================

This guide considers that external dependencies have been installed into the system. 

CMake
------
Call cmake and  set the correct path to the source code (root directory) and choose a build directory.

It is strongly recommended to use a clean subdirectory e.g. build or buildWindows as the build process will create a lot of files and directories. 

![Cmake](images/buildWindows/001_Cmake.png "CMake")


Configure
----------
Push configure and set the desired compiler. Be aware of the difference between a 32 Bit build an a 64 bit build.

![Cmake](images/buildWindows/002_Cmake_32_bit.png "32-Bit build")

Select compiler
--------------

After selecting the compiler a few error messages might pop up, due to unresolved dependencies. This is normal and in the next step a dependency is resolved.

![Cmake](images/buildWindows/003_error_message.png "Error message")

Unresolved dependencies
--------------
In this example the dependency for Matio is unresolved. 
The include directory and the library itself are not automatically found.

![Cmake](images/buildWindows/004_SetLibraries.png "Unresolved dependency")

Choose the correct include directory for the source files and also choose the correct library. Pay special attention to the architecture (32 or 64 bits).

![Cmake](images/buildWindows/004_SetLibraries_solved.png "Resolved dependency")


Useful build parameters
----------------------
After resolving the dependency, other useful parameters can be set. 
Take a look at the image below to see some useful paramters and their values.

![Default](images/buildWindows/005_SetUsefulDefaults.png "Useful default valus")

Finishing cmake configuration
----------------------------
Click 'Configure' and afterwards 'Generate'

![Configure](images/buildWindows/006_Configure.png "Configure")
![Generate](images/buildWindows/007_Generate.png "Generate")


Building the framework in Visual Studio
------------------------------------------
After generation a Visual Studio Project is created in the destination folder.
![Build](images/buildWindows/008_BuildFolder.png "Build folder")

Open it in Visual Studio, set the options either to release or debug.
![Release](images/buildWindows/009_SetRelease.png "Set release")

Create the project. It might take a while. 
![Release](images/buildWindows/010_CreateProject.png "Create project")

You should get a message that everything compiled correctly 
![Finished](images/buildWindows/011_Build.png "Build finished")

Copy missing libraries to the build directory
---------------------------------------------
In the build folder in the sub directory "Release" are the finished libraries and executables. 
For using the executables the missing .dll have to be copied into the folder as shown below.

![Copy dlls](images/buildWindows/012_CopyDependencies.png "Copy dlls")

Simulating
------------
One can now start a simulation from the commandline by providing a simulation scenrario and a current profile.
The Framework comes with some example scenarios as well as two verfication profiles.

The simulation can be invoked by
```bash
./electricalSimulation.exe ../../scenarios/electrical/kokam_13Ah_el.xml ../../src/scenariotest/scenarioData/data_kokam_13Ah_pulse_electrical.mat
```
![Simulation](images/buildWindows/013_ExecuteSimulation.png "Simulation")

After The simulation has finished the simulation results can be found withibn a matlab struct
![Simulation](images/buildWindows/014_SimulationResults.png "Simulation results")

![Results](images/buildWindows/015_SimulationResults.png "Final simulation results")









