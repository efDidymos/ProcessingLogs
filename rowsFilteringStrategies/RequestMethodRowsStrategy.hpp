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
    RequestMethodRowsStrategy(std::ifstream *file,
                              unsigned short rowCount,
                              RequestMethod requestMethod)
        :
        RowInterface(file, rowCount),
        requestedMethod(requestMethod)
    {
    }

    virtual RowInterface *Clone() const override
    {
        return new RequestMethodRowsStrategy(*this);
    }

    long read(long pos, const std::ios_base::seekdir seekdir) override
    {
        using namespace std::chrono;
        auto start = high_resolution_clock::now();

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

        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of RequestMethodRowsStrategy=" << diff.count() << " --- " << std::endl;

        printProgBar((double) pos);

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

    virtual ~RequestMethodRowsStrategy()
    {
        std::cout << "\n --- DESTRUCTOR ~RequestMethodRowsStrategy" << std::endl;
    }

    void hello(bool &work, bool &running, std::mutex &m_mutex, std::condition_variable &m_alarm) override
    {
        std::cout << "\n --- Created new thread--- \n";

        std::unique_lock<std::mutex> lock(m_mutex);
        while (!work)
        {
            if (running == false)
            {
                std::cout << "\n ENDING WORK \n";
                break;
            }

            m_alarm.wait(lock);

            std::cout << "\n ----------------- HELLO FROM RequestMethodRowsStrategy -----------------\n";
            work = false;
        }

        std::cout << "\n ENDED \n";
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
