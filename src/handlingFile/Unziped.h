//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H
#define PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H

#include "IProcessing.hpp"
#include "../Viewer.hpp"

/**
 * Class calls strategies for manipulating with the file
 */
class Unziped : public IProcessing
{
public:
    Unziped(Viewer& view) : view_(view)
    { }

    /**
     * Validate the file and call actions that
     * can be performed on the file
     * @param file_name.log
     */
    void process_file(const std::string file_name) override;

private:
    inline void clearBuffer() const { std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }
    Viewer& view_;
};


#endif //PROCESSINGLOGS_UNZIPEDFILESTRATEGY_H
