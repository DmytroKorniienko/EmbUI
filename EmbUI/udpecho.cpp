// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "EmbUI.h"
#include <WiFiUdp.h>

WiFiUDP Udp;
bool udpApply = false;

void EmbUI::udp(){
    getAPmac();
    strncpy(udpMessage,mc,sizeof(udpMessage)-1); // udpMessage = mc;
}

void EmbUI::udp(const String &message){
    strncpy(udpMessage,message.c_str(),sizeof(udpMessage)-1); //udpMessage = message;
}

void EmbUI::udpBegin(){
    Udp.begin(localUdpPort);
    if(*udpMessage) udpApply = true;
}

void EmbUI::udpLoop(){
    static bool st = false;
    if(!st){
        st = true;
        udpBegin();
    }

    if(!udpApply) return;

    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        strncpy(udpRemoteIP,Udp.remoteIP().toString().c_str(),sizeof(udpRemoteIP)-1); // udpRemoteIP = Udp.remoteIP().toString();
        LOG(printf, PSTR("Received %d bytes from %s, port %d\n"), packetSize, udpRemoteIP, Udp.remotePort());
        int len = Udp.read(incomingPacket, 64);
        if (len > 0)
        {
        incomingPacket[len] = 0;
        }
        LOG(printf, PSTR("UDP packet contents: %s\n"), incomingPacket);

        // send back a reply, to the IP address and port we got the packet from
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        //Udp.write(ip.c_str());
        LOG(println, String(F("Send UDP: ")) + udpMessage);
        Udp.print(udpMessage);
        Udp.endPacket();
    }
}