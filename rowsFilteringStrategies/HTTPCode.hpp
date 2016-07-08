//
// Created by tomas on 16.4.2016.
//

#ifndef PROCESSINGLOGS_HTTPCODESTRATEGY_HPP
#define PROCESSINGLOGS_HTTPCODESTRATEGY_HPP

#include "IRow.hpp"

#ifndef NDEBUG
#include <chrono>
#endif

#include <sstream>

class HTTPCode: public IRow
{

public:
    HTTPCode(std::ifstream *file, unsigned short &rowCount, std::string code)
        :
        IRow(file, rowCount),
        code(code)
    { }

    virtual IRow *Clone() const override
    {
        return new HTTPCode(*this);
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

        std::string c1, c2, c3, c4, c5, c6, c7, c8, c9;
        std::stringstream ss;

        do
        {
            if (getline(*file, line))
            {
                pos = file->tellg();

                ss << line;

                ss >> c1 >> c2 >> c3 >> c4 >> c5 >> c6 >> c7 >> c8 >> c9;

                ss.str(""); // erase the buffer

                if (c9 == code)
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
        std::cout << "\n --- Duration of RequestMethod=" << diff.count() << "\n";
#endif
        return pos;
    }
private:
    std::string code;
};


#endif //PROCESSINGLOGS_HTTPCODESTRATEGY_HPP
