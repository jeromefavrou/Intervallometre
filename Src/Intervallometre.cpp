#include "Intervallometre.hpp"

///-------------------------------------------------------------
///initialisation aux valeurs par defaut
///-------------------------------------------------------------
Intervallometre::Intervallometre(void)
{
    this->debug_mode=false;
}

///-------------------------------------------------------------
///charge la sequance
///-------------------------------------------------------------
void Intervallometre::load(std::string const & file)
{
    std::fstream If;

    If.open(file,std::ios::in);

    if(!If || If.fail() || If.bad() || If.eof())
        throw Intervallometre::Erreur(1,"sequance \""+file+"\" introuvable",Intervallometre::Erreur::ERROR);

    this->m_seq.clear();

    std::string instruction;

    while(getline(If,instruction))
    {
        if(instruction.size()>2)
        {
            if(instruction[0]=='/'&&instruction[1]=='/')//ignore une ligne de sequance mais affiche au terminal celle ci
            {
                std::cout << instruction <<std::endl;
                continue;
            }
            if(instruction[0]=='#')//ignore une ligne de sequance
                continue;
        }

        std::stringstream ss_buf;

        ss_buf << instruction;

        //on vien interpreter le reste de la sequance
        this->m_seq.push_back(this->interpreter(ss_buf));
    }
}


