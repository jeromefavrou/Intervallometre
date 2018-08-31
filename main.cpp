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
        inter.debug_mode=(std::string(argv[1])=="--debug-mode"||std::string(argv[1])=="-D"?true:false);
        apn.debug_mode=(std::string(argv[1])=="--debug-mode"||std::string(argv[1])=="-D"?true:false);
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

    if(argc>=2)
    {
        if(std::string(argv[1])=="--help"||std::string(argv[1])=="-h")
        {
            apn.help();
            inter.help();

            return 0;
        }
    }

    std::cout << inter.size() <<" instructions chargées" << std::endl<<std::endl;

    inter.run_seq(apn);

    return 0;
}
