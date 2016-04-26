//
// Created by tomas on 2.4.2016.
//

#ifndef PROCESSINGLOGS_ROWINTERFACE_HPP
#define PROCESSINGLOGS_ROWINTERFACE_HPP

#include <fstream>

class IRow
{
public:
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

    virtual IRow *Clone() const = 0;

    virtual long read(long pos, const std::ios_base::seekdir &seekdir) = 0;

    friend std::ostream &operator<<(std::ostream &os, IRow *r)
    {
        auto elements = r->getRows();
        for (auto row : elements)
            os << row << "\n";
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
    long theEnd;
};


#endif //PROCESSINGLOGS_ROWINTERFACE_HPP
