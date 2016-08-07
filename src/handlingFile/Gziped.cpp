//
// Created by tomas on 28.3.2016.
//

#include "Gziped.h"

#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

void Gziped::process_file(const std::string file_name)
{
    std::string ext = boost::filesystem::extension(file_name);

    // Check if the file exist and if it can be handled with this object
    if (boost::filesystem::exists(file_name) && (ext == ".gz"))
    {
        std::ifstream file(file_name, std::ios::in);

        std::cout << "Do you want to decompress " << file_name << "?" << std::endl;
        std::cout << "[y - decompress / n - quit application]" << std::endl;
        std::string response;
        std::cin >> response;

        if ((response == "n") || (response == "N"))
            std::cout << "Quitting. Bye..." << std::endl;
        else if ((response == "y") || (response == "Y"))
        {
            std::cout << "Decompression of " << file_name << " has been started. Please be patient." << std::endl;

            namespace bio = boost::iostreams;

            // Prepare name of the new unzipped file
            // by trimming the .gz extension from filename
            try
            {
                std::string unzipedFile = file_name.substr(0, file_name.find(".gz"));

                zlib_decompress(file, unzipedFile);

                // Call the appropriate obj in the chain for next processing
                successor_->process_file(unzipedFile);
            }
            catch (const bio::gzip_error& exception)
            {
                std::cerr << "\nBoost Description of Error: " << exception.what() << std::endl;
            }
        }
        else
            std::cout << "Bad response. Quitting. Bye..." << std::endl;
    }
    else
        // Hand over to successor obj
        successor_->process_file(file_name);
}

void Gziped::zlib_decompress(std::ifstream& ifs, const std::string& unziped_file) const
{
    namespace bio = boost::iostreams;

    // ==============================================================================
    // Solution for detecting issues and handling it for example due to small disk space
    // while writing can be found documented here:
    // http://stackoverflow.com/a/20471593
    // ==============================================================================
    struct safe_ofstream_sink
    {
        typedef char          char_type;
        typedef bio::sink_tag category;
        std::ofstream&        ofs;

        safe_ofstream_sink(std::ofstream& ofs) : ofs(ofs)
        { }

        std::streamsize write(const char *s, std::streamsize n)
        {
            ofs.write(s, n);
            if (!ofs)
                throw std::runtime_error("Failed writing to fstream");

            return n;
        }
    };

    try
    {
        // ==============================================================================
        // The solution fix was tested on the /dev/full
        // ==============================================================================
        // open output stream to the "full" device
        // full device is a "utility-device" to check how applications handle ENOSPC
        // more details in "man full"
//        std::ofstream ofs("/dev/full");
        std::ofstream ofs(unziped_file, std::ios_base::out);

        // Setup the iostreams filter
        bio::filtering_streambuf<bio::output> filters;
        filters.push(bio::gzip_decompressor());
        filters.push(safe_ofstream_sink(ofs));

        // "run" the filter
        bio::copy(ifs, filters);

        std::cout << "Decompressed and saved as " << unziped_file << std::endl;
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