//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_STRATEGYINTERFACE_HPP
#define PROCESSINGLOGS_STRATEGYINTERFACE_HPP

#include <iostream>
#include <memory>

/**
 * Interface for handling argument
 */
class IProcessing
{
public:
    /**
     * Processing the argument
     * @param fileName
     */
    virtual void processFile(std::string fileName) = 0;

    /**
     * Setting the next object which should take over performing if this object can not handle it
     * @param IProcessing * nextObject
     */
    void setSuccessor(std::shared_ptr<IProcessing> nextObj)
    {
        successor = nextObj;
    }

protected:
    std::shared_ptr<IProcessing> successor;
};


#endif //PROCESSINGLOGS_STRATEGYINTERFACE_HPP
