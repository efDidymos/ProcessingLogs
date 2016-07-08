//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_ROWS_HPP
#define PROCESSINGLOGS_ROWS_HPP

#include "IRow.hpp"

#ifndef NDEBUG
#include <chrono>
#endif

class AllRows: public IRow
{
public:
    AllRows(std::ifstream *file, unsigned short int &rowCount)
        : IRow(file, rowCount)
    {
    }

    virtual IRow *Clone() const override
    {
        return new AllRows(*this);
    }

    long read(long pos, const std::ios_base::seekdir &seekdir) override
    {
#ifndef NDEBUG
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
#endif

        std::string line;
        rows.clear();

        file->seekg(pos, seekdir);

        for (int i = 0; i < rowCount; i++)
        {
            if (getline(*file, line))
            {
                rows.push_back(line);
                pos = file->tellg();
            }
            else
            {
                pos = theEnd;
                break;
            }
        }

#ifndef NDEBUG
        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of AllRows=" << diff.count() << "\n";
#endif
        return pos;
    }
};


#endif //PROCESSINGLOGS_ROWS_HPP
