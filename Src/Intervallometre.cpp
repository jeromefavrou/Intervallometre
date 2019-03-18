#include "Intervallometre.hpp"

///-------------------------------------------------------------
///initialisation aux valeurs par defaut
///-------------------------------------------------------------
Intervallometre::Intervallometre(void)
{
    this->debug_mode=false;
    this->nb_capture=0;
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

///-------------------------------------------------------------
///execute la sequance
///-------------------------------------------------------------

void Intervallometre::run_seq(RC_Apn & apn,std::string & last_capt)
{

    std::cout << "La sequance commance " <<std::endl;
    std::cout <<this->nb_capture <<" captures seront effectuées " <<std::endl;
    std::cout <<"environ " << this->nb_capture * (apn.no_delete?23:0)<<" Mo seront utilisés dans l'APN"<<std::endl;
    std::cout <<"environ " << this->nb_capture * (apn.no_download || apn.tcp_client?0:23)<<" Mo seront utilisés sur ce PC"<<std::endl;
    std::cout <<"environ " << this->nb_capture * (apn.no_download || !apn.tcp_client?0:23)<<" Mo seront utilisés sur le serveur"<<std::endl;
    std::cout <<std::endl << "Assurez vous d'avoir asser de memoire" << std::endl;
    std::cin.clear();
    std::cout <<std::endl << "Appuis sur Enter requis" << std::endl<< std::endl;
    std::cin.get();
    apn.Ls_file(this->ref_file_capture);
    this->file_capture=this->ref_file_capture;

    std::string rep_directory("");
    bool disconnect(false);

    for(auto & seq:this->m_seq)
    {
        if(seq.user_local)
        {
            if(seq.work_dir!="-1")
            {
                rep_directory=seq.work_dir;

                apn.mk_dir(seq.work_dir+"/"+"dark");
                apn.mk_dir(seq.work_dir+"/"+"offset");
                apn.mk_dir(seq.work_dir+"/"+"flat");
                apn.mk_dir(seq.work_dir+"/"+"light");
            }
            if(seq.wait) //attente d'un appuis sur touche
            {
                std::cin.clear();
                std::cout <<std::endl << "Appuis sur Enter requis" << std::endl<< std::endl;
                std::cin.get();
            }
            if(seq.stop_server) //attente d'un appuis sur touche
            {
                apn.close_server();
            }

            if(seq.delay>0) // attente d'un delais
            {
                std::cout<<std::endl << "Attendre " << seq.delay <<" s"<<std::endl<<std::endl;
                std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(seq.delay*1000));
            }
        }
        else
        {
            for(auto i=0;i<seq.frame;i++)//execute n fois une instruction grace au parametre frame
            {
                std::cout <<std::endl<< "frame "<<i+1<<"/"<<seq.frame<<" "+(seq.type_raw!="-1"?seq.type_raw:"")<<std::endl;

                try
                {
                    apn.check_apn();//on verifie la presance apn a chaque capture

                    std::cout <<"ne pas débrancher apn, capture en cour; iso: " << seq.iso << " exposition: "<<seq.exposure<<" ouverture: " <<seq.aperture <<std::endl;

                    std::string expo=ss_cast<int,std::string>(seq.exposure);
                    std::string inter=ss_cast<float,std::string>(seq.intervalle);

                    //capture
                    apn.capture_EOS_DSLR(i==0 || disconnect ,inter,seq.iso,expo,seq.aperture,"1","9",seq.shutter!="-1"?seq.shutter:"0","0","4");//parametre par defaut a changé
                    disconnect=false;
                }
                catch(Error & e)
                {
                    std::cerr << e.what() << std::endl;

                    if(e.get_niveau()==Error::niveau::FATAL_ERROR)
                        return ;

                    std::clog << "resoudre l'eurreur et appuyer sur enter, ou quitter le programme" << std::endl;
                    std::cin.get();
                    std::cin.clear();

                    disconnect=true;

                    i--;
                }
            }

            std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000));
            apn.Ls_file(this->file_capture);
            gp2::Folder_data tmps_delta=this->delta_folder(this->ref_file_capture,this->file_capture);
            apn.download(tmps_delta,rep_directory+"/"+seq.type_raw);
            apn.delete_file(tmps_delta);
        }
    }
}
