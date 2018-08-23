#include "Intervallometre.hpp"


int main(int argc,char ** argv)
{
    Intervallometre inter;

    //shunt
    //inter.debug_mode=true;

    if(argc>=2)
    {
        inter.debug_mode=(argv[1]=="--debug-mode"||argv[1]=="-D"?true:false);
    }

    if(!inter.load("Sequance_1"))
        return -1;

    std::cout << inter.size() <<" instructions chargées" << std::endl;

    inter.run_seq();

    return 0;
}
