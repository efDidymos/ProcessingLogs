//
// Created by tomas on 30.3.2016.
//

#ifndef PROCESSINGLOGS_VIEWER_HPP
#define PROCESSINGLOGS_VIEWER_HPP

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>     // because of first argument <STDOUT_FILENO> ioctl

class Viewer
{
public:
    Viewer()
    {
        struct winsize size;

        // Calculating size of the terminal
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

//        std::cout << "size=" << size.ws_row << std::endl;

        // If autodetection fails in recognition of terminal size
        // use 30 rows as default
        rows = size.ws_row;
        if (rows == 0) rows = 30;

        columns = size.ws_col;
        if (columns == 0) columns = 30;

/*
        // Animation of counter
        for (int j = 0; j < 5; j++)
        {
            std::cout.flush();
            std::cout << "\r" << j;
            sleep(1);
        }
*/
    }

    void printCmdMenu() const
    {
        std::cout.width(columns);
        std::cout.fill('-');
        std::cout << "\n[q]-quit  [f]-filter options  [j]-move down  [k]-move up";
    }

    void printFilterCmdMenu() const
    {
        std::cout.flush();
        // Appended four white spaces due to completly overwrite the showPrevRows line
        // without leaving a text fragments from showPrevRows message
        std::cout << '\r' << "FILTER BY: (0) None  (1) HTTP Code  (2) Request method  (3) Date";
    }

    void printFilterRequestMCmdMenu() const
    {
        std::cout << '\r' << "FILTER Request method BY: (0) POST  (1) GET  (2) HEAD  (3) Unknown";
    }

    const unsigned short int &getRowsCount() const
    {
        return rows;
    }

private:
    unsigned short int rows;
    unsigned short int columns;
};


#endif //PROCESSINGLOGS_VIEWER_HPP
