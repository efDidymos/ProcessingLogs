//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_THELOG_HPP
#define PROCESSINGLOGS_THELOG_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <vector>

#include "rowsFilteringStrategies/RowInterface.hpp"

class Log
{
public:
    Log(std::ifstream *file)
    {
        positionAtLadder.push_back(0);

        file->seekg(0, std::ios::end);
        theEnd = file->tellg();
    }

    virtual ~Log()
    {
        std::cout << "Joining threads" << std::endl;

        {
            std::lock_guard<std::mutex> lock(m_mutex);                // Enter critical section
            work = true;
            running = false;
        }
        m_alarm.notify_one();


        if (subThread->joinable())
            subThread->join();

        delete subThread;
        delete mainThread;
    }

    void setDisplayRowStrategy(RowInterface *strategy)
    {
        positionAtLadder.resize(1);

        if (subThread != nullptr)
        {
            std::cout << "Joining the SUB-THREAD for change of strategy" << std::endl;

            {
                std::lock_guard<std::mutex> lock(m_mutex);                // Enter critical section
                work = true;
                running = false;
            }
            m_alarm.notify_one();

            if (subThread->joinable())
                subThread->join();

            delete subThread;

            running = true;
        }

        // Main thread will do part of the work too
        mainThread = strategy;
        long entryPos = mainThread->readRows(positionAtLadder.back(), currRows);
        positionAtLadder.push_back(entryPos);
        data.push_back(currRows);

        // Creating sub-thread
        std::cout << "\n --- Creating SUB-THREAD --- \n";
        subThread = strategy->Clone()->createSubThread(positionAtLadder, nextRows, work, running, m_mutex, m_alarm);
    }

    void showNextRows()
    {
        // Boundary check for not over jump the end of the file
        if (positionAtLadder.back() != theEnd)
        {
            std::unique_lock<std::mutex> lock(m_mutex);                // Enter critical section
            while (work)
            {
                std::cout << "\n Main-Waitting showNextRows... \n";
                m_alarm.wait(lock);
            }

            // data list will hold only a three lists
            if (data.size() == 3)
                data.pop_front();

            data.push_back(nextRows);

            std::swap(currRows, nextRows);
            lock.unlock();

            showCurrRows();
        }
    }

    void showPrevRows()
    {
        // Boundary check for not over jump the begin of the file
        if (positionAtLadder.end()[-2] != 0)
        {
            std::cout << "\nPRE-showPrevRows = " << positionAtLadder.back() << "\n";

            std::unique_lock<std::mutex> lock(m_mutex);                // Enter critical section
            while (work)
            {
                std::cout << "\n Main-Waitting showPrevRows... \n";
                m_alarm.wait(lock);
            }

            // Climb up three steps at the ladder
            positionAtLadder.pop_back();
            positionAtLadder.pop_back();
            positionAtLadder.pop_back();

            // Also climb back three steps in the data list
            // which means to the first element
            data.resize(1);

            lock.unlock();
            m_alarm.notify_one();

            showCurrRows();
        }
        else
            printRows();
    }

    void showCurrRows()
    {
        std::cout << "\nPRE-showCurrRows = " << positionAtLadder.back() << "\n";

        printRows();

        std::unique_lock<std::mutex> lock(m_mutex);                // Enter critical section
        while (work)
        {
            std::cout << "\n Main-Waitting showCurrRows... \n";
            m_alarm.wait(lock);
        }

        work = true;

        lock.unlock();
        m_alarm.notify_one();

        std::cout << "\nPOST-showCurrRows = " << positionAtLadder.back() << "\n";
    }

    void printRows() const
    {
        for (auto row : data.back())
            std::cout << row << std::endl;
    }

private:
    std::vector<long> positionAtLadder;
    long theEnd;

    bool work = false;
    bool running = true;

    std::mutex m_mutex;
    std::condition_variable m_alarm;

    RowInterface *mainThread = nullptr;
    std::thread *subThread = nullptr;

    // Two lists serves as buffers for main and sub thread
    std::list<std::string> currRows;    // main thread will allways show this buffer
    std::list<std::string> nextRows;    // sub thread will allways write to this buffer

    // List serves as data history. It holds at most three
    // elements (due ensuring high performance)
    // [P] revious - previously readed list of rows
    // [C] current - currRows
    // [N] ext     - nextRows
    std::list<std::list<std::string>> data;
};


#endif //PROCESSINGLOGS_THELOG_HPP
