//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_ROWS_HPP
#define PROCESSINGLOGS_ROWS_HPP

#include "RowInterface.hpp"
#include <chrono>

class AllRowsStrategy: public RowInterface
{
public:
    AllRowsStrategy(std::ifstream *file, unsigned short int rowCount)
        : RowInterface(file, rowCount)
    {
    }

    virtual RowInterface *Clone() const override
    {
        return new AllRowsStrategy(*this);
    }

    long read(long pos, const std::ios_base::seekdir seekdir) override
    {
        using namespace std::chrono;
        auto start = high_resolution_clock::now();

        std::string line;
        rows.clear();

        file->seekg(pos, seekdir);

        for (int i = 0; i < rowCount; i++)
        {
            if (getline(*file, line))
            {
                rows.push_back(line);
                pos = file->tellg();
            }
            else
            {
                pos = theEnd;
                break;
            }
        }

        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of AllRowsStrategy=" << diff.count() << std::endl;

        return pos;
    }

    virtual ~AllRowsStrategy()
    {
        std::cout << "\n --- DESTRUCTOR ~AllRowsStrategy" << std::endl;
    }

    void hello(bool &work, bool &show, bool &running, std::mutex &m_mutex, std::condition_variable &m_alarm) override
    {
        std::cout << "\n --- Created new thread--- \n";

        std::unique_lock<std::mutex> lock(m_mutex);
        while (!work)
        {
            m_alarm.wait(lock);

            if (!running)
            {
                std::cout << "\n ENDING WORK \n";
                break;
            }
            else
                std::cout << "\n DO WORK \n";

            if (show)
                std::cout << "\n SHOWING RESULTS \n";
            else
                std::cout << "\n WAITING FOR SHOWING RESULTS \n";

            std::cout << "\n ----------------- HELLO FROM AllRowsStrategy -----------------\n";
            work = false;
            show = false;
        }

        std::cout << "\n ENDED \n";
    }
};


#endif //PROCESSINGLOGS_ROWS_HPP
