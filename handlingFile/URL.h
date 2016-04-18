//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_URLSTRATEGY_H
#define PROCESSINGLOGS_URLSTRATEGY_H

#include "ProcessingInterface.hpp"

#include <tuple>
#include <regex>

class URL: public ProcessingInterface
{
public:
    URL()
        : expresion("^((http[s]?):\\/?\\/?)([^:\\/\\s]+)(.*\\/)(.*)$")
    { }

    void processFile(std::string fileName) override;

private:
    std::tuple<int, std::string>
        get_http_data(std::string server, std::string path, std::string file, std::string protocol);
    std::regex expresion;
    std::smatch match;
};

#endif //PROCESSINGLOGS_URLSTRATEGY_H