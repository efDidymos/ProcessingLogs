# ProcessingLogs
Lightweight adaptation of log manager.
It reads and filter logs that are stored in format:

`
"%h %l %u %t \"%r\" %>s %O \"%{Referer}i\" \"%{User-Agent}i\""
`

More info about log format at: http://httpd.apache.org/docs/2.2/mod/mod_log_config.html


![alt tag](https://raw.githubusercontent.com/efDidymos/ProcessingLogs/Diagram-ProcessingLogs.png)

#Ubuntu 14.04 instructions

First step is to install packages for successfully compiling the Boost library:
```
sudo apt-get install python-dev libbz2-dev
```
Second step is to download latest sources of zlib library from http://www.zlib.net (for the time of writting it is 1.2.8):
```
$ tar -xf /path/to/zlib-1.2.8.tar.gz
```
Please remember the path to uncompressed zlib library because it will be used when compiling the Boost library.

Last step is to download latest version of the Boost library (for the time of writting it is 1.60) and install it via
```
$ tar -xf /path/to/boost_1_60_0.tar.gz
$ cd /path/to/boost_1_60_0
$ ./bootstrap.sh

# This step is optional, but usefull for you. 
# It stops building on first occured error 
# and you could find and install missing parts
# NOTICE: in the -s flag we use the path 
#         to locate uncompressed zlib library
# http://www.boost.org/doc/libs/1_60_0/libs/iostreams/doc/installation.html
$ ./b2 -q -sZLIB_SOURCE="/path/to/zlib-1.2.8"

# Last step is installation of the Boost library.
# The sources and headers are copied to
# /usr/local/include/
# /usr/local/lib
# NOTICE: in the -s flag we use the path 
#         to uncompressed zlib library
# http://www.boost.org/doc/libs/1_60_0/libs/iostreams/doc/installation.html
$ sudo ./b2 install -sZLIB_SOURCE="/path/to/zlib-1.2.8"
```

#Compiling sources
```
$ cmake CMakeLists.txt
$ make
$ ./ProcessingLogs <file>
```
Or
```
$ ./ProcessingLogs <url_address>
```
