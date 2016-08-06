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
 * Class intended as model representation of the log file.
 * There are various scenarious of reading
 * Legend:
 *  p - prevRow
 *  c - currRow
 *  n - nextRow
 *                  +---+
 *                  | p | <-- at the beginning it is empty
 * BEGIN OF FILE ---+---+--+---+------------------------------
 *                  | c |  | p |
 *                  +---+  +---+  +---+
 *                  | n |  | c |  | p |
 *                  +---+  +---+  +---+  +---+
 *                         | n |  | c |  | p |
 *                         +---+  +---+  +---+  +---+
 *                                | n |  | c |  | p |
 *                                +---+  +---+  +---+
 *                                       | n |  | c | <-----+
 * END OF FILE --------------------------+---+--+---+----   |
 *                                              | n | ------+ at the end of readling it holds the same data as
 *                                              +---+
 */
class Log
{
public:
    Log(std::ifstream *file,
        Viewer &view,
        std::unique_ptr<IRow> strategy)
        : view_(view), strategy_(std::move(strategy))
    {
        // Firstly find the last position of the file
        // to get end of the file for later boundary checking
        file->seekg(0, std::ios::end);
        theEnd_ = file->tellg();

        // Perform the first time reading chunk of the file
        firstTimeRead();

        // Prepare for activation of sub-thread
        startPos_    = index_.back();

        // Creating sub-thread
        subThread_ = std::thread(&Log::read2cache, this);
    }

    virtual ~Log()
    {
        shutdownThread();
    }

    /**
     * Changes the way of reading file
     * @param st
     */
    void changeDisplayRowStrategy(std::unique_ptr<IRow> st)
    {
        prevRows_.clear();           // Clear previous recorded results to begin a new filtering
        index_.resize(1);            // Also resize the index to hold beginning position of the file -> zero 0
        startPos_    = index_.back(); // Prepare for activation of read in main-thread
        strategy_    = std::move(st);

        // Perform the first time reading chunk of the file
        firstTimeRead();

        shutdownThread();

        // Prepare for activation of read in sub-thread
        work_        = true;
        running_     = true;
        forward_     = true;
        startPos_    = index_.back();

        // Creating sub-thread
        subThread_ = std::thread(&Log::read2cache, this);
    }

    /**
     * Print the actual rows
     */
    void showCurrRows() const
    {
        for (auto row : currRows_)
            std::cout << row << std::endl;

        view_.printHorizontalLine();
        view_.printProgBar("\n File read", (double) index_.back(), theEnd_);
        view_.printCmdMenu();
    }

    void getNextRows()
    {
        std::lock_guard<std::mutex> lock(mutex_);  // Enter critical section

        // Boundary check to not overjump the end of the file
        if (index_.back() != theEnd_)
        {
            startPos_ = index_.back();
            prevRows_ = currRows_;
            currRows_ = nextRows_;
            work_     = true;
            forward_  = true;
            alarm_.notify_one();
        }
        else if (nextRows_.size() > 0)
            currRows_ = nextRows_;
    }

    void getPrevRows()
    {
        std::lock_guard<std::mutex> lock(mutex_);  // Enter critical section

        // Boundary check to not overjump the beginning of the file
        if ((startPos_ != 0) && (prevRows_.size() > 0))
        {
            index_.pop_back();
            startPos_ = index_.rbegin()[3];
            nextRows_ = currRows_;
            currRows_ = prevRows_;
            work_     = true;
            backward_ = true;
            alarm_.notify_one();
        }
        else if (prevRows_.size() > 0)
            currRows_ = prevRows_;
    }

private:
    /**
     * Read from the file to cache and assign it to target vector
     */
    void read2cache()
    {
        while (1)
        {
            std::unique_lock<std::mutex> lock(mutex_); // Enter critical section
            while (!work_)
                alarm_.wait(lock);

            if (!running_)
                return;

            long outPos = 0;
            strategy_->read(&startPos_, &outPos, &cacheRows_);

            // Decide where to assign results
            if (forward_)
            {
                nextRows_ = cacheRows_;
                forward_  = false;
            }
            else if (backward_)
            {
                prevRows_ = cacheRows_;
                backward_ = false;
            }

            // Check if the address of the newly read chunk of file was not already processed
            if (std::find(index_.begin(),
                          index_.end(),
                          outPos)
                ==
                index_.end())
            {
                index_.push_back(outPos);
            }
            work_ = false;
        }
    }

    /**
     * Read chunk from the beginning of the file
     */
    void firstTimeRead()
    {
        long outPos = 0;
        strategy_->read(&startPos_, &outPos, &currRows_);
        index_.push_back(outPos);
    }

    /**
     * Wakes up the sleeping thread to let it finish and end
     */
    void shutdownThread()
    {
        if (subThread_.joinable())
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);  // Enter critical section
                work_    = true;
                running_ = false;
                alarm_.notify_one();
            }
            subThread_.join();
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
    std::vector<long> index_ = {0};

    // Definition of the starting position for reading into the cacheRows_ from the sub-thread. It can hold
    // position value from the index vector to start reading before the prevRows_ or after nextRows_
    long startPos_           = 0;
    long theEnd_             = 0;    // The last position of the log file. Var intended only to boundary check.
    bool work_               = true; // Switch indicates for the sub-thread if it have to do something new now
    bool running_            = true; // Switch indicates for the sub-thread if it have to run
    bool forward_            = true; // Switch indicates that the cached rows have to be added to the nextRows_ vector
    bool backward_;                  // Switch indicates that the cached rows have to be added to the prevRows_ vector

    Viewer&                  view_;

    // Three buffers for holding data
    std::vector<std::string> prevRows_;  // Vector stores previous read lines of the file
    std::vector<std::string> currRows_;  // Vector stores current lines of the file intended to be displayed
    std::vector<std::string> nextRows_;  // Vector stores next read lines of the file

    // Vector aimed for caching read data.
    // It's content always copy to prevRows_ or nextRows_
    std::vector<std::string> cacheRows_;

    std::unique_ptr<IRow>    strategy_;     // Type of strategy that will be used for manipulating with the file

    // for multi-threading
    std::thread              subThread_;
    std::mutex               mutex_;
    std::condition_variable  alarm_;
};


#endif //PROCESSINGLOGS_THELOG_HPP
