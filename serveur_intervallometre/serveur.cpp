#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include "tram.hpp"

class CSocketTCPServeur
{
    public:
    CSocketTCPServeur(void)
    {

    }

    ~CSocketTCPServeur(void)
    {
    }

    void NewSocket(unsigned int const &idx)
    {
        Sk_Channel[idx]=std::shared_ptr<SOCKET>(new SOCKET(socket(AF_INET,SOCK_STREAM,0)));

        if(*Sk_Channel[idx]==INVALID_SOCKET)
            throw std::string("Socket Serveur[\033[1;5;35mNOK\033[0m]");

        std::cout<<"Socket ["<<idx<<"][\033[1;33mOK\033[0m]"<<std::endl;
    }

    void CloseSocket(unsigned int const & idx)
    {
        std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
        it=Sk_Channel.find(idx);

        if(it!=Sk_Channel.end())
        {
            closesocket(*Sk_Channel[idx]);
            Sk_Channel.erase(it);
        }
    }

    void BindServeur(unsigned int const & idx,uint32_t const &addr,uint32_t const &port)
    {
        std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
        it=Sk_Channel.find(idx);

        if(it==Sk_Channel.end())
            throw std::range_error("cannot use a Socket not open");

        this->ServerAdress.sin_addr.s_addr=addr;
        this->ServerAdress.sin_family=AF_INET;
        this->ServerAdress.sin_port=htons(port);

        if(bind(*Sk_Channel[idx],(struct sockaddr *)&ServerAdress,sizeof(ServerAdress))==SOCKET_ERROR)
            throw std::string("bind [\033[1;5;35mNOK\033[0m]");

        std::cout<<"bind [\033[1;33mOK\033[0m] => server listen on: "<<addr<<":"<<port<<std::endl;
    }
    void Listen(unsigned int const &idx,unsigned int const &nb)
    {
        std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
        it=Sk_Channel.find(idx);

        if(it==Sk_Channel.end())
            throw std::range_error("cannot use a Socket not open");

        listen(*Sk_Channel[idx],nb);
    }

    void AcceptClient(unsigned int const &idx,unsigned int const &idxclient)
    {
        std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
        it=Sk_Channel.find(idx);

        if(it==Sk_Channel.end())
            throw std::range_error("cannot use a Socket not open");

        int cu=sizeof(struct sockaddr_in);
        Sk_Client[idxclient]=std::shared_ptr<SOCKET>(new SOCKET(accept(*Sk_Channel[idx],(struct sockaddr *)&ClientAdress,(socklen_t *)&cu)));

        if(*Sk_Client[idxclient]<0)
            throw std::string("Connection [\033[1;5;35mNOK\033[0m]");

        std::cout<<"\nConnection [\033[1;33mOK\033[0m] with "<<inet_ntoa(ClientAdress.sin_addr)<<":"<<ntohs(ClientAdress.sin_port)<<std::endl;

    }

    void Write(unsigned int const &idx,VCHAR const &buffer)
    {
        std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
        it=Sk_Client.find(idx);

        if(it==Sk_Client.end())
            throw std::range_error("cannot use a Socket not open");

            send(*Sk_Client[idx],buffer.data(),buffer.size(),0);
    }
    template<unsigned int octets>int Read(unsigned int const &idx,VCHAR &buffer)
    {
        std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
        it=Sk_Client.find(idx);
        char buf[octets];
        if(it==Sk_Client.end())
            throw std::range_error("cannot use a Socket not open");

        int lenght= recv(*Sk_Client[idx] ,buf, octets,0);

        buffer.clear();
        for(int i=0;i<lenght;i++)
            buffer.push_back(buf[i]);
        return lenght;
    }

private:

    std::map<unsigned int,std::shared_ptr<SOCKET>> Sk_Channel;
    std::map<unsigned int,std::shared_ptr<SOCKET>> Sk_Client;
    SOCKADDR_IN ServerAdress,ClientAdress;
};

