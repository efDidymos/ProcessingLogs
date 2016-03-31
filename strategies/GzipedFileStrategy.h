//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
#define PROCESSINGLOGS_GZIPEDFILESTRATEGY_H

#include <fstream>
#include "../StrategyInterface.hpp"

class GzipedFileStrategy: public StrategyInterface
{
public:
    GzipedFileStrategy(std::string fileName)
        : fin(fileName, std::ios::in)
    {
    }

    void execute();

private:
    std::ifstream fin;
};


#endif //PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
