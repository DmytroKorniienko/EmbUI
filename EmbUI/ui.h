// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#ifndef ui_h
#define ui_h

#include "EmbUI.h"

#ifdef ESP8266
  #define IFACE_DYN_JSON_SIZE 3072
  #define POST_DYN_JSON_SIZE  1024
#elif defined ESP32
  #define IFACE_DYN_JSON_SIZE 8192
  #define POST_DYN_JSON_SIZE  2048
#endif

// static json doc size
#define IFACE_STA_JSON_SIZE 256

class frameSend {
    public:
        virtual ~frameSend(){};
        virtual void send(const String &data){};
        virtual void send(const JsonObject& data){};
        virtual void flush(){}
};

class frameSendAll: public frameSend {
    private:
        AsyncWebSocket *ws;
    public:
        frameSendAll(AsyncWebSocket *server){ ws = server; }
        ~frameSendAll() { ws = nullptr; }
        void send(const String &data){ if (data.length()) ws->textAll(data); };
        void send(const JsonObject& data);
};

class frameSendClient: public frameSend {
    private:
        AsyncWebSocketClient *cl;
    public:
        frameSendClient(AsyncWebSocketClient *client){ cl = client; }
        ~frameSendClient() { cl = nullptr; }
        void send(const String &data){ if (data.length()) cl->text(data); };
        /**
         * @brief - serialize and send json obj directly to the ws buffer
         */
        void send(const JsonObject& data);
};

class frameSendHttp: public frameSend {
    private:
        AsyncWebServerRequest *req;
        AsyncResponseStream *stream;
    public:
        frameSendHttp(AsyncWebServerRequest *request){
            req = request;
            stream = req->beginResponseStream(FPSTR(PGmimejson));
            stream->addHeader(FPSTR(PGhdrcachec), FPSTR(PGnocache));
        }
        ~frameSendHttp() { /* delete stream; */ req = nullptr; }
        void send(const String &data){
            if (!data.length()) return;
            stream->print(data);
        };
        /**
         * @brief - serialize and send json obj directly to the ws buffer
         */
        void send(const JsonObject& data){
            serializeJson(data, *stream);
        };
        void flush(){
            req->send(stream);
        };
};

class Interface {
    typedef struct section_stack_t{
      JsonArray block;
      String name;
      int idx;
    } section_stack_t;

    DynamicJsonDocument json;
    LList<section_stack_t*> section_stack;
    frameSend *send_hndl;
    EmbUI *embui;

    public:
        Interface(EmbUI *j, AsyncWebSocket *server, size_t size = IFACE_DYN_JSON_SIZE): json(size), section_stack(){
            embui = j;
            send_hndl = new frameSendAll(server);
        }
        Interface(EmbUI *j, AsyncWebSocketClient *client, size_t size = IFACE_DYN_JSON_SIZE): json(size), section_stack(){
            embui = j;
            send_hndl = new frameSendClient(client);
        }
        Interface(EmbUI *j, AsyncWebServerRequest *request, size_t size = IFACE_DYN_JSON_SIZE): json(size), section_stack(){
            embui = j;
            send_hndl = new frameSendHttp(request);
        }
        ~Interface(){
            delete send_hndl;
            send_hndl = nullptr;
            embui = nullptr;
        }

        void json_frame_value();
        void json_frame_interface(const String &name = "");
        bool json_frame_add(JsonObject obj);
        void json_frame_next();
        void json_frame_clear();
        void json_frame_flush();
        /**
         * @brief - serialize and send Interface object to the WebSocket 
         */
        void json_frame_send(){ if (send_hndl) send_hndl->send(json.as<JsonObject>()); };

        /**
         * @brief - begin custom UI secton
         * открывает секцию с указаным типом 'pkg', может быть обработан на клиенсткой стороне отлично от
         * интерфейсных пакетов 
         */
        void json_frame_custom(const String &type);

