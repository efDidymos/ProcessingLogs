//
// Created by tomas on 28.3.2016.
//

#include <termio.h>

#include "UnzipedFileStrategy.h"
#include "../Viewer.hpp"
#include "../Log.hpp"

#include "../rowsFilteringStrategies/AllRowsStrategy.hpp"
#include "../rowsFilteringStrategies/HTTPCodeStrategy.hpp"
#include "../rowsFilteringStrategies/RequestMethodRowsStrategy.hpp"
#include "../rowsFilteringStrategies/DateStrategy.hpp"

void UnzipedFileStrategy::execute()
{
    if (!file.is_open())
    {
        std::cout << "Can not open the file!" << std::endl;
    }
    else
    {
        std::cout << "The file opened successfully" << std::endl;

        Viewer view;
        unsigned short int rowCount = view.getRowsCount();

        Log theLog(&file, view);

        theLog.setDisplayRowStrategy(new AllRowsStrategy(&file, rowCount));
        theLog.showCurrRows();

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
                theLog.showNextRows();
            else if (c == 'k')
                theLog.showPrevRows();
            else if (c == 'f')
                view.printFilterCmdMenu();
            else if (c == '0')    // Default: Show all rows without any filter
            {
                theLog.setDisplayRowStrategy(new AllRowsStrategy(&file, rowCount));
                theLog.showCurrRows();
            }
            else if (c == '1')
            {
                view.printFilterHTTPCodeCmdMenu();

                std::string code;
                std::cin >> code;

                theLog.setDisplayRowStrategy(new HTTPCodeStrategy(&file, rowCount, code));
                theLog.showCurrRows();
            }
            else if (c == '2')  // Show rows filtered by Request method
            {
                view.printFilterRequestMCmdMenu();

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

                theLog.showCurrRows();
            }
            else if (c == '3')
            {
                view.printFilterDateCmdMenu();

                std::string date;
                std::cin >> date;

                theLog.setDisplayRowStrategy(new DateStrategy(&file, rowCount, date));
                theLog.showCurrRows();
            }
            std::cin.get(c);
        }
        std::cout << std::endl;
    }
}