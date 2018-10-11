#ifndef TRAM_HPP_INCLUDED
#define TRAM_HPP_INCLUDED

class Tram
{
    public:
        //ascii correspondance
        static char const SOH=0x01; //Start of Heading
        static char const EOT=0x04; //End of Transmission
        static char const ACK=0x06; //Acknowledge
        static char const NAK=0x15; //Negative Acknowledge
        static char const US= 0x1F; //Unit Separator
};

#endif // TRAM_HPP_INCLUDED
