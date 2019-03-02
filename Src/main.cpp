#define __DEBUG_MODE
//#define __TCP_MODE

#define MNT_CONF_PATH ".mnt_configure"
#define CLIENT_CONF_PATH ".tcp_configure"

#include "Intervallometre.hpp"
#include "parser.hpp"

void init_configure(struct gp2::mnt & _mount)
{
    std::fstream If(MNT_CONF_PATH,std::ios::in);

    if(!If || If.bad() || If.fail())
        throw Error(1,"erreur a la lecture de \""+std::string(MNT_CONF_PATH)+"\"",Error::niveau::WARNING);

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

    try
    {
        if(system(nullptr));
        else
            throw Error(1,"les commandes system ne peuvent etre utilise",Error::niveau::FATAL_ERROR)

        init_configure(_mount);
    }
    catch(Error & e)
    {
        std::cerr << e.what() << std::endl;

        if(e.get_niveau()!=Error::niveau::WARNING)
            return -1;

        #ifndef WIN32
        _mount.cmd="gio mount";
        _mount.path="/run/user/1000/gvfs";
        #elif
        return -1;
        #endif // WIN32
    }

    if(apn.tcp_client)
    {
        std::string mt("");

        std::fstream If(CLIENT_CONF_PATH,std::ios::in);

        try
        {
            if(!If || If.bad() || If.fail())
                throw Error(1,"erreur a la lecture de \""+std::string(CLIENT_CONF_PATH)+"\"",Error::niveau::WARNING);

            If >> tc.addr;
            If >> tc.port;
        }
        catch(Error & e)
        {
            std::cerr << e.what() <<std::endl;

            if(e.get_niveau()!=Error::niveau::WARNING)
                return -1;

            tc.addr="127.0.0.1";
            tc.port=9876;
        }

        try
        {
            apn.connect(tc);
        }
        catch(Error & e)
        {
            std::cerr << e.what() <<std::endl;

            if(e.get_niveau()!=Error::niveau::WARNING)
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

        inter.load("debug_seq");

        inter.check_sequance(apn);

        std::cout << inter.size() <<" instructions chargées" << std::endl<<std::endl;
    }
    catch(Error & e)
    {
        std::cerr << e.what() << std::endl;

        if(e.get_niveau()!=Error::niveau::WARNING)
            return -1;
    }

    std::string last_capt("");

    std::thread th(&GUI::raw_display,&Api,std::ref(last_capt));
    //std::thread th(&GUI::cam_display,&Api,0); a vir pour asi

    std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000));

    inter.run_seq(apn,last_capt);

    last_capt="exit";

    th.join();

    gp2::Mount(_mount);

    return 0;
}
