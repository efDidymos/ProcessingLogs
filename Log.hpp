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
//        prevPos = prevRows->read(prevPos, std::ios_base::beg);
//        prevRows->read(positionAtLadder.back(), std::ios_base::beg);


//        currPos = currRows->read(currPos, std::ios_base::beg);
        currPos = currRows->read(positionAtLadder.back(), std::ios_base::beg);
//        prevPos = prevPos - currPos;
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
        currRows = nextRows;
        nextRows = temp;

        nextPos = nextRows->read(positionAtLadder.back(), std::ios_base::beg);
        positionAtLadder.push_back(nextPos);
    }

    // Just switch the pointers.
    void swapToPrevious()
    {
        auto temp = nextRows;
        nextRows = currRows;
        currRows = prevRows;
        prevRows = temp;

        // Climb up the ladder by one setp
        positionAtLadder.pop_back();

//        std::cout << "second to last: " << positionAtLadder.end()[-3] << std::endl;

        // Climb up the ladder by three setps only for looking
        prevPos = prevRows->read(positionAtLadder.end()[-3], std::ios_base::beg);
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
