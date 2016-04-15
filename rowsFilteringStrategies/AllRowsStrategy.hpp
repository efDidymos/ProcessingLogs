//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_ROWS_HPP
#define PROCESSINGLOGS_ROWS_HPP

#include "RowInterface.hpp"

#ifndef NDEBUG
#include <chrono>
#endif

class AllRowsStrategy: public RowInterface
{
public:
    AllRowsStrategy(std::ifstream *file, unsigned short int rowCount)
        : RowInterface(file, rowCount)
    {
    }

    virtual ~AllRowsStrategy()
    {
    }

    virtual RowInterface *Clone() const override
    {
        return new AllRowsStrategy(*this);
    }

    long readRows(long pos, std::list<std::string> &rowStack) override
    {
#ifndef NDEBUG
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
#endif

        std::string line;
        rowStack.clear();

        file->seekg(pos, std::ios_base::beg);

        for (int i = 0; i < rowCount; i++)
        {
            if (getline(*file, line))
            {
                rowStack.push_back(line);
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

        std::cout << "\n --- Duration of AllRowsStrategy=" << diff.count() << std::endl;
#endif

        return pos;
    }
};


#endif //PROCESSINGLOGS_ROWS_HPP
