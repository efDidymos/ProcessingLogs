//
// Created by tomas on 28.3.2016.
//

#ifndef PROCESSINGLOGS_URLSTRATEGY_H
#define PROCESSINGLOGS_URLSTRATEGY_H

#include "IProcessing.hpp"
#include "../Viewer.hpp"

#include <fstream>
#include <regex>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace baio = boost::asio;
using boost::asio::ip::tcp;

class URL: public IProcessing
{
public:
    URL(Viewer& view) : view_(view)
    { }

    virtual ~URL()
    {
        clear_unfinished();
    }

    /**
     * Download a file from URL. Also checks if the URL is valid.
     * Also check if it contain a redirection if so then recursively
     * atempt to download a file from new location
     * @param file_name as URL
     */
    void process_file(std::string file_name) override
    {
        // Check if the argument is some type of URL
        if (std::regex_match(file_name, match_, expresion_))
        {
            std::cout << "Do you want to download a file from URL link?" << std::endl;
            std::cout << "[y - download / n - quit application]" << std::endl;
            std::string response;
            std::cin >> response;

            if ((response == "n") || (response == "N"))
                std::cout << "Quitting. Bye..." << std::endl;
            else if ((response == "y") || (response == "Y"))
            {
                establish_connection();

                // Start the deadline actor. You will note that we're not setting any
                // particular deadline here. Instead, the connect and input actors will
                // update the deadline prior to each asynchronous operation.
                deadline_.async_wait(boost::bind(&URL::check_deadline, this));

                // Fire the connection
                io_service_.run();
            }
            else
                std::cout << "Bad response. Quitting. Bye..." << std::endl;
        }
        else
            // If the argument is not valid URL, try if the successor
            // can handle it
            successor_->process_file(file_name);
    }

private:
    void establish_connection()
    {
        server_     = match_[3];
        path_       = match_[4];
        file_       = match_[5];
        protocol_   = match_[2];
        form_request(server_, path_, file_);

        // Start an asynchronous resolve to translate the server and service names
        // into a list of endpoints.
        tcp::resolver::query query(server_, protocol_);
        resolver_.async_resolve(query,
                                boost::bind(&URL::handle_resolve, this,
                                            baio::placeholders::error,
                                            baio::placeholders::iterator));
    }

    // This function terminates all the actors to shut down the connection. It
    // may be called by the user of the client class, or by the class itself in
    // response to graceful termination or an unrecoverable error.
    void stop_connection()
    {
        conn_stopped_ = true;
        boost::system::error_code ignored_ec;
        socket_.shutdown(tcp::socket::shutdown_both, ignored_ec);
        socket_.close(ignored_ec);
        deadline_.cancel();

        if (clear_unfinished() != 0)
            std::cerr << "Error while removing temporary file " << tmp_file_ << std::endl;

        std::cout << "Quitting. Bye..." << std::endl;
    }

    void form_request(const std::string& server, const std::string& path, const std::string& file)
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
            std::cout << "Attempting connect to the server." << std::endl;

            // Set a deadline for the connect operation.
            deadline_.expires_from_now(boost::posix_time::seconds(60));

