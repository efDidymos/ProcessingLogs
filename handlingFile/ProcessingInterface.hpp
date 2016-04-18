//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_STRATEGYINTERFACE_HPP
#define PROCESSINGLOGS_STRATEGYINTERFACE_HPP

#include <iostream>

class ProcessingInterface
{
public:
    virtual void processFile(std::string fileName) = 0;

    void setSuccessor(ProcessingInterface *next)
    {
        successor = next;
    }

protected:
    ProcessingInterface *successor;
};


#endif //PROCESSINGLOGS_STRATEGYINTERFACE_HPP
