// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "ui.h"

void Interface::hidden(const String &id, const String &value){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_hidden);
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;

    if (!json_frame_add(obj.as<JsonObject>())) {
        hidden(id, value);
    }
}

void Interface::hidden(const String &id){
    hidden(id, embui->param(id));
}

void Interface::constant(const String &id, const String &value, const String &label){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = F("const");
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        constant(id, value, label);
    }
}

void Interface::constant(const String &id, const String &label){
    constant(id, embui->param(id), label);
}

void Interface::text(const String &id, const String &value, const String &label, bool directly){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_input);
    obj[FPSTR(P_type)] = F("text");
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;

    if (directly) obj[FPSTR(P_directly)] = true;

    if (!json_frame_add(obj.as<JsonObject>())) {
        text(id, value, label, directly);
    }
}

void Interface::text(const String &id, const String &label, bool directly){
    text(id, embui->param(id), label, directly);
}

void Interface::number(const String &id, int value, const String &label, int min, int max){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_input);
    obj[FPSTR(P_type)] = FPSTR(P_number);
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;
    obj[FPSTR(P_min)] = min;
    if (max) obj[FPSTR(P_max)] = max;

    if (!json_frame_add(obj.as<JsonObject>())) {
        number(id, value, label, min, max);
    }
}

void Interface::number(const String &id, const String &label, int min, int max){
    number(id, (int)embui->param(id).toInt(), label, min, max);
}

void Interface::number(const String &id, float value, const String &label, float step, int min, int max){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_input);
    obj[FPSTR(P_type)] = FPSTR(P_number);
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;
    obj[FPSTR(P_min)] = min;
    if (max) obj[FPSTR(P_max)] = max;
    if (step) obj[FPSTR(P_step)] = step;

    if (!json_frame_add(obj.as<JsonObject>())) {
        number(id, value, label, step, min, max);
    }
}

void Interface::number(const String &id, const String &label, float step, int min, int max){
    number(id, embui->param(id).toFloat(), label, step);
}

void Interface::time(const String &id, const String &value, const String &label){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_input);
    obj[FPSTR(P_type)] = FPSTR(P_time);
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        time(id, value, label);
    }
}

void Interface::time(const String &id, const String &label){
    time(id, embui->param(id), label);
}

void Interface::date(const String &id, const String &value, const String &label){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_input);
    obj[FPSTR(P_type)] = FPSTR(P_date);
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        date(id, value, label);
    }
}

void Interface::date(const String &id, const String &label){
    time(id, embui->param(id), label);
}

void Interface::datetime(const String &id, const String &value, const String &label){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_input);
    obj[FPSTR(P_type)] = F("datetime-local");
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        datetime(id, value, label);
    }
}

void Interface::datetime(const String &id, const String &label){
    datetime(id, embui->param(id), label);
}

void Interface::range(const String &id, int value, int min, int max, float step, const String &label, bool directly){
    StaticJsonDocument<256> obj;
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
}

void Interface::range(const String &id, int min, int max, float step, const String &label, bool directly){
    range(id, (int)embui->param(id).toInt(), min, max, step, label, directly);
}

void Interface::email(const String &id, const String &value, const String &label){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_input);
    obj[FPSTR(P_type)] = F("email");
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        email(id, value, label);
    }
}

void Interface::email(const String &id, const String &label){
    email(id, embui->param(id), label);
}

void Interface::password(const String &id, const String &value, const String &label){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_input);
    obj[FPSTR(P_type)] = FPSTR(P_password);
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        password(id, value, label);
    }
}

void Interface::password(const String &id, const String &label){
    password(id, embui->param(id), label);
}

void Interface::option(const String &value, const String &label){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_label)] = label;
    obj[FPSTR(P_value)] = value;

    if (!json_frame_add(obj.as<JsonObject>())) {
        option(value, label);
    }
}

