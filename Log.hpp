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
    Log(int rwCnt, std::ifstream *file)
        :
        rowCount(rwCnt),
        prevPos(0),
        currPos(0), //  1269
        nextPos(0),
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
        prevPos = prevRows->read(prevPos, std::ios_base::beg);

        currPos = currRows->read(currPos, std::ios_base::beg);
        prevPos = prevPos - currPos;

        nextPos = nextRows->read(currPos, std::ios_base::beg);
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
        nextPos = nextRows->read(nextPos, std::ios_base::beg);
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
