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
    RowInterface(std::ifstream *file, unsigned short rowCnt)
        :
        pFin(file),
        rowCount(rowCnt),
        e(R"delim(^(.*?)\[(\d{2})\/(.{3})\/(\d{4})(.*?)"(.*?)\s(.*?)"\s(\d{3}))delim")
    {
        // COMPLETE REGEX
        // ^(.*?)\[(\d{2})\/(.{3})\/(\d{4})(.*?)"(.*?)\s(.*?)"\s(\d{3})
    }

    virtual long read(long pos, const std::ios_base::seekdir seekdir) = 0;

//    friend std::ostream &operator<<(std::ostream &os, AllRowsStrategy *r)
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
    std::ifstream *pFin;
    unsigned short int rowCount;
    std::vector<std::string> rows;
    const std::regex e;
};


#endif //PROCESSINGLOGS_ROWINTERFACE_HPP