void Interface::select(const String &id, const String &value, const String &label, bool directly, bool skiplabel){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = F("select");
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = skiplabel ? "" : label;
    if (directly) obj[FPSTR(P_directly)] = true;

    if (!json_frame_add(obj.as<JsonObject>())) {
        select(id, value, label, directly);
        return;
    }
    section_stack.end()->idx--;
    json_section_begin(FPSTR(P_options), "", false, false, false, section_stack.end()->block.getElement(section_stack.end()->idx));
}

void Interface::select(const String &id, const String &label, bool directly, bool skiplabel){
    select(id, embui->param(id), label, directly, skiplabel);
}

void Interface::checkbox(const String &id, const String &value, const String &label, bool directly){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_input);
    obj[FPSTR(P_type)] = F("checkbox");
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;
    if (directly) obj[FPSTR(P_directly)] = true;

    if (!json_frame_add(obj.as<JsonObject>())) {
        checkbox(id, value, label, directly);
    }
}

void Interface::checkbox(const String &id, const String &label, bool directly){
    checkbox(id, embui->param(id), label, directly);
}

void Interface::color(const String &id, const String &value, const String &label){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_input);
    obj[FPSTR(P_type)] = FPSTR(P_color);
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        color(id, value, label);
    }
}

void Interface::color(const String &id, const String &label){
    color(id, embui->param(id), label);
}

void Interface::file(const String &name, const String &action, const String &label){
     StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_file);
    obj[F("name")] = name;
    obj[F("action")] = action;
    obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        file(name, action, label);
    }
}

void Interface::button(const String &id, const String &label, const String &color){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_button);
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_color)] = color;
    obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        button(id, label, color);
    }
}

void Interface::button_submit(const String &section, const String &label, const String &color){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_button);
    obj[FPSTR(P_submit)] = section;
    obj[FPSTR(P_color)] = color;
    obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        button_submit(section, label, color);
    }
}

void Interface::button_submit_value(const String &section, const String &value, const String &label, const String &color){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = FPSTR(P_button);
    obj[FPSTR(P_submit)] = section;
    obj[FPSTR(P_color)] = color;
    obj[FPSTR(P_label)] = label;
    obj[FPSTR(P_value)] = value;

    if (!json_frame_add(obj.as<JsonObject>())) {
        button_submit_value(section, value, label, color);
    }
}

void Interface::spacer(const String &label){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = F("spacer");
    if (label != "") obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        spacer(label);
    }
}

void Interface::comment(const String &label){
    StaticJsonDocument<512> obj;
    obj[FPSTR(P_html)] = F("comment");
    if (label != "") obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        comment(label);
    }
}

void Interface::textarea(const String &id, const String &value, const String &label){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_html)] = F("textarea");
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = value;
    obj[FPSTR(P_label)] = label;

    if (!json_frame_add(obj.as<JsonObject>())) {
        textarea(id, label);
    }
}

void Interface::textarea(const String &id, const String &label){
    textarea(id, embui->param(id), label);
}

void Interface::value(const String &id, const String &val, bool html){
    StaticJsonDocument<256> obj;
    obj[FPSTR(P_id)] = id;
    obj[FPSTR(P_value)] = val;
    if (html) obj[FPSTR(P_html)] = true;

    if (!json_frame_add(obj.as<JsonObject>())) {
        value(id, val, html);
    }
}

void Interface::value(const String &id, bool html){
    value(id, embui->param(id), html);
}

///////////////////////////////////////
void Interface::json_frame_value(){
    json[F("pkg")] = FPSTR(P_value);
    json[FPSTR(P_final)] = false;

    json_section_begin("root" + String(rand()));
}

void Interface::json_frame_interface(const String &name){
    json[F("pkg")] = F("interface");
    if (name != "") {
        json[F("app")] = name;
        json[F("mc")] = embui->mc;
        json[F("ver")] = F(TOSTRING(EMBUIVER));
    }
    json[FPSTR(P_final)] = false;

    json_section_begin("root" + String(rand()));
}

