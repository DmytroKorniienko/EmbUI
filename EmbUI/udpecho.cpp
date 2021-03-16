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
    udpMessage = mc;
}

void EmbUI::udp(const String &message){
    udpMessage = message;
}

void EmbUI::udpBegin(){
    Udp.begin(localUdpPort);
    if(!udpMessage.isEmpty()) udpApply = true;
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
        char *data = new char[packetSize+1];
        
        LOG(printf, PSTR("Received %d bytes from %s, port %d\n"), packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        uint32_t len = Udp.read(data, packetSize);
        if (len > 0)
        {
            data[len] = 0;
        }
        LOG(printf, PSTR("UDP packet contents: %s\n"), data);

        // send back a reply, to the IP address and port we got the packet from
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        //Udp.write(ip.c_str());
        LOG(println, String(F("Send UDP: ")) + udpMessage);
        Udp.print(udpMessage);
        Udp.endPacket();

        delete[] data;
    }
}