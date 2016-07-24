//
// Created by tomas on 28.3.2016.
//

#include "URL.h"

#include <boost/asio.hpp>

void URL::processFile(std::string fileName)
{
    // Check if the argument is some type of URL
    if (std::regex_match(fileName, match, expresion))
    {
        auto result = get_http_data(match[3], match[4], match[5], match[2], 0);

        // If the returned result performed without error
        // get the file name in the second atribute and
        // process it to successor
        if (std::get<0>(result) == 0)
        {
            std::cout << "The file " << std::get<1>(result) << " downloaded successfully." << std::endl;

            // Just for the case that user has not saw the results
            sleep(5);
            successor->processFile(std::get<1>(result));
        }
        else
            std::cout << std::get<1>(result);
    }
    else
    {
        // If the argument is not valid URL, try if the successor
        // can handle it
        successor->processFile(fileName);
    }
}

std::tuple<int, std::string>
URL::get_http_data(std::string server, std::string path, std::string file, std::string protocol, short redirectCnt)
{
    if (redirectCnt < maxRedirectCnt)
    {
        namespace baio = boost::asio;
        using boost::asio::ip::tcp;

        try
        {
            boost::asio::io_service io_service;

            // Get a list of endpoints corresponding to the server name.
            tcp::resolver resolver(io_service);
            tcp::resolver::query query(server, protocol);
            tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

            // Try each endpoint until we successfully establish a connection.
            tcp::socket socket(io_service);
            baio::connect(socket, endpoint_iterator);

            // Form the request. We specify the "Connection: close" header so that the
            // server will close the socket after transmitting the response. This will
            // allow us to treat all data up until the EOF as the content.
            baio::streambuf request;
            std::ostream request_stream(&request);
            request_stream << "GET " << path + file << " HTTP/1.0\r\n";
            request_stream << "Host: " << server << "\r\n";
            request_stream << "Accept: */*\r\n";
            request_stream << "Connection: close\r\n\r\n";

            // Send the request.
            baio::write(socket, request);

            // Read the response status line. The response streambuf will automatically
            // grow to accommodate the entire line. The growth may be limited by passing
            // a maximum size to the streambuf constructor.
            baio::streambuf response;
            baio::read_until(socket, response, "\r\n");

            // Check that response is OK.
            std::istream response_stream(&response);

            std::string http_version;
            response_stream >> http_version;

            unsigned int status_code;
            response_stream >> status_code;

            std::string status_message;
            std::getline(response_stream, status_message);

            if (!response_stream || http_version.substr(0, 5) != "HTTP/")
                return std::make_tuple(1, "Invalid response.");

            // If status code inform us about some untrivial way of communication
            if (status_code != 200)
            {
                // If it means jumping to other location
                if (status_code == 302)
                {
                    // Process the response headers, which are terminated by a blank line.
                    std::string header;
                    std::string pom;
                    while (std::getline(response_stream, header) && header != "\r")
                    {
//                        std::cout << header << "\n";
                        if (header.substr(0, 9) == "Location:")
                        {
                            pom = header.substr(10);
                            if (auto pos = pom.find("\r"))
                                pom = pom.substr(0, pos);
                            break;
                        }
                    }

                    // Check if the greped URL is valid
                    if (std::regex_match(pom, match, expresion))
                    {
                        ++redirectCnt;  // increment the redirection counter for condition check
                        return get_http_data(match[3], match[4], match[5], match[2], redirectCnt);  // recursively
                    }
                    else
                        return std::make_tuple(1, "Redirectoin URL is not valid!");
                }
                else
                    return std::make_tuple(1, "Response returned with status code " + status_code);
            }

            // Read the response headers, which are terminated by a blank line.
            baio::read_until(socket, response, "\r\n\r\n");

            // Process the response headers.
            std::string header, length;
            while (std::getline(response_stream, header) && header != "\r")
            {
                //            std::cout << header << "\n";
                if (header.substr(0, 15) == "Content-Length:")
                {
                    length = header.substr(16);
                    if (auto pos = length.find("\r"))
                        length = length.substr(0, pos);
                }
            }
            std::cout << "\n";

            // ---------------------------------------------------------------------
            std::string filename = file + ".download";  // added extension to signalize working copy of file
            std::ofstream ofs(filename, std::ios_base::app);
            // ---------------------------------------------------------------------

            // Write whatever content we already have to output.
            if (response.size() > 0)
                ofs << &response;

            long len = std::stol(length);

            if (checkAvailableSpace(len))
                return std::make_tuple(1, "File is too large to be downloaded!");

            // Read until EOF, writing data to output as we go.
            boost::system::error_code error;

            view.printHorizontalLine();
            std::cout << "\n ";         // only visual purpose

            while (baio::read(socket, response, baio::transfer_at_least(1), error))
            {
                // Show the progress bar
                view.printProgBar("Downloading the file " + file, ofs.tellp(), len);
                ofs << &response;
            }
            if (error != baio::error::eof)
                throw boost::system::system_error(error);

            // ---------------------------------------------------------------------
            ofs.close();
            // ---------------------------------------------------------------------

            // After successfully download of the file check the size
            if (boost::filesystem::file_size(filename) == len)
            {
                // And remove the .download extension from the name of the file
                int rc = std::rename(filename.c_str(), file.c_str());
                if (rc)
                    throw "Error renaming downloaded file " + filename + " to " + file;
                else
                    return std::make_tuple(0, file);
            }
            else
                throw "Error downloading the file " + filename + ". The file is not complete!";
        }
        catch (const std::exception &e)
        {
            std::string pom = "ERROR: ";
            pom += e.what();
            return std::make_tuple(1, pom);
        }
    }
    else
        return std::make_tuple(1, maxRedirectCnt + " max redirection count attained!");
}