bool Interface::json_frame_add(JsonObject obj) {
    //LOG(printf_P, PSTR("json_frame_add: %u = %u "), obj.memoryUsage(), json.capacity() - json.memoryUsage());
    if (json.capacity() - json.memoryUsage() > obj.memoryUsage() + 40 && section_stack.end()->block.add(obj)) {
        section_stack.end()->idx++;
        LOG(printf_P, PSTR("UI: OK [%u]\tMEM: %u\n"), section_stack.end()->idx, ESP.getFreeHeap());
        return true;
    }
    LOG(printf_P, PSTR("UI: BAD MEM: %u\n"), ESP.getFreeHeap());

    json_frame_send();
    json_frame_next();
    return false;
}

void Interface::json_frame_next(){
    json.clear();
    JsonObject obj = json.to<JsonObject>();
    for (int i = 0; i < section_stack.size(); i++) {
        if (i) obj = section_stack[i - 1]->block.createNestedObject();
        obj[FPSTR(P_section)] = section_stack[i]->name;
        obj[F("idx")] = section_stack[i]->idx;
        LOG(printf_P, PSTR("UI: section %u %s %u\n"), i, section_stack[i]->name.c_str(), section_stack[i]->idx);
        section_stack[i]->block = obj.createNestedArray(FPSTR(P_block));
    }
    LOG(printf_P, PSTR("json_frame_next: [%u] %u = %u\n"), section_stack.size(), obj.memoryUsage(), json.capacity() - json.memoryUsage());
}

void Interface::json_frame_clear(){
    while(section_stack.size()) {
        section_stack_t *section = section_stack.shift();
        delete section;
    }
    json.clear();
}

void Interface::json_frame_flush(){
    if (!section_stack.size()) return;
    LOG(println, F("json_frame_flush"));
    json[FPSTR(P_final)] = true;
    json_section_end();
    json_frame_send();
    json_frame_clear();
}

void Interface::json_frame_send(){
    String buff;
    serializeJson(json, buff);
    LOG(println, buff.c_str());
    if (send_hndl) send_hndl->send(buff);
}

void Interface::json_section_menu(){
    json_section_begin(FPSTR(P_menu));
}

void Interface::json_section_content(){
    json_section_begin(F("content"));
}

void Interface::json_section_line(const String &name){
    json_section_begin(name, "", false, false, true);
}

void Interface::json_section_main(const String &name, const String &label){
    json_section_begin(name, label, true);
}

void Interface::json_section_hidden(const String &name, const String &label){
    json_section_begin(name, label, false, true);
}

void Interface::json_section_begin(const String &name, const String &label, bool main, bool hidden, bool line){
    JsonObject obj;
    if (section_stack.size()) {
        obj = section_stack.end()->block.createNestedObject();
    } else {
        obj = json.as<JsonObject>();
    }
    json_section_begin(name, label, main, hidden, line, obj);
}

void Interface::json_section_begin(const String &name, const String &label, bool main, bool hidden, bool line, JsonObject obj){
    obj[FPSTR(P_section)] = name;
    if (label != "") obj[FPSTR(P_label)] = label;
    if (main) obj[F("main")] = true;
    if (hidden) obj[FPSTR(P_hidden)] = true;
    if (line) obj[F("line")] = true;

    section_stack_t *section = new section_stack_t;
    section->name = name;
    section->block = obj.createNestedArray(FPSTR(P_block));
    section->idx = 0;
    section_stack.add(section);
    LOG(printf_P, PSTR("UI: section begin %s [%u] %u\n"), name.c_str(), section_stack.size(), json.capacity() - json.memoryUsage());
}

void Interface::json_section_end(){
    if (!section_stack.size()) return;

    section_stack_t *section = section_stack.pop();
    if (section_stack.size()) {
        section_stack.end()->idx++;
    }
    LOG(printf_P, PSTR("UI: section end %s [%u] MEM: %u\n"), section->name.c_str(), section_stack.size(), ESP.getFreeHeap());
    delete section;
}