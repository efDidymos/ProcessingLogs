//
// Created by tomas on 29.3.2016.
//

#ifndef PROCESSINGLOGS_ROWS_HPP
#define PROCESSINGLOGS_ROWS_HPP

#include <fstream>
#include <vector>

#include <regex>

class Rows
{
public:
    Rows(std::ifstream *pFin, unsigned short int rowCount)
        :
        pFin(pFin),
        rowCount(rowCount)
    {
    }

    long read(long pos, const std::ios_base::seekdir seekdir)
    {
        std::string line;
        rows.clear();

//        ^(.*?)\[(\d{2})\/(.{3})\/(\d{4})(.*?)"(.*?)\s(.*?)"\s(\d{3})  // COMPLETE REGEX
        static const std::regex e(R"delim(^(.*?)\[(\d{2})\/(.{3})\/(\d{4})(.*?)"(.*?)\s(.*?)"\s(\d{3}))delim");
        std::smatch m;

        pFin->seekg(pos, seekdir);

        for (int i = 0; i < rowCount; i++)
        {
            getline(*pFin, line);

            if (std::regex_search(line, m, e))
            {
                std::cout << "YES" << std::endl;

                std::cout << "m[0]" << m[0] << std::endl;
                std::cout << "m[1]" << m[1] << std::endl;
                std::cout << "m[2]" << m[2] << std::endl;
                std::cout << "m[3]" << m[3] << std::endl;
                std::cout << "m[4]" << m[4] << std::endl;
                std::cout << "m[5]" << m[5] << std::endl;
                std::cout << "m[6]" << m[6] << std::endl;
                std::cout << "m[7]" << m[7] << std::endl;
                std::cout << "m[8]" << m[8] << std::endl;
                std::cout << "--------------------------" << std::endl;
            }
            else
                std::cout << "NOT" << std::endl;

            if (line == "")
                break;
            else
            {
                rows.push_back(line);
                pos = pFin->tellg();
            }
        }

        return pos;
    }

    friend std::ostream &operator<<(std::ostream &os, Rows *r)
    {
        auto elements = r->getRows();
        for (auto row : elements)
            os << row << std::endl;
        return os;
    }

    const std::vector<std::string> &getRows() const
    {
        return rows;
    }

private:
    std::ifstream *pFin;
    unsigned short int rowCount;
    std::vector<std::string> rows;
};


#endif //PROCESSINGLOGS_ROWS_HPP
