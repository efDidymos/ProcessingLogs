//
// Created by tomas on 28.3.2016.
//

#include <termio.h>
#include "UnzipedFileStrategy.h"
#include "../Log.hpp"
#include "../Viewer.hpp"

void UnzipedFileStrategy::execute()
{
    if (!fin.is_open())
    {
        std::cout << "Can not open the file!" << std::endl;
    }
    else
    {
        std::cout << "The file opened successfully" << std::endl;

        Viewer w;
        unsigned short int rowCount = w.getRows();

        // (rowCount - 2) - last two lines are for displaying pseudo menu
        Log theLog((rowCount - 17), &fin);
        theLog.printRows();
        w.printCmd();

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
                theLog.next();
                w.printCmd();
            }
            else if (c == 'k')
            {
                theLog.previous();
                w.printCmd();
            }
            else if (c == 'f')
                w.printFilterCmd();
//            else if (c == '0')    // default
//            else if (c == '1')
//            else if (c == '2')
//            else if (c == '3')

            std::cin.get(c);
        }
        std::cout << std::endl;
    }
}