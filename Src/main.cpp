#define __DEBUG_MODE
//#define __TCP_MODE

#define CONFIGURE_PATH ".configure"

#include "Intervallometre.hpp"
#include "parser.hpp"

void init_configure(struct gp2::mnt & _mount)
{
    std::fstream If(CONFIGURE_PATH,std::ios::in);

    if(!If || If.bad() || If.fail())
        throw std::string("erreur a la lecture de .config");

    std::getline(If,_mount.cmd);
    std::getline(If,_mount.path);
}

int main(int argc,char ** argv)
{
    std::vector<std::string> Parametre=parser::parser(argc,argv);

    struct gp2::mnt _mount;
    struct t_connect tc;

    GUI Api;
    Intervallometre inter;
    RC_Apn apn;

    bool debug_mode=parser::find("--debug-mode",Parametre) || parser::find("-d",Parametre);

    #ifdef __DEBUG_MODE
        debug_mode=true;
    #endif // __DEBUG_MODE

    inter.debug_mode= debug_mode;
    apn.debug_mode= debug_mode;

    apn.tcp_client= parser::find("--tcp-client",Parametre) || parser::find("-t",Parametre);

    #ifdef __TCP_MODE
        apn.tcp_client=true;
    #endif // __DEBUG_MODE_TCP

    apn.download_and_remove= parser::find("--download-and-remove",Parametre) || parser::find("-f",Parametre);
    apn.older=parser::find("--old-apn",Parametre) || parser::find("-o",Parametre);

    if(system(nullptr));
    else
    {
        std::cerr << _print<unix_color::RED>("les commandse system ne peuve etre utilise") << std::endl;
        return -1;
    }

    try
    {
        init_configure(_mount);
    }
    catch(std::string const & e)
    {
        if(debug_mode)
            std::cerr << _print<unix_color::BLUE>(e) << std::endl;

        #ifndef WIN32
        _mount.cmd="gio mount";
        _mount.path="/run/user/1000/gvfs";
        #elif
        return -1;
        #endif // WIN32
    }
    catch(std::exception const & e)
    {
        std::cerr << _print<unix_color::BLUE>(e.what()) << std::endl;

        return -1;
    }

    if(apn.tcp_client)
    {
        std::string mt("");

        std::fstream If("client.conf",std::ios::in);

        if(!If)
        {
            std::cerr << "fichier de config client introuvable" << std::endl;
            notify_send("fichier de config client introuvable");
            return -1;
        }

        If >> mt;
        if(mt=="IP")
            If >> tc.addr;
        else
        {
            std::cerr << "fichier de config client corrompue" << std::endl;
            notify_send("fichier de config client corrompue");
            return -1;
        }

        If >> mt;
        if(mt=="PORT")
            If >> tc.port;
        else
        {
            std::cerr << _print<unix_color::RED>("fichier de config client corrompue") << std::endl;
            notify_send("fichier de config client corrompue");
            return -1;
        }

        if(!apn.connect(tc))
        {
            std::cerr << _print<unix_color::RED>("connection impossible: ") << tc.addr <<":"<< tc.port << " -> verifié les parametres du ficher de config client si connection NOK debug mode pour plus d'info"<<std::endl;
            notify_send("connection impossible");
            return -1;
        }
    }

    if(parser::find("--version",Parametre) || parser::find("-v",Parametre))
    {
        std::cout <<"Intervallometre version = 0.1.0"<<std::endl;
        std::cout <<"Libraw version: "<<LibRaw::version()<<std::endl;
        system("gphoto2 -v");
        system("gphotofs -v");

        std::cout <<"Opencv version = "<< CV_MAJOR_VERSION<<"."<< CV_MINOR_VERSION <<std::endl;

        return 0;
    }

    if(parser::find("--upgrade",Parametre) || parser::find("-u",Parametre))
    {
        system("sudo apt-get update");
        system("sudo apt-get install gphoto2 libgphoto2*");

        return 0;
    }

    if(parser::find("--help",Parametre) || parser::find("-h",Parametre))
    {
        system("firefox https://github.com/jeromefavrou/Intervallometre/wiki");

        std::fstream If("Help",std::ios::in);

        if(!If)
        {
            std::cerr << _print<unix_color::BLUE>("aide introuvable") <<std::endl;
            notify_send("aide introuvable");
        }

        else
        {
            std::string line_h("");
            while(std::getline(If,line_h))
                std::cout << line_h << std::endl;
        }

        return 0;
    }

    try
    {
        apn.check_apn();

        gp2::Unmount(_mount);

        std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(3000));

        apn.init_conf_param();
    }
    catch(Error  & e)
    {
        std::cerr << e.what() << std::endl;

        if(e.get_niveau()!=Error::niveau::WARNING)
            return -1;
    }
    catch(std::exception const & e)
    {
        std::cerr << e.what() << std::endl;
    }

    if(!inter.load("debug_seq"))
    {
        std::cerr << _print<unix_color::RED>("aucune sequance détectée") << std::endl;
        notify_send("aucune sequance détectée");

        return -1;
    }
    else if(!inter.check_sequance(apn))
    {
        std::cerr << _print<unix_color::RED>("des erreurs ont été trouvées dans la séquance --debug-mode pour détails") << std::endl;
        notify_send("des erreurs ont été trouvées dans la séquance");

        return -1;
    }

    std::cout << inter.size() <<" instructions chargées" << std::endl<<std::endl;

    std::string last_capt("");

    std::thread th(&GUI::raw_display,&Api,std::ref(last_capt));
    //std::thread th(&GUI::cam_display,&Api,0);

    std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000));

    inter.run_seq(apn,last_capt);

    last_capt="exit";

    th.join();

    gp2::Mount(_mount);

    return 0;
}
