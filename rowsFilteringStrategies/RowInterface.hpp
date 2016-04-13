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
        rows.reserve(rowCount);

        file->seekg(0, std::ios::end);
        theEnd = file->tellg();
    }

    virtual ~RowInterface()
    {
        rows.clear();
    }

    virtual RowInterface *Clone() const = 0;

    virtual long readRows(long pos, const std::ios_base::seekdir seekdir) = 0;

    virtual void
        hello(std::vector<long> &positionAtLadder,
                      bool &work,
                      bool &read,
                      bool &show,
                      bool &running,
                      std::mutex &m_mutex,
                      std::condition_variable &m_alarm) = 0;

    std::thread *threadHello(std::vector<long> & positionAtLadder, bool &work, bool & read, bool & show, bool &running, std::mutex &m_mutex, std::condition_variable &m_alarm)
    {
        return new std::thread(&RowInterface::hello,
                               this,
                               std::ref(positionAtLadder),
                               std::ref(work),
                               std::ref(read),
                               std::ref(show),
                               std::ref(running),
                               std::ref(m_mutex),
                               std::ref(m_alarm));
    }

    friend std::ostream &operator<<(std::ostream &os, RowInterface *r)
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

protected:
    std::ifstream *file;
    unsigned short int rowCount;
    std::vector<std::string> rows;
    long theEnd;
};


#endif //PROCESSINGLOGS_ROWINTERFACE_HPP
