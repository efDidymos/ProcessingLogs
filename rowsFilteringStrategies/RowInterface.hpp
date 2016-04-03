//
// Created by tomas on 2.4.2016.
//

#ifndef PROCESSINGLOGS_ROWINTERFACE_HPP
#define PROCESSINGLOGS_ROWINTERFACE_HPP

#include <fstream>
#include <vector>

#include <regex>

class RowInterface
{
public:
    RowInterface(std::ifstream *file, unsigned short rowCount)
        :
        file(file),
        rowCount(rowCount)
    {
    }

    virtual ~RowInterface()
    {
        rows.clear();
    }

    virtual long read(long pos, const std::ios_base::seekdir seekdir) = 0;

    friend std::ostream &operator<<(std::ostream &os, RowInterface *r)
    {
        auto elements = r->getRows();
        for (auto row : elements)
            os << row << std::endl;
        return os;
    }

    const std::vector<std::string> &getRows() const
    {
        return rows;
    }

protected:
    std::ifstream *file;
    unsigned short int rowCount;
    std::vector<std::string> rows;
};


#endif //PROCESSINGLOGS_ROWINTERFACE_HPP
