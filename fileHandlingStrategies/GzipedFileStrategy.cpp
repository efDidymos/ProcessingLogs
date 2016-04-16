//
// Created by tomas on 28.3.2016.
//

#include "GzipedFileStrategy.h"
#include "UnzipedFileStrategy.h"

#include <iostream>

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
    try
    {
        std::string unzipedFile = fileName.substr(0, fileName.find(".gz"));

        bio::filtering_istream out;
        out.push(bio::gzip_decompressor());
        out.push(file);

        std::ofstream outStream(unzipedFile, std::ios_base::out);
        bio::copy(out, outStream);

        std::cout << "Decompressed and saved as " << unzipedFile << std::endl;
        // Just for the case that user has not saw the results
        sleep(5);

        // Call the appropriate strategy
        UnzipedFileStrategy us(unzipedFile);
        us.execute();
    }
    catch (const bio::gzip_error &exception)
    {
        std::cout << "\nBoost Description of Error: " << exception.what() << std::endl;
    }
}