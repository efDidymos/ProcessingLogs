//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_ROWS_HPP
#define PROCESSINGLOGS_ROWS_HPP

#include <fstream>
#include <vector>

class Rows
{
public:
    Rows(std::ifstream *pFin, unsigned short int rowCount)
        :
        pFin(pFin),
        rowCount(rowCount)
    {
        pFin->seekg(0, std::ios::end);
        theEnd = pFin->tellg();

        std::cout << "The end: " << theEnd << std::endl;
    }

    long read(long pos, const std::ios_base::seekdir seekdir)
    {
        std::string line;
        rows.clear();

        pFin->seekg(pos, seekdir);

        for (int i = 0; i < rowCount; i++)
        {
            getline(*pFin, line);

            if (line == "")
                break;
            else
            {
                rows.push_back(line);
                pos = pFin->tellg();
            }
        }

        return pos;
    }

    friend std::ostream &operator<<(std::ostream &os, Rows *r)
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

private:
    std::ifstream *pFin;
    long theEnd;
    unsigned short int rowCount;
    std::vector<std::string> rows;
};


#endif //PROCESSINGLOGS_ROWS_HPP
