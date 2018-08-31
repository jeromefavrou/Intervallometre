#ifndef INTERVALLOMETRE_HPP_INCLUDED
#define INTERVALLOMETRE_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <algorithm>

#include "Remote_controle_apn.hpp"

class Intervallometre
{
public:
    bool debug_mode;

    struct Sequance
    {
        bool user_local:1;
        bool wait:1;

        int frame;
        float intervalle;
        float delay;
        std::string iso;
        int exposure;
        std::string aperture;
        std::string target;
        std::string format;
        std::string shutter;
        std::string effect;
        std::string wb;

    };

    Intervallometre(void)
    {
        this->debug_mode=false;
    }

    bool load(std::string const & file) noexcept
    {
        std::fstream If;
        try
        {
            If.open(file,std::ios::in);

            if(!If || If.fail() || If.bad() || If.eof())
                throw std::string("erreur a l'ouverture de "+file);

            this->m_seq.clear();

            std::string instruction;

            while(getline(If,instruction))
            {
                if(instruction.size()>2)
                {
                    if(instruction[0]=='/'&&instruction[1]=='/')
                    {
                        std::cout << instruction <<std::endl;
                        continue;
                    }
                    if(instruction[0]=='#')
                        continue;
                }

                std::stringstream ss_buf;

                ss_buf << instruction;

                this->m_seq.push_back(this->interpreter(ss_buf));
            }

            return true;
        }
        catch(std::string const & error)
        {
            if(this->debug_mode)
                std::cerr << "0x01 " << error <<std::endl;

            return false;
        }
        catch(std::exception const & error)
        {
            if(this->debug_mode)
                std::cerr << "0x02 " << error.what() <<std::endl;

            return false;
        }

        return false;
    }

    void run_seq(RC_Apn & apn)
    {
        if(this->debug_mode)
            std::cout << " La sequance commance" <<std::endl<<std::endl;
        for(auto & seq:this->m_seq)
        {
            if(seq.user_local)
            {
                if(seq.wait)
                {
                    std::cin.clear();
                    std::cout <<std::endl << "Appuis sur Enter requis" << std::endl<< std::endl;
                    std::cin.get();
                }

                if(seq.delay>0)
                {
                    std::cout<<std::endl << "Attendre " << seq.delay <<" s"<<std::endl<<std::endl;
                    std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(seq.delay*1000));
                }
            }
            else
            {
                for(auto i=0;i<seq.frame;i++)
                {
                    std::cout <<std::endl<< "frame "<<i+1<<"/"<<seq.frame<<std:: endl;

                    if(apn.check_apn())
                    {
                        std::cout <<"ne pas débrancher apn capture en cour iso: " << seq.iso << " exposition: "<<seq.exposure<<" ouverture: " <<seq.aperture <<std::endl;

                        std::stringstream ss_t;
                        ss_t << seq.exposure-1;
                        std::string expo;
                        ss_t >>expo;


                        ss_t.clear();
                        ss_t << seq.intervalle;
                        std::string inter;
                        ss_t >>inter;

                        apn.capture_new_EOS_DSLR(inter,seq.iso,expo,seq.aperture,"1","9","bulb","0","4");
                    }
                    else
                    {
                        std::cout << "capture annulée aucun apn detecté" << std::endl;
                    }
                }
            }
        }
    }

    size_t size(void)const noexcept
    {
        return this->m_seq.size();
    }

    bool check_sequance(RC_Apn & apn)
    {
        bool check(true);
        int line(0);
        for(auto & i: this->m_seq)
        {
            line++;

            std::stringstream ss_buff;
            std::string val;

            if(i.aperture!="-1")
            {
                ss_buff <<i.aperture;
                ss_buff >>val;
                if(check)
                    check=this->check_cmd(apn,RC_Apn::Parameter::APERTURE,line,val);
                else
                    this->check_cmd(apn,RC_Apn::Parameter::APERTURE,line,val);
            }

            if(i.shutter!="-1")
            {
                ss_buff.clear();
                ss_buff <<i.shutter;
                ss_buff >>val;
                if(check)
                    check=this->check_cmd(apn,RC_Apn::Parameter::SHUTTERSPEED,line,val);
                else
                    this->check_cmd(apn,RC_Apn::Parameter::SHUTTERSPEED,line,val);
            }

            if(i.wb!="-1")
            {
                ss_buff.clear();
                ss_buff <<i.wb;
                ss_buff >>val;
                if(check)
                    check=this->check_cmd(apn,RC_Apn::Parameter::WHITE_BALANCE,line,val);
                else
                    this->check_cmd(apn,RC_Apn::Parameter::WHITE_BALANCE,line,val);
            }

            if(i.effect!="-1")
            {
                ss_buff.clear();
                ss_buff <<i.effect;
                ss_buff >>val;
                if(check)
                    check=this->check_cmd(apn,RC_Apn::Parameter::PICTURE_STYLE,line,val);
                else
                    this->check_cmd(apn,RC_Apn::Parameter::PICTURE_STYLE,line,val);
            }


            if(i.iso!="-1")
            {
                ss_buff.clear();
                ss_buff <<i.iso;
                ss_buff >>val;
                if(check)
                    check=this->check_cmd(apn,RC_Apn::Parameter::ISO,line,val);
                else
                    this->check_cmd(apn,RC_Apn::Parameter::ISO,line,val);
            }

            if(i.delay<=0 && i.delay!=-1)
                std::cerr << "ligne " << line << " erreur pour wait"<<i.delay<<" non pris en charge (<=0)"<<std::endl;

            if(i.frame<=0 && i.frame!=-1)
                std::cerr << "ligne " << line << " erreur pour wait"<<i.delay<<" non pris en charge (<=0)"<<std::endl;

            if(i.intervalle<1 && i.intervalle!=-1)
                std::cerr << "ligne " << line << " erreur pour wait"<<i.delay<<" non pris en charge (< 1)"<<std::endl;

        }

        return check;
    }

    void help(void)
    {

    }

