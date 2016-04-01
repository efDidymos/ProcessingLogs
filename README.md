# ProcessingLogs
Lightweight adaptation of log manager.

#Ubuntu 14.04 instructions
Download boost 1.60 and install it via
```
$ cd /path/to/boost_1_60_0
$ ./bootstrap.sh

# This step is optional, but usefull for you. 
# It stops building on first occured error 
# and you could find and install missing parts
$ ./b2 -q  

$ ./b2 install
```

In my case I installed zlib library:
```
sudo apt-get install zlibc zlib1g zlib1g-dev
```
