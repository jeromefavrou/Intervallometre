#ifndef GP2_UTILITY_HPP_INCLUDED
#define GP2_UTILITY_HPP_INCLUDED

#include "utility.hpp"
#include <fstream>

namespace gp2
{
    typedef std::vector<std::string> Data;

    enum Conf_param{ APERTURE=1, ISO , SHUTTERSPEED , FORMAT , TARGET , WHITE_BALANCE , PICTURE_STYLE};
    enum Gest_file_param{LS_FILES=8,LS_FOLDER,RMDIR,MKDIR,GET_FILE,DELETE_FILE};

    struct mnt
    {
        std::string cmd;
        std::string path;
    };

    void Mount(struct mnt const & ifm);
    void Unmount(struct mnt const & ifm);

    //Data ls_folder(std::string const & name,bool debug_mode=false);

    std::string Conf_param_to_str(Conf_param const & param);
    void Get_config(gp2::Conf_param const & param,gp2::Data & gc);
    void Auto_detect(gp2::Data & device);

    //void ls_file(std::string & name);
};


#endif // GP2_UTILITY_HPP_INCLUDED
