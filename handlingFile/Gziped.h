//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
#define PROCESSINGLOGS_GZIPEDFILESTRATEGY_H

#include "IProcessing.hpp"

/**
 * Class unpacks supplied file
 */
class Gziped: public IProcessing
{
public:
    Gziped()
    { }

    /**
     * Unpack the file
     * @param fileName.gz
     */
    void processFile(std::string fileName) override;
};


#endif //PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
