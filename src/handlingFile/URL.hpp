//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_URLSTRATEGY_H
#define PROCESSINGLOGS_URLSTRATEGY_H

#include "IProcessing.hpp"
#include "../Viewer.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <regex>

namespace baio = boost::asio;
using boost::asio::ip::tcp;

class URL: public IProcessing
{
public:
    URL(Viewer & view)
            : view(view), expresion("^((http[s]?):\\/?\\/?)([^:\\/\\s]+)(.*\\/)(.*)$")
    {
    }

    /**
     * Download a file from URL. Also checks if the URL is valid.
     * Also check if it contain a redirection if so then recursively
     * atempt to download a file from new location
     * @param fileName as URL
     */
    void processFile(std::string fileName) override
    {
        // Check if the argument is some type of URL
        if (std::regex_match(fileName, match, expresion))
        {
            server      = match[3];
            path        = match[4];
            file        = match[5];
            protocol    = match[2];
            form_request(server, path, file);

            // Start an asynchronous resolve to translate the server and service names
            // into a list of endpoints.
            tcp::resolver::query query(server, protocol);
            resolver_.async_resolve(query,
                                    boost::bind(&URL::handle_resolve, this,
                                                baio::placeholders::error,
                                                baio::placeholders::iterator));
            // Fire the connection
            io_service.run();
        }
        else
        {
            // If the argument is not valid URL, try if the successor
            // can handle it
            successor->processFile(fileName);
        }
    }

private:
    void form_request(const std::string &server, const std::string &path, const std::string &file)
    {
        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        std::ostream request_stream(&request_);
        request_stream << "GET " << path + file << " HTTP/1.0\r\n";
        request_stream << "Host: " << server << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";
    }

