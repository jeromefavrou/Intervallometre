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
        inter.debug_mode=(argv[1]=="--debug-mode"||argv[1]=="-D"?true:false);
        apn.debug_mode=(argv[1]=="--debug-mode"||argv[1]=="-D"?true:false);
    }

    if(!apn.check_apn())
    {
        std::cerr << "aucun apn detecté" << std::endl;

        return -1;
    }

    if(!inter.load("Sequance_1"))
    {
        std::cerr << "aucune sequance détectée" << std::endl;
        return -1;
    }


    std::cout << inter.size() <<" instructions chargées" << std::endl;

    inter.run_seq(apn);

    return 0;
}
