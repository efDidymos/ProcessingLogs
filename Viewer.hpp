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

        // If autodetection fails in recognition of terminal size
        // use 30 rows as default
        terminalRows = size.ws_row;
        if (terminalRows == 0) terminalRows = 30;

        terminalColumns = size.ws_col;
        if (terminalColumns == 0) terminalColumns = 30;

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

    void printProgBar(double value, long end)
    {
        std::string bar;
        int percent = ((value / end) * 100);

        for (int i = 0; i < 50; i++)
        {
            if (i < (percent / 2))
            {
                bar.replace(i, 1, "=");
            }
            else if (i == (percent / 2))
            {
                bar.replace(i, 1, ">");
            }
            else
            {
                bar.replace(i, 1, " ");
            }
        }

        std::cout << "\n ";
        std::cout << "\r  [" << bar << "] " << percent << "%    " << std::flush;
    }

    void printCmdMenu() const
    {
        std::cout << "\n ";
        printHorizontalLine();

        std::cout << "\n [q]-quit  [f]-filter options  [j]-move down  [k]-move up";
    }

    void printHorizontalLine() const
    {
        std::cout.width(terminalColumns);
        std::cout.fill('-');
    }

    void printFilterCmdMenu() const
    {
        std::cout.flush();
        // Appended four white spaces due to completly overwrite the showPrevRows line
        // without leaving a text fragments from showPrevRows message
        std::cout << "\r FILTER BY: (0) None  (1) HTTP Code  (2) Request method  (3) Date";
    }

    void printFilterRequestMCmdMenu() const
    {
        std::cout << "\r FILTER Request method BY: (0) POST  (1) GET  (2) HEAD  (3) Unknown";
    }

    const unsigned short int getRowsCount() const
    {
        return terminalRows - 5;    // (rowCount - 5) - last lines are for displaying pseudo menu
    }

    void printFilterDateCmdMenu()
    {
        std::cout << "\r Please enter day [01-31] from which you want list rows:          ";
    }

private:
    unsigned short int terminalRows;
    unsigned short int terminalColumns;
};

#endif //PROCESSINGLOGS_VIEWER_HPP
