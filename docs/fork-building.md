# Updates to 2023

## Building under Debian

patch

+ cd /usr/include/eigen3/Eigen/src/SparseCore
+ apply patch (e.g. patch SparseMatrix.h patches/eigen/eigen3_SparseMatrix_fork.patch)
+ mkdir -p build && cd build
+ cmake ../ \
-G "CodeBlocks - Unix Makefiles" \
-DUSE_STRING_AND_STREAMS=ON \
-DUSE_UNITTEST=OFF \
-DUSE_ZLIB_COMPRESSION=OFF \
-DUSE_BOOST_MPI=OFF \
-DUSE_BOOST_THREADS=OFF \
-DBUILD_DOT_EXPORT=ON \
-DBUILD_ELECTRICAL_SIMULATION=ON \
-DBUILD_QUICKVERIFICATION=ON \
-DBUILD_SVG_EXPORT=ON \
-DBUILD_THERMAL_ELECTRICAL_SIMUL=ON \
-DBUILD_THERMAL_SIMULATION=ON \
-DBUILD_VISUALIZER=ON \
-D CMAKE_CXX_FLAGS="-D__EXCEPTIONS__=1" # Code smell
+ ccmake ..
+ check options for compiling
+ make
+ make doc

## Quickstart

Provided you installed ImageMagick, this is how you'd create an SVG from a working circuit and display after a coversion:

`./SVGExport 2 ../scenarios/thermal_electrical/WIMA_2700F_ThEl.xml  | convert  -  circuit.png && display circuit.png`
