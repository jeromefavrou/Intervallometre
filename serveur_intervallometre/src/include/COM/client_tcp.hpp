#ifndef CLIENT_TCP_HPP_INCLUDED
#define CLIENT_TCP_HPP_INCLUDED

#ifdef __linux ||  __unix || __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define closesocket(s) close(s)

#endif

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

class CSocketTCPClient
{
    public:
    enum typeip{HOSTNAME,IP};

    CSocketTCPClient(void)
    {

    }

    ~CSocketTCPClient(void)
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

    void Connect(unsigned int const & idx,std::string const &addr,uint32_t const &port,typeip const & tip)
    {
        std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
        it=Sk_Channel.find(idx);

        if(it==Sk_Channel.end())
            throw std::range_error("cannot use a Socket not open");

        if(tip==this->IP)
            this->ServerAdress.sin_addr.s_addr=inet_addr(addr.c_str());
        else if(tip==this->HOSTNAME)
        {
            struct hostent * hostinfo = nullptr;
            hostinfo = gethostbyname((const char*)addr.c_str());

            if (hostinfo == nullptr)
                throw std::string("hostname dont find");

            this->ServerAdress.sin_addr=*(IN_ADDR *) hostinfo->h_addr;
        }

        this->ServerAdress.sin_family=AF_INET;
        this->ServerAdress.sin_port=htons(port);

        if (connect(*Sk_Channel[idx] , (struct sockaddr *)&ServerAdress , sizeof(ServerAdress)) == SOCKET_ERROR)
            throw std::string("Connection [\033[1;5;35mNOK\033[0m]");

        std::cout<<"Connection [\033[1;33mOK\033[0m] => connected with: "<<addr<<":"<<port<<std::endl;
    }

    void Write(unsigned int const &idx,VCHAR const & buffer)
    {
        std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
        it=Sk_Channel.find(idx);

        if(it==Sk_Channel.end())
            throw std::range_error("cannot use a Socket not open");

        send(*Sk_Channel[idx],buffer.data(),buffer.size(),0);
    }

    template<unsigned int octets>int Read(unsigned int const &idx,VCHAR &buffer)
    {
        std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
        it=Sk_Channel.find(idx);
        char buf[octets];
        if(it==Sk_Channel.end())
            throw std::range_error("cannot use a Socket not open");

        int lenght= recv(*Sk_Channel[idx] ,buf,octets,0);
        for(auto i=0u;i<lenght;i++)
            buffer.push_back(buf[i]);
        return lenght;
    }

private:

    std::map<unsigned int,std::shared_ptr<SOCKET>> Sk_Channel;
    SOCKADDR_IN ServerAdress;
};

#endif // CLIENT_TCP_HPP_INCLUDED
