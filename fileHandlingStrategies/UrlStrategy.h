//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_URLSTRATEGY_H
#define PROCESSINGLOGS_URLSTRATEGY_H

#include "ProcessingInterface.hpp"
#include <fstream>
#include <regex>

class UrlStrategy: public ProcessingInterface
{
public:
    UrlStrategy(const std::string arg)
        :
        url(arg),
        expresion("^((http[s]?):\\/?\\/?)([^:\\/\\s]+)(.*\\/)(.*)$")
    {
    }

    void execute();

    std::string get_http_data(std::string server, std::string path, std::string file, std::string protocol);

    void showProgress(std::ofstream *file, long length);

private:
    std::string url;
//    std::regex e(R"delim("^((http[s]?):\\/?\\/?)([^:\\/\\s]+)(.*)$");
//    std::regex e("^((http[s]?):\\/?\\/?)([^:\\/\\s]+)(.*)$");
// ^((http[s]?|ftp):\/?\/?)([^:\/\s]+)((\/\w+)*\/)([\w\-\.]+[^#?\s]+)(.*)?(#[\w\-]+)?$
    std::regex expresion;
    std::smatch match;
};

#endif //PROCESSINGLOGS_URLSTRATEGY_H