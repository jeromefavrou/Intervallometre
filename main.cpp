#include "Intervallometre.hpp"
#include "parser.hpp"


int main(int argc,char ** argv)
{
    std::vector<std::string> Parametre=parser::parser(argc,argv);

    GNU Api;
    Intervallometre inter;
    RC_Apn apn;

    inter.debug_mode= parser::find("--debug-mode",Parametre) || parser::find("-d",Parametre);

    apn.debug_mode= inter.debug_mode;
    apn.download_and_remove= parser::find("--download-and-remove",Parametre) || parser::find("-f",Parametre);
    apn.tcp_client= parser::find("--tcp-client",Parametre) || parser::find("-t",Parametre);
    apn.older=parser::find("--old-apn",Parametre) || parser::find("-o",Parametre);

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
            std::cout << "aide introuvable" <<std::endl;
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

        return -1;
    }

    apn.init_parameter();

    if(!inter.load("Sequance_2"))
    {
        std::cerr << "aucune sequance détectée" << std::endl;

        return -1;
    }
    else if(!inter.check_sequance(apn))
    {
        std::cerr << "des erreurs ont été trouvées dans la séquance --debug-mode pour détails" << std::endl;
        return -1;
    }

    std::cout << inter.size() <<" instructions chargées" << std::endl<<std::endl;

    std::string last_capt("");

    std::thread th(&GNU::raw_display,&Api,std::ref(last_capt));
    //std::thread th(&GNU::cam_display,&Api,0);

    std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000));

    inter.run_seq(apn,last_capt);

    last_capt="exit";

    th.join();


    return 0;
}
