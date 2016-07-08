//
// Created by tomas on 28.3.2016.
//

#include <termio.h>

#include "Unziped.h"
#include "../Viewer.hpp"
#include "../Log.hpp"

#include "../rowsFilteringStrategies/AllRows.hpp"
#include "../rowsFilteringStrategies/HTTPCode.hpp"
#include "../rowsFilteringStrategies/RequestMethod.hpp"
#include "../rowsFilteringStrategies/Date.hpp"

#include <boost/filesystem.hpp>

void Unziped::processFile(std::string fileName)
{
    std::string ext = boost::filesystem::extension(fileName);

    if (boost::filesystem::exists(fileName) && (ext == ".log"))
    {
        std::ifstream file(fileName, std::ios::in);

        if (!file.is_open())
        {
            std::cout << "File " << fileName << " can not open!" << std::endl;
        }
        else
        {
            std::cout << "The " << fileName << " opened successfully." << std::endl;

            Viewer view;
            unsigned short int rowCount = view.getRowsCount();

            Log theLog(&file, view);

            theLog.setDisplayRowStrategy(new AllRows(&file, rowCount));

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
                    theLog.setDisplayRowStrategy(new AllRows(&file, rowCount));
                else if (c == '1')
                {
                    view.printFilterHTTPCodeCmdMenu();

                    std::string code;
                    std::cin >> code;

                    theLog.setDisplayRowStrategy(new HTTPCode(&file, rowCount, code));
                }
                else if (c == '2')  // Show rows filtered by Request method
                {
                    view.printFilterRequestMCmdMenu();

                    std::cin.get(c);
                    int opt = c - '0';

                    switch (opt)
                    {
                        case Request::POST: // POST
                            theLog.setDisplayRowStrategy(new RequestMethod(&file,
                                                                           rowCount,
                                                                           Request::POST));
                            break;

                        case Request::GET: // GET
                            theLog.setDisplayRowStrategy(new RequestMethod(&file,
                                                                           rowCount,
                                                                           Request::GET));
                            break;

                        case Request::HEAD: // HEAD
                            theLog.setDisplayRowStrategy(new RequestMethod(&file,
                                                                           rowCount,
                                                                           Request::HEAD));
                            break;

                        case Request::UNKNOWN: // Unknown
                            theLog.setDisplayRowStrategy(new RequestMethod(&file,
                                                                           rowCount,
                                                                           Request::UNKNOWN));
                            break;

                        default:
                            break;
                    }
                }
                else if (c == '3')
                {
                    view.printFilterDateCmdMenu();

                    std::string date;
                    std::cin >> date;

                    theLog.setDisplayRowStrategy(new Date(&file, rowCount, date));
                }
                std::cin.get(c);
            }
            std::cout << std::endl;
        }
    }
    else
        std::cout << "No successor for handling " << fileName << "... ignore." << std::endl;
}