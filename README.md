# ETTENTION - Electron Tomography Package

## OVERVIEW
Ettention is a software package for tomographic reconstruction in electron tomography. It implements the algorithms simultaneous algebraic reconstruction technique (SART), simultaneous iterative reconstruction technique (SIRT) and a generic, block-iterative reconstruction algorithm. Ettention is written in C++ and internally uses OpenCL to run on a large number of high-performance computing.

Follow link to our website for more information: http://www.ettention.org/

The software supports single tilt, parallel projections for the use with conventional transmission electron microscopy data. Support for a combined tilt- and focal series for use in high-angle annular dark field scanning transmission electron microscopy (HA-ADF STEM) is provided via plugin. Also via plugin comes support for X-Ray laminography reconstruction.

Precompiled binaries are provided in the download section. The software can be operated from the command line or via an integration in the eTomo graphical user interface of the IMOD software package.

## TL;DR
* Clone Repo
* Run create_solution.cmd
* Find ettention_cmake.sln MS Visual Studio solution file in build\ subdirectory and run
* If using *nix Then you know what to do without guide [CMake]

## LICENSING
Ettention is distributed in both binary and source code distributions under an LGPL license. Ettention features a plugin concept and all plugins provided via this website are also provided via a LGPL license. It is allowed to develop plugins under commercial, closed source licenses and provide them separately from this website.

## ALGORITHMS
Ettention supports a generic block-iterative reconstruction algorithm with configurable block size. The special cases simultaneous algebraic reconstruction technique (SART) and sequential iterative reconstruction technique (SIRT) are also included. Weighted back projection (WBP) and tilt-focal algebraic reconstruction technique (TF-ART) are included via plugins.

Ettention provides support for parallel and perspective forward- and backprojections. The STEM plugin provides support for parallel projections with limited depth of field.

A number of projection iteration schemes are provided, including sequential access, random access and severeal subdivision schemes.

## SUPPORTED PLATFORMS
Ettention supports Windows, RedHat Linux and Ubuntu Linux. We support only 64-bit operation systems because of the large memory requirements for high-resolution volumes, all 32-bit only files are removed from package.

Ettention requires an OpenCL backend. Supported OpenCL platforms including

* AMD GPU
* NVidia GPU (preferentially Tesla)
* Intel CPU, Intel Xeon Phi
