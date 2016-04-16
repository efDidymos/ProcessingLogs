//
// Created by tomas on 3.4.2016.
//

#ifndef PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP
#define PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP

#include "RowInterface.hpp"
#include <map>

#ifndef NDEBUG
#include <chrono>
#endif

#include <sstream>

enum RequestMethod
{
    POST,
    GET,
    HEAD,
    UNKNOWN
};

class RequestMethodRowsStrategy: public RowInterface
{
public:
    RequestMethodRowsStrategy(std::ifstream *file, unsigned short &rowCount, RequestMethod requestMethod)
        :
        RowInterface(file, rowCount),
        requestedMethod(requestMethod)
    {
    }

    virtual RowInterface *Clone() const override
    {
        return new RequestMethodRowsStrategy(*this);
    }

    long read(long pos, const std::ios_base::seekdir &seekdir) override
    {
#ifndef NDEBUG
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
#endif
        std::string line;
        rows.clear();

        file->seekg(pos, seekdir);

        int i = 0;

        std::string c1, c2, c3, c4, c5, c6, c7, c8, c9;
        std::stringstream ss;

        do
        {
            if (getline(*file, line))
            {
                pos = file->tellg();

                ss << line;

                ss >> c1 >> c2 >> c3 >> c4 >> c5 >> c6 >> c7 >> c8 >> c9;

                ss.str(""); // erase the buffer

                c6 = c6.substr(1);  // Trim the Request method in read line

                // If in the specific founded group by regex
                // is the same string as desired and not UNKNOWN
                if (requestedMethod != UNKNOWN)
                {
                    if (c6 == type[requestedMethod])
                    {
                        rows.push_back(line);
                        i++;
                    }
                }
                else
                {
                    // If we want to find non different Request methods than
                    // the traditional POST, GET, HEAD
                    if ((c6 != type[POST]) && (c6 != type[GET]) && (c6 != type[HEAD]))
                    {
                        rows.push_back(line);
                        i++;
                    }
                }
            }
            else
            {
                pos = theEnd;
                break;
            }
        }
        while (i < rowCount);

#ifndef NDEBUG
        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of RequestMethodRowsStrategy=" << diff.count() << " --- " << std::endl;
#endif
        return pos;
    }

private:
    RequestMethod requestedMethod;

    std::map<RequestMethod, std::string> type{
        {POST, "POST"},
        {GET, "GET"},
        {HEAD, "HEAD"},
        {UNKNOWN, "UNKNOWN"},
    };
};


#endif //PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP
