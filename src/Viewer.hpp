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
        terminal_rows_ = size.ws_row;
        if (terminal_rows_ == 0)
            terminal_rows_ = 30;

        terminal_columns_ = size.ws_col;
        if (terminal_columns_ == 0)
            terminal_columns_ = 30;
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
    void print_prog_bar(std::string msg, double value, long base)
    {
        std::string bar;
        int percent = ((value / base) * 100);

        for (int i = 0; i < 50; ++i)
        {
            if (i < (percent / 2))
                bar.replace(i, 1, "=");
            else if (i == (percent / 2))
                bar.replace(i, 1, ">");
            else
                bar.replace(i, 1, " ");
        }

        std::cout << "\r " << msg << " [" << bar << "] " << percent << "%    " << std::flush;
    }

    /**
     * Print the main menu
     */
    void print_cmd_menu() const
    {
        std::cout << "\n ";
        print_horiz_line();

        std::cout << "\n [q]-quit  [f]-filter options  [j]-move down  [k]-move up";
    }

    /**
     * Print the separator
     */
    void print_horiz_line() const
    {
        std::cout.width(terminal_columns_);
        std::cout.fill('-');
    }

    /**
     * Print the sub menu for filtering
     */
    void print_filter_cmd_menu() const
    {
        // Appended four white spaces due to completly overwrite the get_prev_rows line
        // without leaving a text fragments from get_prev_rows message
        std::cout << "\r FILTER BY: (0) None  (1) HTTP Code  (2) Request method  (3) Date";
        std::cout.flush();
    }

    /**
     * Print the sub sub menu for filtering by Request Method
     */
    void print_filter_cmd_menu_req_meth() const
    {
        std::cout << "\r FILTER Request method BY: (0) POST  (1) GET  (2) HEAD  (3) Unknown";
        std::cout.flush();
    }

    /**
     * Print the sub sub menu for filtering by Date
     */
    void print_filter_cmd_menu_date()
    {
        std::cout << "\r Please enter day [01-31] and press ENTER:                        ";
        std::cout.flush();
    }

    /**
     * Print the sub sub menu for filtering by HTTP Code
     */
    void print_filter_cmd_menu_HTTP_code()
    {
        std::cout << "\r Please enter code [1xx - 5xx] and press ENTER:                   ";
        std::cout.flush();
    }

    /**
     * Returning the size of the terminal without the size of menu
     * @return
     */
    const unsigned short int get_rows_cnt() const
    {
        return terminal_rows_ - 5;    // (terminal_rows_ - 5) - last lines are for displaying pseudo menu
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
    void toggle_buff_off()
    {
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag &= ~ICANON;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }

    /**
     * Enables buffered input
     */
    void toggle_buff_on()
    {
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag |= ICANON;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }

private:
    unsigned short int  terminal_rows_;
    unsigned short int  terminal_columns_;
    struct termios      t_;
};

#endif //PROCESSINGLOGS_VIEWER_HPP
