//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_URLSTRATEGY_H
#define PROCESSINGLOGS_URLSTRATEGY_H

#include <iostream>
#include <fstream>
#include "StrategyInterface.hpp"

class UrlStrategy: public StrategyInterface
{
public:
    UrlStrategy(std::string fileName)
        : fin(fileName, std::ios::in)
    {
    }

    void execute() override
    {
        std::cout << "UrlStrategy::TODO" << std::endl;
    }


private:
    std::ifstream fin;
};


#endif //PROCESSINGLOGS_URLSTRATEGY_H
