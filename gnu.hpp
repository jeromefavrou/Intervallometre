#ifndef GNU_HPP_INCLUDED
#define GNU_HPP_INCLUDED

#include <opencv2/highgui/highgui_c.h>
#include "raw_import.hpp"

class GNU
{
 public:
     GNU(void):m_name_blue_windows("BLUE"),m_name_green_windows("GREEN"),m_name_red_windows("RED")
     {
     }

     void display(void)
     {
         //test
        RAW image("test.CR2");
        image.load();

        cv::namedWindow(m_name_red_windows,CV_WINDOW_FREERATIO);
        cv::namedWindow(m_name_blue_windows,CV_WINDOW_FREERATIO);
        cv::namedWindow(m_name_green_windows,CV_WINDOW_FREERATIO);

        cv::imshow( m_name_red_windows, image.data() );
        cv::imshow( m_name_blue_windows, image.data() );
        cv::imshow( m_name_green_windows, image.data() );

        cv::waitKey(0);

        cv::destroyAllWindows();
     }
 private:

     std::string const m_name_red_windows;
     std::string const m_name_green_windows;
     std::string const m_name_blue_windows;
};

#endif // GNU_HPP_INCLUDED
