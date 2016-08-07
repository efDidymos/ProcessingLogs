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
     * @param row_cnt
     * @return
     */
    IRow(std::ifstream *file, unsigned short row_cnt)
        : file_(file), row_cnt_(row_cnt)
    {
        // Before use repeated working with file by changing strategy
        // reset the EOF and failbit
        file->clear();
    }

    /**
     * Read n rows from specific position of the file
     * @param in_pos
     * @param out_pos
     * @param rows
     */
    virtual void read(long *in_pos,
                      long *out_pos,
                      std::vector<std::string> *rows) = 0;

protected:
    std::ifstream *file_;
    unsigned short row_cnt_;
};


#endif //PROCESSINGLOGS_ROWINTERFACE_HPP
