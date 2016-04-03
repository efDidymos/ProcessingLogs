#include <iostream>
#include <boost/filesystem.hpp>

#include "Context.hpp"
#include "fileHandlingStrategies/UnzipedFileStrategy.h"
#include "fileHandlingStrategies/UrlStrategy.h"
#include "fileHandlingStrategies/GzipedFileStrategy.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Argument error: file '" << argv[1] << "' does not exist!" << endl;
        cout << "USAGE:" << argv[0] << " <file>" << endl;
        cout << "OR" << endl;
        cout << "USAGE:" << argv[0] << " <url_address>" << endl;
        return 0;
    }
    else
    {
        Context processing;
        string argument = argv[1];
        string ext = boost::filesystem::extension(argument);

        if (boost::filesystem::exists(argument) && ((ext == ".log") || (ext == ".gz")))
        {
            if (ext == ".log")
                processing.setStrategy(new UnzipedFileStrategy(argument));
            else if (ext == ".gz")
                processing.setStrategy(new GzipedFileStrategy(argument));
            else
            {
                cerr << "ERROR opening the file! Please check the path correctness or extension [.log, .gz]." << endl;
                return 0;
            }
        }
        else
            processing.setStrategy(new UrlStrategy(argument));

        processing.performAction();
    }
    return 0;
}