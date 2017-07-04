cmake_minimum_required (VERSION 3.2)
project (ISEAFrame)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

CMAKE_POLICY(SET CMP0009 NEW)
set (ISEAFrameVERSION_MAJOR 2)
set (ISEAFrameVERSION_MINOR 0)
set (ISEAFramePATCH_LEVEL 2)
set (ISEAFrameNAME "PublicRelease")

option (USE_DEBUG_GDB "Create everything with debug symbols" OFF)
option (USE_UNITTEST "Create unittests for everything" OFF)
option (USE_STRING_AND_STREAMS "Are string and streams available" OFF)
option (USE_BUILD_BENCHMARKS "Create benchmarks" OFF)
option (BUILD_VISUALIZER "Build a quick visualizer executable" OFF)
option (BUILD_ELECTRICAL_SIMULATION "Build commandline tool for electrical Simulation" OFF )
option (BUILD_THERMAL_SIMULATION "Builds a commandline tool for thermal only simulation" OFF)
option (BUILD_THERMAL_ELECTRICAL_SIMULATION "Build a commandline tool for thermal-electrical simulation" OFF)
option (BUILD_SVG_EXPORT "Build Standalone SVG export" OFF )
option (BUILD_QUICKVERIFICATION "Build a quick test for electrical and thermal simulation" OFF )

SET(MATRIX_IMPLEMENTATION Eigen CACHE STRING "Choose matrix implementation: Armadillo, Sparse, Eigen, Symbolic")
SET_PROPERTY(CACHE MATRIX_IMPLEMENTATION PROPERTY STRINGS Armadillo Sparse Eigen Symbolic)
SET(CMAKE_BUILD_TYPE Debug CACHE STRING "default to debug")
SET(CMAKE_CXX_FLAGS "" FALSE CACHE STRING "CMAKE_CXX_FLAGS")
SET (ARCH_TYPE "" CACHE STRING "")

string(FIND ${CMAKE_SYSTEM_NAME} Linux LINUX_SYSTEM)

if (LINUX_SYSTEM GREATER -1)
    string(TIMESTAMP ISEAFrameDATE %Y-%m-%dT)
else (LINUX_SYSTEM GREATER -1)
    execute_process(COMMAND "date" OUTPUT_VARIABLE ISEAFrameDATE)
    STRING(REGEX REPLACE "(\r?\n)+$" "" ISEAFrameDATE "${ISEAFrameDATE}")
endif (LINUX_SYSTEM GREATER -1)

SET (OBJECT_LIB)

##### FUNCTION DEFINITIONS #####
if (MATRIX_IMPLEMENTATION STREQUAL "Armadillo" OR
        MATRIX_IMPLEMENTATION STREQUAL "Sparse" OR
        MATRIX_IMPLEMENTATION STREQUAL "Eigen")
else()
    MESSAGE(FATAL_ERROR "MATRIX_IMPLEMENTATION must be one of the following: Armadillo, Sparse, Eigen ")
endif()


string( TOLOWER "${CMAKE_BUILD_TYPE}" IS_RELEASE )
if (NOT IS_RELEASE STREQUAL "release")
    add_definitions(-D_DEBUG_)
endif (NOT IS_RELEASE STREQUAL "release")

## DEBUG, WARNING
if (NOT WIN32 OR MINGW )
    if (USE_DEBUG_GDB )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb" )
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra" )

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.9.0")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto" )
        endif(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.9.0")
    endif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

elseif (NOT MINGW AND NOT UNIX)
    if (USE_DEBUG_GDB )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DEBUG" )
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox /GF" )
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /MP /bigobj" )
endif ( NOT WIN32 OR MINGW )


SET( COMPILE_FEATURES cxx_alias_templates cxx_auto_type)


###### This should be checked after updating the framework extensions
include(FindSubversion)
find_package(Git)
find_package(MATIO REQUIRED)
INCLUDE_DIRECTORIES(${MATIO_INCLUDE_DIRS})

#### Check if we are in a git repo

SET(REP_VERSION 0)

if (GIT_FOUND)
    if (EXISTS ${PROJECT_SOURCE_DIR}/.git)
        execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse HEAD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE REPO_VERSION RESULT_VARIABLE RES)
        if ( RES)
            message(FATAL_ERROR "Git command failed with\n  ${REPO_VERSION} ")
        endif ( RES)
        SET(REPO_VERSION "GIT ${REPO_VERSION}" )
    endif (EXISTS ${PROJECT_SOURCE_DIR}/.git)
