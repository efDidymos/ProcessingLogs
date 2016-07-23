//
// Created by tomas on 3.4.2016.
//

#ifndef PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP
#define PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP

#include "IRow.hpp"
#include <map>

#ifndef NDEBUG
#include <chrono>
#endif

#include <sstream>

enum Request
{
    POST,
    GET,
    HEAD,
    UNKNOWN
};

/**
 * Class display only rows for specific request method
 */
class RequestMethod : public IRow
{
public:
    RequestMethod(std::ifstream *file, unsigned short rowCount, Request requestMethod) :
            IRow(file, rowCount), requestedMethod(requestMethod)
    { }

    void read(long *inPos,
              long *outPos,
              std::vector<std::string> *rows) override
    {
#ifndef NDEBUG
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
#endif
        std::string line;
        rows->clear();

        file->seekg(*inPos, std::ios_base::beg);

        int i = 0;

        std::string c1, c2, c3, c4, c5, c6;
        std::stringstream ss;

        do
        {
            if (getline(*file, line))
            {
                *outPos = file->tellg();

                ss << line;

                ss >> c1 >> c2 >> c3 >> c4 >> c5 >> c6;

                ss.str(""); // erase the buffer

                c6 = c6.substr(1);  // Trim the Request method in read line

                // ========================================================
                // COMPARISON OF REQUESTS
                // ========================================================
                switch (requestedMethod)
                {
                    case UNKNOWN:
                        // If we want to find non different Request methods than
                        // the traditional POST, GET, HEAD
                        if (std::find(
                                type[UNKNOWN].begin(),
                                type[UNKNOWN].end(),
                                c6)
                            ==
                            type[UNKNOWN].end())
                        {
                            rows->push_back(line);
                            ++i;
                        }
                        break;

                    default:
                        // If in the specific founded group by regex
                        // is the same string as desired and not UNKNOWN
                        if (std::find(
                                type[requestedMethod].begin(),
                                type[requestedMethod].end(),
                                c6)
                            !=
                            type[requestedMethod].end())
                        {
                            rows->push_back(line);
                            ++i;
                        }
                }
            }
            else
            {
                *outPos = theEnd;
                break;
            }
        }
        while (i < rowCount);

#ifndef NDEBUG
        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of RequestMethod=" << diff.count() << "\n";
#endif
    }

private:
    Request requestedMethod;

    std::map<Request, std::vector<std::string>> type{
            {POST,    {"POST"}},
            {GET,     {"GET"}},
            {HEAD,    {"HEAD", "Head"}},
            {UNKNOWN, {"POST", "GET", "HEAD", "Head"}}  // The aim of this is to provide some kind of mask
                                                        // which will be used in the COMPARISON OF REQUESTS METHOD
                                                        // in do while cycle part
    };
};


#endif //PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP
