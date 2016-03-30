#include <iostream>
#include <boost/filesystem.hpp>
#include "Context.hpp"
#include "strategies/UnzipedFileStrategy.h"
#include "strategies/GzipedFileStrategy.h"
#include "strategies/UrlStrategy.h"

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
        Context context;
        string argument = argv[1];

        if (boost::filesystem::exists(argument))
        {
            string ext = boost::filesystem::extension(argument);
            if (ext == ".log")
                context.setStrategy(new UnzipedFileStrategy(argument));
            else if (ext == ".gz")
                context.setStrategy(new GzipedFileStrategy(argument));
            else
            {
                cout << "Unknown file extension. Expected .log or .gz" << endl;
                return 0;
            }
        }
        else
            context.setStrategy(new UrlStrategy(argument));

        context.performAction();

//        cout << "\nSize: " << boost::filesystem::file_size(fileName) << endl;
    }
    return 0;
}