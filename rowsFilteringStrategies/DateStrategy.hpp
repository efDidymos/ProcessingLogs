//
// Created by tomas on 16.4.2016.
//

#ifndef PROCESSINGLOGS_DATESTRATEGY_HPP
#define PROCESSINGLOGS_DATESTRATEGY_HPP

#include "RowInterface.hpp"

#ifndef NDEBUG
#include <chrono>
#endif

#include <sstream>

class DateStrategy: public RowInterface
{

public:
    DateStrategy(std::ifstream *file, unsigned short &rowCount, std::string date)
        :
        RowInterface(file, rowCount),
        date(date)
    { }

    virtual RowInterface *Clone() const override
    {
        return new DateStrategy(*this);
    }

    virtual long read(long pos, const std::ios_base::seekdir &seekdir) override
    {
#ifndef NDEBUG
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
#endif

        std::string line;
        rows.clear();

        file->seekg(pos, seekdir);

        int i = 0;

        std::string c1, c2, c3, c4;
        std::stringstream ss;

        do
        {
            if (getline(*file, line))
            {
                pos = file->tellg();

                ss << line;

                ss >> c1 >> c2 >> c3 >> c4;

                ss.str(""); // erase the buffer

                c4 = c4.substr(1, 2);  // Trim the date in read line

                if (c4 == date)
                {
                    rows.push_back(line);
                    i++;
                }
            }
            else
            {
                pos = theEnd;
                break;
            }
        }
        while (i < rowCount);

#ifndef NDEBUG
        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of RequestMethodRowsStrategy=" << diff.count() << " --- " << std::endl;
#endif
        return pos;
    }

private:
    std::string date;
};


#endif //PROCESSINGLOGS_DATESTRATEGY_HPP
