#ifndef TRAM_HPP_INCLUDED
#define TRAM_HPP_INCLUDED

#include <vector>
#include <string>

#ifndef CLIENT_TCP_HPP_INCLUDED
typedef std::vector<char> VCHAR;

inline VCHAR StringToVChar(std::string const& str)
{
    VCHAR buffer;
    for(auto i:str)
        buffer.push_back(i);
    return buffer;
}
inline std::string VCharToString(VCHAR const & buffer)
{
    std::string str="";
    for(auto i:buffer)
        str+=i;
    return str;
}
#endif // CLIENT_TCP_HPP_INCLUDED

class Tram
{
    public:
        //ascii correspondance
        static char const SOH=0x01; //Start of Heading
        static char const EOT=0x04; //End of Transmission
        static char const ACK=0x06; //Acknowledge
        static char const NAK=0x15; //Negative Acknowledge
        static char const US= 0x1F; //Unit Separator

        Tram(void){}
        Tram(Tram const & cpy):m_data(cpy.get_c_data()){}
        Tram(std::string const & init):m_data(StringToVChar(init)){}
        Tram(VCHAR const & init):m_data(init){}

        Tram operator=(Tram const & cpy)
        {
            this->m_data=cpy.get_c_data();
            return *this;
        }
        Tram operator=(std::string const & init)
        {
            this->m_data=StringToVChar(init);
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
            for(auto & i : add)
                this->m_data.push_back(i);

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
