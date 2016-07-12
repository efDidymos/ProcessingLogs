#include <iostream>
#include <memory>
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
        shared_ptr<IProcessing> url(new URL);
        shared_ptr<IProcessing> gzip(new Gziped);
        shared_ptr<IProcessing> unziped(new Unziped);

        // Create the chain of responsibility
        url->setSuccessor(gzip);
        gzip->setSuccessor(unziped);

        // Issue argument
        url->processFile(argv[1]);
    }
    return 0;
}