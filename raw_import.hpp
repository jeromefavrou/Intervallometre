#ifndef RAW_IMPORT_HPP_INCLUDED
#define RAW_IMPORT_HPP_INCLUDED

#include <string>
#include <memory>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <libraw/libraw.h>

class RAW
{
public:
///-------------------------------------------------------------
///initialisation valeur
///-------------------------------------------------------------
    RAW(void):m_file(""),m_data(nullptr)
    {
    }

    ~RAW()
    {
    }

    RAW(std::string const & file):m_file(file),m_data(nullptr)
    {
    }

///-------------------------------------------------------------
///constructeur de copie (poiteur partager sur data)
///-------------------------------------------------------------
    RAW(RAW const & cpy):m_file(cpy.get_file()),m_data(std::make_shared<cv::Mat>(cpy.get_data()))
    {
    }

///-------------------------------------------------------------
///operateur de copie (poiteur partager sur data)
///-------------------------------------------------------------
    RAW operator=(RAW const & cpy)
    {
        this->m_file=cpy.get_file();
        this->m_data=std::make_shared<cv::Mat>(cpy.get_data());

        return *this;
    }
///-------------------------------------------------------------
///renvoie le chemin de l'image
///-------------------------------------------------------------
    std::string get_file(void) const
    {
        return this->m_file;
    }

///-------------------------------------------------------------
///renvoie les données image const
///-------------------------------------------------------------
    cv::Mat get_data(void)const
    {
        return *this->m_data;
    }

///-------------------------------------------------------------
///renvoie les données image referencer
///-------------------------------------------------------------
    cv::Mat & data(void)
    {
        return *this->m_data;
    }

///-------------------------------------------------------------
///renvoie le pointeur des données image
///-------------------------------------------------------------
    std::shared_ptr<cv::Mat> get_data(void)
    {
        return this->m_data;
    }

///-------------------------------------------------------------
///import les image brut au format d'opencv2
///-------------------------------------------------------------
    bool load(void)
    {
        try
        {
            LibRaw lrProc;

            ///gestion des erreur de libraw
            if( LIBRAW_SUCCESS != lrProc.open_file( this->m_file.c_str()) )
                throw std::string("brute introuvable: "+this->m_file);
            if( LIBRAW_SUCCESS != lrProc.unpack())
                throw std::string("brute indecompressable: "+this->m_file);
            if( LIBRAW_SUCCESS != lrProc.raw2image() )
                throw std::string("brute impoossible a convertire en data: "+this->m_file);

            ///taille de l'image
            int width = lrProc.imgdata.sizes.iwidth;
            int height = lrProc.imgdata.sizes.iheight;

            ///interpollation de la matrice de bayer
            std::vector<ushort> vBayerData;
            for ( int y = 0; y < height; y++ )
            {
                for ( int x = 0; x < width; x++ )
                {
                    int idx = y * width + x;

                    ushort * uRGBG = lrProc.imgdata.image[idx];

                    if ( y % 2 == 0 )
                    {
                        bool red = x % 2 == 0;
                        vBayerData.push_back( uRGBG[red ? 0 : 1] );
                    }
                    else
                    {
                        bool green = x % 2 == 0;
                        vBayerData.push_back( uRGBG[green ? 3 : 2] );
                    }
                }
            }

            ///convertion au format bgr d'opencv2 avec ajustement gamma

            lrProc.recycle();
            cv::Mat imgBayer( height, width, CV_16UC1, vBayerData.data() );

            cv::Mat imgDeBayer;
            cv::cvtColor( imgBayer, imgDeBayer, CV_BayerBG2BGR );

            float fGamma = 2.2f;
            imgDeBayer.convertTo( imgDeBayer, CV_32FC3, 1. / 4096);//4096-> 2^12 ( 12-Bit)
            cv::pow( imgDeBayer, fGamma, imgDeBayer );

            this->m_data=std::make_shared<cv::Mat>(imgDeBayer);

            return true;
        }
        catch(std::string const & e)
        {
            std::cerr << e <<std::endl;

            ///image vide si erreur
            this->m_data=std::make_shared<cv::Mat>(cv::Mat::zeros(10, 10,  CV_32FC3));

            return false;
        }
        catch(std::exception const & e)
        {
            std::cerr << e.what() <<std::endl;

            ///image vide si erreur
            this->m_data=std::make_shared<cv::Mat>(cv::Mat::zeros(10, 10,  CV_32FC3));

            return false;
        }

        ///image vide si erreur
        this->m_data=std::make_shared<cv::Mat>(cv::Mat::zeros(10, 10,  CV_32FC3));
        return false;
    }

private:
    std::string m_file;
    std::shared_ptr<cv::Mat> m_data;
};

#endif // RAW_IMPORT_HPP_INCLUDED