private:
    Sequance interpreter(std::stringstream  & ss_buffer)
    {
        Sequance seq;
        seq.wait=false;
        seq.delay=-1;
        seq.frame=-1;
        seq.intervalle=-1;
        seq.iso="-1";
        seq.aperture="-1";
        seq.exposure=-1;
        seq.shutter="-1";
        seq.effect="-1";
        seq.wb="-1";

        std::string cmd;

        while(ss_buffer)
        {
            ss_buffer >> cmd;

            if(cmd=="LOC") seq.user_local=true;
            else if(cmd=="GP2") seq.user_local=false;
            else if(cmd=="WAIT") seq.wait=true;
            else if(cmd=="DELAY") ss_buffer >> seq.delay;
            else if(cmd=="FRAME") ss_buffer >> seq.frame;
            else if(cmd=="INTER") ss_buffer >> seq.intervalle;
            else if(cmd=="ISO") ss_buffer >> seq.iso;
            else if(cmd=="EXPO") ss_buffer >> seq.exposure;
            else if(cmd=="APER") ss_buffer >> seq.aperture;
            else if(cmd=="TARGET") ss_buffer >> seq.target;
            else if(cmd=="FORMAT") ss_buffer >> seq.format;
            else if(cmd=="SHUTTER") ss_buffer >> seq.shutter;
            else if(cmd=="EFFECT") ss_buffer >> seq.effect;
            else if(cmd=="WB") ss_buffer >> seq.wb;
        }

        return seq;
    }

    bool check_cmd(RC_Apn & apn,RC_Apn::Parameter const & param,int line,std::string value)
    {
        auto v= apn.get_parameter(param);

        auto res= std::find(v.begin(),v.end(),value);

        if(res == v.end())
        {
            std::cerr << "ligne " << line << " erreur pour " << RC_Apn::parameter_to_string(param) <<" "<<value<<" non pris en charge"<<std::endl;

            return false;
        }
        else if(this->debug_mode)
            std::cout << "commande checker avec succes" <<std::endl;

        return true;
    }

    std::vector<Sequance> m_seq;
};

#endif // INTERVALLOMETRE_HPP_INCLUDED
