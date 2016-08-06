//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
#define PROCESSINGLOGS_GZIPEDFILESTRATEGY_H

#include "IProcessing.hpp"

/**
 * Class unpacks supplied file
 */
class Gziped : public IProcessing
{
public:
    Gziped() {}

    /**
     * Unpack the file
     * @param file_name.gz
     */
    void process_file(std::string file_name) override;

private:
    /**
     * Unpack the file
     * @param file          - name of input file
     * @param unziped_file   - name of output file
     */
    void zlib_decompress(std::ifstream& file, const std::string& unziped_file) const;
};


#endif //PROCESSINGLOGS_GZIPEDFILESTRATEGY_H
