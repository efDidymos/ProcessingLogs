//
// Created by tomas on 3.4.2016.
//

#ifndef PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP
#define PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP

#include "RowInterface.hpp"

#include <iostream>
#include <map>

#include <chrono>
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
        file->seekg(0, std::ios::end);
        theEnd = file->tellg();
    }

    virtual RowInterface *Clone() const override
    {
        return new RequestMethodRowsStrategy(*this);
    }

    long &read(long &pos, const std::ios_base::seekdir &seekdir) override
    {
        using namespace std::chrono;
        auto start = high_resolution_clock::now();

        std::string line;
        rows.clear();

        file->seekg(pos, seekdir);

        int i = 0;

        std::string c1, c2, c3, c4, c5, c6, c7, c8, c9;

        do
        {
            getline(*file, line);

            if ((line != ""))
            {
                pos = file->tellg();

                std::istringstream ss(line);

                ss >> c1
                    >> c2
                    >> c3
                    >> c4
                    >> c5
                    >> c6
                    >> c7
                    >> c8
                    >> c9;

                c6 = c6.substr(1);  // Trim the

                // If in the specific founded group by regex
                // is the same string as desired and not UNKNOWN
                if (requestedMethod != UNKNOWN)
                {
                    if (c6 == type[requestedMethod])
                    {
                        rows.push_back(line);
//                        pos = file->tellg();
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
//                        pos = file->tellg();
                        i++;
                    }
                }
            }
            else
                break;
        }
        while (i < rowCount);

        printProgBar((double) pos);

        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of RequestMethodRowsStrategy=" << diff.count() << std::endl;

        return pos;
    }

    void printProgBar(double value)
    {
        std::string bar;
        int percent = ((value / theEnd) * 100);

        for (int i = 0; i < 50; i++)
        {
            if (i < (percent / 2))
            {
                bar.replace(i, 1, "=");
            }
            else if (i == (percent / 2))
            {
                bar.replace(i, 1, ">");
            }
            else
            {
                bar.replace(i, 1, " ");
            }
        }

        std::cout << "\r  [" << bar << "] " << percent << "%    " << std::flush;
    }

private:
    RequestMethod requestedMethod;

    std::map<RequestMethod, std::string> type{
        {POST, "POST"},
        {GET, "GET"},
        {HEAD, "HEAD"},
        {UNKNOWN, "UNKNOWN"},
    };

    long theEnd;
};


#endif //PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP
