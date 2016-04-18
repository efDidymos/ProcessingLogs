#include <iostream>
#include "handlingFile/URL.h"
#include "handlingFile/Unziped.h"
#include "handlingFile/Gziped.h"

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
        // Create instances for processing objects
        URL *url = new URL();
        Gziped *gzip = new Gziped();
        Unziped *unziped = new Unziped();

        // Create the chain of responsibility
        url->setSuccessor(gzip);
        gzip->setSuccessor(unziped);

        // Issue argument
        url->processFile(argv[1]);

        delete url;
        delete gzip;
        delete unziped;
    }
    return 0;
}