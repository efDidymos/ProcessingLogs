//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H
#define PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H

#include "IProcessing.hpp"

/**
 * Class calls strategies for manipulating with the file
 */
class Unziped: public IProcessing
{
public:
    Unziped()
    { }

    /**
     * Validate file and call actions that
     * can be performed on the file
     * @param fileName.log
     */
    void processFile(std::string fileName) override;
};


#endif //PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H
