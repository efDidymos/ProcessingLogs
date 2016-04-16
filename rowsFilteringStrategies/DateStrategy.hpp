//
// Created by tomas on 16.4.2016.
//

#ifndef PROCESSINGLOGS_DATESTRATEGY_HPP
#define PROCESSINGLOGS_DATESTRATEGY_HPP

#include "RowInterface.hpp"

class DateStrategy: public RowInterface
{

public:
    DateStrategy(std::ifstream *file, unsigned short &rowCount)
        : RowInterface(file, rowCount)
    { }

    virtual RowInterface *Clone() const override
    {
        return new DateStrategy(*this);
    }

    virtual long read(long pos, const std::ios_base::seekdir &seekdir) override
    {
        return 0;
    }

};


#endif //PROCESSINGLOGS_DATESTRATEGY_HPP
