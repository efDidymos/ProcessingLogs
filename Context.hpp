//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_CONTEXT_HPP
#define PROCESSINGLOGS_CONTEXT_HPP

#include "StrategyInterface.hpp"

class Context
{
public:
    Context()
    { }
//    Context(StrategyInterface *s) : strategy(s) { }

    void setStrategy(StrategyInterface *s)
    {
        strategy = s;
    }

    void performAction() const
    {
        strategy->execute();
    }
private:
    StrategyInterface *strategy;
};


#endif //PROCESSINGLOGS_CONTEXT_HPP
