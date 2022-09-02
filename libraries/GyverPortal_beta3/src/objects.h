#pragma once
#include "utils.h"

#define GP_EMPTY_INT INT32_MAX
#define GP_EMPTY_FLOAT NAN

struct GP_TITLE {
    GP_TITLE(const char* n = nullptr, const String& t = "") : name(n), text(t) {}
    const char* name;
    String text;
};
struct GP_LABEL {
    GP_LABEL(const char* n = nullptr, const String& t = "") : name(n), text(t) {}
    const char* name;
    String text;
};
struct GP_LABEL_BLOCK {
    GP_LABEL_BLOCK(const char* n = nullptr, const String& t = "", GPstyle st = GP_GREEN) : name(n), text(t), style(st) {}
    const char* name;
    String text;
    GPstyle style;
};

struct GP_LED {
    GP_LED(const char* n = nullptr, bool st = 0) : name(n), state(st) {}
    const char* name;
    bool state;
};
struct GP_LED_RED {
    GP_LED_RED(const char* n = nullptr, bool st = 0) : name(n), state(st) {}
    const char* name;
    bool state;
};
struct GP_LED_GREEN {
    GP_LED_GREEN(const char* n = nullptr, bool st = 0) : name(n), state(st) {}
    const char* name;
    bool state;
};

struct GP_BUTTON {
    GP_BUTTON(const char* n = nullptr, const String& t = "", const String& tar = "", GPstyle st = GP_GREEN, const String& w = "") : 
    name(n), text(t), target(tar), style(st), width(w) {}
    const char* name;
    String text;
    String target;
    GPstyle style;
    String width;
};
struct GP_BUTTON_MINI {
    GP_BUTTON_MINI(const char* n = nullptr, const String& t = "", const String& tar = "", GPstyle st = GP_GREEN, const String& w = "") : 
    name(n), text(t), target(tar), style(st), width(w) {}
    const char* name;
    String text;
    String target;
    GPstyle style;
    String width;
};

struct GP_NUMBER {
    GP_NUMBER(const char* n = nullptr, const String& place = "", int val = GP_EMPTY_INT, const String& w = "", bool dis = false) : 
    name(n), placeholder(place), value(val), width(w), disabled(dis) {}
    const char* name;
    String placeholder;
    int value;
    String width;
    String min = "";
    String max = "";
    bool disabled;
};
struct GP_NUMBER_F {
    GP_NUMBER_F(const char* n = nullptr, const String& place = "", float val = GP_EMPTY_FLOAT, uint8_t dec = 2, const String& w = "", bool dis = false) : 
    name(n), placeholder(place), value(val), decimals(dec), width(w), disabled(dis) {}
    const char* name;
    String placeholder;
    float value;
    uint8_t decimals;
    String width;
    String min = "";
    String max = "";
    bool disabled;
};

struct GP_TEXT {
    GP_TEXT(const char* n = nullptr, const String& place = "", const String& txt = "", const String& w = "", bool dis = false) : 
    name(n), placeholder(place), text(txt), width(w), disabled(dis) {}
    const char* name;
    String placeholder;
    String text;
    String width;
    bool disabled;
};
struct GP_PASS {
    GP_PASS(const char* n = nullptr, const String& place = "", const String& txt = "", const String& w = "") : 
    name(n), placeholder(place), text(txt), width(w) {}
    const char* name;
    String placeholder;
    String text;
    String width;
};

struct GP_AREA {
    GP_AREA(const char* n = nullptr, int r = 1, const String& txt = "", bool dis = false) : 
    name(n), rows(r), text(txt), disabled(dis) {}
    const char* name;
    int rows;
    String text;
    bool disabled;
};

struct GP_CHECK {
    GP_CHECK(const char* n = nullptr, bool st = 0, bool dis = false) : name(n), state(st), disabled(dis) {}
    const char* name;
    bool state;
    bool disabled;
};
struct GP_SWITCH {
    GP_SWITCH(const char* n = nullptr, bool st = 0, bool dis = false) : name(n), state(st), disabled(dis) {}
    const char* name;
    bool state;
    bool disabled;
};

struct GP_DATE {
    GP_DATE(const char* n = nullptr) : name(n) {}
    GP_DATE(const char* n, GPdate d, bool dis = false) : name(n), date(d), disabled(dis) {}
    const char* name;
    GPdate date;
    bool disabled = false;
};
struct GP_TIME {
    GP_TIME(const char* n = nullptr) : name(n) {}
    GP_TIME(const char* n, GPtime t, bool dis = false) : name(n), time(t), disabled(dis) {}
    const char* name;
    GPtime time;
    bool disabled = false;
};
struct GP_COLOR {
    GP_COLOR(const char* n = nullptr) : name(n) {}
    GP_COLOR(const char* n, GPcolor c, bool dis = false) : name(n), color(c), disabled(dis) {}
    const char* name;
    GPcolor color;
    bool disabled = false;
};

struct GP_SPINNER {
    GP_SPINNER(const char* n = nullptr, float v = 0, float mn = NAN, float mx = NAN, float stp = 1, uint8_t dec = 0, GPstyle st = GP_GREEN, const String& w = "") : 
    name(n), value(v), min(mn), max(mx), step(stp), decimals(dec), style(st), width(w) {}
    const char* name;
    float value;
    float min;
    float max;
    float step;
    uint8_t decimals;
    GPstyle style;
    String width;
};

struct GP_SLIDER {
    GP_SLIDER(const char* n = nullptr, float val = 0, float mn = 0, float mx = 10, float stp = 1, uint8_t dec = 0, GPstyle st = GP_GREEN, bool dis = false) : 
    name(n), value(val), min(mn), max(mx), step(stp), decimals(dec), style(st), disabled(dis) {}
    const char* name;
    float value = 0;
    float min = 0;
    float max = 10;
    float step = 1;
    uint8_t decimals;
    GPstyle style;
    bool disabled;
};

struct GP_SELECT {
    GP_SELECT(const char* n = nullptr, const String& l = "", int8_t sel = 0, bool dis = false) : 
    name(n), list(l), selected(sel), disabled(dis) {}
    const char* name;
    String list;
    int8_t selected;
    bool disabled;
    
    String getValue() {
        return GPlistIdx(selected, list);
    }
};
struct GP_SELECT_IDX {
    GP_SELECT_IDX(const char* n = nullptr, const String& l = "", int8_t sel = 0, bool dis = false) : 
    name(n), list(l), selected(sel), disabled(dis) {}
    const char* name;
    String list;
    int8_t selected;
    bool disabled;
    
    String getValue() {
        return GPlistIdx(selected, list);
    }
};