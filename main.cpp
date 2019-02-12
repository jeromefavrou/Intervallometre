#include "Intervallometre.hpp"
#include "parser.hpp"

int main(int argc,char ** argv)
{
    std::vector<std::string> Parametre=parser::parser(argc,argv);

    GUI Api;
    Intervallometre inter;
    RC_Apn apn;

    inter.debug_mode= parser::find("--debug-mode",Parametre) || parser::find("-d",Parametre);

    apn.debug_mode= inter.debug_mode;
    apn.download_and_remove= parser::find("--download-and-remove",Parametre) || parser::find("-f",Parametre);
    apn.tcp_client= parser::find("--tcp-client",Parametre) || parser::find("-t",Parametre);
    apn.older=parser::find("--old-apn",Parametre) || parser::find("-o",Parametre);

    //apn.tcp_client=true;
    //apn.debug_mode=true;

    if(apn.tcp_client)
    {
        std::string addr(""),mt("");
        uint32_t port(0);

        std::fstream If("client.conf",std::ios::in);

        if(!If)
        {
            std::cerr << "fichier de config client introuvable" << std::endl;
            notify_send("fichier de config client introuvable");
            return -1;
        }

        If >> mt;
        if(mt=="IP")
            If >> addr;
        else
        {
            std::cerr << "fichier de config client corrompue" << std::endl;
            notify_send("fichier de config client corrompue");
            return -1;
        }

        If >> mt;
        if(mt=="PORT")
            If >> port;
        else
        {
            std::cerr << "fichier de config client corrompue" << std::endl;
            notify_send("fichier de config client corrompue");
            return -1;
        }

        if(!apn.connect(addr,port))
        {
            std::cerr << "connection impossible: " << addr <<":"<< port << " -> verifié les parametres du ficher de config client si connection NOK debug mode pour plus d'info"<<std::endl;
            notify_send("connection impossible");
            return -1;
        }


    }

    if(parser::find("--version",Parametre) || parser::find("-v",Parametre))
    {
        std::cout <<"Intervallometre version = 0.0.0"<<std::endl;
        system("gphoto2 -v");

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
            std::cerr << "aide introuvable" <<std::endl;
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

    if(!apn.check_apn())
    {
        std::cerr << "aucun apn detecté" << std::endl;
        notify_send("aucun apn detecté");

        return -1;
    }
    apn.init_parameter();
    if(!inter.load("debug_seq"))
    {
        std::cerr << "aucune sequance détectée" << std::endl;
        notify_send("aucune sequance détectée");

        return -1;
    }
    else if(!inter.check_sequance(apn))
    {
        std::cerr << "des erreurs ont été trouvées dans la séquance --debug-mode pour détails" << std::endl;
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


    return 0;
}
