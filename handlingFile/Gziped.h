//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
#define PROCESSINGLOGS_GZIPEDFILESTRATEGY_H

#include "ProcessingInterface.hpp"

class Gziped: public ProcessingInterface
{
public:
    Gziped()
    { }

    void processFile(std::string fileName) override;
};


#endif //PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
