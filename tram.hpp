#ifndef TRAM_HPP_INCLUDED
#define TRAM_HPP_INCLUDED

#include <vector>
#include <string>

typedef std::vector<char> VCHAR;

class Tram
{
    public:
        //ascii correspondance
        class Com_bytes
        {
        public:
            static char constexpr SOH=0x01; //Start of Heading
            static char constexpr EOT=0x04; //End of Transmission
            static char constexpr ACK=0x06; //Acknowledge
            static char constexpr NAK=0x15; //Negative Acknowledge
            static char constexpr US= 0x1F; //Unit Separator
        };
        Tram(void){}
        Tram(Tram const & cpy):m_data(cpy.get_c_data()){}
        Tram(std::string const & init):m_data(VCHAR(init.begin(),init.end())){}
        Tram(VCHAR const & init):m_data(init){}

        Tram operator=(Tram const & cpy)
        {
            this->m_data=cpy.get_c_data();
            return *this;
        }
        Tram operator=(std::string const & init)
        {
            this->m_data=VCHAR(init.begin(),init.end());
            return *this;
        }
        Tram operator=(VCHAR const & init)
        {
            this->m_data=init;
            return *this;
        }

        Tram operator+(Tram const & add)
        {
            Tram t(*this);

            for(auto & i : add.get_c_data())
                t.get_data().push_back(i);

            return t;
        }
        Tram operator+(std::string const & add)
        {
            Tram t(*this);

            for(auto & i : add)
                t.get_data().push_back(i);

            return t;
        }
        Tram operator+(VCHAR const & add)
        {
            Tram t(*this);

            for(auto & i : add)
                t.get_data().push_back(i);

            return t;
        }
        Tram operator+(char const & add)
        {
            Tram t(*this);

            t.get_data().push_back(add);

            return t;
        }

        Tram operator+=(Tram const & add)
        {
            for(auto & i : add.get_c_data())
                this->m_data.push_back(i);

            return *this;
        }
        Tram operator+=(std::string const & add)
        {
            for(auto & i : add)
                this->m_data.push_back(i);

            return *this;
        }
        Tram operator+=(VCHAR const & add)
        {
            this->m_data.insert(this->m_data.end(),add.begin(),add.end());

            return *this;
        }
        Tram operator+=(char const & add)
        {
            this->m_data.push_back(add);

            return *this;
        }

        void clear() noexcept
        {
            m_data.clear();
        }
        size_t size() const noexcept
        {
            return this->m_data.size();
        }

        VCHAR  & get_data(void)
        {
            return this->m_data;
        }

        VCHAR get_c_data(void) const
        {
            return this->m_data;
        }

    private:

        VCHAR m_data;

};

#endif // TRAM_HPP_INCLUDED
