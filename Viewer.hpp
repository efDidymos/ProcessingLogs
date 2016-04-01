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

    void printCmd()
    {
        std::cout.width(columns);
        std::cout.fill('-');
        std::cout << "\n[q]-quit  [f]-filter options  [j]-move down  [k]-move up";
    }

    void printFilterCmd()
    {
        std::cout.flush();
        // Appended four white spaces due to completly overwrite the previous line
        // without leaving a text fragments from previous message
        std::cout << '\r' << "FILTER BY: (1) HTTP Code  (2) Request method  (3) Date   ";
    }

    unsigned short int getRows() const
    {
        return rows;
    }

private:
    unsigned short int rows;
    unsigned short int columns;
};


#endif //PROCESSINGLOGS_VIEWER_HPP
