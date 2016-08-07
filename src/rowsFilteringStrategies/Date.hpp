//
// Created by tomas on 16.4.2016.
//

#ifndef PROCESSINGLOGS_DATESTRATEGY_HPP
#define PROCESSINGLOGS_DATESTRATEGY_HPP

#include "IRow.hpp"

#ifndef NDEBUG
#include <chrono>
#endif

#include <sstream>

/**
 * Class display only rows for specific day
 */
class Date : public IRow
{

public:
    Date(std::ifstream *file, unsigned short row_cnt, std::string date)
        : IRow(file, row_cnt), date_(date)
    { }

    virtual void read(long *in_pos,
                      long *out_pos,
                      std::vector<std::string> *rows) override
    {
#ifndef NDEBUG
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
#endif

        rows->clear();

        // If we previously hit EOF clear flags failbit
        // to be able start working with the file again
        file_->clear();
        file_->seekg(*in_pos, std::ios_base::beg);

        int i = 0;
        std::string c1, c2, c3, c4;
        std::stringstream ss;
        std::string line;

        try
        {
            while (getline(*file_, line))
            {
                if (i < row_cnt_)
                {
                    *out_pos = file_->tellg();

                    ss << line;
                    ss >> c1 >> c2 >> c3 >> c4;
                    ss.str(""); // erase the buffer
                    c4 = c4.substr(1, 2);  // Trim the date in read line

                    if (c4 == date_)
                    {
                        rows->push_back(line);
                        ++i;
                    }
                }
                else
                    break;
            }
        }
        catch (std::ifstream::failure exception)
        {
            std::cerr << "In Date strategy Exception happened: " << exception.what() << "\n"
                      << "Error bits are: "
                      << "\nfailbit: " << file_->fail()
                      << "\neofbit: " << file_->eof()
                      << "\nbadbit: " << file_->bad() << std::endl;
        }

#ifndef NDEBUG
        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of Date=" << diff.count() << "\n";
#endif
    }

private:
    std::string date_;
};


#endif //PROCESSINGLOGS_DATESTRATEGY_HPP
