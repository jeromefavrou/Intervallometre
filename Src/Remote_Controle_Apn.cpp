#include "Remote_controle_apn.hpp"


///-------------------------------------------------------------
///initialisation aux valeurs par defaut
///-------------------------------------------------------------
RC_Apn::RC_Apn(void):m_client(nullptr),m_id_client(0),m_void(_Data(0)),m_aperture(_Data(0)),m_shutterspeed(_Data(0)),m_iso(_Data(0)),
m_format(_Data(0)),m_target(_Data(0)),m_effect(_Data(0)),m_file(_Data(0)),m_wb(_Data(0)),debug_mode(false),tcp_client(false),
download_and_remove(false),older(false){}

///-------------------------------------------------------------
///initialisation aux valeurs par defaut
///-------------------------------------------------------------
RC_Apn::~RC_Apn()
{
    if(this->m_client!=nullptr)
    {
        this->m_client->CloseSocket(this->m_id_client);
    }
}

///-------------------------------------------------------------
///cree un client et se connecte au serveur et envoie les info de bases
///-------------------------------------------------------------
void RC_Apn::connect(struct t_connect const & tc)
{
    if(this->tcp_client)
    {

        this->m_client=std::make_unique<CSocketTCPClient>(CSocketTCPClient());

        this->m_client->NewSocket(this->m_id_client);

        this->m_client->Connect(this->m_id_client,tc,CSocketTCPClient::IP);

        VCHAR rep_tram;
        this->m_client->Write(this->m_id_client,Tram(VCHAR{Tram::Com_bytes::SOH,RC_Apn::Com_bytes::Older,this->older?'1':'0',Tram::Com_bytes::EOT}).get_c_data());
        this->m_client->Read<2048>(this->m_id_client,rep_tram);
        if(!this->check_acknowledge(rep_tram))throw RC_Apn::Erreur(1,"erreur de transmition",RC_Apn::Erreur::WARNING);

        this->m_client->Write(this->m_id_client,Tram(VCHAR{Tram::Com_bytes::SOH,RC_Apn::Com_bytes::Tcp_client,'0',Tram::Com_bytes::EOT}).get_c_data());
        this->m_client->Read<2048>(this->m_id_client,rep_tram);
        if(!this->check_acknowledge(rep_tram))throw RC_Apn::Erreur(2,"erreur de transmition",RC_Apn::Erreur::WARNING);

        this->m_client->Write(this->m_id_client,Tram(VCHAR{Tram::Com_bytes::SOH,RC_Apn::Com_bytes::Debug_mode,this->debug_mode?'1':'0',Tram::Com_bytes::EOT}).get_c_data());
        this->m_client->Read<2048>(this->m_id_client,rep_tram);
        if(!this->check_acknowledge(rep_tram))throw RC_Apn::Erreur(3,"erreur de transmition",RC_Apn::Erreur::WARNING);
    }
}

///-------------------------------------------------------------
///enregistre tout les parametres utile en memoire vive
///-------------------------------------------------------------
void RC_Apn::init_conf_param(void)
{
    ///enregistre tout les parametres utile en memoire vive
    this->get_config(gp2::Conf_param::APERTURE,this->m_aperture);
    this->get_config(gp2::Conf_param::SHUTTERSPEED,this->m_shutterspeed);
    this->get_config(gp2::Conf_param::ISO,this->m_iso);
    this->get_config(gp2::Conf_param::FORMAT,this->m_format);
    this->get_config(gp2::Conf_param::TARGET,this->m_target);
    this->get_config(gp2::Conf_param::WHITE_BALANCE,this->m_wb);
    this->get_config(gp2::Conf_param::PICTURE_STYLE,this->m_effect);
    //this->m_file=this->get_config(gp2::Parameter::FILE);
}

///-------------------------------------------------------------
///verifie qu'un apn est bien accessible par gphoto2
///-------------------------------------------------------------
void RC_Apn::check_apn(void)
{
    //sauvegarde temporaire sur disque du resulatat
    if(!this->tcp_client)
    {
        _Data device;

        gp2::Auto_detect(device);

        if(this->debug_mode)
        {
            for(auto & i: device)
                std::clog << "device: "<<i<<std::endl;
        }

        if(device.size()<=0)
            throw RC_Apn::Erreur(1,"Aucun APN détecté",RC_Apn::Erreur::niveau::ERROR);
    }
    else
    {
        //envoie la demande de check apn au serveur
        this->m_client->Write(this->m_id_client,Tram(VCHAR{Tram::Com_bytes::SOH,RC_Apn::Com_bytes::Check_Apn,Tram::Com_bytes::EOT}).get_c_data());

        //attend la réponse
        Tram rep_tram=Read_Tram(Tram::Com_bytes::EOT,*this->m_client,this->m_id_client,1);

        if(!this->check_acknowledge(rep_tram.get_data()))
            throw RC_Apn::Erreur(1,"Tram incorrecte",RC_Apn::Erreur::niveau::ERROR);
    }
    throw RC_Apn::Erreur(1,"Le programme n'a pas reussi a check la présence d'APN",RC_Apn::Erreur::niveau::FATAL_ERROR);
}

