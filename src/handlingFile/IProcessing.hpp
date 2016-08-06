//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_STRATEGYINTERFACE_HPP
#define PROCESSINGLOGS_STRATEGYINTERFACE_HPP

#include <iostream>
#include <boost/filesystem.hpp>

/**
 * Interface for handling argument
 */
class IProcessing
{
public:
    /**
     * Processing the argument
     * @param file_name
     */
    virtual void process_file(std::string file_name) = 0;

    /**
     * Setting the next object which should take over performing if this object can not handle it
     * @param IProcessing *nextObject
     */
    void set_successor(std::shared_ptr<IProcessing> next_obj)
    {
        successor_ = next_obj;
    }

protected:
    /**
     * Check if the size of a file is not bigger than available space
     * @param size
     * @return bool true if the file is too big
     */
    bool chck_avail_space(const long size)
    {
        // Get available space of the current directory
        boost::filesystem::space_info si = boost::filesystem::space(".");

        return size > si.available;
    }

    std::shared_ptr<IProcessing> successor_;
};


#endif //PROCESSINGLOGS_STRATEGYINTERFACE_HPP
