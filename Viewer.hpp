//
// Created by tomas on 30.3.2016.
//

#ifndef PROCESSINGLOGS_VIEWER_HPP
#define PROCESSINGLOGS_VIEWER_HPP

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
        rowsCount = size.ws_row;
        if (rowsCount == 0) rowsCount = 30;

        columns = size.ws_col;
        if (columns == 0) columns = 30;
    }

    void animation()
    {
        // Animation of counter
        for (int i = 0; i < 3; i++)
        {
            std::cout.flush();
            switch (i)
            {
                case 0:
                    std::cout << "\r \\ READING                                                ";
                    break;

                case 1:
                    std::cout << "\r | READING                                                      ";
                    break;

                case 2:
                    std::cout << "\r / READING                                                      ";
                    break;

                case 3:
                    std::cout << "\r --- READING                                                  ";
                    break;
            }
            sleep(1);
        }
    }

    void printHorizLine() const
    {
        std::cout.width(columns);
        std::cout.fill('-');
    }

    void printCmdMenu() const
    {
        printHorizLine();
        std::cout << "\n [q]-quit  [f]-filter options  [j]-move down  [k]-move up";
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

    const unsigned short int getRowsCount() const
    {
        return rowsCount - 2; // (rowCount - 7) - last lines are for displaying pseudo menu
    }

    void printProgBar(double value, long full) const
    {
        std::string bar;
        int percent = ((value / full) * 100);

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
        printHorizLine();
        std::cout << "\n ";
        std::cout << "\r  File read [" << bar << "] " << percent << "%    \n" << std::flush;
    }

private:
    unsigned short int rowsCount;
    unsigned short int columns;
};


#endif //PROCESSINGLOGS_VIEWER_HPP
