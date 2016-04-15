//
// Created by tomas on 2.4.2016.
//

#ifndef PROCESSINGLOGS_ROWINTERFACE_HPP
#define PROCESSINGLOGS_ROWINTERFACE_HPP

class RowInterface
{
public:
    RowInterface(std::ifstream *file, unsigned short rowCount)
        :
        file(file),
        rowCount(rowCount)
    {
        file->seekg(0, std::ios::end);
        theEnd = file->tellg();
    }

    virtual ~RowInterface()
    {
    }

    virtual RowInterface *Clone() const = 0;

    virtual long readRows(long pos, std::list<std::string> &rowStack) = 0;

    void readRowsThreaded(std::vector<long> &positionAtLadder,
                          std::list<std::string> &rowStack,
                          bool &work,
                          bool &running,
                          std::mutex &m_mutex,
                          std::condition_variable &m_alarm)
    {
        std::cout << "\n --- Created new thread :: " << work << "--- \n";

        while (running)
        {
            std::unique_lock<std::mutex> lock(m_mutex);                // Enter critical section
            while (!work)
            {
                std::cout << "\n SUB-Waitting ... \n";
                m_alarm.wait(lock);
            }

            std::cout << "\n READ ROWS \n";
            long newPos = readRows(positionAtLadder.back(), rowStack);
            if (newPos != theEnd)   // check for not over jumping through boundary of end file
                positionAtLadder.push_back(newPos);
            work = false;

            lock.unlock();
            m_alarm.notify_one();
        }

        std::cout << "\n ENDED \n";
    }

    std::thread *createSubThread(std::vector<long> &positionAtLadder,
                                 std::list<std::string> &rowStack,
                                 bool &work,
                                 bool &running,
                                 std::mutex &m_mutex,
                                 std::condition_variable &m_alarm)
    {
        return new std::thread([&]
                               {
                                   this->readRowsThreaded(positionAtLadder,
                                                          rowStack,
                                                          work,
                                                          running,
                                                          m_mutex,
                                                          m_alarm);
                               });
    }

protected:
    std::ifstream *file;
    unsigned short int rowCount;
    long theEnd;
};


#endif //PROCESSINGLOGS_ROWINTERFACE_HPP
