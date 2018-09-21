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
#include "gnu.hpp"
#include "utility.hpp"

class Intervallometre
{
public:
    ///defini si mode debug activé
    bool debug_mode;

    ///defini les parametre d'une sequance
    struct Sequance
    {

        bool user_local:1; //si la commande vien du programe ou concerne gphoto2
        bool wait:1;

        int frame;
        int exposure;

        float intervalle;
        float delay;

        std::string iso;
        std::string type_raw;
        std::string work_dir;
        std::string aperture;
        std::string target;
        std::string format;
        std::string shutter;
        std::string effect;
        std::string wb;
    };

///-------------------------------------------------------------
///initialisation aux valeurs par defaut
///-------------------------------------------------------------
    Intervallometre(void)
    {
        this->debug_mode=false;
    }

///-------------------------------------------------------------
///charge la sequance
///-------------------------------------------------------------
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

            return true;
        }
        //gestion des erreur
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

///-------------------------------------------------------------
///execute la sequance
///-------------------------------------------------------------
    void run_seq(RC_Apn & apn,std::string & last_capt)
    {
        if(this->debug_mode)
            std::cout << " La sequance commance" <<std::endl<<std::endl;

            std::string rep_directory("");

        for(auto & seq:this->m_seq)
        {
            if(seq.user_local)
            {
                if(seq.wait) //attente d'un appuis sur touche
                {
                    std::cin.clear();
                    std::cout <<std::endl << "Appuis sur Enter requis" << std::endl<< std::endl;
                    std::cin.get();
                }

                if(seq.delay>0) // attente d'un delais
                {
                    std::cout<<std::endl << "Attendre " << seq.delay <<" s"<<std::endl<<std::endl;
                    std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(seq.delay*1000));
                }

                if(seq.work_dir!="-1") // gere le repertoire de travaille
                {
                    bool d(false),o(false),f(false),l(false);

                    //on regarde si les dossier dof sont présent et on les cree sinon
                    for(auto & i : ls(seq.work_dir))
                    {
                        if(i=="dark")
                            d=true;
                        else if(i=="offset")
                            o=true;
                        else if(i=="flat")
                            f=true;
                        else if(i=="light")
                            l=true;
                    }

                    if(!d)
                        system(std::string("mkdir "+seq.work_dir+"/dark").c_str());
                    if(!o)
                        system(std::string("mkdir "+seq.work_dir+"/offset").c_str());
                    if(!f)
                        system(std::string("mkdir "+seq.work_dir+"/flat").c_str());
                    if(!l)
                        system(std::string("mkdir "+seq.work_dir+"/light").c_str());

                    if(this->debug_mode)
                        std::cout << "changement de repertoire de travail: "<<seq.work_dir<<std::endl;

                    rep_directory=seq.work_dir;
                }


            }
            else
            {
                for(auto i=0;i<seq.frame;i++)//execute n fois une instruction grace au parametre frame
                {
                    std::cout <<std::endl<< "frame "<<i+1<<"/"<<seq.frame<<std::endl;

                    if(apn.check_apn())//on verifie la presance apn a chaque capture
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

                        //capture
                        apn.capture_EOS_DSLR(inter,seq.iso,expo,seq.aperture,"1","9",seq.shutter!="-1"?seq.shutter:"bulb","0","4");//parametre par defaut a changé

                        auto v=apn.get_parameter(RC_Apn::Parameter::FILE);
                        apn.init_parameter();
                        auto temp=apn.get_parameter(RC_Apn::Parameter::FILE);

                        std::string new_capt("");
                        for(auto i : temp)
                        {
                             auto res= std::find(v.begin(),v.end(),i);

                             if(res == v.end())
                             {
                                 new_capt=i;
                                 break;
                             }
                        }

                        apn.download(new_capt,rep_directory+"/"+(seq.type_raw!="-1"?seq.type_raw:""));
                        last_capt=(rep_directory+"/"+(seq.type_raw!="-1"?(seq.type_raw+"/"):""))+new_capt;
                    }
                    else
                    {
                        std::cout << "capture annulée aucun apn detecté" << std::endl;
                    }
                }
            }
        }
    }

