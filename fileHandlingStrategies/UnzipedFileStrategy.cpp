//
// Created by tomas on 28.3.2016.
//
#include <iostream>
#include <termio.h>

#include "UnzipedFileStrategy.h"
#include "../Log.hpp"
#include "../Viewer.hpp"

#include "../rowsFilteringStrategies/AllRowsStrategy.hpp"
#include "../rowsFilteringStrategies/RequestMethodRowsStrategy.hpp"

void UnzipedFileStrategy::execute()
{
    if (!file.is_open())
    {
        std::cout << "Can not open the file!" << std::endl;
    }
    else
    {
        std::cout << "The file opened successfully" << std::endl;

        Viewer w;
        unsigned short int rowCount = w.getRowsCount();
        rowCount = rowCount - 5;    // (rowCount - 2) - last two lines are for displaying pseudo menu

        Log theLog(&file);

        theLog.setDisplayRowStrategy(new AllRowsStrategy(&file, rowCount));
        theLog.printRows();
        w.printCmdMenu();

        // Black magic to prevent Linux from buffering keystrokes.
        // http://www.doctort.org/adam/nerd-notes/reading-single-keystroke-on-linux.html
        // http://stackoverflow.com/a/912796
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag &= ~ICANON;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);

        char c = ' ';
        std::cin.get(c);

        while (c != 'q')
        {
            if (c == 'j')
            {
                theLog.showNextRows();
                w.printCmdMenu();
            }
            else if (c == 'k')
            {
                theLog.showPrevRows();
                w.printCmdMenu();
            }
            else if (c == 'f')
                w.printFilterCmdMenu();
            else if (c == '0')    // Default: Show all rows without any filter
            {
                theLog.setDisplayRowStrategy(new AllRowsStrategy(&file, rowCount));
                theLog.printRows();
                w.printCmdMenu();
            }
//            else if (c == '1')
            else if (c == '2')  // Show rows filtered by Request method
            {
                w.printFilterRequestMCmdMenu();

                std::cin.get(c);
                int opt = c - '0';

                switch (opt)
                {
                    case RequestMethod::POST: // POST
                        theLog.setDisplayRowStrategy(new RequestMethodRowsStrategy(&file,
                                                                                   rowCount,
                                                                                   RequestMethod::POST));
                        break;

                    case RequestMethod::GET: // GET
                        theLog.setDisplayRowStrategy(new RequestMethodRowsStrategy(&file,
                                                                                   rowCount,
                                                                                   RequestMethod::GET));
                        break;

                    case RequestMethod::HEAD: // HEAD
                        theLog.setDisplayRowStrategy(new RequestMethodRowsStrategy(&file,
                                                                                   rowCount,
                                                                                   RequestMethod::HEAD));
                        break;

                    case RequestMethod::UNKNOWN: // Unknown
                        theLog.setDisplayRowStrategy(new RequestMethodRowsStrategy(&file,
                                                                                   rowCount,
                                                                                   RequestMethod::UNKNOWN));
                        break;

                    default:
                        break;
                }

                theLog.printRows();
                w.printCmdMenu();
            }
//            else if (c == '3')

            std::cin.get(c);
        }
        std::cout << std::endl;
    }
}