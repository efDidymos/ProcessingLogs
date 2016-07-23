//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H
#define PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H

#include "IProcessing.hpp"
#include "../Viewer.hpp"

/**
 * Class calls strategies for manipulating with the file
 */
class Unziped: public IProcessing
{
public:
    Unziped(Viewer & view) : view(view)
    { }

    /**
     * Validate the file and call actions that
     * can be performed on the file
     * @param fileName.log
     */
    void processFile(std::string fileName) override;

private:
    Viewer & view;
};


#endif //PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H