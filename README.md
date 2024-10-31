# Test NTT Project

## Requirement
```
sudo apt-get install build-essential git libboost-all-dev cmake libgmp3-dev libssl-dev libprocps3-dev pkg-config libsodium-dev
gnuplot-x11
```
## Build
```
mkdir build
cd build
cmake ..
make
```
### Option
- Default : multi-core
- Serial : `cmake .. -DMULTICORE=OFF`

## Usage
```
./generate_input -n 3
./polynomial_multiplication -m
```
### generate_input
- `n` : option referce to polynomial size : 2^n

### polynomial_multiplication
- `m` : parallel mode using openmp
- `t` : test mode, using small (hard coded) value 

## ETC
- My COnfig
```
# CMAKE
$ $cmake -LAH | grep CMAKE_CXX_COMPILER
CMAKE_CXX_COMPILER:FILEPATH=/usr/bin/c++
CMAKE_CXX_COMPILER_AR:FILEPATH=/usr/bin/gcc-ar-12
CMAKE_CXX_COMPILER_RANLIB:FILEPATH=/usr/bin/gcc-ranlib-12

# C++
$ c++ --version
c++ (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0
Copyright (C) 2022 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

- Core Check
```
sudo apt-get install sysstat
mpstat -P ALL 1
```

- Python Test
```
pip3 install gmpy2
```
