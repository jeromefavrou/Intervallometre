#ifndef GNU_HPP_INCLUDED
#define GNU_HPP_INCLUDED

#include "raw_import.hpp"

class GNU
{
 public:
     GNU(void):m_name_blue_windows("BLUE"),m_name_green_windows("GREEN"),m_name_red_windows("RED")
     {
     }

     void display(void)
     {
        cv::namedWindow(m_name_red_windows);
        cv::namedWindow(m_name_blue_windows);
        cv::namedWindow(m_name_green_windows);

        cv::waitKey(0);

        cv::destroyAllWindows();
     }
 private:

     std::string const m_name_red_windows;
     std::string const m_name_green_windows;
     std::string const m_name_blue_windows;
};

#endif // GNU_HPP_INCLUDED
