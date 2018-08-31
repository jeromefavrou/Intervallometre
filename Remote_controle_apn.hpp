#ifndef REMOTE_CONTROLE_APN_HPP_INCLUDED
#define REMOTE_CONTROLE_APN_HPP_INCLUDED

#include <fstream>
#include <iostream>
#include <cstdlib>

class RC_Apn
{
public:

    bool debug_mode;

    enum Parameter{ APERTURE, ISO , SHUTTERSPEED , FORMAT , TARGET , WHITE_BALANCE};

    RC_Apn(void)
    {
        debug_mode=false;
        this->m_void=std::vector<std::string>(0);
    }

    void init_parameter(void)
    {
        this->m_aperture=this->get_config(RC_Apn::Parameter::APERTURE);
        this->m_shutterspeed=this->get_config(RC_Apn::Parameter::SHUTTERSPEED);
        this->m_iso=this->get_config(RC_Apn::Parameter::ISO);
        this->m_format=this->get_config(RC_Apn::Parameter::FORMAT);
        this->m_target=this->get_config(RC_Apn::Parameter::TARGET);
        this->m_wb=this->get_config(RC_Apn::Parameter::WHITE_BALANCE);
    }

    bool check_apn(void)
    {
        system("gphoto2 --auto-detect > ck_apn");

        std::fstream If("ck_apn",std::ios::in);

        std::string buff;
        int nb_line=0;

        while(std::getline(If,buff))
        {
            if(this->debug_mode)
                std::cout << buff << std::endl;

            nb_line++;
        }

        std::remove("ck_apn");

        return (nb_line>2?true:false);
    }

    void capture_new_model(std::string iso,int exposure,std::string aperture,std::string target,std::string format)
    {

    }

    std::vector<std::string> get_config(RC_Apn::Parameter const & param)
    {
        system(std::string(std::string("gphoto2 --get-config=")+this->parameter_to_string(param)+std::string(" > buff")).c_str());
        std::vector<std::string> gc;

        std::fstream If("buff",std::ios::in);

        std::string buffer;

        while(std::getline(If,buffer))
        {

            std::stringstream ss_buffer;
            ss_buffer << buffer;
            ss_buffer >> buffer;

            if(buffer=="Choice:")
            {
                ss_buffer >> buffer >> buffer;

                gc.push_back(buffer);
            }
        }

        std::remove("buff");

        return gc;
    }

    void set_config(RC_Apn::Parameter const & param)
    {
        this->free_cmd("gphoto2 --set-config="+this->parameter_to_string(param));
    }

    std::vector<std::string> & get_parameter(RC_Apn::Parameter const & param)
    {
        switch(param)
        {
            case RC_Apn::Parameter::APERTURE: return this->m_aperture; break;
            case RC_Apn::Parameter::SHUTTERSPEED: return this->m_shutterspeed; break;
            case RC_Apn::Parameter::ISO: return this->m_iso; break;
            case RC_Apn::Parameter::FORMAT: return this->m_format; break;
            case RC_Apn::Parameter::TARGET: return this->m_target; break;
            case RC_Apn::Parameter::WHITE_BALANCE: return this->m_wb; break;

            default : return this->m_void;
        }
        return this->m_void;
    }

    static std::string parameter_to_string(RC_Apn::Parameter const & param)
    {
        switch(param)
        {
            case RC_Apn::Parameter::APERTURE: return "aperture"; break;
            case RC_Apn::Parameter::SHUTTERSPEED: return "shutterspeed"; break;
            case RC_Apn::Parameter::ISO: return "iso"; break;
            case RC_Apn::Parameter::FORMAT: return "imageformat"; break;
            case RC_Apn::Parameter::TARGET: return "capturetarget"; break;
            case RC_Apn::Parameter::WHITE_BALANCE: return "whitebalance"; break;

            default : return "";
        }
        return "";
    }

private:

    void free_cmd(std::string const & cmd)
    {
        system(std::string(cmd+std::string(!this->debug_mode?" > free_cmd":"")).c_str());

        std::remove("free_cmd");
    }

    std::vector<std::string> m_aperture;
    std::vector<std::string> m_shutterspeed;
    std::vector<std::string> m_iso;
    std::vector<std::string> m_format;
    std::vector<std::string> m_target;
    std::vector<std::string> m_wb;
    std::vector<std::string> m_void;
};

#endif // REMOTE_CONTROLE_APN_HPP_INCLUDED
