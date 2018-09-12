#ifndef RAW_IMPORT_HPP_INCLUDED
#define RAW_IMPORT_HPP_INCLUDED

#include <string>
#include <memory>
#include <opencv2/opencv.hpp>

template<class T> class RAW
{
public:
    RAW(void):m_file(""),m_data(nullptr)
    {
    }

    ~RAW()
    {
    }

    RAW(std::string const & file):m_file(file),m_data(nullptr)
    {
    }

    RAW(RAW const & cpy):m_file(cpy.get_file()),m_data(std::make_shared<cv::Mat>(cpy.get_data()))
    {
    }

    RAW operator=(RAW const & cpy)
    {
        this->m_file=cpy.get_file();
        this->m_data=std::make_shared<cv::Mat>(cpy.get_data());

        return *this;
    }

    std::string get_file(void) const
    {
        return this->m_file;
    }

    cv::Mat get_data(void)const
    {
        return *this->m_data;
    }

    cv::Mat & data(void)
    {
        return *this->m_data;
    }

    std::shared_ptr<cv::Mat> get_data(void)
    {
        return this->m_data;
    }


    bool load(void)
    {
        return false;
    }

private:
    std::string m_file;
    std::shared_ptr<cv::Mat> m_data;
};

#endif // RAW_IMPORT_HPP_INCLUDED
