//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H
#define PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H

#include "IProcessing.hpp"

class Unziped: public IProcessing
{
public:
    Unziped()
    { }

    void processFile(std::string fileName) override;
};


#endif //PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H