    void handle_resolve(const boost::system::error_code& err,
                        tcp::resolver::iterator endpoint_iterator)
    {
        if (!err)
        {
            // Attempt a connection to each endpoint in the list until we
            // successfully establish a connection.
            baio::async_connect(socket_, endpoint_iterator,
                                       boost::bind(&URL::handle_connect, this,
                                                   baio::placeholders::error));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_connect(const boost::system::error_code& err)
    {
        if (!err)
        {
            // The connection was successful. Send the request.
            baio::async_write(socket_, request_,
                                     boost::bind(&URL::handle_write_request, this,
                                                 baio::placeholders::error));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_write_request(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Read the response status line. The response_ streambuf will
            // automatically grow to accommodate the entire line. The growth may be
            // limited by passing a maximum size to the streambuf constructor.
            baio::async_read_until(socket_, response_, "\r\n",
                                          boost::bind(&URL::handle_read_status_line, this,
                                                      baio::placeholders::error));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_read_status_line(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Check that response is OK.
            std::istream response_stream(&response_);
            std::string http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);
            if (!response_stream || http_version.substr(0, 5) != "HTTP/")
            {
                std::cout << "Invalid response\n";
                return;
            }

            // If status code inform us about redirection to other location
            // and we have not reached the maximum redirections
            if ((status_code == 302 || status_code == 301) && (redirectCnt < maxRedirectCnt))
            {
                // Read the response headers, which are terminated by a blank line.
                baio::async_read_until(socket_, response_, "\r\n\r\n",
                                              boost::bind(&URL::handle_read_headers_redirection, this,
                                                          baio::placeholders::error));
            }
            // If we reached the maximum redirections
            else if (redirectCnt == maxRedirectCnt)
            {
                std::cout << maxRedirectCnt + " max redirection count attained!" << std::endl;
                return;
            }
            // If status code inform us about some untrivial way of communication
            else if (status_code != 200 && (status_code != 301 || status_code != 302))
            {
                std::cout << "Response returned with status code ";
                std::cout << status_code << "\n";
                return;
            }
            else
            {
                // Read the response headers, which are terminated by a blank line.
                baio::async_read_until(socket_, response_, "\r\n\r\n",
                                              boost::bind(&URL::handle_read_headers, this,
                                                          baio::placeholders::error));
            }
        }
        else
        {
            std::cout << "Error: " << err << "\n";
        }
    }

    void handle_read_headers_redirection(const boost::system::error_code &err)
    {
        if (!err)
        {
            // Process the response headers, which are terminated by a blank line.
            std::istream response_stream(&response_);
            std::string header, location;
            while (std::getline(response_stream, header) && header != "\r")
            {
//                        std::cout << header << "\n";
                if (header.substr(0, 9) == "Location:")
                {
                    location = header.substr(10);
                    if (auto pos = location.find("\r"))
                        location = location.substr(0, pos);
                    break;
                }
            }

            // Check if the greped URL is valid
            if (std::regex_match(location, match, expresion))
            {
                // !!! VERY ESSENTIAL STEP !!!
                // Remove any remaining extra data in the response buffer that we will not need.
                // If we will not remove them, the async processes tries to read them and the
                // new request ends up in message: Invalid response
                response_.consume(response_.size());

                server      = match[3];
                path        = match[4];
                file        = match[5];
                protocol    = match[2];
                form_request(server, path, file);

                // Note that we performed redirection
                ++redirectCnt;

                // Start an asynchronous resolve to translate the server and service names
                // into a list of endpoints.
                tcp::resolver::query query(server, protocol);
                resolver_.async_resolve(query,
                                        boost::bind(&URL::handle_resolve, this,
                                                    baio::placeholders::error,
                                                    baio::placeholders::iterator));
            }
            else
            {
//                        return std::make_tuple(1, "Redirectoin URL is not valid!");
                std::cout << "Redirectoin URL is not valid!" << std::endl;
                return;
            }
        }
        else
        {
            std::cout << "Error: " << err << "\n";
        }
    }

    void handle_read_headers(const boost::system::error_code &err)
    {
        if (!err)
        {
            // Process the response headers.
            std::istream response_stream(&response_);
            std::string header, contentLght;

            while (std::getline(response_stream, header) && header != "\r")
            {
                std::cout << header << "\n";
                if (header.substr(0, 15) == "Content-Length:")
                {
                    contentLght = header.substr(16);
                    if (auto pos = contentLght.find("\r"))
                        len = std::stol(contentLght.substr(0, pos));
                }
            }
            std::cout << "\n";

            if (checkAvailableSpace(len))
            {
//                return std::make_tuple(1, "File is too large to be downloaded!");
                std::cout << "File is too large to be downloaded!" << std::endl;
                return;
            }

            // Create the temporary file
            // ---------------------------------------------------------------------
            // Added extension to name of the file signalize working copy of file
            tmpFile = file + fileExtension;
            ofs     = std::ofstream(tmpFile, std::ios_base::app);
            // ---------------------------------------------------------------------

            if (response_.size() > 0)
            {
                // Start reading remaining data until EOF.
                baio::async_read(socket_, response_,
                                        baio::transfer_at_least(1),
                                        boost::bind(&URL::handle_read_content, this,
                                                    baio::placeholders::error));
            }
        }
        else
        {
            std::cout << "Error: " << err << "\n";
        }
    }

    void handle_read_content(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Write all of the data that has been read so far.
            ofs << &response_;

            // Show the progress bar
            view.printProgBar("Downloading the file " + file, ofs.tellp(), len);

            // Continue reading remaining data until EOF.
            baio::async_read(socket_, response_,
                                    baio::transfer_at_least(1),
                                    boost::bind(&URL::handle_read_content, this,
                                                baio::placeholders::error));
        }
        // After successfully download of the content
//        else if ((baio::error::eof == err) ||
//            (baio::error::connection_reset == err))
        else if (baio::error::eof == err)
        {
            downloading_finished();
        }
        else if (err != baio::error::eof)
        {
            std::cout << "Error: " << err << "\n";
        }
    }

    void downloading_finished()
    {
        // ---------------------------------------------------------------------
        ofs.close();
        // ---------------------------------------------------------------------

        if (boost::filesystem::file_size(tmpFile) == len)
        {

        // Rename the temporary file with .download extension to the name of the original file
        int rc = std::rename(tmpFile.c_str(), file.c_str());
        if (rc)
//            throw
            std::cout << "Error renaming downloaded file " + tmpFile + " to " + file << std::endl;

        // Send to process downloaded file to successor
        successor->processFile(file);

//        else
//            return std::make_tuple(0, file);
        }
        else
            std::cout << "Error downloading the file " + tmpFile + ". The file is not complete!" << std::endl;
    }

    Viewer &            view;

    const std::string   fileExtension   = ".download";
    const short         maxRedirectCnt  = 3; // maximum redirection
    short               redirectCnt     = 0; // counter of total redirections
    long                len;                 // Length of the file provided by server
    std::regex          expresion;
    std::smatch         match;

    baio::io_service    io_service;
    tcp::resolver       resolver_{io_service};
    tcp::socket         socket_{io_service};
    baio::streambuf     request_;
    baio::streambuf     response_;

    std::string         server, path, file, tmpFile, protocol;
    std::ofstream       ofs;
};

#endif //PROCESSINGLOGS_URLSTRATEGY_H