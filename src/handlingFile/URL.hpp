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

//#include <tuple>
//#include <regex>
//
///**
// * Class checks and downloads a file from URL
// */
//class URL: public IProcessing
//{
//public:
//    URL(Viewer & view)
//        : view(view), expresion("^((http[s]?):\\/?\\/?)([^:\\/\\s]+)(.*\\/)(.*)$")
//    { }
//
//    /**
//     * Download a file from URL. Also checks if the URL is valid.
//     * Also check if it contain a redirection if so then recursively
//     * atempt to download a file from new location
//     * @param fileName as URL
//     */
//    void processFile(std::string fileName) override;
//
//private:
//    Viewer & view;
//    std::tuple<int, std::string>
//    get_http_data(std::string server,
//                  std::string path,
//                  std::string file,
//                  std::string protocol,
//                  short redirectCnt);
//    std::regex expresion;
//    std::smatch match;
//    short maxRedirectCnt = 3;   // maximum redirection
//};

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
            std::string server(match[3]), path(match[4]), file(match[5]), protocol(match[2]);
            form_request(server, path, file);

            // Start an asynchronous resolve to translate the server and service names
            // into a list of endpoints.
            tcp::resolver::query query(server, protocol);
            resolver_.async_resolve(query,
                                    boost::bind(&URL::handle_resolve, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::iterator));
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
            boost::asio::async_connect(socket_, endpoint_iterator,
                                       boost::bind(&URL::handle_connect, this,
                                                   boost::asio::placeholders::error));
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
            boost::asio::async_write(socket_, request_,
                                     boost::bind(&URL::handle_write_request, this,
                                                 boost::asio::placeholders::error));
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
            boost::asio::async_read_until(socket_, response_, "\r\n",
                                          boost::bind(&URL::handle_read_status_line, this,
                                                      boost::asio::placeholders::error));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    /*
    HTTP/1.1 302 Found
    Date: Wed, 03 Aug 2016 15:16:37 GMT
    Server: Apache
    Location: http://download.abc.com/special/joinabc/access.log.gz
    Vary: Accept-Encoding
    Content-Length: 239
    Connection: close
    Content-Type: text/html; charset=iso-8859-1

    <!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
    <html><head>
    <title>302 Found</title>
    </head><body>
    <h1>Found</h1>
    <p>The document has moved <a href="http://download.abc.com/special/joinabc/access.log.gz">here</a>.</p>
    </body></html>

    --------------------------------------

    HTTP/1.1 200 OK
    Date: Wed, 03 Aug 2016 16:15:36 GMT
    Server: Apache
    Last-Modified: Mon, 02 Mar 2015 12:21:13 GMT
    ETag: "1db14634-5104d410c1a47"
    Accept-Ranges: bytes
    Content-Length: 498157108
    Connection: close
    Content-Type: text/plain; charset=ISO-8859-1
    */
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
            if (status_code == 302 || status_code == 301)
            {
                std::cout << "idem precitat preskok\n";

                // Read the response headers, which are terminated by a blank line.
                boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
                                              boost::bind(&URL::handle_read_headers_redirection, this,
                                                          boost::asio::placeholders::error));
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
                std::cout << "idem precitat standardne\n";

                // Read the response headers, which are terminated by a blank line.
                boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
                                              boost::bind(&URL::handle_read_headers, this,
                                                          boost::asio::placeholders::error));
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
            std::cout << "citam preskok\n";

            // Process the response headers, which are terminated by a blank line.
            std::istream response_stream(&response_);
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
                // !!! VERY ESSENTIAL STEP !!!
                // Remove any remaining extra data in the response buffer that we will not need.
                // If we will not remove them, the async processes tries to read them and the
                // new request ends up in message: Invalid response
                response_.consume(response_.size());

                std::cout << "Going to redirect to "
                          << match[3] << " - "
                          << match[4] << " - "
                          << match[5] << " - "
                          << match[2] << std::endl;
                std::string server(match[3]), path(match[4]), file(match[5]), protocol(match[2]);
                form_request(server, path, file);

                // Start an asynchronous resolve to translate the server and service names
                // into a list of endpoints.
                tcp::resolver::query query(server, protocol);
                resolver_.async_resolve(query,
                                        boost::bind(&URL::handle_resolve, this,
                                                    boost::asio::placeholders::error,
                                                    boost::asio::placeholders::iterator));
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
            std::cout << "citam normalne\n";

            // Process the response headers.
            std::istream response_stream(&response_);
            std::string header;

            while (std::getline(response_stream, header) && header != "\r")
                std::cout << header << "\n";
            std::cout << "\n";

            // Write whatever content we already have to output.
            if (response_.size() > 0)
                std::cout << &response_;

            // Start reading remaining data until EOF.
            /*boost::asio::async_read(socket_, response_,
                                    boost::asio::transfer_at_least(1),
                                    boost::bind(&URL::handle_read_content, this,
                                                boost::asio::placeholders::error));*/
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
            std::cout << &response_;

            // Continue reading remaining data until EOF.
            boost::asio::async_read(socket_, response_,
                                    boost::asio::transfer_at_least(1),
                                    boost::bind(&URL::handle_read_content, this,
                                                boost::asio::placeholders::error));
        }
        else if (err != boost::asio::error::eof)
        {
            std::cout << "Error: " << err << "\n";
        }
    }

    Viewer & view;

    short                   maxRedirectCnt  = 3;   // maximum redirection
    static short            redirectCnt;
    std::regex              expresion;
    std::smatch             match;

    baio::io_service        io_service;
    tcp::resolver           resolver_{io_service};
    tcp::socket             socket_{io_service};
    boost::asio::streambuf  request_;
    boost::asio::streambuf  response_;
};

short URL::redirectCnt = 0;

#endif //PROCESSINGLOGS_URLSTRATEGY_H