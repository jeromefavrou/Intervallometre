#ifndef REMOTE_CONTROLE_APN_HPP_INCLUDED
#define REMOTE_CONTROLE_APN_HPP_INCLUDED

#include <fstream>
#include <iostream>
#include <cstdlib>

class RC_Apn
{
public:

    bool debug_mode;

    enum Parameter{ APERTURE=1, ISO , SHUTTERSPEED , FORMAT , TARGET , WHITE_BALANCE , PICTURE_STYLE};

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
        this->m_effect=this->get_config(RC_Apn::Parameter::PICTURE_STYLE);
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

    void capture_new_EOS_DSLR(std::string inter,std::string iso,std::string exposure,std::string aperture,std::string target,std::string format, std::string shutter,std::string wb,std::string effect)
    {
        system("gphoto2 --set-config capture=on");

        this->set_config(RC_Apn::Parameter::TARGET,target);
        this->set_config(RC_Apn::Parameter::FORMAT,format);
        this->set_config(RC_Apn::Parameter::APERTURE,aperture);
        this->set_config(RC_Apn::Parameter::ISO,iso);

        this->set_config(RC_Apn::Parameter::SHUTTERSPEED,shutter);
        this->set_config(RC_Apn::Parameter::WHITE_BALANCE,wb);
        this->set_config(RC_Apn::Parameter::PICTURE_STYLE,effect);

        std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(500));

        this->free_cmd("gphoto2 --set-config eosremoterelease=5 --wait-event=2s --set-config eosremoterelease=0 --wait-event="+exposure+"s");
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

    void set_config(RC_Apn::Parameter const & param,std::string value)
    {
        this->free_cmd("gphoto2 --set-config "+this->parameter_to_string(param)+"="+value);
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
            case RC_Apn::Parameter::PICTURE_STYLE: return this->m_effect; break;

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
            case RC_Apn::Parameter::PICTURE_STYLE: return "picturestyle"; break;

            default : return "";
        }
        return "";
    }

    void help(void)
    {

       /* a metre dans intervallometre
       std::cout<< std::endl<< "parametres d'ouverture possible: " <<std::endl;
        for(auto & m: this->m_aperture)
            std::cout <<'\t'<< m << std::endl;

        std::cout<< std::endl<< "parametres iso possible: " <<std::endl;
        for(auto & m: this->m_iso)
            std::cout <<'\t'<< m << std::endl;

        std::cout<< std::endl<< "parametres mode d'exposition possible: " <<std::endl;
        for(auto & m: this->m_shutterspeed)
            std::cout <<'\t'<< m << std::endl;

        std::cout<< std::endl<< "parametres de format possible: " <<std::endl;
        for(auto & m: this->m_format)
            std::cout <<'\t'<< m << std::endl;

        std::cout<< std::endl<< "parametres d'effet possible: " <<std::endl;
        for(auto & m: this->m_effect)
            std::cout <<'\t'<< m << std::endl;

        std::cout<< std::endl<< "parametres de cible possible: " <<std::endl;
        for(auto & m: this->m_target)
            std::cout <<'\t'<< m << std::endl;

        std::cout<< std::endl<< "parametres de balance des blancs possible: " <<std::endl;
        for(auto & m: this->m_wb)
            std::cout <<'\t'<< m << std::endl;*/
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
    std::vector<std::string> m_effect;
    std::vector<std::string> m_void;
};

#endif // REMOTE_CONTROLE_APN_HPP_INCLUDED
