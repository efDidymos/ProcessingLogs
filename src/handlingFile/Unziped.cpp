//
// Created by tomas on 28.3.2016.
//

#include <termio.h>

#include "Unziped.h"
#include "../Log.hpp"

#include "../rowsFilteringStrategies/AllRows.hpp"
#include "../rowsFilteringStrategies/HTTPCode.hpp"
#include "../rowsFilteringStrategies/RequestMethod.hpp"
#include "../rowsFilteringStrategies/Date.hpp"

#include <boost/filesystem.hpp>

void Unziped::processFile(std::string fileName)
{
    std::string ext = boost::filesystem::extension(fileName);

    // Check if the file exist and if it can be handled with this object
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

            unsigned short int rowCount = view.getRowsCount();

            // Create the log representation of the file
            // Firstly we define to read and display all rows
            Log theLog(&file, view, std::make_shared<AllRows>(&file, rowCount));
            theLog.showCurrRows();

            view.toggleBufferOff();

            char c = ' ';
            std::cin.get(c);

            while (c != 'q')        // q - quit application
            {
                if (c == 'j')       // j - move down one page
                {
                    theLog.getNextRows();
                    theLog.showCurrRows();
                }
                else if (c == 'k')  // k - move up one page
                {
                    theLog.getPrevRows();
                    theLog.showCurrRows();
                }
                else if (c == 'f')  // f - display filtering options
                {
                    view.printFilterCmdMenu();

                    std::cin.get(c);

                    if (c == '0')  // Default: Remove filter and show all rows
                    {
                        theLog.changeDisplayRowStrategy(std::make_shared<AllRows>(&file, rowCount));
                        theLog.showCurrRows();
                    }
                    else if (c == '1')  // filter rows by HTTP Code
                    {
                        view.printFilterHTTPCodeCmdMenu();

                        std::string code;
                        std::cin >> code;

                        theLog.changeDisplayRowStrategy(std::make_shared<HTTPCode>(&file, rowCount, code));
                        theLog.showCurrRows();
                    }
                    else if (c == '2')  // filter rows by Request method
                    {
                        view.printFilterRequestMCmdMenu();

                        std::cin.get(c);
                        int opt = c - '0';  // convert char to int

                        switch (opt)
                        {
                            case Request::POST: // POST
                                theLog.changeDisplayRowStrategy(std::make_shared<RequestMethod>(&file, rowCount, Request::POST));
                                theLog.showCurrRows();
                                break;

                            case Request::GET: // GET
                                theLog.changeDisplayRowStrategy(std::make_shared<RequestMethod>(&file, rowCount, Request::GET));
                                theLog.showCurrRows();
                                break;

                            case Request::HEAD: // HEAD
                                theLog.changeDisplayRowStrategy(std::make_shared<RequestMethod>(&file, rowCount, Request::HEAD));
                                theLog.showCurrRows();
                                break;

                            case Request::UNKNOWN: // Unknown
                                theLog.changeDisplayRowStrategy(std::make_shared<RequestMethod>(&file, rowCount, Request::UNKNOWN));
                                theLog.showCurrRows();
                                break;

                            default:
                                break;
                        }
                    }
                    else if (c == '3')  // filter rows by Date
                    {
                        view.printFilterDateCmdMenu();

                        std::string date;
                        std::cin >> date;

                        theLog.changeDisplayRowStrategy(std::make_shared<Date>(&file, rowCount, date));
                        theLog.showCurrRows();
                    }
                }
                std::cin.get(c);
            }
            std::cout << std::endl;
        }
    }
    else
        // Hand over to successor obj
        std::cout << "No successor for handling " << fileName << "... ignore." << std::endl;
}
