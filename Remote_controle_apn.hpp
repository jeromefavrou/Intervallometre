#ifndef REMOTE_CONTROLE_APN_HPP_INCLUDED
#define REMOTE_CONTROLE_APN_HPP_INCLUDED

#include <fstream>
#include <iostream>
#include <cstdlib>

class RC_Apn
{
public:
    bool debug_mode;
    RC_Apn(void)
    {
        debug_mode=false;
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

    void capture(int iso,int exposure,int aperture,int target,int format)
    {

    }

    std::vector<std::string> get_config(std::string const & cmd)
    {
        system(std::string(std::string("gphoto2 --get-config=")+cmd+std::string(" > buff")).c_str());
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
    void set_config(std::string const & cmd)
    {
        this->free_cmd("gphoto2 --set-config="+cmd);
    }
private:
    void free_cmd(std::string const & cmd)
    {
        system(std::string(cmd+std::string(!this->debug_mode?" > free_cmd":"")).c_str());


        std::remove("free_cmd");
    }

};

#endif // REMOTE_CONTROLE_APN_HPP_INCLUDED
