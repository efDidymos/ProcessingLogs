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
    }

    static void waitAnim(bool &work)
    {
        std::cout << "\n";
        std::string anim[] = {"- working                                                ",
                              "\\ working                                                ",
                              "| working                                                ",
                              "/ working                                                "};

        int i = 0;

        while (work)
        {
            std::cout << "\r " << anim[i] << std::flush;
            if (i == 3)
                i = 0;
            else
                ++i;
            sleep(1);
        }
    }

    void printProgBar(std::string msg, double value, long base)
    {
        std::string bar;
        int percent = ((value / base) * 100);

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

        std::cout << "\r " << msg << " [" << bar << "] " << percent << "%    " << std::flush;
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
        std::cout << "\r Please enter day [01-31] and press ENTER:                        ";
    }

    void printFilterHTTPCodeCmdMenu()
    {
        std::cout << "\r Please enter code [1xx - 5xx] and press ENTER:                   ";
    }

private:
    unsigned short int terminalRows;
    unsigned short int terminalColumns;
};

#endif //PROCESSINGLOGS_VIEWER_HPP
