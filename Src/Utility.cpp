#include "utility.hpp"

std::vector<std::string> ls(std::string const & file)noexcept
{
    ///importation du resultat dans la memoire vive du programme
    std::vector<std::string> mem_ls;

    try
    {
        DIR & rep = *opendir(std::string(file+".").c_str());

        if (&rep != nullptr)
        {
            struct dirent * ent;

            while ((ent = readdir(&rep)) != nullptr)
            {
                if(std::string(ent->d_name)=="." || std::string(ent->d_name)=="..")
                    continue;
                else
                    mem_ls.push_back(ent->d_name);
            }

            closedir(&rep);
        }
    }
    catch(std::system_error& e)
    {
        std::cerr <<file+"-echec listing (ls): " << e.what() <<std::endl;
    }
    catch(std::exception const & e)
    {
        std::cerr <<file+"-echec listing (ls): " << e.what() <<std::endl;
    }

    ///retour du resultat
    return mem_ls;
}

void notify_send(std::string const & msg)
{
    system(std::string("notify-send -u normal -i logo.png \"Intervallometre\" \""+msg+"\" -t 10").c_str());
}

///-------------------------------------------------------------
///execute une commande sans afficher en terminal  sauf si debug-mode activé
///-------------------------------------------------------------
void free_cmd(std::string const & cmd,bool debug_mode)
{
    system(std::string(cmd+std::string(!debug_mode?" > .free_cmd":"")).c_str());

    std::remove(".free_cmd");
}
