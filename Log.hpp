//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_THELOG_HPP
#define PROCESSINGLOGS_THELOG_HPP

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

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


        if (t->joinable())
            t->join();

        delete t;
    }

    void setDisplayRowStrategy(RowInterface *strategy)
    {
        positionAtLadder.resize(1);

        if (t != nullptr)
        {
            std::cout << "Joining the SUB-THREAD for change of strategy" << std::endl;

            {
                std::lock_guard<std::mutex> lock(m_mutex);                // Enter critical section
                work = true;
                running = false;
            }
            m_alarm.notify_one();

            if (t->joinable())
                t->join();

            delete t;

            running = true;
        }

        // Creating sub-thread
        std::cout << "\n --- Creating SUB-THREAD --- \n";
        t = strategy->threadHello(positionAtLadder, work, read, show, running, m_mutex, m_alarm);
//        tB = strategy->Clone()->threadHello(positionAtLadder, work, read, show, running, m_mutex, m_alarm);
    }

    void showNextRows()
    {
        // Boundary check for not over jump the end of the file
        if (positionAtLadder.back() != theEnd)
        {
            std::cout << "\nPRE-showNextRows = " << positionAtLadder.back() << "\n";
            std::unique_lock<std::mutex> lock(m_mutex);                // Enter critical section
            while (work)
            {
                std::cout << "\n Main-Waitting showNextRows... \n";
                m_alarm.wait(lock);
            }
            work = true;
            read = true;

            lock.unlock();
            m_alarm.notify_one();

            std::cout << "\nPOST-showNextRows = " << positionAtLadder.back() << "\n";
//        swapToNextRows();
        }
        showCurrRows();
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

            // Climb up one setp at the ladder
            positionAtLadder.pop_back();
            positionAtLadder.pop_back();

            work = true;
            read = true;

            lock.unlock();
            m_alarm.notify_one();

            std::cout << "\nPOST-showPrevRows = " << positionAtLadder.back() << "\n";

//        swapToPrevRows();
        }
        showCurrRows();
    }

    void showCurrRows()
    {
        std::cout << "\nPRE-showCurrRows = " << positionAtLadder.back() << "\n";

        std::unique_lock<std::mutex> lock(m_mutex);                // Enter critical section
        while(work)
        {
            std::cout << "\n Main-Waitting showCurrRows... \n";
            m_alarm.wait(lock);
        }
        work = true;
        show = true;

        lock.unlock();
        m_alarm.notify_one();
        std::cout << "\nPOST-showCurrRows = " << positionAtLadder.back() << "\n";
    }
private:
    std::vector<long> positionAtLadder;
    long theEnd;

    bool work = true;
    bool read = true;
    bool show = false;
    bool running = true;

    std::mutex m_mutex;
    std::condition_variable m_alarm;

    std::thread * t = nullptr;
};


#endif //PROCESSINGLOGS_THELOG_HPP
