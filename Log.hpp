//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_THELOG_HPP
#define PROCESSINGLOGS_THELOG_HPP

#include <fstream>
#include <vector>
#include <iostream>
#include "Rows.hpp"

class Log
{
public:
    Log(unsigned short int rwCnt, std::ifstream *file)
        :
        prevRows(new Rows(file, rwCnt)),
        currRows(new Rows(file, rwCnt)),
        nextRows(new Rows(file, rwCnt))
    {
        positionAtLadder.push_back(0);
    }

    virtual ~Log()
    {
        delete prevRows;
        delete currRows;
        delete nextRows;
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
        // Boundary check for not jumping the begining of file
//        if (positionAtLadder.back() != theEnd)
        swapToNext();
        showRows();
    }

    void previous()
    {
        // Boundary check for not jumping the begining of file
//        if (positionAtLadder.end()[-3] != 0)
        swapToPrevious();
        showRows();
    }

private:
    void showRows()
    {
        std::cout << currRows;

        std::cout << "position -3: " << positionAtLadder.end()[-3] << std::endl;
        std::cout << "position -2: " << positionAtLadder.end()[-2] << std::endl;
        std::cout << "position back: " << positionAtLadder.back() << std::endl;
    }

    // Just switch the pointers.
    void swapToNext()
    {
        auto temp = prevRows;
        prevRows = currRows;
        currRows = nextRows;
        nextRows = temp;

        long nextPos = nextRows->read(positionAtLadder.back(), std::ios_base::beg);
        positionAtLadder.push_back(nextPos);

//        std::cout << "nextPos: " << nextPos << std::endl;
    }

    // Just switch the pointers.
    void swapToPrevious()
    {
        if (positionAtLadder.end()[-3] != 0)
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

    Rows *prevRows;
    Rows *currRows;
    Rows *nextRows;
};


#endif //PROCESSINGLOGS_THELOG_HPP
