//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_STRATEGYINTERFACE_HPP
#define PROCESSINGLOGS_STRATEGYINTERFACE_HPP

#include <iostream>

class IProcessing
{
public:
    virtual void processFile(std::string fileName) = 0;

    void setSuccessor(IProcessing *next)
    {
        successor = next;
    }

protected:
    IProcessing *successor;
};


#endif //PROCESSINGLOGS_STRATEGYINTERFACE_HPP
