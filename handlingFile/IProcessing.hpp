//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_STRATEGYINTERFACE_HPP
#define PROCESSINGLOGS_STRATEGYINTERFACE_HPP

#include <iostream>
#include <memory>

/**
 * Interface for handling arguments
 */
class IProcessing
{
public:
    /**
     * Processing the arguments
     * @param fileName
     */
    virtual void processFile(std::string fileName) = 0;

    /**
     * Setting the next one in a chain
     * @param IProcessing * next
     */
    void setSuccessor(std::shared_ptr<IProcessing> next)
    {
        successor = next;
    }

protected:
    std::shared_ptr<IProcessing> successor;
};


#endif //PROCESSINGLOGS_STRATEGYINTERFACE_HPP
