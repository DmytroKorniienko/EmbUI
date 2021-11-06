// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#ifndef ui_h
#define ui_h

#include "EmbUI.h"

#ifdef ESP8266
#ifndef IFACE_DYN_JSON_SIZE
  #define IFACE_DYN_JSON_SIZE 2048
#endif
#ifndef SMALL_JSON_SIZE
  #define SMALL_JSON_SIZE  768
#endif
#elif defined ESP32
#ifndef IFACE_DYN_JSON_SIZE
  #define IFACE_DYN_JSON_SIZE 8192
#endif
#ifndef SMALL_JSON_SIZE
  #define SMALL_JSON_SIZE  1024
#endif
#endif

// static json doc size
#define IFACE_STA_JSON_SIZE SMALL_JSON_SIZE
#define FRAME_ADD_RETRY 5

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
    EmbUI *_embui;

    /**
     * @brief - add object to frame with mem overflow protection 
     */
    bool frame_add_safe(const JsonObjectConst &jobj);

    public:
        Interface(EmbUI *j, AsyncWebSocket *server, size_t size = IFACE_DYN_JSON_SIZE): json(size), section_stack(){
            _embui = j;
            send_hndl = new frameSendAll(server);
        }
        Interface(EmbUI *j, AsyncWebSocketClient *client, size_t size = IFACE_DYN_JSON_SIZE): json(size), section_stack(){
            _embui = j;
            send_hndl = new frameSendClient(client);
        }
        Interface(EmbUI *j, AsyncWebServerRequest *request, size_t size = IFACE_DYN_JSON_SIZE): json(size), section_stack(){
            _embui = j;
            send_hndl = new frameSendHttp(request);
        }
        ~Interface(){
            delete send_hndl;
            send_hndl = nullptr;
            _embui = nullptr;
        }

        void json_frame_value();
        void json_frame_interface(const String &name = "");
        bool json_frame_add(JsonObjectConst obj);
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

        void raw_html(const String &id, const String &value);
        void iframe(const String &id, const String &value);

        /**
         * @brief - Add 'value' object to the Interface frame
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        void value(const String &id, const String &val, bool html = false);
        void value(const String &id, bool html = false);
        inline void value(JsonObjectConst data);

        void hidden(const String &id);
        void hidden(const String &id, const String &value);

        // Can use like non clickable button with loading spinner with label or not
        void constant(const String &id, const String &value, const String &label, bool loading = false, const String &color = "", uint8_t top_margine = 10);
        void constant(const String &id, const String &label, bool loading = false, const String &color = "", uint8_t top_margine = 10);

        void text(const String &id, const String &value, const String &label, bool directly = false);
        void text(const String &id, const String &label, bool directly = false);
        
        void password(const String &id, const String &value, const String &label);
        void password(const String &id, const String &label);

        /**
         * @brief - create "number" html field with optional step, min, max constraints
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        void number(const String &id, const String &label);
        void number(const String &id, const String &value, const String &label);
        void number(const String &id, const String &label, const String &step, const String &min, const String &max);
        void number(const String &id, const String &value, const String &label, const String &step, const String &min, const String &max);

        void time(const String &id, const String &value, const String &label);
        void time(const String &id, const String &label);
        
        void date(const String &id, const String &value, const String &label);
        void date(const String &id, const String &label);
        
        // Step - show seconds 
        void datetime(const String &id, const String &value, const String &label, bool step = false);
        void datetime(const String &id, const String &label, bool step = false);
        
        void email(const String &id, const String &value, const String &label);
        void email(const String &id, const String &label);

        /**
         * @brief - create "range" html field with step, min, max constraints
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        void range(const String &id, const String &min, const String &max, const String &step, const String &label, bool directly = false);
        void range(const String &id, const String &value, const String &min, const String &max, const String &step, const String &label, bool directly = false);

        void select(const String &id, const String &label, bool directly = false, bool skiplabel = false, bool editable = false);
        void select(const String &id, const String &value, const String &label, bool directly = false, bool skiplabel = false, const String &exturl = (char*)0, bool editable = false);

        void select_edit(const String &id, const String &label, bool directly = false, bool skiplabel = false, bool editable = true);
        void select_edit(const String &id, const String &value, const String &label, bool directly = false, bool skiplabel = false, const String &exturl = (char*)0, bool editable = true);
        

        void option(const String &value, const String &label);

        /**
         * элемент интерфейса checkbox
         * @param directly - значение чекбокса при изменении сразу передается на сервер без отправки формы
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        void checkbox(const String &id, const String &value, const String &label, bool directly = false);
        void checkbox(const String &id, const String &label, bool directly = false);
        
        void color(const String &id, const String &value, const String &label);
        void color(const String &id, const String &label);

        /**
         * элемент интерфейса textarea
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        void textarea(const String &id, const String &value, const String &label);
        void textarea(const String &id, const String &label);

        void file(const String &name, const String &action, const String &label);
        void button(const String &id, const String &label, const String &color = "", uint8_t top_margine=10, const String &message = "");
        void button_confirm(const String &id, const String &label, const String &message = "", const String &color = "", uint8_t top_margine=10);
        void button_submit(const String &section, const String &label, const String &color = "", uint8_t top_margine=10, const String &message = "");
        void button_submit_confirm(const String &section, const String &label, const String &message = "", const String &color = "", uint8_t top_margine=10);
        void button_submit_value(const String &section, const String &value, const String &label, const String &color = "", uint8_t top_margine=10, const String &message = "");
        void button_submit_value_confirm(const String &section, const String &value, const String &label, const String &message = "", const String &color = "", uint8_t top_margine=10);
        void spacer(const String &label = "");
        void comment(const String &label = "");

        /**
         * @brief - create generic html input element
         * @param id - element id
         * @param type - html element type, ex. 'text'
         * @param value - element value
         * @param label - element label
         * @param direct - if true, element value in send via ws on-change 
         * @param step - show seconds for datetime control 
         */
        void html_input(const String &id, const String &type, const String &value, const String &label, bool direct = false, bool step = false);

        /**
         * @brief - create "display" div with custom css selector
         * could be used for making all kinds of "sensor" outputs on the page with live-updated values without the need to redraw interface element
         * @param id - element/div DOM id
         * @param value  - element value (treated as text)
         * @param class - base css class for Display, full css seletor created as "class id" to allow many sensors inherit base class
         * @param params - additional parameters (reserved for future use)
         */
        void display(const String &id, const String &value, const String &css = String(), const JsonObject &params = JsonObject());

        /**
         * @brief - Creates html element with image type
         * could be used for display image and animation from intertal or external storage
         * @param id elevent/div DOM id
         * @param value - element value (source destination, example - "/image/myimage.svg" or "https://example.com/image/myimage.jpg")
         */
        void image(const String &id, const String &value);

        /**
         * @brief - Creates html element with cutomized type and arbitrary parameters
         * used to create user-defined interface elements with custom css/js handlers
         */
        void custom(const String &id, const String &type, const String &value, const String &label, const JsonObject &param = JsonObject());
};

#endif