#include "Intervallometre.hpp"

int main(int argc,char ** argv)
{
    GNU Api;
    Intervallometre inter;
    RC_Apn apn;

    if(argc>=2)
    {
        inter.debug_mode=(std::string(argv[1])=="--debug-mode"||std::string(argv[1])=="-d"?true:false);
        apn.debug_mode=(std::string(argv[1])=="--debug-mode"||std::string(argv[1])=="-d"?true:false);
        apn.download_and_remove=(std::string(argv[1])=="--download-and-remove"||std::string(argv[1])=="-f"?true:false);

        if(std::string(argv[1])=="--version"||std::string(argv[1])=="-v")
        {
            std::cout <<"Intervallometre version = 0.0.0"<<std::endl;
            system("gphoto2 -v");

            return 0;
        }

        if(std::string(argv[1])=="--upgrade"||std::string(argv[1])=="-u")
        {
            system("sudo apt-get update");
            system("sudo apt-get install gphoto2 libgphoto2*");

            return 0;
        }

        if(std::string(argv[1])=="--help"||std::string(argv[1])=="-h")
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
        if(std::string(argv[1])=="--old-apn"||std::string(argv[1])=="-o")
        {
            apn.older=true;
        }
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

    std::string last_capt;

    inter.run_seq(apn,last_capt);

    Api.display(last_capt);

    return 0;
}
