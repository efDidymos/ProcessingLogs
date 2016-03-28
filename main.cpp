#include <iostream>
#include <fstream>

#include <sys/ioctl.h>    // because of second argument ioctl
#include <boost/filesystem.hpp>

using namespace std;

int main(int argc, char * argv[])
{
    string fileName = "/home/tomas/Verejné/Cpp/ProcessingLogs/access.log";
    ifstream fin(fileName, ios::in);

    if (!fin.is_open())
        cout << "Can not open file" << endl;
    else
    {
        cout << "Opened" << endl;

        // fastest way of print file
//        cout << fin.rdbuf();

        string line;

//        fin.seekg(1269, ios::beg);

        for (int i = 0;i != 5; i++)
        {
            getline (fin, line);
            cout << line << endl;
        }

//        fin.clear();
        cout << "pozicia:" << fin.tellg() << endl;
    }

    fin.close();


    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

    cout << "rows: " << size.ws_row << " columns: " << size.ws_col;

//    cout << '\r';
//    cout << "rows: 123456 columns: 123456" << endl;


    for (int j = 0; j < 5; j++)
    {
        cout.flush();
        cout << "\r" << j;
        sleep(1);
    }

    cout << "\nVelkost: " << boost::filesystem::file_size(fileName) << endl;

    return 0;
}