///-------------------------------------------------------------
///prend une capture et enregistre celle ci sur le disuqe
///-------------------------------------------------------------
void RC_Apn::capture_EOS_DSLR(std::string inter,std::string iso,std::string exposure,std::string aperture,std::string target,std::string format, std::string shutter,std::string wb,std::string effect)
{
    if(this->debug_mode)
        std::clog << "capture debute" << std::endl;
    //on prepare la capture
    system("gphoto2 --set-config capture=on");

    //on parametre l'apn
    this->set_config(gp2::Conf_param::TARGET,target);
    this->set_config(gp2::Conf_param::FORMAT,format);
    this->set_config(gp2::Conf_param::APERTURE,aperture);
    this->set_config(gp2::Conf_param::ISO,iso);

    this->set_config(gp2::Conf_param::SHUTTERSPEED,shutter);
    this->set_config(gp2::Conf_param::WHITE_BALANCE,wb);
    this->set_config(gp2::Conf_param::PICTURE_STYLE,effect);

    //attente de 0.1 s pour evité busy sur apn
    std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(100));

    //commande gphoto de capture pour new et old
    if(!this->tcp_client)
    {
        if(!this->older)
            free_cmd("gphoto2 --set-config eosremoterelease=5 --wait-event=2s --set-config eosremoterelease=0 --wait-event="+exposure+"s",this->debug_mode);
        else
            free_cmd("gphoto2 --set-config bulb=1 --wait-event=2s --set-config bulb=0 --wait-event="+exposure+"s",this->debug_mode);
    }
    else
    {
        Tram tram,rep_tram;

        tram+=char(Tram::Com_bytes::SOH);
        tram+=char(RC_Apn::Com_bytes::Capture_Eos_Dslr);
        tram+=char(RC_Apn::Com_bytes::Exposure);
        tram+=exposure;
        tram+=char(Tram::Com_bytes::US);
        tram+=char(RC_Apn::Com_bytes::Intervalle);
        tram+=inter;
        tram+=char(Tram::Com_bytes::EOT);

        this->m_client->Write(this->m_id_client,tram.get_data());

        rep_tram=this->Recv(1);
    }

    if(this->debug_mode)
        std::clog << "capture termine" << std::endl;
}

///-------------------------------------------------------------
///configure un parametre de l'apn via gphoto2
///-------------------------------------------------------------
void RC_Apn::set_config(gp2::Conf_param const & param,std::string value)
{
   /* if(param==RC_Apn::Conf_param::FILE)
        return;*/
    if(this->debug_mode)
        std::clog << "set_config debute" <<std::endl;

    if(!this->tcp_client)
    {
        free_cmd("gphoto2 --set-config "+gp2::Conf_param_to_str(param)+"="+value,this->debug_mode);
    }
    else
    {
        Tram tram,rep_tram;

        tram+=char(Tram::Com_bytes::SOH);
        tram+=char(RC_Apn::Com_bytes::Set_Config);
        tram+=this->get_byte(param);
        tram+=value;
        tram+=char(Tram::Com_bytes::EOT);

        this->m_client->Write(this->m_id_client,tram.get_data());

        rep_tram=this->Recv(1);

    }
    if(this->debug_mode)
        std::clog << "set_config termine" <<std::endl;
}

///-------------------------------------------------------------
///renvoie toutes les possibilités pour un paramatre de l'apn (sans passer par gphoto2 seulement donné sauvegarder en memoire)
///-------------------------------------------------------------
RC_Apn::_Data & RC_Apn::get_parameter(gp2::Conf_param const & param)
{
    switch(param)
    {
        case gp2::Conf_param::APERTURE: return this->m_aperture; break;
        case gp2::Conf_param::SHUTTERSPEED: return this->m_shutterspeed; break;
        case gp2::Conf_param::ISO: return this->m_iso; break;
        case gp2::Conf_param::FORMAT: return this->m_format; break;
        case gp2::Conf_param::TARGET: return this->m_target; break;
        case gp2::Conf_param::WHITE_BALANCE: return this->m_wb; break;
        case gp2::Conf_param::PICTURE_STYLE: return this->m_effect; break;
       // case gp2::Conf_param::FILE: return this->m_file; break;

        default : return this->m_void;
    }
    return this->m_void;
}
///-------------------------------------------------------------
///renvoie le byte associer pour un paramatre de l'apn (sans passer par gphoto2 seulement donné sauvegarder en memoire)
///-------------------------------------------------------------
char RC_Apn::get_byte(gp2::Conf_param const & param)
{
    switch(param)
    {
        case gp2::Conf_param::APERTURE: return RC_Apn::Com_bytes::Aperture; break;
        case gp2::Conf_param::SHUTTERSPEED: return RC_Apn::Com_bytes::Shutterspeed; break;
        case gp2::Conf_param::ISO: return RC_Apn::Com_bytes::Iso; break;
        case gp2::Conf_param::FORMAT: return RC_Apn::Com_bytes::Format; break;
        case gp2::Conf_param::TARGET: return RC_Apn::Com_bytes::Target; break;
        case gp2::Conf_param::WHITE_BALANCE: return RC_Apn::Com_bytes::White_balance; break;
        case gp2::Conf_param::PICTURE_STYLE: return RC_Apn::Com_bytes::Picture_style; break;
        //case RC_Apn::Conf_param::FILE: return RC_Apn::Com_bytes::File; break;

        default : return static_cast<char>(0x00);
    }
    return static_cast<char>(0x00);
}

