//
// Created by tomas on 28.3.2016.
//

#include <iostream>
#include "GzipedFileStrategy.h"
#include "UnzipedFileStrategy.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

void GzipedFileStrategy::execute()
{
    std::cout << "The file opened successfully" << std::endl;
    std::cout << "Starting decompressing the " << fileName << std::endl;

    namespace bio = boost::iostreams;

    // Prepare name of the new unzipped file
    // by trimming the .gz extension from filename
    std::string unzipedFile = fileName.substr(0, fileName.find(".gz"));

    bio::filtering_istream out;
    out.push(bio::gzip_decompressor());
    out.push(fin);

    std::ofstream outStream(unzipedFile, std::ios_base::out);
    bio::copy(out, outStream);

    std::cout << "Decompressed and saved as " << unzipedFile << std::endl;
    sleep(5);   // just to see the results

    // Call the appropriate strategy
    UnzipedFileStrategy us(unzipedFile);
    us.execute();
}