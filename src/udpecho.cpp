// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "EmbUI.h"

#ifdef TEMP_OFF
#include <ESPAsyncUDP.h>

AsyncUDP Udp;
bool udpApply = false;

void EmbUI::udp(){
    getAPmac();
    udpMessage = mc;
}

void EmbUI::udp(const String &message){
    udpMessage = message;
}

void EmbUI::udpBegin(){
    if(Udp.listen(5568)){
        Udp.onPacket([](AsyncUDPPacket packet){
            LOG(printf, PSTR("Received %d bytes from %s, port %d\n"), packet.length(), packet.remoteIP().toString().c_str(), packet.remotePort());
            LOG(print, PSTR("UDP packet contents: "));
            LOG(write, packet.data(), packet.length());
            LOG(println, String(F("\nSend UDP: ")) + EmbUI::GetInstance()->udpMessage);
            packet.print(EmbUI::GetInstance()->udpMessage);
            });
    }
}

void EmbUI::udpLoop(){
    static bool st = false;

    if(!st){
        st = true;
        udpBegin();
    }
    if(!udpApply) return;
}
#else
void EmbUI::udp(){}

void EmbUI::udp(const String &message){}

void EmbUI::udpBegin(){}

void EmbUI::udpLoop(){}
#endif
