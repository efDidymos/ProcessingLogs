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
        positionAtLadder.push_back(pos2Read);

        file->seekg(0, std::ios::end);
        theEnd = file->tellg();
    }

    virtual ~Log()
    {
        delete prevRows;
        delete currRows;
        delete nextRows;

        std::cout << "Joining threads" << std::endl;

        {
            std::lock_guard<std::mutex> lock(m_mutex);                // Enter critical section
            work = true;
            running = false;
        }
        m_alarm.notify_one();


        if (t->joinable())
            t->join();

        delete tempRows;
        delete t;
    }

    void setDisplayRowStrategy(RowInterface *strategy)
    {
        positionAtLadder.resize(1);

        if (prevRows != nullptr)
            delete prevRows;

        if (currRows != nullptr)
            delete currRows;

        if (nextRows != nullptr)
            delete nextRows;

        if (tempRows != nullptr)
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

            delete tempRows;
            delete t;

            running = true;
        }

        prevRows = strategy;
        currRows = strategy->Clone();
        nextRows = strategy->Clone();

        tempRows = strategy->Clone();

        // Creating sub-thread
        std::cout << "\n --- Creating SUB-THREAD --- \n";
        t = tempRows->threadHello(positionAtLadder, work, show, running, m_mutex, m_alarm);
    }

    // Firstime print rows
    void printRows()
    {
//        long currPos = currRows->read(positionAtLadder.back(), std::ios_base::beg);
//        positionAtLadder.push_back(currPos);
//
//        long nextPos = nextRows->read(currPos, std::ios_base::beg);
//        positionAtLadder.push_back(nextPos);

        showCurrRows();
    }

    void showNextRows()
    {
        std::cout << "\nPRE-showNextRows = " << positionAtLadder.back() << "\n";
        std::unique_lock<std::mutex> lock(m_mutex);                // Enter critical section
        while (work)
        {
            std::cout << "\n Main-Waitting showNextRows... \n";
            m_alarm.wait(lock);
        }
//        pos2Read = positionAtLadder.back();
        work = true;
        lock.unlock();
        m_alarm.notify_one();

        std::cout << "\nPOST-showNextRows = " << positionAtLadder.back() << "\n";
//        swapToNextRows();
        showCurrRows();
    }

    void showPrevRows()
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

//        pos2Read = positionAtLadder.back();
        work = true;
        lock.unlock();
        m_alarm.notify_one();

        std::cout << "\nPOST-showPrevRows = " << positionAtLadder.back() << "\n";

//        swapToPrevRows();
        showCurrRows();
    }

private:
//    void showCurrRows() const
    void showCurrRows()
    {
//        std::cout << std::endl << currRows;

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

    // Just switch the pointers.
    void swapToNextRows()
    {
        // Boundary check for not over jump the end of the file
        if (positionAtLadder.back() != theEnd)
        {
            auto temp = prevRows;
            prevRows = currRows;
            currRows = nextRows;
            nextRows = temp;

            long nextPos = nextRows->read(positionAtLadder.back(), std::ios_base::beg);
            positionAtLadder.push_back(nextPos);
        }
    }

    // Just switch the pointers.
    void swapToPrevRows()
    {
        // Boundary check for not over jump the begin of the file
        if (positionAtLadder.end()[-2] != 0)
        {
            auto temp = nextRows;
            nextRows = currRows;
            currRows = prevRows;
            prevRows = temp;

            // Climb up one setp at the ladder
            positionAtLadder.pop_back();

            // Climb up three setps only for looking
            prevRows->read(positionAtLadder.end()[-3], std::ios_base::beg);
        }
    }

    std::vector<long> positionAtLadder;
    long pos2Read = 0;
    long theEnd;

    // Three buffers
    RowInterface *prevRows = nullptr;
    RowInterface *currRows = nullptr;
    RowInterface *nextRows = nullptr;

    RowInterface *tempRows = nullptr;

    bool work = true;
    bool running = true;
    bool show = false;
    std::mutex m_mutex;
    std::condition_variable m_alarm;
    std::thread * t;
};


#endif //PROCESSINGLOGS_THELOG_HPP
