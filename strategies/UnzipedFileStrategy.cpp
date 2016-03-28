//
// Created by tomas on 28.3.2016.
//

#include "UnzipedFileStrategy.h"

void UnzipedFileStrategy::execute()
{
    if (!fin.is_open())
    {
        std::cout << "Can not open the file!" << std::endl;
    }
    else
    {
        std::cout << "The file opened successfully" << std::endl;

        // fastest way of print file
//        cout << fin.rdbuf();

        std::string line;

//        fin.seekg(1269, ios::beg);

        for (int i = 0; i != 5; i++)
        {
            getline(fin, line);
            std::cout << line << std::endl;
        }

//        fin.clear();
        currentPos = fin.tellg();
        std::cout << "Last read position:" << currentPos << std::endl;
        fin.close();
    }
}