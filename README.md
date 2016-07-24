#ProcessingLogs
Lightweight adaptation of log manager.
It reads and filter logs that are stored in format:

`
"%h %l %u %t \"%r\" %>s %O \"%{Referer}i\" \"%{User-Agent}i\""
`

More info about log format at: http://httpd.apache.org/docs/2.2/mod/mod_log_config.html

#Table of content
[Structure of the application](#structure-of-the-application)

[Ubuntu 14.04 instructions](#ubuntu-1404-instructions)

[Getting the sources](#getting-the-sources)

[Compilation](#compilation)

[Compiling only the sources](#compiling-only-the-sources)

[Compiling the whole application src + test](#compiling-the-whole-application-src--test)

[Run](#run)

[Running only sources](#running-only-sources)

[Usage](#usage)

[Running tests](#running-tests)

##Structure of the application
The application was designed to be modular. It can be extended for example to add an another chain for manipulating with the input argument (dir handlingFile) or add another strategy for displaying contents (dir rowsFilteringStrategies). Also the class RequestMethod was designed in such manner that one can be more specific for selecting names of the request method by adjusting the enum Request and define its explicit version in 
```
std::map<Request, std::vector<std::string>> type;
```
![alt tag](https://raw.githubusercontent.com/efDidymos/ProcessingLogs/master/Diagram-ProcessingLogs.png)

#Ubuntu 14.04 instructions

First step is to install additional packages for successfully compiling the Boost library which some parts depends on the third-party libraries:
```
sudo apt-get install build-essential cmake python-dev libbz2-dev
```
Second step is to download latest sources of zlib library from http://www.zlib.net (for the time of writting it is **1.2.8**):
```
$ tar -xf /path/to/zlib-1.2.8.tar.gz
```
Please remember the above **/path/to/uncompressed-zlib** library because it will be later used when compiling the Boost library.

Last step is to download latest version of the Boost library (for the time of writting it is **1.61**) and install it via
```
$ tar -xf /path/to/boost_1_61_0.tar.gz
$ cd /path/to/boost_1_61_0
$ ./bootstrap.sh

# This step is optional, but usefull for you. 
# It stops building on first occured error 
# and you could find and install missing parts
# NOTICE: in the -s flag we use the path 
#         to locate uncompressed zlib library
# http://www.boost.org/doc/libs/1_61_0/libs/iostreams/doc/installation.html
$ ./b2 -q -sZLIB_SOURCE="/path/to/zlib-1.2.8"

# Last step is installation of the Boost library.
# The sources and headers are copied to
# /usr/local/include/
# /usr/local/lib
# NOTICE: in the -s flag we use the path 
#         to uncompressed zlib library
# http://www.boost.org/doc/libs/1_61_0/libs/iostreams/doc/installation.html
$ sudo ./b2 install -sZLIB_SOURCE="/path/to/zlib-1.2.8"
```

##Getting the sources
```
$ git clone git@github.com:efDidymos/ProcessingLogs.git
```
If you want to start using the application without (unit) tests continue to [compiling only the sources section](#compiling-only-the-sources). At its end it will point you to [run section](#running-only-sources) for start using the application.

##Compilation
If you are interested to get deeply insights, recommended process is jump over the compile of source to [compile src + tests](#compiling-the-whole-application-src-+-test).

###Compiling only the sources
```
$ cd /path/to/ProcessingLogs/src/
$ cmake CMakeLists.txt
$ make
```
And now, if you want you can [execute the application](#running-only-sources).

###Compiling the whole application src + test
If you are interested to get deeply insights for next step you need to download (google) unit test framework. It is available at https://github.com/google/googletest/releases as zip or tar.gz (for the time of writting it is **1.7.0**). After downloading unpack **.tar.gz** with command:
```
$ mkdir /path/to/ProcessingLogs/gtest
$ tar xf /path/to/googletest-release-1.7.0.tar.gz -C /path/to/ProcessingLogs/gtest --strip-components=1
```
or **.zip** with command:
```
$ unzip /path/to/googletest-release-1.7.0.zip -d /path/to/ProcessingLogs/
$ mv /path/to/ProcessingLogs/googletest-release-1.7.0 /path/to/ProcessingLogs/gtest
```
After unpacking and renaming in two arbitrary two foregoing commands, you can begin with compiling the whole:
```
$ cd /path/to/ProcessingLogs/
$ cmake CMakeLists.txt
$ make
```

##Run
###Running only sources
```
$ ./path/to/ProcessingLogs/src/ProcessingLogs <file>
```
or
```
$ ./path/to/ProcessingLogs/src/ProcessingLogs <url_address>
```
####Usage
```
[q]-quit  [f]-filter options  [j]-move down  [k]-move up
           |
           +-- FILTER BY: (0) None  (1) HTTP Code  (2) Request method  (3) Date
                                    |               |                   |
                                    |               |                   +-- Please enter day [01-31] and press ENTER:
                                    |               |
                                    |               +-- FILTER Request method BY: (0) POST  (1) GET  (2) HEAD  (3) Unknown
                                    |
                                    +-- Please enter code [1xx - 5xx] and press ENTER:
```
###Running tests
For this step is required that the application was compiled with the unit tests. If not jump to [compile src + tests](#compiling-the-whole-application-src--test) section.
```
$ ./path/to/ProcessingLogs/test/<file>
```
