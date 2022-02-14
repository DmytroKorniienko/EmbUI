// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "EmbUI.h"

#ifdef EMBUI_USE_MQTT

void EmbUI::connectToMqtt() {
    LOG(println, PSTR("UI: Connecting to MQTT..."));

    m_pref=param(FPSTR(P_m_pref));
    m_host=param(FPSTR(P_m_host));
    m_port=param(FPSTR(P_m_port));
    m_user=param(FPSTR(P_m_user));
    m_pass=param(FPSTR(P_m_pass));
    m_will=id(F("embui/pub/online"));
    
    IPAddress ip; 
    bool isIP = ip.fromString(m_host);
    if(isIP)
        mqttClient.setServer(ip, m_port.toInt());
    else
        mqttClient.setServer(m_host.c_str(), m_port.toInt());

    mqttClient.disconnect();
    mqttClient.setKeepAlive(30).setWill(m_will.c_str(), 0, true, "0").setCredentials(m_user.c_str(), m_pass.c_str()).setClientId(m_pref.isEmpty() ? mc : m_pref.c_str());
    mqttClient.connect();
}

String EmbUI::id(const String &topic){
    String ret = m_pref;
    if (ret.isEmpty()) return topic;

    ret += '/'; ret += topic;
    return ret;
}

void EmbUI::onMqttSubscribe(uint16_t packetId, uint8_t qos) {

}

void EmbUI::onMqttUnsubscribe(uint16_t packetId) {

}

void EmbUI::onMqttPublish(uint16_t packetId) {

}

typedef void (*mqttCallback) (const String &topic, const String &payload);
mqttCallback mqt;

void mqtt_dummy_connect(){ EmbUI::GetInstance()->onMqttConnect(); }
void mqtt_emptyFunction(const String &, const String &){}

void EmbUI::mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), bool remotecontrol){
    if (host.length()==0){
        LOG(println, PSTR("UI: MQTT host is empty - disabled!"));
        return;   // выходим если host не задан
    }

    if(m_pref == FPSTR(P_null)) var(FPSTR(P_m_pref), pref);
    if(m_host == FPSTR(P_null)) var(FPSTR(P_m_host), host);
    if(m_port == FPSTR(P_null)) var(FPSTR(P_m_port), String(port));
    if(m_user == FPSTR(P_null)) var(FPSTR(P_m_user), user);
    if(m_pass == FPSTR(P_null)) var(FPSTR(P_m_pass), pass);

    LOG(println, PSTR("UI: MQTT Init completed"));

    if (remotecontrol) sysData.mqtt_remotecontrol = true;
    mqt = mqttFunction;

    mqttClient.onConnect(_onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    
    sysData.mqtt_enable = true;
    mqtt_reconnect();
}

void EmbUI::mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload)){
    mqtt(pref, host, port, user, pass, mqttFunction, false);
    onConnect = mqtt_dummy_connect;
}

void EmbUI::mqtt(const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload)){
    getAPmac();
    mqtt(mc, host, port, user, pass, mqttFunction, false);
    onConnect = mqtt_dummy_connect;
}

void EmbUI::mqtt(const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), bool remotecontrol){
    getAPmac();
    mqtt(mc, host, port, user, pass, mqttFunction, remotecontrol);
    onConnect = mqtt_dummy_connect;
}

void EmbUI::mqtt(const String &host, int port, const String &user, const String &pass, bool remotecontrol){
    getAPmac();
    mqtt(mc, host, port, user, pass, mqtt_emptyFunction, remotecontrol);
    onConnect = mqtt_dummy_connect;
}

void EmbUI::mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, bool remotecontrol){
    getAPmac();
    mqtt(pref, host, port, user, pass, mqtt_emptyFunction, remotecontrol);
    onConnect = mqtt_dummy_connect;
}

void EmbUI::mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) (), bool remotecontrol){
    getAPmac();
    mqtt(pref, host, port, user, pass, mqtt_emptyFunction, remotecontrol);
    onConnect = mqttConnect;

}
void EmbUI::mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) ()){
    getAPmac();
    mqtt(pref, host, port, user, pass, mqttFunction, false);
    onConnect = mqttConnect;
}
void EmbUI::mqtt(const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) ()){
    getAPmac();
    mqtt(mc, host, port, user, pass, mqttFunction, false);
    onConnect = mqttConnect;
}
void EmbUI::mqtt(const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) (), bool remotecontrol){
    getAPmac();
    mqtt(mc, host, port, user, pass, mqttFunction, remotecontrol);
    onConnect = mqttConnect;
}

void EmbUI::mqtt(void (*mqttFunction) (const String &topic, const String &payload), bool remotecontrol){
    mqt = mqttFunction;
    sysData.mqtt_remotecontrol = remotecontrol;
}

void EmbUI::mqtt(void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) (), bool remotecontrol){
    onConnect = mqttConnect;
    mqt = mqttFunction;
    sysData.mqtt_remotecontrol = remotecontrol;
}

