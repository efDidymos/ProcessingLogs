//
// Created by tomas on 28.3.2016.
//

#include <iostream>
#include "GzipedFileStrategy.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

void GzipedFileStrategy::execute()
{
    std::cout << "GzipedFileStrategy::TODO" << std::endl;

    namespace bio = boost::iostreams;

    bio::filtering_istream out;
    out.push(bio::gzip_decompressor());
    out.push(fin);

    // Open the file to be read
    // only for first 10 lines
    // just for test
    std::string itReadLine;
    for (int j = 0; j < 10; ++j)
    {
        std::getline(out, itReadLine);
        // Initialise the parser
        std::cout << itReadLine << std::endl;
    }
}