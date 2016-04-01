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
    GzipedFileStrategy(std::string file)
        :
        fileName(file),
        fin(file, std::ios::in)
    {
    }

    void execute();

private:
    std::string fileName;
    std::ifstream fin;
};


#endif //PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
