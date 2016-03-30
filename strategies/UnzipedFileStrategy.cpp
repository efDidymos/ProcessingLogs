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

        Log theLog(10, &fin);
        theLog.printRows();

        std::cout << "moving next" << std::endl;
        theLog.next();

        std::cout << "moving next" << std::endl;
        theLog.next();

        // fastest way of print file
//        cout << fin.rdbuf();
    }
}