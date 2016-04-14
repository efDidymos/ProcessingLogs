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

    long readRows(long pos, std::vector<std::string> &rowStack) override
    {
        using namespace std::chrono;
        auto start = high_resolution_clock::now();

        std::string line;
//        rows.clear();
        rowStack.clear();

        file->seekg(pos, std::ios_base::beg);

        for (int i = 0; i < rowCount; i++)
        {
            if (getline(*file, line))
            {
//                rows.push_back(line);
                rowStack.push_back(line);
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

    void hello(std::vector<long> &positionAtLadder,
                   std::vector<std::string> &rowStack,
                   bool &work,
                   bool &running,
                   std::mutex &m_mutex,
                   std::condition_variable &m_alarm) override
    {
        std::cout << "\n --- Created new thread :: " << work << "--- \n";

        while (running)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            while (!work)
            {
                std::cout << "\n SUB-Waitting ... \n";
                m_alarm.wait(lock);
            }

            std::cout << "\n READ ROWS \n";
            long newPos = readRows(positionAtLadder.back(), rowStack);
            if (newPos != theEnd)   // check for not over jumping through boundary of end file
                positionAtLadder.push_back(newPos);
            work = false;

            lock.unlock();
            m_alarm.notify_one();
        }

        std::cout << "\n ENDED \n";
    }
};


#endif //PROCESSINGLOGS_ROWS_HPP
