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
        rowCount(rwCnt),
        prevPos(0),
        currPos(0), //  1269
        nextPos(0),
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

    void printRows()
    {
        prevPos = prevRows->read(prevPos, std::ios_base::beg);

        currPos = currRows->read(currPos, std::ios_base::beg);
        prevPos = prevPos - currPos;
        positionAtLadder.push_back(currPos);

        nextPos = nextRows->read(currPos, std::ios_base::beg);
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
        std::cout << currRows;
    }

    // Just switch the pointers.
    void swapToNext()
    {
        auto temp = prevRows;

        prevRows = currRows;
        prevPos = currPos;

        currRows = nextRows;
        currPos = nextPos;

        nextRows = temp;

//        nextPos = nextRows->read(nextPos, std::ios_base::beg);
        nextPos = nextRows->read(positionAtLadder.back(), std::ios_base::beg);
        positionAtLadder.push_back(nextPos);
    }

    // Just switch the pointers.
    void swapToPrevious()
    {
        auto temp = nextRows;

        nextRows = currRows;
        nextPos = currPos;

        currRows = prevRows;
        currPos = prevPos;

        prevRows = temp;

        // Climb up the ladder
        positionAtLadder.pop_back();
        positionAtLadder.pop_back();
        positionAtLadder.pop_back();
        positionAtLadder.pop_back();

        prevPos = prevRows->read(positionAtLadder.back(), std::ios_base::beg);
    }

    unsigned short int rowCount;

    std::vector<long> positionAtLadder;

    long prevPos;
    long currPos;
    long nextPos;

    Rows *prevRows;
    Rows *currRows;
    Rows *nextRows;
};


#endif //PROCESSINGLOGS_THELOG_HPP
