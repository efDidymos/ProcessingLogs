//
// Created by tomas on 2.4.2016.
//

#ifndef PROCESSINGLOGS_ROWINTERFACE_HPP
#define PROCESSINGLOGS_ROWINTERFACE_HPP

#include <fstream>

/**
 * Interface for displaying different rows strategies
 */
class IRow
{
public:
    /**
     * Opens file and reserve container for reading n rows
     * @param file.log
     * @param rowCount
     * @return
     */
    IRow(std::ifstream *file, unsigned short rowCount) :
            file_(file), rowCount_(rowCount)
    {
        // Before use repeated working with file by changing strategy
        // reset the EOF and failbit
        file->clear();
    }

    /**
     * Read n rows from specific position of the file
     * @param inPos
     * @param outPos
     * @param rows
     */
    virtual void read(long *inPos,
                      long *outPos,
                      std::vector<std::string> *rows) = 0;

protected:
    std::ifstream *file_;
    unsigned short rowCount_;
};


#endif //PROCESSINGLOGS_ROWINTERFACE_HPP
