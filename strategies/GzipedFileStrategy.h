//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
#define PROCESSINGLOGS_GZIPEDFILESTRATEGY_H

#include <iostream>
#include <fstream>
#include "../StrategyInterface.hpp"

class GzipedFileStrategy: public StrategyInterface
{
public:
    GzipedFileStrategy(std::string fileName)
        : fin(fileName, std::ios::in)
    {
    }

    void execute() override
    {
        std::cout << "GzipedFileStrategy::TODO" << std::endl;
    }

private:
    std::ifstream fin;
};


#endif //PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
