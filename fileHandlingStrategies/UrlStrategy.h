//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_URLSTRATEGY_H
#define PROCESSINGLOGS_URLSTRATEGY_H

#include <iostream>
#include <fstream>
#include "ProcessingInterface.hpp"

class UrlStrategy: public ProcessingInterface
{
public:
    UrlStrategy(std::string fileName)
        : file(fileName, std::ios::in)
    {
    }

    void execute() override
    {
        std::cout << "UrlStrategy::TODO" << std::endl;
    }


private:
    std::ifstream file;
};


#endif //PROCESSINGLOGS_URLSTRATEGY_H