endif (GIT_FOUND)

if ( NOT REPO_VERSION )
    set( REPO_VERSION "Unversioned")
endif ( NOT REPO_VERSION )

string(REPLACE "\n"
    "" REPO_VERSION
    ${REPO_VERSION})
string(REPLACE "\r"
    "" REPO_VERSION
    ${REPO_VERSION})

set(CMAKE_LINK_LIBRARIES "")
SET(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} ${MATIO_LIBRARIES} )
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#SET PATH TO ARMADILLO AND EIGEN
if(MATRIX_IMPLEMENTATION STREQUAL "Armadillo"
        OR MATRIX_IMPLEMENTATION STREQUAL "Sparse"
        OR MATRIX_IMPLEMENTATION STREQUAL "Eigen")

    SET(PATH_TO_ARMADILLO "" CACHE PATH "Path to Additional Includes (e.g. armadillo, eigen,...)" )
    INCLUDE_DIRECTORIES(${PATH_TO_ARMADILLO}/include)
    LINK_DIRECTORIES(${PATH_TO_ARMADILLO}/lib)
endif(MATRIX_IMPLEMENTATION STREQUAL "Armadillo"
    OR MATRIX_IMPLEMENTATION STREQUAL "Sparse"
    OR MATRIX_IMPLEMENTATION STREQUAL "Eigen")


if(MATRIX_IMPLEMENTATION STREQUAL "Armadillo")
    add_definitions(-D_ARMADILLO_)
    set(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} armadillo)
elseif(MATRIX_IMPLEMENTATION STREQUAL "Sparse")
    add_definitions(-D_ARMADILLO_)
    add_definitions(-DSPARSE_MATRIX_FORMAT)
    set(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} armadillo)
elseif(MATRIX_IMPLEMENTATION STREQUAL "Eigen")
    add_definitions(-D_EIGEN_)
elseif(MATRIX_IMPLEMENTATION STREQUAL "Symbolic")
    add_definitions(-D_SYMBOLIC_)
    SET(USE_STRING_AND_STREAMS TRUE CACHE BOOL "needs to be cached")
    SET(USE_STRING_AND_STREAMS TRUE)
    UNSET(USE_BUILD_BENCHMARKS CACHE)
    UNSET(USE_BUILD_BENCHMARKS)
endif()

option (USE_BOOST_THREADS "Enable multithreading support" OFF)
option (USE_BOOST_MPI "Enable mpi support" OFF)
option (USE_ZLIB_COMPRESSION "Compress Data" OFF)

if(USE_ZLIB_COMPRESSION)
    SET(PATH_TO_ZLIB "/usr/lib/x86_64-linux-gnu" CACHE PATH "Path to ZLib" )
    add_definitions(-D USE_ZLIB)
    SET(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} boost_iostreams z)
    LINK_DIRECTORIES(${PATH_TO_ZLIB})
endif(USE_ZLIB_COMPRESSION)

# ADD ALL *.cpp to our project which are in src/ minus the UNINCLUDE
# directories
SET(UNINCLUDE ${UNINCLUDE}
    ${PROJECT_SOURCE_DIR}/src/unittest
    ${PROJECT_SOURCE_DIR}/src/thermal/executable
    ${PROJECT_SOURCE_DIR}/src/version/
    ${PROJECT_SOURCE_DIR}/src/exceptions/
    )
#${PROJECT_SOURCE_DIR}/src/threading/)

if(MATRIX_IMPLEMENTATION STREQUAL "Symbolic")
    SET(UNINCLUDE ${UNINCLUDE}
        ${PROJECT_SOURCE_DIR}/src/export/baseExport
        ${PROJECT_SOURCE_DIR}/src/export/esbVisualizer
        ${PROJECT_SOURCE_DIR}/src/export/spiceExport
        ${PROJECT_SOURCE_DIR}/src/factory/electricalEquationSystem/chooseElectricSystemType
        ${PROJECT_SOURCE_DIR}/src/system/constantstepdglsystemsolver
        ${PROJECT_SOURCE_DIR}/src/system/linAlg_sys
        ${PROJECT_SOURCE_DIR}/src/system/linearsystemsolver
        ${PROJECT_SOURCE_DIR}/src/system/system
        ${PROJECT_SOURCE_DIR}/src/system/systemSolver
        ${PROJECT_SOURCE_DIR}/src/system/variablestepdglsystemsolver
        ${PROJECT_SOURCE_DIR}/src/thermal/block_observer
        ${PROJECT_SOURCE_DIR}/src/observer/
        )
