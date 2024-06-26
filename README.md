# This is the FastChem Cond fork used in the mobile app PHREEQC plus

## Changes in this fork

* fastchem_get_set.cpp::
options.element_density_minlimit = std::pow(10.0, value); changed to options.element_density_minlimit = value;

options.molecule_density_minlimit = std::pow(10.0, value); changed to options.molecule_density_minlimit = value;

* read_files.cpp::
commented line abundance = std::pow(10., abundance - 12.);

* options.h::
bool cond_use_data_validity_limits = true; changed to bool cond_use_data_validity_limits = false;

* chemical_elements.dat::
added D and heavy elements

* chemical_element_data.h::
added D and heavy elements

## Compilation

* arm

```bash
$ export CC=/path/to/armv7a-linux-androideabi33-clang
$ export CXX=/path/to/armv7a-linux-androideabi33-clang++
$ export CXXFLAGS="-fPIC"
$ export LDFLAGS="-fPIC -static-libstdc++"
$ cd FastChem
$ mkdir build
$ cd build
$ cmake ..
$ make
```

* aarch64
```bash
$ export CC=/path/to/aarch64-linux-android33-clang
$ export CXX=/path/to/aarch64-linux-android33-clang++
$ export CXXFLAGS="-fPIC"
$ export LDFLAGS="-fPIC -static-libstdc++"
$ cd FastChem
$ mkdir build
$ cd build
$ cmake ..
$ make
```

* x86
```bash
$ export CC=/path/to/i686-linux-android33-clang
$ export CXX=/path/to/i686-linux-android33-clang++
$ export CXXFLAGS="-fPIC"
$ export LDFLAGS="-fPIC -static-libstdc++"
$ cd FastChem
$ mkdir build
$ cd build
$ cmake ..
$ make
```

* x86_64
```bash
$ export CC=/path/to/x86_64-linux-android33-clang
$ export CXX=/path/to/x86_64-linux-android33-clang++
$ export CXXFLAGS="-fPIC"
$ export LDFLAGS="-fPIC -static-libstdc++"
$ cd FastChem
$ mkdir build
$ cd build
$ cmake ..
$ make
```

# ORIGINAL DESCRIPTION:

# FastChem Cond (FastChem 3.1) #
#### Authors: Daniel Kitzmann, Joachim Stock ####

# Overview #

FastChem is an equilibrium chemistry code that calculates the chemical composition of the gas and condensed phase for given temperatures and pressures. The calculation of the gas phase is based on a semi-analytic approach, described in detail in Stock et al. (2018) and Stock et al. (2022). The new version 3 version of FastChem, called FastChem Cond, adds condensation to the code. It can now compute the chemical composition using equilibrium condensation or the rainout approximation that is commonly used in the field of exoplanets or brown dwarfs. FastChem Cond is described in detail in Kitzmann, Stock & Patzer (2023).

The code is written in object-oriented C++, including template programming that allows the model to run with either double or long double precision. The exact computational precision of long double depends on your compiler and operating system. Long double precision usually allows the model to properly converge for very low temperatures. FastChem has been tested for temperatures as low as 100 K. For many cases, we were also able to obtain converged results for temperatures well below 100 K. Overall, the model has been successfully tested for temperatures from 100 K to 6000 K and pressures from 1e-13 bar to 1000 bar for solar element abundances.

Besides the actual FastChem model, we provide a C++ stand-alone version in the *model_src* folder that allows to calculate the equilibrium chemistry for a given temperature-pressure structure. This stand-alone version can be adapted further to perform also more sophisticated calculations. In addition to the C++ stand-alone version, we also provide the Python interface PyFastChem.

Version 3.1 includes updates to the calculation of the gas-phase chemistry. The new version will now switch to a multi-dimensional Newton's method for more complicated scenarios that were previously challenging to solve with the standard hierarchical approach of FastChem.


# PyFastChem #

FastChem includes the Python interface, PyFastChem, that allows to run the C++ code as a normal Python module. PyFastChem is also available as a PyPy package and can easily be installed via pip. We provide several examples that show how to call FastChem from within a Python script, including the possibility to, for example, iterate over different metallicity values or C/O ratios or how to compute chemical compositions with condensation. The Python examples can be found in the *python* directory.  


# User guide #

FastChem comes with a user guide that can be found here: https://exoclime.github.io/FastChem/ . It describes the installation and usage of FastChem and covers both the C++ stand-alone version as well as the Python interface. The manual also contains detailed information on the internal interface functions that the FastChem object class and its Python interface provide.


# Licence #

This project is Copyright (c) Daniel Kitzmann and Joachim Stock.

FastChem is released under the GNU Public Licence (GPL) 3.0. That means, it can be freely copied, edited, and re-distributed. If the code is re-distributed it has to be released under at least a GPL 3.0 licence as well. The full licence of FastChem can be found in the repository *licence.md* file or under https://www.gnu.org/licenses/gpl-3.0.html.

The user guide is released under the Creative Commons Licence (CC BY SA). Licensees may copy and distribute the work and make derivative works based on it only if they give the authors (Daniel Kitzmann & Joachim Stock) the credits by providing a reference to the original guide and this GitHub repository. Licensees may also distribute derivative works only under a license identical to ("not more restrictive than") the license that governs the original work.


