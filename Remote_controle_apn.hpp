#ifndef REMOTE_CONTROLE_APN_HPP_INCLUDED
#define REMOTE_CONTROLE_APN_HPP_INCLUDED

#include <fstream>
#include <iostream>
#include <cstdlib>

class RC_Apn
{
public:

    ///defini si mode debug activé
    bool debug_mode;
    ///coupe coller
    bool download_and_remove;
    ///passage des instruction a un serveur
    bool tcp_client;
    ///definit si mode de compatibilité avec ancien apn activé
    bool older;

    ///definition des parametre apn utile
    enum Parameter{ APERTURE=1, ISO , SHUTTERSPEED , FORMAT , TARGET , WHITE_BALANCE , PICTURE_STYLE, FILE};

///-------------------------------------------------------------
///initialisation aux valeurs par defaut
///-------------------------------------------------------------
    RC_Apn(void)
    {
        this->debug_mode=false;
        this->tcp_client=false;
        this->download_and_remove=false;
        this->older=false;
        this->m_void=std::vector<std::string>(0);
    }

///-------------------------------------------------------------
///enregistre tout les parametres utile en memoire vive
///-------------------------------------------------------------
    void init_parameter(void)
    {
        ///enregistre tout les parametres utile en memoire vive
        this->m_aperture=this->get_config(RC_Apn::Parameter::APERTURE);
        this->m_shutterspeed=this->get_config(RC_Apn::Parameter::SHUTTERSPEED);
        this->m_iso=this->get_config(RC_Apn::Parameter::ISO);
        this->m_format=this->get_config(RC_Apn::Parameter::FORMAT);
        this->m_target=this->get_config(RC_Apn::Parameter::TARGET);
        this->m_wb=this->get_config(RC_Apn::Parameter::WHITE_BALANCE);
        this->m_effect=this->get_config(RC_Apn::Parameter::PICTURE_STYLE);
        this->m_file=this->get_config(RC_Apn::Parameter::FILE);
    }

///-------------------------------------------------------------
///verifie qu'un apn est bien accessible par gphoto2
///-------------------------------------------------------------
    bool check_apn(void)
    {
        //sauvegarde temporaire sur disque du resulatat
        system("gphoto2 --auto-detect > ck_apn");

        //lecture du resultat
        std::fstream If("ck_apn",std::ios::in);

        std::string buff;
        int nb_line=0;

        while(std::getline(If,buff))
        {
            if(this->debug_mode)
                std::cout << buff << std::endl;

            nb_line++;
        }

        //efface sauvegarde temporaire du disque
        std::remove("ck_apn");

        //retourn true si apn presant sinon false
        return (nb_line>2?true:false);
    }

///-------------------------------------------------------------
///prend une capture et enregistre celle ci sur le disuqe
///-------------------------------------------------------------
    void capture_EOS_DSLR(std::string inter,std::string iso,std::string exposure,std::string aperture,std::string target,std::string format, std::string shutter,std::string wb,std::string effect)
    {
        //on prepare la capture
        system("gphoto2 --set-config capture=on");

        //on parametre l'apn
        this->set_config(RC_Apn::Parameter::TARGET,target);
        this->set_config(RC_Apn::Parameter::FORMAT,format);
        this->set_config(RC_Apn::Parameter::APERTURE,aperture);
        this->set_config(RC_Apn::Parameter::ISO,iso);

        this->set_config(RC_Apn::Parameter::SHUTTERSPEED,shutter);
        this->set_config(RC_Apn::Parameter::WHITE_BALANCE,wb);
        this->set_config(RC_Apn::Parameter::PICTURE_STYLE,effect);

        //attente de 0.1 s pour evité busy sur apn
        std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(100));

        //commande gphoto de capture pour new et old
        if(!this->older)
            this->free_cmd("gphoto2 --set-config eosremoterelease=5 --wait-event=2s --set-config eosremoterelease=0 --wait-event="+exposure+"s");
        else
            this->free_cmd("gphoto2 --set-config bulb=1 --wait-event=2s --set-config bulb=0 --wait-event="+exposure+"s");
    }

