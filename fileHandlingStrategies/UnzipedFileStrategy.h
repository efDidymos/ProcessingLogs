//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H
#define PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H

#include <iostream>
#include <fstream>
#include "ProcessingInterface.hpp"
#include "GzipedFileStrategy.h"

class UnzipedFileStrategy: public ProcessingInterface
{
public:
    UnzipedFileStrategy(std::string fileName)
        : file(fileName, std::ios::in)
    {
    }

    void execute();

private:
    std::ifstream file;
};


#endif //PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H
