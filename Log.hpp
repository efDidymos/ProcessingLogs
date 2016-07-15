//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_THELOG_HPP
#define PROCESSINGLOGS_THELOG_HPP

#include <fstream>
#include <vector>
#include <thread>
#include "rowsFilteringStrategies/IRow.hpp"

class Log
{
public:
    Log(std::ifstream *file, Viewer &view)
        : view(view)
    {
        positionAtLadder.push_back(0);

        file->seekg(0, std::ios::end);
        theEnd = file->tellg();
    }

    void setDisplayRowStrategy(std::shared_ptr<IRow> strategy)
    {
        positionAtLadder.resize(1);

        // Starting thread and show animation of cycling cursor
        bool work = true;
        std::thread thread(Viewer::waitAnim, std::ref(work));

        prevRows = strategy;
        currRows = strategy->Clone();
        nextRows = strategy->Clone();

        long currPos = currRows->read(positionAtLadder.back(), std::ios_base::beg);
        positionAtLadder.push_back(currPos);

        showCurrRows();

        long nextPos = nextRows->read(currPos, std::ios_base::beg);
        positionAtLadder.push_back(nextPos);

        // Turn off animation
        work = false;
        if (thread.joinable())
            thread.join();
    }

    void showCurrRows() const
    {
        std::cout << "\n" << currRows;

        view.printHorizontalLine();
        view.printProgBar("\n File read", (double) positionAtLadder.back(), theEnd);
        view.printCmdMenu();
    }

    void showNextRows()
    {
        // Starting thread and show animation of cycling cursor
        bool work = true;
        std::thread thread(Viewer::waitAnim, std::ref(work));

        swapToNextRows();

        // Turn off animation
        work = false;
        if (thread.joinable())
            thread.join();

        showCurrRows();
    }

    void showPrevRows()
    {
        // Starting thread and show animation of cycling cursor
        bool work = true;
        std::thread thread(Viewer::waitAnim, std::ref(work));

        swapToPrevRows();

        // Turn off animation
        work = false;
        if (thread.joinable())
            thread.join();

        showCurrRows();
    }

private:
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
    long theEnd;

    // Three buffers
    std::shared_ptr<IRow> prevRows;
    std::shared_ptr<IRow> currRows;
    std::shared_ptr<IRow> nextRows;

    Viewer &view;
};


#endif //PROCESSINGLOGS_THELOG_HPP
