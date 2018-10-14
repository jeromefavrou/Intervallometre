#ifndef UTILITY_HPP_INCLUDED
#define UTILITY_HPP_INCLUDED

std::vector<std::string> ls(std::string const & file)noexcept
{
    std::string cmd("");

    #ifdef __linux ||  __unix || __linux__
    cmd="ls";
    #endif // __linux

    ///extraction de la commande ls de unix dans un fichier temporaire
    system(std::string(cmd+" "+file+" > tps_ls.cmd_unix").c_str());

    ///importation du resultat dans la memoire vive du programme
    std::vector<std::string> mem_ls;
    std::string rep("");
    std::fstream If;
    If.exceptions(std::ifstream::badbit);

    try
    {
        If.open("tps_ls.cmd_unix",std::ios::in);

        while(getline(If,rep))
            mem_ls.push_back(rep);
    }
    catch(std::system_error& e)
    {
        std::cerr <<file+"-echec listing ("+cmd+"): " << e.what() <<std::endl;
    }
    catch(std::exception const & e)
    {
        std::cerr <<file+"-echec listing ("+cmd+"): " << e.what() <<std::endl;
    }

    ///supression du fichier temporaire
    std::remove("tps_ls.cmd_unix");

    ///retour du resultat
    return mem_ls;
}

#endif // UTILITY_HPP_INCLUDED
