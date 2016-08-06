//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_ROWS_HPP
#define PROCESSINGLOGS_ROWS_HPP

#include "IRow.hpp"

#ifndef NDEBUG
#include <chrono>
#endif

/**
 * Class display all rows from file
 */
class AllRows: public IRow
{
public:
    AllRows(std::ifstream *file, unsigned short row_cnt) :
            IRow(file, row_cnt)
    {
    }

    void read(long *in_pos,
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
        std::string line;

        try
        {
            while (getline(*file_, line))
            {
                if (i < row_cnt_)
                {
                    *out_pos = file_->tellg();
                    rows->push_back(line);
                    ++i;
                }
                else
                    break;
            }
        }
        catch (std::ifstream::failure exception)
        {
            std::cerr << "In AllRows strategy exception happened: " << exception.what() << "\n"
                      << "Error bits are: "
                      << "\nfailbit: " << file_->fail()
                      << "\neofbit: " << file_->eof()
                      << "\nbadbit: " << file_->bad() << std::endl;
        }

#ifndef NDEBUG
        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of AllRows=" << diff.count() << "\n";
#endif
    }
};


#endif //PROCESSINGLOGS_ROWS_HPP
