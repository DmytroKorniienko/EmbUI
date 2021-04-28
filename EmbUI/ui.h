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
    EmbUI *embui;

    /**
     * @brief - add object to frame with mem overflow protection 
     */
    void frame_add_safe(const JsonObjectConst &jobj);


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

            frame_add_safe(obj.as<JsonObject>());
        };

        void value(const String &id, bool html = false);

        /**
         * @brief - Add the whole JsonObject to the Interface frame
         * actualy it is a copy-object method used to echo back the data to the WebSocket in one-to-many scenarios
         */
        inline void value(JsonObjectConst data){
            frame_add_safe(data);
        }

        void hidden(const String &id);
        void hidden(const String &id, const String &value);

        void constant(const String &id, const String &value, const String &label);
        void constant(const String &id, const String &label);

        // 4-й параметр обязателен, т.к. компилятор умудряется привести F() к булевому виду и использует неверный оверлоад (под esp32)
        template <typename T>
        void text(const String &id, const T &value, const String &label, bool directly){ html_input(id, F("text"), value, label, directly); };
        void text(const String &id, const String &label, bool directly = false);

        template <typename T>
        void password(const String &id, const T &value, const String &label){ html_input(id, FPSTR(P_password), value, label); };
        void password(const String &id, const String &label);

        /**
         * @brief - create "number" html field with optional step, min, max constraints
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        void number(const String &id, const String &label){
            number(id, embui->param(id), label, 0);
        };

        template <typename T>
        void number(const String &id, const String &label, T step = 0, T min = 0, T max = 0){
            number(id, embui->param(id), label, step, min, max);
        };

        template <typename V>
        void number(const String &id, V value, const String &label){
            number(id, value, label, 0);
        };

        template <typename V, typename T>
        void number(const String &id, V value, const String &label, T step, T min = 0, T max = 0){
            StaticJsonDocument<IFACE_STA_JSON_SIZE> obj;
            obj[FPSTR(P_html)] = FPSTR(P_input);
            obj[FPSTR(P_type)] = FPSTR(P_number);
            obj[FPSTR(P_id)] = id;
            obj[FPSTR(P_value)] = value;
            obj[FPSTR(P_label)] = label;
            if (min) obj[FPSTR(P_min)] = min;
            if (max) obj[FPSTR(P_max)] = max;
            if (step) obj[FPSTR(P_step)] = step;

            if (!json_frame_add(obj.as<JsonObject>())) {
                number(id, value, label, step, min, max);
            }
        };

        template <typename T>
        void time(const String &id, const T &value, const String &label){ html_input(id, FPSTR(P_time), value, label); };
        void time(const String &id, const String &label);

        template <typename T>
        void date(const String &id, const T &value, const String &label){ html_input(id, FPSTR(P_date), value, label); };
        void date(const String &id, const String &label);

        template <typename T>
        void datetime(const String &id, const T &value, const String &label){ html_input(id, F("datetime-local"), value, label); };
        void datetime(const String &id, const String &label);

        template <typename T>
        void email(const String &id, const T &value, const String &label){ html_input(id, F("email"), value, label); };
        void email(const String &id, const String &label);

        /**
         * @brief - create "range" html field with step, min, max constraints
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        template <typename T>
        void range(const String &id, T min, T max, T step, const String &label, bool directly = false){
            range(id, embui->param(id), min, max, step, label, directly);
        };

        /**
         * @brief - create "range" html field with step, min, max constraints
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        template <typename V, typename T>
        void range(const String &id, V value, T min, T max, T step, const String &label, bool directly){
            StaticJsonDocument<IFACE_STA_JSON_SIZE> obj;
            obj[FPSTR(P_html)] = FPSTR(P_input);
            obj[FPSTR(P_type)] = F("range");
            obj[FPSTR(P_id)] = id;
            obj[FPSTR(P_value)] = value;
            obj[FPSTR(P_label)] = label;
            if (directly) obj[FPSTR(P_directly)] = true;

            obj[FPSTR(P_min)] = min;
            obj[FPSTR(P_max)] = max;
            obj[FPSTR(P_step)] = step;

            if (!json_frame_add(obj.as<JsonObject>())) {
                range(id, value, min, max, step, label, directly);
            }
        };

        void select(const String &id, const String &label, bool directly = false, bool skiplabel = false);

        template <typename T>
        void select(const String &id, const T &value, const String &label, bool directly = false, bool skiplabel = false, const String &exturl = (char*)0){
            StaticJsonDocument<IFACE_STA_JSON_SIZE> obj;
            obj[FPSTR(P_html)] = F("select");
            obj[FPSTR(P_id)] = id;
            obj[FPSTR(P_value)] = value;
            obj[FPSTR(P_label)] = skiplabel ? "" : label;
            if (directly) obj[FPSTR(P_directly)] = true;
            if (!exturl.isEmpty())
                obj[F("url")] = exturl;

            if (!json_frame_add(obj.as<JsonObject>())) {
                select(id, value, label, directly);
                return;
            }
            section_stack.end()->idx--;
            json_section_begin(FPSTR(P_options), "", false, false, false, section_stack.end()->block.getElement(section_stack.end()->idx));
        };

        void option(const String &value, const String &label);

        /**
         * элемент интерфейса checkbox
         * @param directly - значение чекбокса при изменении сразу передается на сервер без отправки формы
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        template <typename T>
        void checkbox(const String &id, const T& value, const String &label, bool directly = false){ html_input(id, F("checkbox"), value, label, directly); };
        void checkbox(const String &id, const String &label, bool directly = false);

        template <typename T>
        void color(const String &id, const T &value, const String &label){ html_input(id, FPSTR(P_color), value, label); };
        void color(const String &id, const String &label);

        /**
         * элемент интерфейса textarea
         * Template accepts types suitable to be added to the ArduinoJson document used as a dictionary
         */
        template <typename T>
        void textarea(const String &id, const T &value, const String &label){ html_input(id, F("textarea"), value, label); };
        void textarea(const String &id, const String &label);

        void file(const String &name, const String &action, const String &label);
        void button(const String &id, const String &label, const String &color = "");
        void button_submit(const String &section, const String &label, const String &color = "");
        void button_submit_value(const String &section, const String &value, const String &label, const String &color = "");
        void spacer(const String &label = "");
        void comment(const String &label = "");

        /**
         * @brief - create generic html input element
         * @param id - element id
         * @param type - html element type, ex. 'text'
         * @param value - element value
         * @param label - element label
         * @param direct - if true, element value in send via ws on-change 
         */
        template <typename T>
        void html_input(const String &id, const String &type, const T &value, const String &label, bool direct = false){
            StaticJsonDocument<IFACE_STA_JSON_SIZE> obj;
            obj[FPSTR(P_html)] = FPSTR(P_input);
            obj[FPSTR(P_type)] = type;
            obj[FPSTR(P_id)] = id;
            obj[FPSTR(P_value)] = value;
            obj[FPSTR(P_label)] = label;
            if (direct) obj[FPSTR(P_directly)] = true;

            frame_add_safe(obj.as<JsonObject>());
        };

        /**
         * @brief - create "display" div with custom css selector
         * could be used for making all kinds of "sensor" outputs on the page with live-updated values without the need to redraw interface element
         * @param id - element/div DOM id
         * @param value  - element value (treated as text)
         * @param class - base css class for Display, full css seletor created as "class id" to allow many sensors inherit base class
         * @param params - additional parameters (reserved for future use)
         */
        template <typename T>
        void display(const String &id, const T &value, const String &css = String(), const JsonObject &params = JsonObject() ){
            String cssclass(css);   // make css slector like "class id", id used as a secondary distinguisher 
            if (!css.length())
                cssclass += F("display");
            cssclass += F(" ");
            cssclass += id;
            custom(id, F("txt"), value, cssclass, params);
        };

        /**
         * @brief - Creates html element with cutomized type and arbitrary parameters
         * used to create user-defined interface elements with custom css/js handlers
         */
        template <typename T>
        void custom(const String &id, const String &type, const T &value, const String &label, const JsonObject &param = JsonObject()){
            StaticJsonDocument<IFACE_STA_JSON_SIZE*2> obj; // по этот контрол выделяем IFACE_STA_JSON_SIZE*2 т.к. он может быть большой...
            obj[FPSTR(P_html)] = F("custom");;
            obj[FPSTR(P_type)] = type;
            obj[FPSTR(P_id)] = id;
            obj[FPSTR(P_value)] = value;
            obj[FPSTR(P_label)] = label;
            JsonObject nobj = obj.createNestedObject(String(F("param")));
            nobj.set(param);
            frame_add_safe(obj.as<JsonObject>());
        }

};

#endif