#include "Intervallometre.hpp"

int main(int argc,char ** argv)
{
    Intervallometre inter;
    RC_Apn apn;

    //shunt
    //inter.debug_mode=true;
    //apn.debug_mode=true;

    if(argc>=2)
    {
        inter.debug_mode=(std::string(argv[1])=="--debug-mode"||std::string(argv[1])=="-d"?true:false);
        apn.debug_mode=(std::string(argv[1])=="--debug-mode"||std::string(argv[1])=="-d"?true:false);

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
            //lister en plus les commande possible // lire fichier help redefinit

            return 0;
        }
    }

    if(!apn.check_apn())
    {
        std::cerr << "aucun apn detecté" << std::endl;

        return -1;
    }

    apn.init_parameter();

    if(!inter.load("Sequance_1"))
    {
        std::cerr << "aucune sequance détectée" << std::endl;
        return -1;
    }
    else if(!inter.check_sequance(apn))
    {
        std::cerr << "des erreurs ont été trouvées dans la séquance --debug-mode pour détails" << std::endl;
        return -1;
    }
    else if(argc>=2)
    {
        if(std::string(argv[1])=="--list-conf"||std::string(argv[1])=="-l")
        {
            //lister en plus les commande possible // lire fichier help redefinit

            return 0;
        }
    }


    std::cout << inter.size() <<" instructions chargées" << std::endl<<std::endl;

    inter.run_seq(apn);

    return 0;
}
