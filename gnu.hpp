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
        int luminosite(50),contraste(50);

        cv::namedWindow(m_name_red_windows,CV_WINDOW_FREERATIO);
        cv::namedWindow(m_name_blue_windows,CV_WINDOW_FREERATIO);
        cv::namedWindow(m_name_green_windows,CV_WINDOW_FREERATIO);
        cv::namedWindow(m_name_full_windows,CV_WINDOW_FREERATIO);

        cv::resizeWindow(m_name_red_windows, 467, 350);
        cv::resizeWindow(m_name_blue_windows, 467, 350);
        cv::resizeWindow(m_name_green_windows, 467, 350);
        cv::resizeWindow(m_name_full_windows, 640, 480);

        cv::createTrackbar("luminosoté", m_name_full_windows,&luminosite,100);
        cv::createTrackbar("contraste", m_name_full_windows,&contraste,100);


        while(capt!="exit")
        {
            RAW image(capt);
            image.load();

            int radius=image.get_resolution().x/16;
            cv::circle(image.data(),image.get_resolution()/2,radius,cv::Scalar( 0, 0, 255 ),7,CV_AA);
            cv::line(image.data(), image.get_resolution()/2+cv::Point(0,radius/5), image.get_resolution()/2+cv::Point(0,radius), cv::Scalar( 255, 0, 0 ), 5, CV_AA);
            cv::line(image.data(), image.get_resolution()/2+cv::Point(radius/5,0), image.get_resolution()/2+cv::Point(radius,0), cv::Scalar( 0, 255, 0 ), 5, CV_AA);

            std::vector<cv::Mat> rgbChannels(3);

            while(capt==t_capt && capt!="exit")
            {
                cv::Mat dst;

                image.data().convertTo(dst, -1, static_cast<float>(contraste)/50, static_cast<float>(luminosite)/10-5);

                cv::split(dst, rgbChannels);

                cv::imshow( m_name_red_windows, rgbChannels[2]);
                cv::imshow( m_name_blue_windows, rgbChannels[0]);
                cv::imshow( m_name_green_windows, rgbChannels[1]);
                cv::imshow( m_name_full_windows, dst);

                cv::waitKey(900);
            }

            t_capt=capt;
        }

        cv::destroyAllWindows();
     }

 private:

     std::string const m_name_red_windows;
     std::string const m_name_green_windows;
     std::string const m_name_blue_windows;
     std::string const m_name_full_windows;
};

#endif // GNU_HPP_INCLUDED