endif(MATRIX_IMPLEMENTATION STREQUAL "Symbolic")


FILE(GLOB_RECURSE TMP_SOURCE *.cpp)  #RELATIVE ${PROJECT_SOURCE_DIR}/src/*.cpp)
foreach(f ${TMP_SOURCE})

    #Checks if file is in "${PROJECT_SOURCE_DIR}/src/"
    string(FIND ${f} "${PROJECT_SOURCE_DIR}/src/" not_source_path REVERSE)
    if( NOT ${not_source_path} EQUAL -1)
        #if part of uninclude then drop it
        foreach(g ${UNINCLUDE})

            string(FIND ${f} ${g} tmp_found REVERSE)
            if ( NOT ${tmp_found} EQUAL -1)
                break()
            endif( NOT ${tmp_found} EQUAL -1)

        endforeach(g ${UNINCLUDE})

        if ( ${tmp_found} EQUAL -1 )
            #else add it to sources
            SET( SOURCE
                ${SOURCE}
                ${f}
                )
        endif ( ${tmp_found} EQUAL -1 )
    endif( NOT ${not_source_path} EQUAL -1)
endforeach(f ${TMP_SOURCE})

FILE(GLOB_RECURSE HEADERS *.h)
foreach(f ${TMP_SOURCE})

    #Checks if file is in "${PROJECT_SOURCE_DIR}/src/"
    string(FIND ${f} "${PROJECT_SOURCE_DIR}/src/" not_source_path REVERSE)
    if( NOT ${not_source_path} EQUAL -1)
        #if part of uninclude then drop it
        foreach(g ${UNINCLUDE})

            string(FIND ${f} ${g} tmp_found REVERSE)
            if ( NOT ${tmp_found} EQUAL -1)
                break()
            endif( NOT ${tmp_found} EQUAL -1)

        endforeach(g ${UNINCLUDE})

        if ( ${tmp_found} EQUAL -1 )
            #else add it to sources
            SET( HEADERS
                ${HEADERS}
                ${f}
                )
        endif ( ${tmp_found} EQUAL -1 )
    endif( NOT ${not_source_path} EQUAL -1)
endforeach(f ${TMP_SOURCE})


if (USE_BOOST_THREADS)
    add_definitions(-DBOOST_THREAD)
    add_definitions(-DBOOST_HAS_THREADS)
    SET(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} boost_system boost_thread)
endif(USE_BOOST_THREADS)

if (USE_BOOST_MPI)
    SET(PATH_TO_MPI "/usr/include/openmpi" CACHE PATH "Path to MPIHeaders" )
    INCLUDE_DIRECTORIES( ${PATH_TO_MPI} )

    add_definitions(-DBOOST_MPI)
    SET(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} boost_system boost_serialization boost_mpi mpi_cxx mpi)
endif(USE_BOOST_MPI)

if (USE_BOOST_THREADS OR
        USE_BOOST_MPI OR
        MATRIX_IMPLEMENTATION STREQUAL "Armadillo" OR
        MATRIX_IMPLEMENTATION STREQUAL "Sparse"
        )
    if ( NOT WIN32 )
        set(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} lapack blas)
    endif ( NOT WIN32 )
endif()

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (USE_STRING_AND_STREAMS)
    add_definitions(-D__EXCEPTIONS__)
else()
    add_definitions(-D__NO_STRING__)
endif (USE_STRING_AND_STREAMS)



