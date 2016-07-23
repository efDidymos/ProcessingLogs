//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_THELOG_HPP
#define PROCESSINGLOGS_THELOG_HPP

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include "rowsFilteringStrategies/IRow.hpp"

/**
 * Class intended as model representation of the log file
 */
class Log
{
public:
    Log(std::ifstream *file,
        Viewer &view,
        std::shared_ptr<IRow> strategy)
        : view(view), strategy(strategy)
    {
        // Firstly find the last position of the file
        // to get end of the file for later boundary checking
        file->seekg(0, std::ios::end);
        theEnd = file->tellg();

        // Perform the first time reading chunk of the file
        firstTimeRead();

        // Prepare for activation of sub-thread
        startPos    = index.back();

        // Creating sub-thread
        subThread = std::thread(&Log::read2cache, this);
    }

    virtual ~Log()
    {
        shutdownThread();
    }

    /**
     * Changes the way of reading file
     * @param st
     */
    void changeDisplayRowStrategy(std::shared_ptr<IRow> st)
    {
        strategy    = st;
        startPos    = 0;

        // Perform the first time reading chunk of the file
        firstTimeRead();

        shutdownThread();

        // Prepare for activation of sub-thread
        work    = true;
        running = true;
        forward = true;

        // Creating sub-thread
        subThread = std::thread(&Log::read2cache, this);
    }

    /**
     * Print the actual rows
     */
    void showCurrRows() const
    {
        for (auto row : currRows)
            std::cout << row << std::endl;

        view.printHorizontalLine();
        view.printProgBar("\n File read", (double) index.back(), theEnd);
        view.printCmdMenu();
    }

    void getNextRows()
    {
        std::lock_guard<std::mutex> lock(m_mutex);  // Enter critical section

        // Boundary check to not overjump the end of the file
        if (index.back() != theEnd)
        {
            startPos = index.back();
            currRows = nextRows;
            work     = true;
            forward  = true;
            m_alarm.notify_one();
        }
        else
            currRows = nextRows;
    }

    void getPrevRows()
    {
        std::lock_guard<std::mutex> lock(m_mutex);  // Enter critical section

        // Boundary check to not overjump the beginning of the file
        if (startPos != 0)
        {
            index.pop_back();
            startPos = index.rbegin()[3];
            currRows = prevRows;
            work     = true;
            backward = true;
            m_alarm.notify_one();
        }
        else
            currRows = prevRows;
    }

private:
    /**
     * Read from the file to cache and assign it to target vector
     */
    void read2cache()
    {
        while (running)
        {
            std::unique_lock<std::mutex> lock(m_mutex); // Enter critical section
            while (!work)
                m_alarm.wait(lock);

            long outPos = 0;
            strategy->read(&startPos, &outPos, &cacheRows);

            // Decide where to assign results
            if (forward)
            {
                prevRows = currRows;
                nextRows = cacheRows;
                forward  = false;
            }
            else if (backward)
            {
                nextRows = currRows;
                prevRows = cacheRows;
                backward = false;
            }

            // Check if the address of the newly read chunk of file was not already processed
            if (std::find(index.begin(),
                          index.end(),
                          outPos)
                ==
                index.end())
            {
                index.push_back(outPos);
            }
            work = false;
        }
    }

    /**
     * Read chunk from the beginning of the file
     */
    void firstTimeRead()
    {
        long outPos = 0;
        strategy->read(&startPos, &outPos, &currRows);
        index.push_back(outPos);
    }

    /**
     * Wakes up the sleeping thread to let it finish and end
     */
    void shutdownThread()
    {
        if (subThread.joinable())
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);  // Enter critical section
                work    = true;
                running = false;
                m_alarm.notify_one();
            }
            subThread.join();
        }
    }

    // The vector holds indexed start positions for the appropriate chunks of the file
    // [    0] - holds index to beginning of the file. Application start
    //           reading from this position.
    // [4 016] - After read first chunk of the file the position where reading finished is
    //           added to index to indicate the next starting position for reading
    // [7 504] - And after reading second chunk of the file the ending reading position is
    //           again added to the index indicating next starting position for reading
    // And so on ................
    std::vector<long> index = {0};

    // Definition of the starting position for reading into the cacheRows from the sub-thread. It can hold
    // position value from the index vector to start reading before the prevRows or after nextRows
    long startPos  = 0;
    long theEnd             = 0;    // The last position of the log file. Var intended only to boundary check.
    bool work               = true; // Switch indicates for the sub-thread if it have to do something new now
    bool running            = true; // Switch indicates for the sub-thread if it have to run
    bool forward            = true; // Switch indicates that the cached rows have to be added to the nextRows vector
    bool backward;                  // Switch indicates that the cached rows have to be added to the prevRows vector

    Viewer &view;

    // Three buffers for holding data
    std::vector<std::string> prevRows;  // Vector stores previous read lines of the file
    std::vector<std::string> currRows;  // Vector stores current lines of the file intended to be displayed
    std::vector<std::string> nextRows;  // Vector stores next read lines of the file

    // Vector aimed for caching read data.
    // It's content always copy to prevRows or nextRows
    std::vector<std::string> cacheRows;

    std::shared_ptr<IRow> strategy;     // Type of strategy that will be used for manipulating with the file

    // for multi-threading
    std::thread             subThread;
    std::mutex              m_mutex;
    std::condition_variable m_alarm;
};


#endif //PROCESSINGLOGS_THELOG_HPP
