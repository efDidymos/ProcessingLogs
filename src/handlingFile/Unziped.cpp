//
// Created by tomas on 28.3.2016.
//

#include "Unziped.h"
#include "../Log.hpp"

#include "../rowsFilteringStrategies/AllRows.hpp"
#include "../rowsFilteringStrategies/HTTPCode.hpp"
#include "../rowsFilteringStrategies/RequestMethod.hpp"
#include "../rowsFilteringStrategies/Date.hpp"

void Unziped::process_file(std::string file_name)
{
    std::string ext = boost::filesystem::extension(file_name);

    // Check if the file exist and if it can be handled with this object
    if (boost::filesystem::exists(file_name) && (ext == ".log"))
    {
        std::cout << "Do you want to display the uncompressed log?" << std::endl;
        std::cout << "[y - display / n - quit application]" << std::endl;
        std::string response;
        std::cin >> response;

        if ((response == "n") || (response == "N"))
        {
            std::cout << "Quitting. Bye..." << std::endl;
            return;
        }
        else if ((response == "y") || (response == "Y"))
        {
            std::ifstream file(file_name, std::ios::in);

            if (!file.is_open())
            {
                std::cout << "File " << file_name << " can not open!" << std::endl;
            }
            else
            {
                std::cout << "The " << file_name << " opened successfully." << std::endl;

                unsigned short int row_cnt = view_.get_rows_cnt();

                // Create the log representation of the file
                // Firstly we define to read and display all rows
                Log the_log(&file, view_, std::make_unique<AllRows>(&file, row_cnt));
                the_log.show_curr_rows();

                view_.toggle_buff_off();

                char c = ' ';
                std::cin.get(c);

                while (c != 'q')        // q - quit application
                {
                    if (c == 'j')       // j - move down one page
                    {
                        the_log.get_next_rows();
                        the_log.show_curr_rows();
                    }
                    else if (c == 'k')  // k - move up one page
                    {
                        the_log.get_prev_rows();
                        the_log.show_curr_rows();
                    }
                    else if (c == 'f')  // f - display filtering options
                    {
                        view_.print_filter_cmd_menu();

                        // Ignore what is buffered and after that read single char
                        // USE BELLOW CODE ONLY IF THE view_.toggleBufferON();
    //                    clearBuffer();
                        std::cin.get(c);

                        if (c == '0')  // Default: Remove filter and show all rows
                        {
                            the_log.change_display_row_strategy(std::make_unique<AllRows>(&file, row_cnt));
                            the_log.show_curr_rows();
                        }
                        else if (c == '1')  // filter rows by HTTP Code
                        {
                            view_.print_filter_cmd_menu_HTTP_code();

                            std::string code;
                            std::cin >> code;

                            the_log.change_display_row_strategy(std::make_unique<HTTPCode>(&file, row_cnt, code));
                            the_log.show_curr_rows();
                        }
                        else if (c == '2')  // filter rows by Request method
                        {
                            view_.print_filter_cmd_menu_req_meth();

                            // Ignore what is buffered and after that read single char
                            // USE BELLOW CODE ONLY IF THE view_.toggleBufferON();
    //                        clearBuffer();
                            std::cin.get(c);
                            int opt = c - '0';  // convert char to int

                            switch (opt)
                            {
                                case Request::POST: // POST
                                    the_log.change_display_row_strategy(
                                            std::make_unique<RequestMethod>(&file, row_cnt, Request::POST));
                                    the_log.show_curr_rows();
                                    break;

                                case Request::GET: // GET
                                    the_log.change_display_row_strategy(
                                            std::make_unique<RequestMethod>(&file, row_cnt, Request::GET));
                                    the_log.show_curr_rows();
                                    break;

                                case Request::HEAD: // HEAD
                                    the_log.change_display_row_strategy(
                                            std::make_unique<RequestMethod>(&file, row_cnt, Request::HEAD));
                                    the_log.show_curr_rows();
                                    break;

                                case Request::UNKNOWN: // Unknown
                                    the_log.change_display_row_strategy(
                                            std::make_unique<RequestMethod>(&file, row_cnt, Request::UNKNOWN));
                                    the_log.show_curr_rows();
                                    break;

                                default:
                                    break;
                            }
                        }
                        else if (c == '3')  // filter rows by Date
                        {
                            view_.print_filter_cmd_menu_date();

                            std::string date;
                            std::cin >> date;

                            the_log.change_display_row_strategy(std::make_unique<Date>(&file, row_cnt, date));
                            the_log.show_curr_rows();
                        }
                    }
                    // Ignore what is buffered and after that read single char
                    // USE BELLOW CODE ONLY IF THE view_.toggleBufferON();
    //                clearBuffer();
                    std::cin.get(c);
                }
                std::cout << "Qutting. Bye..." << std::endl;
                file.close();
            }
        }
    }
    else
        // Hand over to successor obj
        std::cout << "No successor for handling " << file_name << "... ignore." << std::endl;
}
