//
// Created by tomas on 28.3.2016.
//

#include "UnzipedFileStrategy.h"
#include "../Log.hpp"

void UnzipedFileStrategy::execute()
{
    if (!fin.is_open())
    {
        std::cout << "Can not open the file!" << std::endl;
    }
    else
    {
        std::cout << "The file opened successfully" << std::endl;

        Log theLog(10, 1269, &fin);
//        Log theLog(10, 0, &fin);
        theLog.printRows();
//        theLog.next();

        // fastest way of print file
//        cout << fin.rdbuf();
    }
}