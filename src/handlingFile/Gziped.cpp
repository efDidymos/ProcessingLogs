//
// Created by tomas on 28.3.2016.
//

#include "Gziped.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

void Gziped::processFile(std::string fileName)
{
    std::string ext = boost::filesystem::extension(fileName);

    // Check if the file exist and if it can be handled with this object
    if (boost::filesystem::exists(fileName) && (ext == ".gz"))
    {
        std::ifstream file(fileName, std::ios::in);

        std::cout << "Do you want to decompress " << fileName << "?" << std::endl;
        std::cout << "[y - decompress / n - quit application]" << std::endl;
        std::string response;
        std::cin >> response;

        if ((response == "n") || (response == "N"))
            std::cout << "Quitting. Bye..." << std::endl;
        else if ((response == "y") || (response == "Y"))
        {
            std::cout << "Decompression of " << fileName << " has been started. Please be patient." << std::endl;

            namespace bio = boost::iostreams;

            // Prepare name of the new unzipped file
            // by trimming the .gz extension from filename
            try
            {
                std::string unzipedFile = fileName.substr(0, fileName.find(".gz"));

                zlib_decompress(file, unzipedFile);

                std::cout << "Do you want to display the uncompressed log?" << std::endl;
                std::cout << "[y - display / n - quit application]" << std::endl;
                std::cin >> response;

                if ((response == "n") || (response == "N"))
                {
                    std::cout << "Quitting. Bye..." << std::endl;
                    return;
                }
                else if ((response == "y") || (response == "Y"))
                    // Call the appropriate obj in the chain for next processing
                    successor->processFile(unzipedFile);
            }
            catch (const bio::gzip_error &exception)
            {
                std::cerr << "\nBoost Description of Error: " << exception.what() << std::endl;
            }
        }
        else
            std::cout << "Bad response. Quitting. Bye..." << std::endl;
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
    catch (const bio::gzip_error& exception)
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

            default:
                std::cerr << exception.what() << std::endl;
                break;
        }
    }
}