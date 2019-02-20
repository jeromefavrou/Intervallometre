#include "gp2_utility.hpp"

///passe un parametre en sa valeur string pour gphoto2

std::string gp2::Conf_param_to_str(gp2::Conf_param const & param)
{
    switch(param)
    {
        case gp2::Conf_param::APERTURE: return "aperture"; break;
        case gp2::Conf_param::SHUTTERSPEED: return "shutterspeed"; break;
        case gp2::Conf_param::ISO: return "iso"; break;
        case gp2::Conf_param::FORMAT: return "imageformat"; break;
        case gp2::Conf_param::TARGET: return "capturetarget"; break;
        case gp2::Conf_param::WHITE_BALANCE: return "whitebalance"; break;
        case gp2::Conf_param::PICTURE_STYLE: return "picturestyle"; break;

        default : return "";
    }
    return "";
}

///Monatage du peripherique photo
void gp2::Mount(struct gp2::mnt const & ifm)
{
    std::string command("");
    command+=ifm.cmd;
    command+=" gphoto2://[$(gphoto2 --auto-detect | awk 'END { print $NF }')]";
    system(command.c_str());
}

///Demontage du peripherique photo
void gp2::Unmount(struct gp2::mnt const & ifm)
{
    std::string command("");
    command+=ifm.cmd;
    command+=" -u gphoto2://[$(gphoto2 --auto-detect | awk 'END { print $NF }')]";
    system(command.c_str());
}

///Get_config gphoto2
void gp2::Get_config(gp2::Conf_param const & param,gp2::Data & gc)
{
    system(std::string(std::string("gphoto2 --get-config=")+gp2::Conf_param_to_str(param)+std::string(" > buff")).c_str());

    //on lit les valeurs de parametre et on trie juste les données adequates
    std::fstream If("buff",std::ios::in);

    if(!If || If.bad() || If.fail())
        throw std::string("probleme de lecture de la config gp2: "+gp2::Conf_param_to_str(param));

    gc.clear();

    std::string buffer;

    //passage des donné en memoire vive
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
}

void gp2::Auto_detect(gp2::Data & device)
{
    system("gphoto2 --auto-detect > ck_apn");

    //lecture du resultat
    std::fstream If("ck_apn",std::ios::in);

    if(!If || If.bad() || If.fail())
        throw std::string("probleme de lecture des apareils connecte");

    device.clear();
    std::string buff;

    while(std::getline(If,buff))
        device.push_back(buff);

    device.erase(device.begin(),device.begin()+2);

    //efface sauvegarde temporaire du disque
    std::remove("ck_apn");
}
