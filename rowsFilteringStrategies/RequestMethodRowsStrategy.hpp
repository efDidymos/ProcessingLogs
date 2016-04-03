//
// Created by tomas on 3.4.2016.
//

#ifndef PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP
#define PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP

#include <iostream>
#include "RowInterface.hpp"

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
    RequestMethodRowsStrategy(std::ifstream *file,
                              unsigned short rowCount,
                              RequestMethod requestMethod)
        :
        RowInterface(file, rowCount),
        requestedMethod(requestMethod),
        e(R"delim(^(.*?)\[(\d{2})\/(.{3})\/(\d{4})(.*?)"(.*?)\s(.*?)"\s(\d{3}))delim")
    {
    }

    long read(long pos, const std::ios_base::seekdir seekdir) override
    {
        std::string line;
        rows.clear();

        std::smatch m;

        file->seekg(pos, seekdir);

        int i = 0;

        do
        {
            getline(*file, line);

            if (line == "")
                break;
            else
            {
                // Search line for suitable format
                if (std::regex_search(line, m, e))
                {
                    // If we want to find non different Request methods than
                    // the traditional POST, GET, HEAD
                    if (requestedMethod == UNKNOWN)
                    {
                        // If in the specific founded group by regex
                        // is the same string as desired
                        if ((m[6] != type[POST]) && (m[6] != type[GET]) && (m[6] != type[HEAD]))
                        {
                            rows.push_back(line);
                            pos = file->tellg();
                            i++;
                        }
                    }
                    else
                    {
                        // If in the specific founded group by regex
                        // is the same string as desired
                        if (m[6] == type[requestedMethod])
                        {
                            rows.push_back(line);
                            pos = file->tellg();
                            i++;
                        }
                    }
                }
                else    // If regex did not find suitable line
                    continue;
            }
        }
        while (i < rowCount);

        return pos;
    }

private:
    const std::regex e;
    RequestMethod requestedMethod;

    std::map<RequestMethod, std::string> type{
        {POST, "POST"},
        {GET, "GET"},
        {HEAD, "HEAD"},
        {UNKNOWN, "UNKNOWN"},
    };
};


#endif //PROCESSINGLOGS_REQUESTMETHODROWSSTRATEGY_HPP
