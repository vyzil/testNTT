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

## ETC
- Core Check
```
sudo apt-get install sysstat
mpstat -P ALL 1
```

- Python Test
```
pip3 install gmpy2
```