            // Attempt a connection to each endpoint in the list until we
            // successfully establish a connection.
            baio::async_connect(socket_, endpoint_iterator,
                                       boost::bind(&URL::handle_connect, this,
                                                   baio::placeholders::error));
        }
        else
        {
            std::cerr << "Error in handle resolve: " << err.message() << std::endl;
            stop_connection();
        }
    }

    void handle_connect(const boost::system::error_code& err)
    {
        if (conn_stopped_)
            return;

        // The async_connect() function automatically opens the socket at the start
        // of the asynchronous operation. If the socket is closed at this time then
        // the timeout handler must have run first.
        if (!socket_.is_open())
        {
            std::cout << "Connect timed out\n";
            stop_connection();
        }
        // Check if the connect operation failed before the deadline expired.
        else if (err)
        {
            std::cerr << "Error in handle connect: " << err.message() << std::endl;

            // We need to close the socket used in the previous connection attempt
            // before starting a new one.
            socket_.close();
        }
        else
        {
            // The connection was successful. Send the request.
            baio::async_write(socket_, request_,
                              boost::bind(&URL::handle_write_request, this,
                                          baio::placeholders::error));
        }
    }

    void handle_write_request(const boost::system::error_code& err)
    {
        if (conn_stopped_)
            return;

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
            std::cerr << "Error in handle write request: " << err.message() << std::endl;
    }

    void handle_read_status_line(const boost::system::error_code& err)
    {
        if (conn_stopped_)
            return;

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
                std::cerr << "Error in handle read status line: invalid response from server" << std::endl;
                return;
            }

            // If status code inform us about redirection to other location
            // and we have not reached the maximum redirections
            if ((status_code == 302 || status_code == 301) && (act_redirect_cnt_ < max_redirect_cnt_))
            {
                // Read the response headers, which are terminated by a blank line.
                baio::async_read_until(socket_, response_, "\r\n\r\n",
                                              boost::bind(&URL::handle_read_headers_redirection, this,
                                                          baio::placeholders::error));
            }
            // If we reached the maximum redirections
            else if (act_redirect_cnt_ == max_redirect_cnt_)
                std::cerr << "Error in handle read status line: " << max_redirect_cnt_ + " max redirection count attained!" << std::endl;
            // If status code inform us about some untrivial way of communication
            else if (status_code != 200 && (status_code != 301 || status_code != 302))
                std::cerr << "Error in handle read status line: response returned with status code " << status_code << std::endl;
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
            std::cerr << "Error in handle read status line: " << err << std::endl;
            stop_connection();
        }
    }

    void handle_read_headers_redirection(const boost::system::error_code& err)
    {
        if (conn_stopped_)
            return;

        if (!err)
        {
            // Process the response headers, which are terminated by a blank line.
            std::istream response_stream(&response_);
            std::string header, location;
            while (std::getline(response_stream, header) && header != "\r")
            {
//                std::cout << header << "\n";
                if (header.substr(0, 9) == "Location:")
                {
                    location = header.substr(10);
                    if (auto pos = location.find("\r"))
                        location = location.substr(0, pos);
                    break;
                }
            }

            // !!! VERY ESSENTIAL STEP !!!
            // Remove any remaining extra data in the response buffer that we will not need.
            // If we will not remove them, the async processes tries to read them and the
            // new request ends up in message: Invalid response
            response_.consume(response_.size());

            // Check if the greped URL is valid
            if (std::regex_match(location, match_, expresion_))
            {
                // Note that we performed redirection
                ++act_redirect_cnt_;

                std::cout << "Found redirection. Changing location to another server." << std::endl;
                establish_connection();
            }
            else
                std::cerr << "Error in handle: redirectoin URL is not valid!" << std::endl;
        }
        else
        {
            std::cerr << "Error in handle read headers redirection: " << err << std::endl;
            stop_connection();
        }
    }

    void handle_read_headers(const boost::system::error_code& err)
    {
        if (conn_stopped_)
            return;

        if (!err)
        {
            // Process the response headers.
            std::istream response_stream(&response_);
            std::string header, contentLght;

            while (std::getline(response_stream, header) && header != "\r")
            {
//                std::cout << header << "\n";
                if (header.substr(0, 15) == "Content-Length:")
                {
                    contentLght = header.substr(16);
                    if (auto pos = contentLght.find("\r"))
                        len_ = std::stol(contentLght.substr(0, pos));
                }
            }

            if (chck_avail_space(len_))
            {
                std::cerr << "Error in handle read headers: file is too large to be downloaded!" << std::endl;
                return;
            }

            // ---------------------------------------------------------------------
            // Create the temporary file into which we will store the downloaded data
            // ---------------------------------------------------------------------
            // Add extension to name of the file signalize we work with the copy of the file
            tmp_file_ = file_ + file_extens_;
            ofs_     = std::ofstream(tmp_file_, std::ios_base::app);
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
            std::cerr << "Error in handle read headers: " << err << std::endl;
            stop_connection();
        }
    }

    void handle_read_content(const boost::system::error_code& err)
    {
        if (conn_stopped_)
            return;

        if (!err)
        {
            // Write all of the data that has been read so far.
            ofs_ << &response_;

            // Show the progress bar
            view_.print_prog_bar("Downloading the file " + file_, ofs_.tellp(), len_);

            // Set a deadline for the read operation.
            deadline_.expires_from_now(boost::posix_time::seconds(30));

            // Continue reading remaining data until EOF.
            baio::async_read(socket_, response_,
                                    baio::transfer_at_least(1),
                                    boost::bind(&URL::handle_read_content, this,
                                                baio::placeholders::error));
        }
        // After successfully download of the content
        else if (baio::error::eof == err)
            downloading_finished();
        else if (err != baio::error::eof)
        {
            std::cerr << "Error in handle read content: " << err << std::endl;
            stop_connection();
        }
    }

    void downloading_finished()
    {
        // ---------------------------------------------------------------------
        // Close the temporary file
        // ---------------------------------------------------------------------
        ofs_.close();
        // ---------------------------------------------------------------------
        if (boost::filesystem::file_size(tmp_file_) == len_)
        {
            // Rename the temporary file with .download extension to the name of the original file
            if (std::rename(tmp_file_.c_str(), file_.c_str()))
                std::cerr << "Error in downloading finished: renaming downloaded file from " + tmp_file_ + " to " + file_ << std::endl;
            else
                // Send to process downloaded file to successor
                successor_->process_file(file_);
        }
        else
            std::cerr << "Error in downloading finished: downloaded file " + tmp_file_ + " is not complete!" << std::endl;
    }

    // delete the temporary file
    int clear_unfinished()
    {
        if (boost::filesystem::exists(tmp_file_))
            return std::remove(tmp_file_.c_str());
        else
            return 0;
    }

    void check_deadline()
    {
        if (conn_stopped_)
            return;

        // Check whether the deadline has passed. We compare the deadline against
        // the current time since a new asynchronous operation may have moved the
        // deadline before this actor had a chance to run.
        if (deadline_.expires_at() <= baio::deadline_timer::traits_type::now())
        {
            std::cout << "\nThe network seems to be very slow. Keep on trying?" << std::endl;
            std::cout << "[y - I'll wait / n - Cancel]" << std::endl;
            std::string response;
            std::cin >> response;

            if ((response == "n") || (response == "N"))
                stop_connection();
            else if ((response == "y") || (response == "Y"))
                // Set a new deadline
                deadline_.expires_from_now(boost::posix_time::seconds(30));
        }

        // Put the actor back to sleep.
        deadline_.async_wait(boost::bind(&URL::check_deadline, this));
    }

    Viewer&                 view_;

    const std::string       file_extens_   = ".download";
    const short             max_redirect_cnt_  = 3; // maximum redirection
    short                   act_redirect_cnt_     = 0; // counter of total redirections
    bool                    conn_stopped_{false}; // for fast indicating of closed connection in the async methods
    long                    len_;                 // Length of the file provided by server
    std::regex              expresion_{"^((http[s]?):\\/?\\/?)([^:\\/\\s]+)(.*\\/)(.*)$"};
    std::smatch             match_;

    baio::io_service        io_service_;
    tcp::resolver           resolver_{io_service_};
    tcp::socket             socket_{io_service_};
    baio::deadline_timer    deadline_{io_service_};
    baio::streambuf         request_;
    baio::streambuf         response_;

    std::string             server_, path_, file_, tmp_file_, protocol_;
    std::ofstream           ofs_;
};

#endif //PROCESSINGLOGS_URLSTRATEGY_H