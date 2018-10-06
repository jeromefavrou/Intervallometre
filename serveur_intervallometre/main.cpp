#include <iostream>
#include <array>
#include "serveur.cpp"
#include "Remote_controle_apn.hpp"

#define SERVEUR_ID 0
#define CLIENT_ID 1
#define NbStats 2 //{continuer,client}
#define client 1
#define continuer 0

void th_Connection(CSocketTCPServeur & Server,std::array<bool,NbStats> & stats)
{
    //boucle tant que le bit continue est a 1
    while(stats[continuer])
    {
        if(!stats[client])
        {
            //attant 2 secondes avant de rafraichire le terminal
            std::this_thread::sleep_for( std::chrono::duration<double, std::milli>(2000));
            system("clear");

            std::cout<<"attente du client..."<<std::endl;

            //attante du client
            Server.AcceptClient(SERVEUR_ID,CLIENT_ID);

            //mise a 1 octet client
            stats[client]=true;
        }

        //si pas de client limite les frames
        std::this_thread::sleep_for( std::chrono::duration<double, std::milli>(20));
    }
}

int main()
{

    RC_Apn apn;

    CSocketTCPServeur serveur;

    std::array<bool,NbStats> Stats{true,false};

    try
    {
        serveur.NewSocket(SERVEUR_ID);
        serveur.BindServeur(SERVEUR_ID,INADDR_ANY,9876);
        serveur.Listen(SERVEUR_ID,1);

        std::this_thread::sleep_for( std::chrono::duration<double, std::milli>(2000));

        std::thread Connect(th_Connection,std::ref(serveur),std::ref(Stats));

        std::cin.get();

        Stats[continuer]=false;

        Connect.join();
    }
    catch(std::string const & error)
    {
        std::cerr<<error<<std::endl;
    }
    catch(std::range_error const & error)
    {
        std::cerr<<error.what()<<std::endl;
    }

    serveur.CloseSocket(SERVEUR_ID);


    return 0;
}