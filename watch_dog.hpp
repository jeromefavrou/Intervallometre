#ifndef WATCH_DOG_HPP_INCLUDED
#define WATCH_DOG_HPP_INCLUDED

#define _DEBUG_MOD

#include "net_device.hpp"
#include "serveur.cpp" //passer hpp
#include "picosha2.hpp"
#include <thread>
#include <memory>
#include <sstream>
#include <fstream>


class Watch_dog
{
public:
    Watch_dog(void);

    bool init_server(uint32_t const port);
    void main_loop_server(void);
    void update_debit(float time_ms);

    void watch(void);

    ~Watch_dog(void);

private:
    struct State
    {
        bool stopping:1;
        bool quarentine:1;
        bool as_client:1;
        bool tcp_ip:1;
        bool monitor:1;
    };

    void server_client(void);
    void accept_client(void);
    bool auth(void);
    bool rcv_data(std::stringstream & data);
    bool check_pwd(void);


    void restart(void);
    void stop(void);
    void quarentine(void);
    void log(void);

    float debit_Mo_s;

    std::string password;
    State states;
    std::unique_ptr<Net_devices> devices;
    std::unique_ptr<CSocketTCPServeur> Server;
};

#endif // WATCH_DOG_HPP_INCLUDED
