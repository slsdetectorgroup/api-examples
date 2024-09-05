# api-examples


* E1 reading a config file
* E2 using sls::Result
* E3 Setting and getting exposure time
* E4 Receiver callbacks
* E5 Client callback in acquire 


# compilation

## compile and install the package
Refer to [software wiki](https://slsdetectorgroup.github.io/devdoc/installation.html#build-using-cmake) on installation instructions

## compile api-examples pointing to the package install path
```
# outside api-examples folder
mkdir build && cd build

# configure & generate Makefiles
cmake ../api-examples -DCMAKE_PREFIX_PATH=/path/to/sls/install

# compile
make -j12 

```