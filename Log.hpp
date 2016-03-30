//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_THELOG_HPP
#define PROCESSINGLOGS_THELOG_HPP

#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include "Rows.hpp"

class Log
{
public:
    Log(int rwCnt, long curPos, std::ifstream *file)
        :
        rowCount(rwCnt),
        currPos(curPos),
        pFin(file),
        prevRows(new Rows(file, rwCnt)),
        currRows(new Rows(file, rwCnt)),
        nextRows(new Rows(file, rwCnt))
    {
    }

    virtual ~Log()
    {
        delete prevRows;
        delete currRows;
        delete nextRows;
    }

    void printRows()
    {
//        prevPos = prevRows->read(-currentPos, std::ios_base::beg);
        currPos = currRows->read(currPos, std::ios_base::beg);
        nextPos = nextRows->read(currPos, std::ios_base::beg);
        showRows();
    }

    void next()
    {
        swapToNext();
        printRows();
    }

    void previous()
    {
        swapToPrevious();
        printRows();
    }

private:
    void showRows()
    {
        std::cout << currRows;
    }

    void swapToNext()
    {
        // Just switch the pointers.

//        prevRows // delete its content
        prevRows = currRows;
        prevPos = currPos;

        currRows = nextRows;
        currPos = nextPos;

//        nextPos     = nextRows->read(nextPos, std::ios_base::beg);
        nextPos = nextRows->read(nextPos, std::ios_base::beg);

        showRows();
    }

    void swapToPrevious()
    {
//        std::reverse(currentRows.begin(), currentRows.end());
        showRows();
    }

    std::ifstream *pFin;
    int rowCount;

    long prevPos;
    long currPos;
    long nextPos;

    Rows *prevRows;
    Rows *currRows;
    Rows *nextRows;
};


#endif //PROCESSINGLOGS_THELOG_HPP
