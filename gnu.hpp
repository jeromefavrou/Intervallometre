#ifndef GNU_HPP_INCLUDED
#define GNU_HPP_INCLUDED

#include <vector>
#include <opencv2/highgui/highgui_c.h>
#include "raw_import.hpp"

class GNU
{
 public:
     GNU(void):m_name_blue_windows("BLUE"),m_name_green_windows("GREEN"),m_name_red_windows("RED"),m_name_full_windows("RGB")
     {
     }

     void display(std::string & capt)
     {
        std::string t_capt("");

        while(capt!="exit")
        {
            RAW image(capt);
            image.load();

            std::vector<cv::Mat> rgbChannels(3);
            std::vector<cv::Mat> LabChannels(3);

            cv::split(image.data(), rgbChannels);
            cv::namedWindow(m_name_red_windows,CV_WINDOW_FREERATIO);
            cv::namedWindow(m_name_blue_windows,CV_WINDOW_FREERATIO);
            cv::namedWindow(m_name_green_windows,CV_WINDOW_FREERATIO);
            cv::namedWindow(m_name_full_windows,CV_WINDOW_FREERATIO);

            cv::imshow( m_name_red_windows, rgbChannels[2]);
            cv::imshow( m_name_blue_windows, rgbChannels[0]);
            cv::imshow( m_name_green_windows, rgbChannels[1]);
            cv::imshow( m_name_full_windows, image.data());

            while(capt==t_capt && capt!="exit")
                cv::waitKey(3000);

            t_capt=capt;

            cv::destroyAllWindows();
        }

     }

 private:

     std::string const m_name_red_windows;
     std::string const m_name_green_windows;
     std::string const m_name_blue_windows;
     std::string const m_name_full_windows;
};

#endif // GNU_HPP_INCLUDED
