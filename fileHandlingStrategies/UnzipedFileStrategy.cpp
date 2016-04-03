//
// Created by tomas on 28.3.2016.
//

#include <termio.h>
#include "UnzipedFileStrategy.h"
#include "../Log.hpp"
#include "../Viewer.hpp"

#include "../rowsFilteringStrategies/AllRowsStrategy.hpp"

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

        // (rowCount - 2) - last two lines are for displaying pseudo menu
        Log theLog(&file);

        theLog.setDisplayRowStrategy(new AllRowsStrategy(&file, (rowCount - 17)));

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
//            else if (c == '0')    // default
//            else if (c == '1')
//            else if (c == '2')
//            else if (c == '3')

            std::cin.get(c);
        }
        std::cout << std::endl;
    }
}