FILE(GLOB_RECURSE SUBDIRECTORIES ${PROJECT_SOURCE_DIR}/src/*)

foreach(f ${SUBDIRECTORIES})
    GET_FILENAME_COMPONENT(dir_path ${f} PATH)
    STRING(REGEX REPLACE ${PROJECT_SOURCE_DIR}/src/ "" dir_path ${dir_path})
    STRING(REGEX REPLACE  "/"   "\\\\"  name ${dir_path} )
    SOURCE_GROUP(${name}  REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/src/${dir_path}/*)
endforeach(f ${SRCFILES})

find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile_release.in
        ${CMAKE_CURRENT_BINARY_DIR}/DoxygenRelease/Doxyfile @ONLY)
    add_custom_target(docRelease
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/DoxygenRelease/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/DoxygenRelease
        COMMENT "Generating API documentation with Doxygen for Release" VERBATIM
        )
endif(DOXYGEN_FOUND)

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    LINK_DIRECTORIES (${Boost_LIBRARY_DIRS})
    SET(Boost_USE_STATIC_LIBS On)
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF (Boost_FOUND)

###################### AFTER HERE NO MORE CXX FLAGS #############
SET(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
LIST(REMOVE_ITEM CMAKE_C_FLAGS -fno-rtti )

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/version)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/exceptions)

if (NOT WIN32 OR (WIN32 AND CMAKE_TOOLCHAIN_FILE))
    SET(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} Error)
    add_library(ISEAFrame ${SOURCE} ${HEADERS} ${OBJECT_LIB})
    target_link_libraries (ISEAFrame ${Boost_LIBRARIES} ${CMAKE_LINK_LIBRARIES} )
    target_compile_features(ISEAFrame PRIVATE ${COMPILE_FEATURES})
else (NOT WIN32)
    SET(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} libError)
    add_library(libISEAFrame ${SOURCE} ${HEADERS} ${OBJECT_LIB})

    target_link_libraries (libISEAFrame  ${Boost_LIBRARIES} ${CMAKE_LINK_LIBRARIES} libError )
    if ( MSVC )
        set_target_properties( libISEAFrame PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
        set_target_properties( libISEAFrame PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR} )
        set_target_properties( libISEAFrame PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR} )
        set_target_properties( libISEAFrame PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
        set_target_properties( libISEAFrame PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR} )
        set_target_properties( libISEAFrame PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR} )

        LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
    endif ( MSVC )

    target_compile_features(libISEAFrame PRIVATE ${COMPILE_FEATURES})
endif (NOT WIN32 OR (WIN32 AND CMAKE_TOOLCHAIN_FILE))


if (MSVC)
    SET(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} ${MATIO_LIBRARIES} libVersion )
    SET(ISEALIB libISEAFrame.lib)
    SET(ISEALIB_NAME libISEAFrame)
elseif(UNIX)
    SET(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} ${MATIO_LIBRARIES} Version )
    SET(ISEALIB ISEAFrame)
    SET(ISEALIB_NAME ISEAFrame)
else(  )
    SET(CMAKE_LINK_LIBRARIES ${CMAKE_LINK_LIBRARIES} ${MATIO_LIBRARIES} Version )
    SET(ISEALIB ISEAFrame)
    SET(ISEALIB_NAME ISEAFrame)
endif (MSVC)

if (USE_UNITTEST)
    ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/unittests)
    add_dependencies(unittest ${ISEALIB_NAME} )
endif(USE_UNITTEST)

if (BUILD_VISUALIZER)
    add_executable (QuickVisualize ${PROJECT_SOURCE_DIR}/standalone/QuickVisualizer.cpp)
    target_link_libraries (QuickVisualize ${CMAKE_LINK_LIBRARIES} ${ISEALIB})
    add_dependencies(QuickVisualize ${ISEALIB_NAME} )
    target_compile_features(QuickVisualize PRIVATE ${COMPILE_FEATURES})
endif (BUILD_VISUALIZER)

if (BUILD_SVG_EXPORT)
    add_executable (SVGExport ${PROJECT_SOURCE_DIR}/standalone/SVGExport/main.cpp)
    add_dependencies(SVGExport ${ISEALIB_NAME} )
    target_link_libraries (SVGExport ${CMAKE_LINK_LIBRARIES} ${ISEALIB})
    target_compile_features(SVGExport PRIVATE ${COMPILE_FEATURES})
endif (BUILD_SVG_EXPORT)

if ( BUILD_ELECTRICAL_SIMULATION )
    add_executable (electricalSimulation ${PROJECT_SOURCE_DIR}/standalone/electricalSimulation.cpp)
    add_dependencies(electricalSimulation ${ISEALIB_NAME} )
    target_link_libraries (electricalSimulation ${CMAKE_LINK_LIBRARIES} ${ISEALIB})
    target_compile_features(electricalSimulation PRIVATE ${COMPILE_FEATURES})
endif ( BUILD_ELECTRICAL_SIMULATION )

if (BUILD_THERMAL_SIMULATION)
    add_executable (thermalSimulation ${PROJECT_SOURCE_DIR}/standalone/thermalSimulation.cpp)
    add_dependencies(thermalSimulation ${ISEALIB_NAME} )
    target_link_libraries(thermalSimulation ${CMAKE_LINK_LIBRARIES} ${ISEALIB})
    target_compile_features(thermalSimulation PRIVATE ${COMPILE_FEATURES})
endif (BUILD_THERMAL_SIMULATION)

if (BUILD_THERMAL_ELECTRICAL_SIMULATION)
    add_executable (thermal_electrical_Simulation ${PROJECT_SOURCE_DIR}/standalone/thermal_electric_simulation.cpp)
    add_dependencies(thermal_electrical_Simulation ${ISEALIB_NAME} )
    target_link_libraries(thermal_electrical_Simulation ${CMAKE_LINK_LIBRARIES} ${ISEALIB})
    target_compile_features(thermal_electrical_Simulation PRIVATE ${COMPILE_FEATURES})
endif (BUILD_THERMAL_ELECTRICAL_SIMULATION)


if (BUILD_QUICKVERIFICATION)
    add_executable (verification ${PROJECT_SOURCE_DIR}/standalone/verification.cpp)
    add_dependencies(verification ${ISEALIB_NAME} )
    target_link_libraries(verification ${CMAKE_LINK_LIBRARIES} ${ISEALIB})
    target_compile_features(verification PRIVATE ${COMPILE_FEATURES})
endif (BUILD_QUICKVERIFICATION)



if (USE_BUILD_BENCHMARKS)
    add_executable (frameworkSingleThreadBenchmark ${PROJECT_SOURCE_DIR}/benchmark/frameworkBenchmark.cpp)
    add_dependencies(frameworkSingleThreadBenchmark ${ISEALIB_NAME} )
    target_link_libraries (frameworkSingleThreadBenchmark ${CMAKE_LINK_LIBRARIES} ${ISEALIB})

    target_compile_features(frameworkSingleThreadBenchmark PRIVATE ${COMPILE_FEATURES})

    add_executable (frameworkBenchmark_ThEl ${PROJECT_SOURCE_DIR}/benchmark/frameworkBenchmark_ThEl.cpp)
    add_dependencies(frameworkBenchmark_ThEl ${ISEALIB_NAME} )
    target_link_libraries (frameworkBenchmark_ThEl ${CMAKE_LINK_LIBRARIES} ${ISEALIB})

    target_compile_features(frameworkBenchmark_ThEl PRIVATE ${COMPILE_FEATURES})

    add_executable (benchmarkBuildThermal ${PROJECT_SOURCE_DIR}/benchmark/benchmarkBuildThermal.cpp)
    add_dependencies(benchmarkBuildThermal ${ISEALIB_NAME} )
    target_link_libraries (benchmarkBuildThermal ${CMAKE_LINK_LIBRARIES} ${ISEALIB})
    target_compile_features(benchmarkBuildThermal PRIVATE ${COMPILE_FEATURES})

    if (USE_BOOST_THREADS)
        add_executable (frameworkMultiThreadBenchmark ${PROJECT_SOURCE_DIR}/benchmark/frameworkBenchmark.cpp )
        add_dependencies(frameworkMultiThreadBenchmark ${ISEALIB_NAME} )
        set_target_properties(frameworkMultiThreadBenchmark PROPERTIES COMPILE_DEFINITIONS "USE_BOOST_THREADS")
        target_link_libraries (frameworkMultiThreadBenchmark ${CMAKE_LINK_LIBRARIES} ${ISEALIB} ${BOOST_THREAD_FOR_THERMAL})

        target_compile_features(frameworkMultiThreadBenchmark PRIVATE ${COMPILE_FEATURES})
    endif (USE_BOOST_THREADS)

    if (USE_BOOST_MPI)
        add_executable (frameworkMPIBenchmark ${PROJECT_SOURCE_DIR}/benchmark/frameworkBenchmark.cpp)
        add_dependencies(frameworkMPIBenchmark ${ISEALIB_NAME} )
        set_target_properties(frameworkMPIBenchmark PROPERTIES COMPILE_DEFINITIONS "USE_BOOST_MPI")
        target_link_libraries (frameworkMPIBenchmark ${CMAKE_LINK_LIBRARIES} ${ISEALIB} ${BOOST_THREAD_FOR_THERMAL})

        target_compile_features(frameworkMPIBenchmark PRIVATE ${COMPILE_FEATURES})
    endif (USE_BOOST_MPI)
endif(USE_BUILD_BENCHMARKS)