///-------------------------------------------------------------
///taille de sequance
///-------------------------------------------------------------
    size_t size(void)const noexcept
    {
        return this->m_seq.size();
    }

///-------------------------------------------------------------
///verifie si la sequance possede des erreur
///-------------------------------------------------------------
    bool check_sequance(RC_Apn & apn)
    {
        bool check(true);
        int line(0);
        for(auto & i: this->m_seq)
        {
            //on determine la ligne en cause
            line++;

            std::stringstream ss_buff;
            std::string val;

            //on verifie les parametre d'ouveture
            if(i.aperture!="-1")
            {
                ss_buff <<i.aperture;
                ss_buff >>val;
                if(check)
                    check=this->check_cmd(apn,RC_Apn::Parameter::APERTURE,line,val);
                else
                    this->check_cmd(apn,RC_Apn::Parameter::APERTURE,line,val);
            }

            //on verifie les parametre du shutterspeed
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
            //on verifie les balance de blanc
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
            //on verifie les effect appliquer
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
            //on verifie les iso
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

            //on verifie le delay, frame et intervalle
            if(i.delay<=0 && i.delay!=-1)
            {
                std::cerr << "ligne " << line << " erreur pour wait "<<i.delay<<" non pris en charge (<=0)"<<std::endl;
                check=false;
            }


            if(i.frame<=0 && i.frame!=-1)
            {
                std::cerr << "ligne " << line << " erreur pour wait "<<i.delay<<" non pris en charge (<=0)"<<std::endl;
                check=false;
            }


            if(i.intervalle<1 && i.intervalle!=-1)
            {
                std::cerr << "ligne " << line << " erreur pour wait "<<i.delay<<" non pris en charge (< 1)"<<std::endl;
                check=false;
            }


            if(i.type_raw != "dark" && i.type_raw != "flat" && i.type_raw != "offset" && i.type_raw != "light" && i.type_raw != "-1")
            {
                std::cerr << "ligne " << line << " erreur pour TYPE_RAW "<<i.type_raw<<" non pris en charge (!= dark || offset || flat || light)"<<std::endl;
                check=false;
            }


            if(i.work_dir != "-1")
            {
                std::fstream Of(i.work_dir+"test_dir",std::ios::out);
                if(!Of || Of.fail() || Of.bad())
                {
                    std::cerr << "ligne " << line << " erreur pour REP_DIRECTORY "<<i.work_dir<<" introuvable"<<std::endl;
                    check=false;
                }

                else
                    std::remove(std::string(i.work_dir+"test_dir").c_str());
            }

        }
        //true si pas d'erreur
        return check;
    }

private:

///-------------------------------------------------------------
///on interprete chaque instruction
///-------------------------------------------------------------
    Sequance interpreter(std::stringstream  & ss_buffer)
    {
        //on initialise npar defaut a false ou -1
        Sequance seq;
        seq.wait=false;
        seq.type_raw="-1";
        seq.work_dir="-1";
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

        //on interprete la sequance et les parametre associer
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
            else if(cmd=="WORK_DIRECTORY") ss_buffer >> seq.work_dir;
            else if(cmd=="TYPE_RAW") ss_buffer >> seq.type_raw;
            else
            {
                std::cout << "instruction: "<<cmd << "invalide donc ignoré" << std::endl;
            }
        }

        return seq;
    }

///-------------------------------------------------------------
///verifie si les instruction sont erroné
///-------------------------------------------------------------
    bool check_cmd(RC_Apn & apn,RC_Apn::Parameter const & param,int line,std::string value)
    {
        auto v= apn.get_parameter(param);

        auto res= std::find(v.begin(),v.end(),value);

        if(res == v.end())
        {
            //informe sur l'erreur
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