///-------------------------------------------------------------
///Telecharge une capture donné
///-------------------------------------------------------------
void RC_Apn::download(std::string const & why,std::string const & where)
{
    unsigned int id(0);

    //on extrait l'id correspondant a la chaine de caractere
    for(auto i=0u;i<this->m_file.size();i++)
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

///-------------------------------------------------------------
///Telecharge une capture donné
///-------------------------------------------------------------
void RC_Apn::download(unsigned int why)
{
    std::string fcast=ss_cast<unsigned int,std::string>(why);

    //on telecharge

    if(!this->tcp_client)
    {
        system(std::string("gphoto2 --get-file="+fcast).c_str());

        if(this->download_and_remove)
            system(std::string("gphoto2 -f 1 -d "+fcast).c_str());//preciser le -f en "/.../..."
    }
    else
    {
        Tram tram,rep_tram;
        tram+=char(Tram::Com_bytes::SOH);
        tram+=char(!this->download_and_remove?RC_Apn::Com_bytes::Download:RC_Apn::Com_bytes::Download_And_Remove);
        tram+=why;
        tram+=char(Tram::Com_bytes::EOT);

        //envoir et reception du server par le client
        this->m_client->Write(this->m_id_client,tram.get_data());

        std::string name("");
        //titre this->m_client->Read<2048>(this->m_id_client,rep_tram.get_data());

        rep_tram=this->Recv(15);
    }
}

///-------------------------------------------------------------
///renvoie toutes les possibilités pour un paramatre de l'apn
///-------------------------------------------------------------
void RC_Apn::get_config(gp2::Conf_param const & param, _Data & gc)
{
    //on eregistre via gphoto2 les  valeurs possible de l'apn sur disque

    if(!this->tcp_client)
    {
        if(this->debug_mode)
            std::clog << "recherche de config en local" <<std::endl;

        gp2::Get_config(param,gc);
    }
    else
    {
        if(this->debug_mode)
            std::clog << "recherche de config avec le serveur" <<std::endl;

        this->m_client->Write(this->m_id_client,Tram(VCHAR{Tram::Com_bytes::SOH,RC_Apn::Com_bytes::Get_Config,this->get_byte(param),Tram::Com_bytes::EOT}).get_c_data());

        Tram rep_tram=this->Recv(1);

        //lecture de gc
    }
}

///-------------------------------------------------------------
///verifie si la reponse du serveur est conforme
///-------------------------------------------------------------
bool RC_Apn::check_acknowledge(VCHAR const & rep_tram)
{
    if(rep_tram.size()>0)
    {
        if(rep_tram[0]==Tram::Com_bytes::SOH && rep_tram.back()==Tram::Com_bytes::EOT)
        {
            if(static_cast<int>(rep_tram[1])==Tram::Com_bytes::ACK)
                return true;
            else if(static_cast<int>(rep_tram[1])==Tram::Com_bytes::NAK)
            {
                if(this->debug_mode)
                {
                    std::cerr <<"erreur renvoyer par le serveur: " ;

                    for(auto &t : rep_tram)
                    {
                        if(t==Tram::Com_bytes::EOT)
                            break;
                        if(t==Tram::Com_bytes::SOH)
                            continue;

                        std::cerr << t ;
                    }

                    std::cerr<<std::endl;
                }

                return false;
            }
            else
            {
                if(this->debug_mode)
                {
                    std::cerr <<"erreur de tram avec le serveur"<<std::endl;

                    for(auto &t : rep_tram)
                        std::cerr << "0x" << std::hex << static_cast<int>(t) << std::dec <<" ";
                    std::cerr<<std::endl;
                }

                return false;
            }
        }
        else
        {
            if(this->debug_mode)
                std::cerr << " header et footer tram non respecté par le serveur"<<std::endl;
            return false;
        }
    }
    else
    {
        std::cout <<"erreur de connection avec le serveur" <<std::endl;
        return false;
    }
    return false;
}

///-------------------------------------------------------------
///reception reponse serveur
///-------------------------------------------------------------
Tram RC_Apn::Recv(int time_out)
{
    Tram rep_tram;
    try
    {
        rep_tram=Read_Tram(Tram::Com_bytes::EOT,*this->m_client,this->m_id_client,time_out);

        if(!this->check_acknowledge(rep_tram.get_data()))
                throw RC_Apn::Erreur(1,"erreur de transmition",RC_Apn::Erreur::WARNING);
    }
    catch(Error & e)
    {
        std::cerr << e.what() <<std::endl;
    }

    return rep_tram;
}

