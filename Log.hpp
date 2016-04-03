//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_THELOG_HPP
#define PROCESSINGLOGS_THELOG_HPP

#include <fstream>
#include <vector>
#include <iostream>
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
        delete prevRows;
        delete currRows;
        delete nextRows;
    }

    void setDisplayRowStrategy(RowInterface *strategy)
    {
//        delete prevRows;
//        delete currRows;
//        delete nextRows;

        prevRows = strategy;
        currRows = strategy;
        nextRows = strategy;
    }

    // Firstime print rows
    void printRows()
    {
        long currPos = currRows->read(positionAtLadder.back(), std::ios_base::beg);
        positionAtLadder.push_back(currPos);

        long nextPos = nextRows->read(currPos, std::ios_base::beg);
        positionAtLadder.push_back(nextPos);

        showRows();
    }

    void next()
    {
        swapToNext();
        showRows();
    }

    void previous()
    {
        swapToPrevious();
        showRows();
    }

private:
    void showRows()
    {
        std::cout << std::endl << currRows;
    }

    // Just switch the pointers.
    void swapToNext()
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
    void swapToPrevious()
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

    RowInterface *prevRows;
    RowInterface *currRows;
    RowInterface *nextRows;
};


#endif //PROCESSINGLOGS_THELOG_HPP
