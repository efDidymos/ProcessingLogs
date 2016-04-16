//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_URLSTRATEGY_H
#define PROCESSINGLOGS_URLSTRATEGY_H

#include <fstream>
#include "ProcessingInterface.hpp"

class UrlStrategy: public ProcessingInterface
{
public:
    UrlStrategy(const std::string arg)
        : url(arg)
    {
    }

    void execute();

private:
    std::string url;
};


#endif //PROCESSINGLOGS_URLSTRATEGY_H
