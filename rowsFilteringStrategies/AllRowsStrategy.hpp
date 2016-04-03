//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_ROWS_HPP
#define PROCESSINGLOGS_ROWS_HPP

#include "RowInterface.hpp"

class AllRowsStrategy: public RowInterface
{
public:
    AllRowsStrategy(std::ifstream *file, unsigned short int rowCount)
        : RowInterface(file, rowCount)
    {
    }

    virtual RowInterface *Clone() const override
    {
        return new AllRowsStrategy(*this);
    }

    long read(long pos, const std::ios_base::seekdir seekdir) override
    {
        std::string line;
        rows.clear();

        std::smatch m;

        file->seekg(pos, seekdir);

        for (int i = 0; i < rowCount; i++)
        {
            getline(*file, line);

            if (line == "")
                break;
            else
            {
                rows.push_back(line);
                pos = file->tellg();
            }
        }

        return pos;
    }
};


#endif //PROCESSINGLOGS_ROWS_HPP
