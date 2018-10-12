#include <iostream>
#include <array>
#include "serveur.cpp"
#include "Remote_controle_apn.hpp"
#include "tram.hpp"

#define SERVEUR_ID 0
#define CLIENT_ID 1
#define NbStats 2 //{continuer,client}
#define client 1
#define continuer 0

Tram interpretteur(VCHAR const & tram, RC_Apn & apn)
{
    Tram t;
    if(tram[0] !=Tram::Com_bytes::SOH || tram.back() != Tram::Com_bytes::EOT)
    {
        t;
        t+=char(Tram::Com_bytes::SOH);
        t+=char(Tram::Com_bytes::NAK);
        t+="header ou/et footer incorrect";
        t+=char(Tram::Com_bytes::EOT);

        return t;
    }


    for(auto  i=1;i< tram.size();i++)
    {
        if(tram[i]==Tram::Com_bytes::EOT)
            break;

        if(static_cast<int>(tram[i])==RC_Apn::Com_bytes::Check_Apn)
        {
            if(apn.check_apn())
                return Tram(VCHAR{Tram::Com_bytes::SOH,Tram::Com_bytes::ACK,Tram::Com_bytes::EOT});
            else
            {
                t+=char(Tram::Com_bytes::SOH);
                t+=char(Tram::Com_bytes::NAK);
                t+="aucun apn detecte";
                t+=char(Tram::Com_bytes::EOT);

                return t.get_data();
            }

        }
        else if(static_cast<int>(tram[i])==RC_Apn::Com_bytes::Set_Config)
        {
        }
        else if(static_cast<int>(tram[i])==RC_Apn::Com_bytes::Get_Config)
        {
        }
        else if(static_cast<int>(tram[i])==RC_Apn::Com_bytes::Capture_Eos_Dslr)
        {
        }
        else if(static_cast<int>(tram[i])==RC_Apn::Com_bytes::Download)
        {
        }
        else if(static_cast<int>(tram[i])==RC_Apn::Com_bytes::Download_And_Remove)
        {
        }

    }

    t+=char(Tram::Com_bytes::SOH);
    t+=char(Tram::Com_bytes::NAK);
    t+="trame invalide du client";
    t+=char(Tram::Com_bytes::EOT);

    return t.get_data();
}

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

void th_Ecoute(CSocketTCPServeur & Server,std::array<bool,NbStats> &stats,RC_Apn & apn)
{
    //boucle tant que le bit continue est a 1
    while(stats[continuer])
    {
        if(stats[client])
        {
            VCHAR BufferReq;

            //ecoute en attante d'une requte du client.
            int lenght=Server.Read<2048>(CLIENT_ID,BufferReq);


            //si deconnection du client
            if(lenght==0)
            {
                std::cout<<"Le client à déconnecté"<<std::endl;
                //mise a 0 de l'octet presance client
                stats[client]=false;
            }
            //sinon si requete reçu
            else if(lenght>0)
            {
                //affichage tram
                for(auto &i : BufferReq)
                    std::cout <<"0x"<<std::hex <<static_cast<int>(i)<<" " ;
                std::cout <<std::dec<< std::endl;

                Server.Write(CLIENT_ID,interpretteur(BufferReq,apn).get_c_data());

            }
        }
        //si pas de client limite les frames
        std::this_thread::sleep_for( std::chrono::duration<double, std::milli>(20));
    }
}

int main()
{

    RC_Apn apn;

    apn.debug_mode=true;

    CSocketTCPServeur serveur;

    std::array<bool,NbStats> Stats{true,false};

    try
    {
        serveur.NewSocket(SERVEUR_ID);
        serveur.BindServeur(SERVEUR_ID,INADDR_ANY,9876);
        serveur.Listen(SERVEUR_ID,1);

        std::this_thread::sleep_for( std::chrono::duration<double, std::milli>(2000));

        std::thread Connect(th_Connection,std::ref(serveur),std::ref(Stats));
        Connect.detach();
        std::thread Listen(th_Ecoute,std::ref(serveur),std::ref(Stats),std::ref(apn));

        std::cin.get();

        Stats[continuer]=false;


        Listen.join();
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
