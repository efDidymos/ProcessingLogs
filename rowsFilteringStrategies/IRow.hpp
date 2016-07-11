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
    IRow(std::ifstream *file, unsigned short &rowCount)
        :
        file(file),
        rowCount(rowCount)
    {
        rows.reserve(rowCount);

        file->seekg(0, std::ios::end);
        theEnd = file->tellg();
    }

    virtual ~IRow()
    {
        rows.clear();
    }

    /**
     * Implementation of the prototype design pattern
     * @return
     */
    virtual IRow *Clone() const = 0;

    /**
     * Read n rows from specific position of the file
     * @param pos
     * @param seekdir
     * @return
     */
    virtual long read(long pos, const std::ios_base::seekdir &seekdir) = 0;

    /**
     * Friend function for easier printing of the rows
     * @param os
     * @param r
     * @return
     */
    friend std::ostream &operator<<(std::ostream &os, IRow *r)
    {
        auto elements = r->getRows();
        for (auto row : elements)
            os << row << "\n";
        return os;
    }

    /**
     * Getter
     * @return number of rows
     */
    const std::vector<std::string> &getRows() const
    {
        return rows;
    }

protected:
    std::ifstream *file;
    unsigned short int rowCount;
    std::vector<std::string> rows;
    long theEnd;
};


#endif //PROCESSINGLOGS_ROWINTERFACE_HPP
