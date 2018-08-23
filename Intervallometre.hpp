#ifndef INTERVALLOMETRE_HPP_INCLUDED
#define INTERVALLOMETRE_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

class Intervallometre
{
public:
    bool debug_mode;

    struct Sequance
    {
        bool user_local:1;
        bool wait:1;

        int frame;
        int intervalle;
        int delay;
        int iso;
        int exposure;
        int aperture;
        int target;
        int format;
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

    void run_seq(void)
    {
        if(this->debug_mode)
            std::cout << " La sequance commance" <<std::endl;
        for(auto & seq:this->m_seq)
        {
            if(seq.user_local)
            {
                if(seq.wait)
                {
                    std::cin.clear();
                    std::cout << "Appuis sur Enter requis" << std::endl;
                    std::cin.get();
                }

                if(seq.delay>0)
                {
                    std::cout << "Attendre " << seq.delay <<" s"<<std::endl;
                    std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(seq.delay*1000));
                }

            }
            else
            {
                for(auto i=0;i<seq.frame;i++)
                {
                    std::cout << "frame "<<i+1<<"/"<<seq.frame<<std:: endl;
                }
            }

        }
    }
    size_t size(void)const noexcept
    {
        return this->m_seq.size();
    }


private:
    Sequance interpreter(std::stringstream  & ss_buffer)
    {
        Sequance seq;
        seq.wait=false;
        seq.delay=0;

        std::string cmd;

        while(ss_buffer)
        {
            ss_buffer >> cmd;

            if(cmd=="LOC")
                seq.user_local=true;
            else if(cmd=="GP2")
                seq.user_local=false;
            else if(cmd=="WAIT")
                seq.wait=true;
            else if(cmd=="DELAY")
                ss_buffer >> seq.delay;
            else if(cmd=="FRAME")
                ss_buffer >> seq.frame;
            else if(cmd=="INTER")
                ss_buffer >> seq.intervalle;
            else if(cmd=="ISO")
                ss_buffer >> seq.iso;
            else if(cmd=="EXPO")
                ss_buffer >> seq.exposure;
            else if(cmd=="APER")
                ss_buffer >> seq.aperture;
            else if(cmd=="TARGET")
                ss_buffer >> seq.target;
            else if(cmd=="FORMAT")
                ss_buffer >> seq.format;
        }

        return seq;
    }

    std::vector<Sequance> m_seq;
};

#endif // INTERVALLOMETRE_HPP_INCLUDED
