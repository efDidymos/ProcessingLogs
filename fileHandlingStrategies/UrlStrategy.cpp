//
// Created by tomas on 28.3.2016.
//

#include "UrlStrategy.h"
#include "../Viewer.hpp"

#include <boost/asio.hpp>
#include <future>

void UrlStrategy::execute()
{
    // Check if the argument is some type of URL
    if (std::regex_match(url, match, expresion))
    {
        std::string result = get_http_data(match[3],
                                           match[4],
                                           match[5],
                                           match[2]);

        std::cout << result << std::endl;
    }
    else
    {
        std::cerr << "!NOT Valid URL" << std::endl;
    }
}

std::string UrlStrategy::get_http_data(std::string server, std::string path, std::string file, std::string protocol)
{
    try
    {
        std::cout << "SERVER: " << server << " PATH: " << path << " FILE: " << file << std::endl;

        boost::asio::ip::tcp::iostream s(server, protocol);

        if (!s)
            throw "Unable to connect: " + s.error().message();

        // ask for the file
        s << "GET " << path.append(file) << " HTTP/1.0\r\n";
        s << "Host: " << server << "\r\n";
        s << "Accept: */*\r\n";
        s << "Connection: close\r\n\r\n";

        // Check that response is OK.
        std::string http_version;
        s >> http_version;

        unsigned int status_code;
        s >> status_code;

        std::string status_message;
        std::getline(s, status_message);

        if (!s && http_version.substr(0, 5) != "HTTP/")
            throw "Invalid response";

        if (status_code == 302)
        {
            // Process the response headers, which are terminated by a blank line.
            std::string header;
            std::string pom;
            while (std::getline(s, header) && header != "\r")
            {
                if (header.substr(0, 9) == "Location:")
                {
                    pom = header.substr(10);
                    if (auto pos = pom.find("\r"))
                        pom = pom.substr(0, pos);
//                    break;
                }
            }

            // Produce second dowload
            if (std::regex_match(pom, match, expresion))
            {
                std::string result = get_http_data(match[3],
                                                   match[4],
                                                   match[5],
                                                   match[2]);

                std::cout << "\n returned from recursion \n";

                std::cout << result << std::endl;
            }
            else
                throw "NOT Valid URL in redirection";
        }
        else if (status_code != 200)
            throw "Response returned with status code " + status_code;
        else
        {
            std::cout << "saving " << std::endl;

            // Process the response headers, which are terminated by a blank line.
            std::string header, length;
            while (std::getline(s, header) && header != "\r")
            {
                std::cout << header << "\n";
                if (header.substr(0, 15) == "Content-Length:")
                {
                    length = header.substr(16);
                    if (auto pos = length.find("\r"))
                        length = length.substr(0, pos);
                }
            }
            std::cout << "\n";

            std::cout << "writting " << std::endl;

            std::ofstream ofs(file.append(".download"), std::ios_base::app);

            // Run in paralel
//            std::async(std::launch::async, [&] { showProgress(&ofs, std::stol(length)); });

            ofs << s.rdbuf();

            ofs.close();

            auto pos = file.find(".download");

            // Remove the .dowload extension from name of the file
            int rc = std::rename(file.c_str(), file.substr(0, pos).c_str());
            if (rc)
                throw "Error renaming downloaded file " + file + " to " + file.substr(0, pos);
        }
    }
    catch (const std::exception &e)
    {
        return e.what();
    }
}

void UrlStrategy::showProgress(std::ofstream *file, long length)
{
    Viewer view;

    double currSize = file->tellp();

    while (currSize != length)
    {
        view.printProgBar(currSize, length);
        sleep(5);
        currSize = file->tellp();
        std::cout << "size: " << currSize << std::endl;
    }
}

