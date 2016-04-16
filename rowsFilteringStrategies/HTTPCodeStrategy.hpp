//
// Created by tomas on 16.4.2016.
//

#ifndef PROCESSINGLOGS_HTTPCODESTRATEGY_HPP
#define PROCESSINGLOGS_HTTPCODESTRATEGY_HPP

#include "RowInterface.hpp"

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
        return 0;
    }

};


#endif //PROCESSINGLOGS_HTTPCODESTRATEGY_HPP