///-------------------------------------------------------------
///configure un parametre de l'apn via gphoto2
///-------------------------------------------------------------
    void set_config(RC_Apn::Parameter const & param,std::string value)
    {
        if(param==RC_Apn::Parameter::FILE)
            return;

        this->free_cmd("gphoto2 --set-config "+this->parameter_to_string(param)+"="+value);
    }

///-------------------------------------------------------------
///renvoie toutes les possibilités pour un paramatre de l'apn (sans passer par gphoto2 seulement donné sauvegarder en memoire)
///-------------------------------------------------------------
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
            case RC_Apn::Parameter::FILE: return this->m_file; break;

            default : return this->m_void;
        }
        return this->m_void;
    }

///-------------------------------------------------------------
///passe un parametre en sa veleur string pour gphoto2
///-------------------------------------------------------------
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

///-------------------------------------------------------------
///Telecharge une capture donné
///-------------------------------------------------------------
    void download(std::string const & why,std::string const & where)
    {
        unsigned int id(0);

        //on extrait l'id correspondant a la chaine de caractere
        for(auto i=0;i<this->m_file.size();i++)
        {
            if(m_file[i]==why)
            {
                id=i+1;
                break;
            }
        }

        //telechargements
        this->download(id);

        //deplacement dans le repertoire demandé
        if(where!="")
        {
            if(this->debug_mode)
                std::cout << "telechargement vers: "<<where<<std::endl;
            system(std::string("cp "+why+" "+where).c_str());
            system(std::string("rm "+why).c_str());
        }

    }

    private:

///-------------------------------------------------------------
///Telecharge une capture donné
///-------------------------------------------------------------
    void download(unsigned int why)
    {
        std::stringstream cast;
        cast << why;
        std::string fcast("");
        cast >> fcast;

        //on telecharge
        system(std::string("gphoto2 --get-file="+fcast).c_str());

        if(this->download_and_remove)
            system(std::string("gphoto2 -f 1 -d "+fcast).c_str());//preciser le -f en "/.../..."
    }

///-------------------------------------------------------------
///renvoie toutes les possibilités pour un paramatre de l'apn
///-------------------------------------------------------------
    std::vector<std::string> get_config(RC_Apn::Parameter const & param)
    {
        //on eregistre via gphoto2les les valeur possible de l'apn sur disque
        if(param==RC_Apn::Parameter::FILE)
            system(std::string(std::string("gphoto2 --list-files")+std::string(" > buff")).c_str());
        else
            system(std::string(std::string("gphoto2 --get-config=")+this->parameter_to_string(param)+std::string(" > buff")).c_str());

        std::vector<std::string> gc;

        //on lit les valeur de parametre et on trie juste les donné adequate
        std::fstream If("buff",std::ios::in);

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
            else if(buffer[0]=='#' && param==RC_Apn::Parameter::FILE)
            {
                ss_buffer >> buffer;
                gc.push_back(buffer);
            }
        }

        //supression memoire temporaire disque
        std::remove("buff");

        //return un tableau des resultats
        return gc;
    }

///-------------------------------------------------------------
///execute une commande sans afficher le resulat sauf si debug-mode activé
///-------------------------------------------------------------
    void free_cmd(std::string const & cmd)
    {
        system(std::string(cmd+std::string(!this->debug_mode?" > free_cmd":"")).c_str());

        std::remove("free_cmd");
    }

    ///donné de sauvegarde parametre apn
    std::vector<std::string> m_aperture;
    std::vector<std::string> m_shutterspeed;
    std::vector<std::string> m_iso;
    std::vector<std::string> m_format;
    std::vector<std::string> m_target;
    std::vector<std::string> m_wb;
    std::vector<std::string> m_effect;
    std::vector<std::string> m_file;
    //si pas de donné ou erreur renvoie vide
    std::vector<std::string> m_void;
};

#endif // REMOTE_CONTROLE_APN_HPP_INCLUDED