        void json_section_menu();
        void json_section_content();
        void json_section_line(const String &name = "");
        void json_section_main(const String &name, const String &label);
        void json_section_hidden(const String &name, const String &label);
        void json_section_begin(const String &name, const String &label = "", bool main = false, bool hidden = false, bool line = false);
        void json_section_begin(const String &name, const String &label, bool main, bool hidden, bool line, JsonObject obj);
        void json_section_end();

        void custom(const String &id, const String &type, const String &value, const String &label, const JsonObject &param = JsonObject());
        void frame(const String &id, const String &value);
        void frame2(const String &id, const String &value);

        /**
         * @brief - Add 'value' object to the Interface frame
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        template <typename T> void value(const String &id, const T& val, bool html = false){
            StaticJsonDocument<IFACE_STA_JSON_SIZE> obj;
            obj[FPSTR(P_id)] = id;
            obj[FPSTR(P_value)] = val;
            if (html) obj[FPSTR(P_html)] = true;

            if (!json_frame_add(obj.as<JsonObject>())) {
                value(id, val, html);
            }
        };

        void value(const String &id, bool html = false);

        inline void value(JsonObject &data){
            if (!json_frame_add(data))
                value(data);
        }

        void hidden(const String &id);
        void hidden(const String &id, const String &value);
        void constant(const String &id, const String &label);
        void constant(const String &id, const String &value, const String &label);
        void text(const String &id, const String &label, bool directly = false);
        void text(const String &id, const String &value, const String &label, bool directly);   // 4-й параметр обязателен, т.к. компилятор умудряется привести F() к булевому виду и использует неверный оверлоад (под esp32)
        void password(const String &id, const String &label);
        void password(const String &id, const String &value, const String &label);
        void number(const String &id, const String &label, int min = 0, int max = 0);
        void number(const String &id, int value, const String &label, int min = 0, int max = 0);
        void number(const String &id, const String &label, float step, float min = 0, float max = 0);
        void number(const String &id, float value, const String &label, float step, float min = 0, float max = 0);
        void time(const String &id, const String &label);
        void time(const String &id, const String &value, const String &label);
        void date(const String &id, const String &label);
        void date(const String &id, const String &value, const String &label);
        void datetime(const String &id, const String &label);
        void datetime(const String &id, const String &value, const String &label);
        void email(const String &id, const String &label);
        void email(const String &id, const String &value, const String &label);
        void range(const String &id, float min, float max, float step, const String &label, bool directly = false);
        void range(const String &id, float value, float min, float max, float step, const String &label, bool directly = false);
        void select(const String &id, const String &label, bool directly = false, bool skiplabel = false);
        void select(const String &id, const String &value, const String &label, bool directly = false, bool skiplabel = false);
        void option(const String &value, const String &label);

        /**
         * элемент интерфейса checkbox
         * @param directly - значение чекбокса при изменении сразу передается на сервер без отправки формы
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        template <typename T> void checkbox(const String &id, const T& value, const String &label, bool directly = false){
            StaticJsonDocument<IFACE_STA_JSON_SIZE> obj;
            obj[FPSTR(P_html)] = FPSTR(P_input);
            obj[FPSTR(P_type)] = F("checkbox");
            obj[FPSTR(P_id)] = id;
            obj[FPSTR(P_value)] = value;
            obj[FPSTR(P_label)] = label;
            if (directly) obj[FPSTR(P_directly)] = true;

            if (!json_frame_add(obj.as<JsonObject>())) {
                checkbox(id, value, label, directly);
            }
        };
        void checkbox(const String &id, const String &label, bool directly = false);
        void color(const String &id, const String &label);
        void color(const String &id, const String &value, const String &label);
        void textarea(const String &id, const String &label);
        void textarea(const String &id, const String &value, const String &label);
        void file(const String &name, const String &action, const String &label);
        void button(const String &id, const String &label, const String &color = "");
        void button_submit(const String &section, const String &label, const String &color = "");
        void button_submit_value(const String &section, const String &value, const String &label, const String &color = "");
        void spacer(const String &label = "");
        void comment(const String &label = "");
};

#endif