//
// Created by tomas on 16.4.2016.
//

#ifndef PROCESSINGLOGS_HTTPCODESTRATEGY_HPP
#define PROCESSINGLOGS_HTTPCODESTRATEGY_HPP

#include "IRow.hpp"

#ifndef NDEBUG
#include <chrono>
#endif

#include <sstream>

/**
 * Class display only rows for specific HTTP Code
 */
class HTTPCode : public IRow
{

public:
    HTTPCode(std::ifstream *file, unsigned short row_cnt, std::string code)
            : IRow(file, row_cnt), code_(code)
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
        std::string c1, c2, c3, c4, c5, c6, c7, c8, c9;
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
                    ss >> c1 >> c2 >> c3 >> c4 >> c5 >> c6 >> c7 >> c8 >> c9;
                    ss.str(""); // erase the buffer

                    if (c9 == code_)
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
            std::cerr << "In HTTPCode Exception happened: " << exception.what() << "\n"
                      << "Error bits are: "
                      << "\nfailbit: " << file_->fail()
                      << "\neofbit: " << file_->eof()
                      << "\nbadbit: " << file_->bad() << std::endl;
        }

#ifndef NDEBUG
        auto end = high_resolution_clock::now();
        duration<double> diff = end - start;
        std::cout << "\n --- Duration of HTTPCode=" << diff.count() << "\n";
#endif
    }
private:
    std::string code_;
};


#endif //PROCESSINGLOGS_HTTPCODESTRATEGY_HPP