void EmbUI::mqtt_handle(){
    if (!sysData.wifi_sta || cfg[FPSTR(P_m_host)].as<String>().isEmpty()) return;
    if (sysData.mqtt_connect) onMqttConnect();
    mqtt_reconnect();
}

void EmbUI::mqttReconnect(){ // принудительный реконнект, при смене чего-либо в UI
    sysData.mqtt_connected = false;
}

/*
 * TODO: убрать этот бардак в планировщик
 */
void EmbUI::mqtt_reconnect(){
    static unsigned long tmout = 0;
    if (tmout + 15000 > millis()) return;
    tmout = millis();
    if ( sysData.wifi_sta && !sysData.mqtt_connected) connectToMqtt();
}

void EmbUI::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  LOG(println,F("UI: Disconnected from MQTT."));
  EmbUI::GetInstance()->sysData.mqtt_connect = false;
  EmbUI::GetInstance()->sysData.mqtt_connected = false;
}

void EmbUI::_onMqttConnect(bool sessionPresent) {
    EmbUI::GetInstance()->sysData.mqtt_connect = true;
    if(EmbUI::GetInstance()->onConnect)
        EmbUI::GetInstance()->onConnect();
}

void EmbUI::onMqttConnect(){
    sysData.mqtt_connect = false;
    sysData.mqtt_connected = true;
    LOG(println,F("UI: Connected to MQTT."));
    if(sysData.mqtt_remotecontrol){
        subscribeAll();
        String strue = FPSTR(P_true);
        mqttClient.publish(m_will.c_str(), 0, true, strue.c_str());
    }
}

void EmbUI::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    LOG(print, F("UI: Publish received: "));
    LOG(println, topic);

    char buffer[len + 2];
    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, payload, len);

    String tpc = String(topic);
    String m_pref = EmbUI::GetInstance()->param(FPSTR(P_m_pref)); 
    if (!m_pref.isEmpty()) tpc = tpc.substring(m_pref.length() + 1, tpc.length());

    if (tpc.equals(F("embui/get/config"))) {
        EmbUI::GetInstance()->publish(F("embui/pub/config"), EmbUI::GetInstance()->deb(), false);    
    } else if (tpc.startsWith(F("embui/get/"))) {
        String param = tpc.substring(10); // sizeof embui/set/
        if(EmbUI::GetInstance()->isparamexists(param))
            EmbUI::GetInstance()->publish(String(F("embui/pub/")) + param, EmbUI::GetInstance()->param(param), false);
        else {
            httpCallback(param, String(buffer), false); // нельзя напрямую передавать payload, это не ASCIIZ
            mqt(tpc, String(buffer)); // отправим во внешний обработчик
        }
    } else if (EmbUI::GetInstance()->sysData.mqtt_remotecontrol && tpc.startsWith(F("embui/set/"))) {
       String cmd = tpc.substring(10); // sizeof embui/set/
       httpCallback(cmd, String(buffer), true); // нельзя напрямую передавать payload, это не ASCIIZ
    } else if (EmbUI::GetInstance()->sysData.mqtt_remotecontrol && tpc.startsWith(F("embui/jsset/"))) {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload, len);
        JsonObject obj = doc.as<JsonObject>();
        EmbUI::GetInstance()->post(obj);
    } else {
        mqt(tpc, String(buffer));
    }
}

void EmbUI::subscribeAll(bool isOnlyGetSet){
    if(isOnlyGetSet){
        mqttClient.subscribe(id(F("embui/set/#")).c_str(), 0);
        mqttClient.subscribe(id(F("embui/get/#")).c_str(), 0);
        LOG(println, F("UI: Subscribe embui/get/# & embui/set/#"));
    } else {
        mqttClient.subscribe(id(F("embui/#")).c_str(), 0);
        LOG(println, F("UI: Subscribe All (embui/#)"));
    }
}

void EmbUI::publish(const String &topic, const String &payload, bool retained){
    if (!sysData.wifi_sta || !sysData.mqtt_enable) return;
    mqttClient.publish(id(topic).c_str(), 0, retained, payload.c_str());
}

void EmbUI::publish(const String &topic, const String &payload){
    if (!sysData.wifi_sta || !sysData.mqtt_enable) return;
    mqttClient.publish(id(topic).c_str(), 0, false, payload.c_str());
}

void EmbUI::publishto(const String &topic, const String &payload, bool retained){
    if (!sysData.wifi_sta || !sysData.mqtt_enable) return;
    mqttClient.publish(topic.c_str(), 0, retained, payload.c_str());
}

void EmbUI::pub_mqtt(const String &key, const String &value){
    if(!sysData.mqtt_remotecontrol) return;
    publish(key, value, true);
}

void EmbUI::subscribe(const String &topic){
    mqttClient.subscribe(id(topic).c_str(), 0);
}

#endif