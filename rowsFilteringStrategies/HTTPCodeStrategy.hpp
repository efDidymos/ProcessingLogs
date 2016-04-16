//
// Created by tomas on 16.4.2016.
//

#ifndef PROCESSINGLOGS_HTTPCODESTRATEGY_HPP
#define PROCESSINGLOGS_HTTPCODESTRATEGY_HPP

#include "RowInterface.hpp"

#ifndef NDEBUG
#include <chrono>
#endif

#include <sstream>

class HTTPCodeStrategy: public RowInterface
{

public:
    HTTPCodeStrategy(std::ifstream *file, unsigned short &rowCount)
        : RowInterface(file, rowCount)
    { }

    virtual RowInterface *Clone() const override
    {
        return new HTTPCodeStrategy(*this);
    }

    virtual long read(long pos, const std::ios_base::seekdir &seekdir) override
    {
#ifndef NDEBUG
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
#endif

#ifndef NDEBUG
        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of RequestMethodRowsStrategy=" << diff.count() << " --- " << std::endl;
#endif
        return pos;
    }

};


#endif //PROCESSINGLOGS_HTTPCODESTRATEGY_HPP
