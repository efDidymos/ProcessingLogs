//
// Created by tomas on 30.3.2016.
//

#ifndef PROCESSINGLOGS_VIEWER_HPP
#define PROCESSINGLOGS_VIEWER_HPP

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>     // because of first argument <STDOUT_FILENO> ioctl
#include <termio.h>

/**
 * Class intended as view
 */
class Viewer
{
public:
    /**
     * Constructor checks if it is possible to adjust size of the printed rows
     * @return
     */
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

    /**
     * Displays progress bar for long-lasting operations with percentages
     *
     * [=====>    ]
     *
     * @param msg
     * @param value
     * @param base
     */
    void printProgBar(std::string msg, double value, long base)
    {
        std::string bar;
        int percent = ((value / base) * 100);

        for (int i = 0; i < 50; ++i)
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

    /**
     * Print the main menu
     */
    void printCmdMenu() const
    {
        std::cout << "\n ";
        printHorizontalLine();

        std::cout << "\n [q]-quit  [f]-filter options  [j]-move down  [k]-move up";
    }

    /**
     * Print the separator
     */
    void printHorizontalLine() const
    {
        std::cout.width(terminalColumns);
        std::cout.fill('-');
    }

    /**
     * Print the sub menu for filtering
     */
    void printFilterCmdMenu() const
    {
        // Appended four white spaces due to completly overwrite the getPrevRows line
        // without leaving a text fragments from getPrevRows message
        std::cout << "\r FILTER BY: (0) None  (1) HTTP Code  (2) Request method  (3) Date";
        std::cout.flush();
    }

    /**
     * Print the sub sub menu for filtering by Request Method
     */
    void printFilterRequestMCmdMenu() const
    {
        std::cout << "\r FILTER Request method BY: (0) POST  (1) GET  (2) HEAD  (3) Unknown";
        std::cout.flush();
    }

    /**
     * Returning the size of the terminal without the size of menu
     * @return
     */
    const unsigned short int getRowsCount() const
    {
        return terminalRows - 5;    // (rowCount - 5) - last lines are for displaying pseudo menu
    }

    /**
     * Print the sub sub menu for filtering by Date
     */
    void printFilterDateCmdMenu()
    {
        std::cout << "\r Please enter day [01-31] and press ENTER:                        ";
        std::cout.flush();
    }

    /**
     * Print the sub sub menu for filtering by HTTP Code
     */
    void printFilterHTTPCodeCmdMenu()
    {
        std::cout << "\r Please enter code [1xx - 5xx] and press ENTER:                   ";
        std::cout.flush();
    }

    // ==============================================================================
    // For comfortableness application incorporate little hack
    // ==============================================================================
    // Black magic to prevent Linux from buffering keystrokes.
    // http://www.doctort.org/adam/nerd-notes/reading-single-keystroke-on-linux.html
    // http://stackoverflow.com/a/912796
    // ==============================================================================

    /**
     * Disables buffered input
     */
    void toggleBufferOff()
    {
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag &= ~ICANON;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }

    /**
     * Enables buffered input
     */
    void toggleBufferOn()
    {
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag |= ICANON;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }

private:
    unsigned short int terminalRows;
    unsigned short int terminalColumns;
    struct termios t;
};

#endif //PROCESSINGLOGS_VIEWER_HPP
