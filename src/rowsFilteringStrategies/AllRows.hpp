//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_ROWS_HPP
#define PROCESSINGLOGS_ROWS_HPP

#include "IRow.hpp"

#ifndef NDEBUG
#include <chrono>
#endif

/**
 * Class display all rows from file
 */
class AllRows: public IRow
{
public:
    AllRows(std::ifstream *file, unsigned short rowCount) :
            IRow(file, rowCount)
    {
    }

    void read(unsigned long *inPos,
              unsigned long *outPos,
              std::vector<std::string> *rows) override
    {
#ifndef NDEBUG
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
#endif

        std::string line;
        rows->clear();

        file->seekg(*inPos, std::ios_base::beg);

        for (int i = 0; i < rowCount; ++i)
        {
            if (getline(*file, line))
            {
                rows->push_back(line);
                *outPos = file->tellg();
            }
            else
            {
                *outPos = theEnd;
                break;
            }
        }

#ifndef NDEBUG
        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of AllRows=" << diff.count() << "\n";
#endif
    }
};


#endif //PROCESSINGLOGS_ROWS_HPP
