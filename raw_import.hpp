#ifndef RAW_IMPORT_HPP_INCLUDED
#define RAW_IMPORT_HPP_INCLUDED

#include <string>
#include <opencv2/opencv.hpp>

template<class T> class RAW
{
public:
    RAW(std::string const & file):m_file(file)
    {
    }
    RAW(RAW const & cpy):m_file(cpy.get_file()),m_data(cpy.get_data())
    {
    }
    RAW operator=(RAW const & cpy)
    {
        this->m_file=cpy.get_file();
        this->m_data=cpy.get_data();

        return *this;
    }

    std::string get_file(void) const
    {
        return this->m_file;
    }
    cv::Mat get_data(void)const
    {
        return this->m_data;
    }
    cv::Mat & data(void)
    {
        return this->m_data;
    }

    bool load(void)
    {
        return false;
    }

private:
    std::string m_file;
    cv::Mat m_data;
};

#endif // RAW_IMPORT_HPP_INCLUDED
