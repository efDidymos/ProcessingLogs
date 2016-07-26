//
// Created by tomas on 28.3.2016.
//

#include "Gziped.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <stdlib.h>

void Gziped::processFile(std::string fileName)
{
    std::string ext = boost::filesystem::extension(fileName);

    // Check if the file exist and if it can be handled with this object
    if (boost::filesystem::exists(fileName) && (ext == ".gz"))
    {
        std::ifstream file(fileName, std::ios::in);

        std::cout << "Please be patient while decompressing the file." << std::endl;
        std::cout << "Starting decompressing " << fileName << std::endl;

        namespace bio = boost::iostreams;

        // Prepare name of the new unzipped file
        // by trimming the .gz extension from filename
        try
        {
            std::string unzipedFile = fileName.substr(0, fileName.find(".gz"));

            zlib_decompress(file, unzipedFile);

            // Just for the case that user has not saw the results
            sleep(5);

            // Call the appropriate obj in the chain for next processing
            successor->processFile(unzipedFile);
        }
        catch (const bio::gzip_error &exception)
        {
            std::cerr << "\nBoost Description of Error: " << exception.what() << std::endl;
        }
    }
    else
        // Hand over to successor obj
        successor->processFile(fileName);
}

void Gziped::zlib_decompress(std::ifstream &file, const std::string &unzipedFile) const
{
    namespace bio = boost::iostreams;

    bio::filtering_istream in;
    try
    {
        in.push(bio::gzip_decompressor());
        in.push(file);

        std::ofstream out(unzipedFile, std::ios_base::out);
        bio::copy(in, out);

        std::cout << "Decompressed and saved as " << unzipedFile << std::endl;
    }
    catch(const bio::gzip_error& exception)
    {
        std::cerr << "ERROR while decompressing the file. " << std::endl;

        int error = exception.error();

        switch (error)
        {
            case boost::iostreams::gzip::zlib_error:
                std::cerr << "A zlib error has occured!" << std::endl;
                break;

            case boost::iostreams::gzip::bad_crc:
                std::cerr << "Recorded crc doesn't match data!" << std::endl;
                break;

            case boost::iostreams::gzip::bad_length:
                std::cerr << "Recorded length doesn't match data!" << std::endl;
                break;

            case boost::iostreams::gzip::bad_header:
                std::cerr << "Malformed file header!" << std::endl;
                break;

            case boost::iostreams::gzip::bad_footer:
                std::cerr << "Portion of file following compressed data is malformed!" << std::endl;
                break;
        }
    }
}