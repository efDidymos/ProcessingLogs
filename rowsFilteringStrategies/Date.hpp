//
// Created by tomas on 16.4.2016.
//

#ifndef PROCESSINGLOGS_DATESTRATEGY_HPP
#define PROCESSINGLOGS_DATESTRATEGY_HPP

#include "IRow.hpp"

#ifndef NDEBUG
#include <chrono>
#endif

#include <sstream>

/**
 * Class display only rows for specific day
 */
class Date: public IRow
{

public:
    Date(std::ifstream *file, unsigned short rowCount, std::string date) :
            IRow(file, rowCount), date(date)
    { }

    virtual void read(long *inPos,
                      long *outPos,
                      std::vector<std::string> *rows) override
    {
#ifndef NDEBUG
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
#endif

        std::string line;
        rows->clear();

        file->seekg(*inPos, std::ios_base::beg);

        int i = 0;

        std::string c1, c2, c3, c4;
        std::stringstream ss;

        do
        {
            if (getline(*file, line))
            {
                *outPos = file->tellg();

                ss << line;

                ss >> c1 >> c2 >> c3 >> c4;

                ss.str(""); // erase the buffer

                c4 = c4.substr(1, 2);  // Trim the date in read line

                if (c4 == date)
                {
                    rows->push_back(line);
                    ++i;
                }
            }
            else
            {
                *outPos = theEnd;
                break;
            }
        }
        while (i < rowCount);

#ifndef NDEBUG
        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of RequestMethod=" << diff.count() << "\n";
#endif
    }

private:
    std::string date;
};


#endif //PROCESSINGLOGS_DATESTRATEGY_HPP
