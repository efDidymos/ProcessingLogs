//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
#define PROCESSINGLOGS_GZIPEDFILESTRATEGY_H

#include <fstream>
#include "ProcessingInterface.hpp"

class GzipedFileStrategy: public ProcessingInterface
{
public:
    GzipedFileStrategy(const std::string &fileName)
        :
        fileName(fileName),
        file(fileName, std::ios::in)
    {
    }

    void execute();

private:
    std::string fileName;
    std::ifstream file;
};


#endif //PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
