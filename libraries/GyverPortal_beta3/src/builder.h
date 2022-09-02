#pragma once
#include "utils.h"
#include "objects.h"

#ifdef ESP8266
#include <ESP8266WebServer.h>
extern ESP8266WebServer* _gp_s;
#else
#include <WebServer.h>
extern WebServer* _gp_s;
#endif

extern String* _gp_uri;
static uint8_t _gp_nav_pos = 0;
static uint8_t _gp_nav_id = 0;

struct Builder {
    // ======================= БИЛДЕР =======================
    void BUILD_BEGIN(int width = 350) {
        PAGE_BEGIN();
        JS_TOP();
        PAGE_BLOCK_BEGIN(width);
    }
    void BUILD_BEGIN(const String& url, int width = 350) {
        PAGE_BEGIN();
        String s;
        s += F("<script src='");
        s += url;
        s += F("GP_scripts.js'></script>");
        SEND(s);
        PAGE_BLOCK_BEGIN(width);
    }
    void BUILD_END() {
        PAGE_BLOCK_END();
        JS_BOTTOM();
        PAGE_END();
    }
    
    // ===================== ОТПРАВКА RAW =====================
    void SEND(const String& s) {
        _gp_s->sendContent(s);
    }
    void SEND_P(PGM_P s) {
        _gp_s->sendContent_P(s);
    }
    
    // ================== ОТПРАВКА ОБЪЕКТОВ ===================
    void TITLE(GP_TITLE& title) {
        TITLE(title.text, title.name);
    }
    void LABEL(GP_LABEL& label) {
        LABEL(label.text, label.name);
    }
    void LABEL_BLOCK(GP_LABEL_BLOCK& label) {
        LABEL_BLOCK(label.text, label.name, label.style);
    }
    
    void LED(GP_LED& led) {
        LED(led.name, led.state);
    }
    void LED_RED(GP_LED_RED& led) {
        LED_RED(led.name, led.state);
    }
    void LED_GREEN(GP_LED_GREEN& led) {
        LED_GREEN(led.name, led.state);
    }
    
    void BUTTON(GP_BUTTON& btn) {
        BUTTON(btn.name, btn.text, btn.target, btn.style, btn.width);
    }
    void BUTTON_MINI(GP_BUTTON_MINI& btn) {
        BUTTON_MINI(btn.name, btn.text, btn.target, btn.style, btn.width);
    }
    
    void NUMBER(GP_NUMBER& num) {
        NUMBER_RAW(num.name, num.placeholder, (num.value == INT32_MAX ? String("") : String(num.value)), num.min, num.max, num.width, num.disabled);
    }
    void NUMBER_F(GP_NUMBER_F& num) {
        NUMBER_F(num.name, num.placeholder, num.value, num.decimals, num.width, num.disabled);
        NUMBER_RAW(num.name, num.placeholder, (isnan(num.value) ? String("") : String(num.value, (uint16_t)num.decimals)), num.min, num.max, num.width, num.disabled);
    }
    
    void TEXT(GP_TEXT& num) {
        TEXT(num.name, num.placeholder, num.text, num.width, num.disabled);
    }
    void PASS(GP_PASS& num) {
        PASS(num.name, num.placeholder, num.text, num.width);
    }
    
    void AREA(GP_AREA& ar) {
        AREA(ar.name, ar.rows, ar.text, ar.disabled);
    }
    
    void CHECK(GP_CHECK& ch) {
        CHECK(ch.name, ch.state, ch.disabled);
    }
    void SWITCH(GP_SWITCH& sw) {
        SWITCH(sw.name, sw.state, sw.disabled);
    }
    
    void DATE(GP_DATE& d) {
        DATE(d.name, d.date, d.disabled);
    }
    void TIME(GP_TIME& t) {
        TIME(t.name, t.time, t.disabled);
    }
    void COLOR(GP_COLOR& c) {
        COLOR(c.name, c.color, c.disabled);
    }
    
    void SPINNER(GP_SPINNER& s) {
        SPINNER(s.name, s.value, s.min, s.max, s.step, s.decimals, s.style, s.width);
    }
    void SLIDER(GP_SLIDER& s) {
        SLIDER(s.name, s.value, s.min, s.max, s.step, s.decimals, s.style, s.disabled);
    }
    
    void SELECT(GP_SELECT& s) {
        SELECT(s.name, s.list, s.selected, s.disabled);
    }
    void SELECT_IDX(GP_SELECT_IDX& s) {
        SELECT_IDX(s.name, s.list, s.selected, s.disabled);
    }


    // ======================= СТРАНИЦА =======================
    void PAGE_BEGIN() {
        _gp_nav_id = 0;
        SEND_P(PSTR("<!DOCTYPE HTML><html><head>\n"
        "<meta charset='utf-8'>\n"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>\n"
        "</head><body>\n"));
    }
    void JS_TOP() {
        SEND_P(PSTR("<script>\n"
        "function GP_send(req){var xhttp=new XMLHttpRequest();xhttp.open('POST',req,true);xhttp.send();}\n"
        "function GP_clickUD(arg,dir){GP_send('/GP_click?'+arg.name+'=1&_dir='+dir);}\n"
        "function GP_click(arg){var v; if(arg.type=='number'){\n"
        "if(arg.hasAttribute('min')&&Number(arg.value)<=Number(arg.min))arg.value=arg.min;\n"
        "if(arg.hasAttribute('max')&&Number(arg.value)>=Number(arg.max))arg.value=arg.max;}\n"
        "if(arg.type=='checkbox')v=arg.checked?'1':'0';else v=arg.value;\n"
        "if(v.charAt(0)=='#')v=v.substring(1);GP_send('/GP_click?'+arg.name+'='+v);}\n"
        "function GP_clickid(btn,tar){GP_send('/GP_click?'+btn+'='+document.getElementById(tar).value);}\n"
        "function GP_change(arg){arg.style.backgroundSize=(arg.value-arg.min)*100/(arg.max-arg.min)+'% 100%';\n"
        "document.getElementById(arg.id+'_val').value=arg.value}\n"
        "function GP_wheel(arg){var e=window.event;arg.value-=Math.sign(e.deltaY||e.detail||e.wheelDelta)*Number(arg.step);}\n"
        "function getFile(id){document.getElementById(id).click();}\n"
        "function GP_subm(id){document.getElementById(id).submit();event.preventDefault();}\n"
        "function openTab(tab,btn,blk){var x=document.getElementsByClassName(blk);\n"
        "for(var i=0;i<x.length;i++)x[i].style.display='none';\n"
        "document.getElementById(tab).style.display='block';\n"
        "x=document.getElementsByClassName(btn.className);\n"
        "for(var i=0;i<x.length;i++)x[i].style.background='';\n"
        "btn.style.background='#2a2d35';}\n"
        "function GP_spin(id,stp,dec){var num=document.getElementById(id);num.value=(Number(num.value)+stp).toFixed(dec);var e=new Event('change');num.dispatchEvent(e);}\n"
        "</script>\n"));
    }
    void JS_BOTTOM() {
        SEND_P(PSTR("<script>document.querySelectorAll('input[type=range]').forEach(x=>{GP_change(x)})</script>\n"));
    }
    
    void THEME(PGM_P style) {
        SEND_P(style);
    }
    void THEME_FILE(const String& url) {
        String s;
        s += F("<link rel='stylesheet' href='");
        s += url;
        s += F("'>\n");
        SEND(s);
    }
    
    void PAGE_BLOCK_BEGIN(int width = 350) {
        String s;
        s.reserve(60);
        s += F("<div align='center' style='margin:auto;max-width:");
        s += width;
        s += F("px;'>\n");
        SEND(s);
    }
    void PAGE_BLOCK_END() {
        SEND_P(PSTR("</div>\n"));
    }
    void PAGE_END() {
        SEND_P(PSTR("</body></html>"));
    }
    
    // ======================= UPDATE =======================
    void AJAX_UPDATE(const String& list, int prd = 1000) {
        UPDATE(list, prd);
    }
    void UPDATE(const String& list, int prd = 1000) {
        String s;
        s.reserve(500);
        s += F("<script>setInterval(function(){\n");
        s += "var ids='";
        s += list;
        s += F("'.split(',');\n"
        "ids.forEach(function(id){\n"
        "var xhttp=new XMLHttpRequest();\n"
        "xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200){\n"
        "var resp=this.responseText;\n"
        "var item=document.getElementById(id);if(item==null)return;\n"
        "if(item.type=='hidden'&&item.value=='reload'&&resp=='1')location.reload();\n"
        "else if(item.type=='checkbox'||item.type=='radio')item.checked=Number(resp);\n"
        "else if(item.type=='select-one')document.querySelector('#'+id).value=resp;\n"
        "else if(item.type==undefined)item.innerHTML=resp;\n"
        "else item.value=resp;\n"
        "if(item.type=='range')GP_change(item);\n"
        "}};xhttp.open('GET','/GP_update?'+id+'=',true);xhttp.send();});},");
        s += prd;
        s += F(");</script>\n");
        SEND(s);
    }
    
    void JQ_SUPPORT(const String& url = "") {
        if (url.length()) {
            String s;
            s += F("<script src='");
            s += url;
            s += F("GP_jquery.js'></script>\n");
            SEND(s);
        }
        else SEND_P(PSTR("<script src='https://code.jquery.com/jquery-3.6.1.min.js'></script>\n"));
    }
    void JQ_UPDATE_BEGIN(const String& id, int prd = 1000) {
        String s;
        s += F("<script>setInterval(function(){");
        s += F("$('#");
        s += id;
        s += F("').load('# #");
        s += id;
        s += F("');},");
        s += prd;
        s += F(");</script>\n");
        s += F("<div id='");
        s += id;
        s += F("'>\n");
        SEND(s);
    }
    void JQ_UPDATE_END() {
        SEND_P(PSTR("</div>\n"));
    }
    
    void RELOAD(const String& name) {
        HIDDEN(name, F("reload"));
    }
    
    
    // ====================== АЛЕРТЫ =======================
    void ALERT(const String& name, int prd = 1000) {
        String s;
        s.reserve(500);
        s += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length)alert(this.responseText);};\n"
        "xhttp.open('GET','/GP_update?'+'");
        s += name;
        s += F("=',true);xhttp.send();},");
        s += prd;
        s += F(");</script>\n");
        SEND(s);
    }
    
    void PROMPT(const String& name, const String& text, const String& place = "", int prd = 1000) {
        String s;
        s.reserve(500);
        s += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length){let res=prompt('");
        s += text;
        s += ',';
        s += place;
        s += F("');GP_send('/GP_click?");
        s += name;
        s += F("='+res);}};\n xhttp.open('GET','/GP_update?'+'");
        s += name;
        s += F("=',true);xhttp.send();},");
        s += prd;
        s += F(");</script>\n");
        SEND(s);
    }
    
    void CONFIRM(const String& name, const String& text, int prd = 1000) {
        String s;
        s.reserve(500);
        s += F("<script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200&&this.responseText.length){let res=confirm('");
        s += text;
        s += F("');GP_send('/GP_click?");
        s += name;
        s += F("='+(res?'1':'0'));}};\n xhttp.open('GET','/GP_update?'+'");
        s += name;
        s += F("=',true);xhttp.send();},");
        s += prd;
        s += F(");</script>\n");
        SEND(s);
    }
    
    // ======================= ФОРМА =======================
    void FORM_BEGIN(const String& action) {
        String s;
        s.reserve(40);
        s += F("<form action='");
        s += action;
        s += F("' method='POST'><input type='hidden' name='_dummy' value=''>\n");
        SEND(s);
    }
    void FORM_END() {
        SEND_P(PSTR("</form>\n"));
    }
    void SUBMIT(const String& text, GPstyle st = GP_GREEN) {
        String s;
        s.reserve(40);
        s += F("<input type='submit' value='");
        s += text;
        if (st != GP_GREEN) {
            s += F("' style='background:");
            s += GPgetStyle(st);
        }
        s += F("'>\n");
        SEND(s);
    }
    void FORM_SUBMIT(const String& name, const String& text) {
        FORM_BEGIN(name);
        SUBMIT(text);
        FORM_END();
    }
    void HIDDEN(const String& name, const String& text) {
		String s;
		s += F("<input type='hidden' name='");
		s += name;
		s += F("' value='");
		s += text;
        s += F("' id='");
		s += name;
		s += F("'>\n");
		SEND(s);
	}
	void FORM_SUBMIT(const String& name, const String& text, const String& namehidden, const String& valuehidden) {
		FORM_BEGIN(name);
		HIDDEN(namehidden, valuehidden);
		SUBMIT(text);
		FORM_END();
	}

    // ======================= ОФОРМЛЕНИЕ =======================
    void GRID_BEGIN() {
        SEND_P(PSTR("<div class='grid' id='grid'>\n"));
    }
    void GRID_END() {
        BLOCK_END();
    }
    void GRID_RESPONSIVE(int width) {
        String s;
        s += F("<style type='text/css'>@media screen and (max-width:");
        s += width;
        s += F("px){\n.grid{display:block;}\n#grid .block{margin:20px 10px;width:unset;}}</style>\n");
        SEND(s);
    }
    
    void BLOCK_BEGIN(const String& width = "") {
        String s;
        s += F("<div class='block' id='blockBack'");
        if (width.length()) {
            s += F(" style='max-width:");
            s += width;
            s += "'";
        }
        s += F(">\n");
        SEND(s);
    }
    void BLOCK_TAB_BEGIN(const String& label, const String& width = "", GPstyle st = GP_GREEN) {
        String s;
        s += F("<div class='block blockTab' id='blockBack'");
        if (width.length()) {
            s += F(" style='max-width:");
            s += width;
            s += "'";
        }
        s += F("><div class='blockHeader'");
        if (st != GP_GREEN) {
            s += F(" style='background:");
            s += GPgetStyle(st);
            s += "'";
        }
        s += F(">");
        s += label;
        s += F("</div>\n");
        SEND(s);
    }
    void BLOCK_THIN_BEGIN(const String& width = "") {
        String s;
        s += F("<div class='block thinBlock'");
        if (width.length()) {
            s += F(" style='max-width:");
            s += width;
            s += "'";
        }
        s += F(">\n");
        SEND(s);
    }
    void BLOCK_THIN_TAB_BEGIN(const String& label, const String& width = "") {
        String s;
        s += F("<div class='block blockTab thinBlock'");
        if (width.length()) {
            s += F(" style='max-width:");
            s += width;
            s += "'";
        }
        s += F("><div class='blockHeader thinTab'><span class='thinText'>");
        s += label;
        s += F("</span></div>\n");
        SEND(s);
    }
    void BLOCK_END() {
        SEND_P(PSTR("</div>\n"));
    }
    
    void BOX_BEGIN(GPalign al = GP_EDGES, const String& w = "100%") {
        String s;
        s.reserve(100);
        s += F("<div style='width:");
        s += w;
        s += F(";justify-content:");
        s += FPSTR(GPgetAlign(al));
        s += F("' class='inliner'>\n");
        SEND(s);
    }
    void BOX_END() {
        SEND_P(PSTR("</div>\n"));
    }
    
    void BREAK() {
        SEND_P(PSTR("<br>\n"));
    }
    void HR() {
        SEND_P(PSTR("<hr>\n"));
    }
    
    // ======================= ТЕКСТ =======================
    void TAG_RAW(const String& tag, const String& val, const String& name) {
        String s;
        s.reserve(50);
        s += F("<");
        s += tag;
        if (name.length()) {
            s += F(" id='");
            s += name;
            s += "'";
        }
        s += F(">");
        s += val;
        s += F("</");
        s += tag;
        s += F(">\n");
        SEND(s);
    }
    void TITLE(const String& val, const String& name="") {
        TAG_RAW(F("h2"), val, name);
    }
    
    void LABEL(const String& val, const String& name="") {
        TAG_RAW(F("label"), val, name);
    }
    
    void LABEL_BLOCK(const String& val, const String& name="", GPstyle st = GP_GREEN) {
        String s;
        s.reserve(50);
        s += F("<label class='display' ");
        if (st != GP_GREEN) {
            s += F("style='background:");
            s += GPgetStyle(st);
            s += F(";' ");
        }
        if (name.length()) {
            s += F("id='");
            s += name;
            s += "'";
        }
        s += F(">");
        s += val;
        s += F("</label>\n");
        SEND(s);
    }
    
    // ======================= ЛЕДЫ =======================
    void LED(const String& name, bool state = 0) {
        String s;
        s.reserve(70);
        s += F("<input class='ledn' type='radio' disabled ");
        if (state) s += F("checked ");
        s += F("name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("'>\n");
        SEND(s);
    }
    void LED_RED(const String& name, bool state = 0) {
        String s;
        s.reserve(70);
        s += F("<input class='led red' type='radio' disabled ");
        if (state) s += F("checked ");
        s += F("name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("'>\n");
        SEND(s);
    }
    void LED_GREEN(const String& name, bool state = 0) {
        String s;
        s.reserve(70);
        s += F("<input class='led green' type='radio' disabled ");
        if (state) s += F("checked ");
        s += F("name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("'>\n");
        SEND(s);
    }
    
    // ======================= ИКОНКИ =======================
    void ICON_SUPPORT() {
        SEND_P(PSTR("<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>\n"));
    }
    
    String ICON(const String& name, int size = 0) {
        String s(F("<i class='fa fa-"));
        s += name;
        if (size) {
            s += F("' style='font-size:");
            s += size;
            s += F("px;");
        }
        s += F("'></i>");
        return s;
    }
    String ICON_FILE(const String& uri, int size) {
        String s(F("<i style='background-repeat:no-repeat;background-image:url("));
        s += uri;
        s += F(");width:");
        s += size;
        s += F("px;height:");
        s += size;
        s += F("px;'></i>");
        return s;
    }
    
    // ======================= НАВИГАЦИЯ =======================
    void NAV_TABS_LINKS(const String& urls, const String& names, GPstyle st = GP_GREEN) {
        String s;
        s.reserve(300);
        s += F("<div class='navtab'><ul ");
        if (st != GP_GREEN) {
            s += F("style='background:");
            s += GPgetStyle(st);
            s += F("'");
        }
        s += F(">\n");
        GP_parser n;
        GP_parser u;
        while (n.parse(names)) {
            u.parse(urls);
            s += F("<li ");
            if (_gp_uri->equals(u.str)) s += F("style='background:#2a2d35' ");
            s += F("onclick='location.href=\"");
            s += u.str;
            s += F("\";'>");
            s += n.str;
            s += F("</li>\n");
        }
        s += F("</ul></div>\n");
        SEND(s);
    }
    
    void NAV_TABS(const String& list, GPstyle st = GP_GREEN) {
        String s;
        s.reserve(300);
        _gp_nav_id++;
        _gp_nav_pos = 0;
        s += F("<div class='navtab'><ul ");
        if (st != GP_GREEN) {
            s += F("style='background:");
            s += GPgetStyle(st);
            s += "' ";
        }
        s += F(">\n");
        GP_parser p;
        while (p.parse(list)) {
            s += F("<li ");
            if (!p.count) s += F("style='background:#2a2d35' ");
            s += F("class='nt-");
            s += _gp_nav_id;
            s += F("' onclick='openTab(\"ntab-");
            s += _gp_nav_id;
            s += "-";
            s += p.count;
            s += F("\",this,\"nb-");
            s += _gp_nav_id;
            s += F("\")'>");
            s += p.str;
            s += F("</li>\n");
        }
        s += F("</ul></div>\n");
        SEND(s);
    }
    
    void NAV_BLOCK_BEGIN() {
        String s;
        s.reserve(70);
        s += F("<div class='navblock nb-");
        s += _gp_nav_id;
        s += F("' id='ntab-");
        s += _gp_nav_id;
        s += "-";
        s += _gp_nav_pos;
        s += "' ";
        if (!_gp_nav_pos) s += F("style='display:block'");
        s += F(">\n");
        SEND(s);
        _gp_nav_pos++;
    }
    void NAV_BLOCK_END() {
        SEND_P(PSTR("</div>\n"));
    }
    
    // ======================= ФАЙЛЫ =======================
    void FILE_UPLOAD(const String& name, const String& accept = "") {
        String s;
        s.reserve(250);
        s += F("<form action='/GP_upload' method='POST' enctype='multipart/form-data' id='");
        s += name;
        s += F("_form'>\n");
        s += F("<div id='ubtn' onclick='getFile(\"");
        s += name;
        s += F("_inp\")'>📁</div>\n");
        s += F("<div id='ubtnclr'><input name='");
        s += name;
        if (accept.length()) {
            s += F("' accept='");
            s += accept;
        }
        s += F("' id='");
        s += name;
        s += F("_inp' type='file' onchange='GP_subm(\"");
        s += name;
        s += F("_form\")'/></div>\n");
        s += F("</form>\n");
        SEND(s);
    }
    
    void IMAGE(const String& uri, const String& w = "") {
        String s;
        s.reserve(100);
        s += F("<img src='");
        s += uri;
        s += F("' style='width:");
        s += w;
        s += F("'>\n");
        SEND(s);
    }
    void VIDEO(const String& uri, const String& w = "") {
        String s;
        s.reserve(100);
        s += F("<video src='");
        s += uri;
        s += F("' style='width:");
        s += w;
        s += F("' controls>Browser doesn't support video.</video>\n");
        SEND(s);
    }
    void EMBED(const String& uri, const String& w = "100%", const String& h = "") {
        String s;
        s.reserve(100);
        s += F("<embed src='");
        s += uri;
        s += F("' style='background:white;width:");
        s += w;
        s += F(";height:");
        s += h;
        s += F("'>\n");
        SEND(s);
    }
    
    // ======================= КНОПКА =======================
    void BUTTON_RAW(const String& name, const String& value, const String& tar, GPstyle st, const String& width = "", const String& cls = "") {
        String s;
        s.reserve(120);
        s += F("<input type='button' ");
        if (cls.length()) {
            s += F("class='");
            s += cls;
            s += "' ";
        }
        s += F("style='");
        if (st != GP_GREEN) {
            s += F("background:");
            s += GPgetStyle(st);
            s += ';';
        }
        if (width.length()) {
            s += F("width:");
            s += width;
            s += ';';
        }
        s += F("' name='");
        s += name;
        s += F("' value='");
        s += value;
        s += F("' onmousedown='GP_clickUD(this,0)' onmouseup='GP_clickUD(this,1)' ");
        if (tar.length()) {
            s += F("onclick=\"GP_clickid('");
            s += name;
            s += F("','");
            s += tar;
            s += F("')\">\n");
        } else {
            s += F("onclick='GP_click(this)'>\n");
        }
        SEND(s);
    }
    
    void BUTTON(const String& name, const String& value, const String& tar="", GPstyle st = GP_GREEN, const String& width = "") {
        BUTTON_RAW(name, value, tar, st, width);
    }
    void BUTTON_MINI(const String& name, const String& value, const String& tar="", GPstyle st = GP_GREEN, const String& width = "") {
        BUTTON_RAW(name, value, tar, st, width, F("miniButton"));
    }
    
    // ======================= КНОПКА-ССЫЛКА =======================
    void BUTTON_LINK_RAW(const String& url, const String& value, GPstyle st = GP_GREEN, const String& width = "", const String& cls = "") {
        String s;
        s.reserve(120);
        s += F("<input type='button' ");
        if (cls.length()) {
            s += F("class='");
            s += cls;
            s += "' ";
        }
        s += F("style='");
        if (st != GP_GREEN) {
            s += F("background:");
            s += GPgetStyle(st);
            s += ';';
        }
        if (width.length()) {
            s += F("width:");
            s += width;
            s += ';';
        }
        s += F("' value='");
        s += value;
        s += F("' onclick='location.href=\"");
        s += url;
        s += F("\";'>\n");
        SEND(s);
    }
    void BUTTON_LINK(const String& url, const String& value, GPstyle st = GP_GREEN, const String& width = "") {
        BUTTON_LINK_RAW(url, value, st, width);
    }
    void BUTTON_MINI_LINK(const String& url, const String& value, GPstyle st = GP_GREEN, const String& width = "") {
        BUTTON_LINK_RAW(url, value, st, width, F("miniButton"));
    }
    
    // ==================== КНОПКА-СКАЧКА ====================
    void BUTTON_DOWNLOAD_RAW(const String& url, const String& value, GPstyle st = GP_GREEN, const String& width = "", const String& cls = "") {
        String s;
        s.reserve(120);
        s += F("<a style='text-decoration:none;' href='");
        s += url;
        s += F("' download><input type='button' value='");
        s += value;
        s += "' ";
        if (cls.length()) {
            s += F("class='");
            s += cls;
            s += "' ";
        }
        s += F("style='");
        if (st != GP_GREEN) {
            s += F("background:");
            s += GPgetStyle(st);
            s += ';';
        }
        if (width.length()) {
            s += F("width:");
            s += width;
            s += ';';
        }
        s += F("'>");
        s += F("</a>\n");
        SEND(s);
    }
    void BUTTON_DOWNLOAD(const String& url, const String& value, GPstyle st = GP_GREEN, const String& width = "") {
        BUTTON_DOWNLOAD_RAW(url, value, st, width);
    }
    void BUTTON_MINI_DOWNLOAD(const String& url, const String& value, GPstyle st = GP_GREEN, const String& width = "") {
        BUTTON_DOWNLOAD_RAW(url, value, st, width, F("miniButton"));
    }
    
    // ========================= ВВОД ========================
    void NUMBER_RAW(const String& name, const String& place, const String& value, const String& minv, const String& maxv, const String& width, bool dis) {
        String s;
        s.reserve(110);
        s += F("<input type='number' step='any' name='");
        s += name;
        s += F("' id='");
        s += name;
        if (value.length()) {
            s += F("' value='");
            s += value;
        }
        if (width.length()) {
            s += F("' style='width:");
            s += width;
        }
        if (minv.length()) {
            s += F("' min='");
            s += minv;
        }
        if (maxv.length()) {
            s += F("' max='");
            s += maxv;
        }
        s += F("' placeholder='");
        s += place;
        s += F("' onchange='GP_click(this)'");
        if (dis) s += F(" disabled");
        s += F(">\n");
        SEND(s);
    }
    void NUMBER(const String& name, const String& place, int value = INT32_MAX, const String& width = "", bool dis = false) {
        NUMBER_RAW(name, place, (value == INT32_MAX ? String("") : String(value)), "", "", width, dis);
    }
    void NUMBER_F(const String& name, const String& place, float value = NAN, uint8_t dec = 2, const String& width = "", bool dis = false) {
        NUMBER_RAW(name, place, (isnan(value) ? String("") : String(value, (uint16_t)dec)), "", "", width, dis);
    }
    
    void TEXT(const String& name, const String& place, const String& value = "", const String& width = "", bool dis = false) {
        String s;
        s.reserve(120);
        s += F("<input type='text' name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("' value='");
        s += value;
        if (width.length()) {
            s += F("' style='width:");
            s += width;
        }
        s += F("' placeholder='");
        s += place;
        s += F("' onchange='GP_click(this)'");
        if (dis) s += F(" disabled");
        s += F(">\n");
        SEND(s);
    }
    void PASS(const String& name, const String& place, const String& value = "", const String& width = "") {
        String s;
        s.reserve(110);
        s += F("<input type='password' name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("' value='");
        s += value;
        if (width.length()) {
            s += F("' style='width:");
            s += width;
        }
        s += F("' placeholder='");
        s += place;
        s += F("' onchange='GP_click(this)'>\n");
        SEND(s);
    }
    
    void AREA(const String& name, int rows = 1, const String& value = "", bool dis = false) {
        String s;
        s.reserve(120);
        s += F("<textarea onchange='GP_click(this)' style='height:auto' name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("' rows='");
        s += rows;
        s += "'";
        if (dis) s += F(" disabled");
        s += ">";
        if (value.length()) s += value;
        s += F("</textarea>\n");
        SEND(s);
    }
    
    void AREA_LOG(int rows = 5) {
        String s;
        s.reserve(500);
        s += F("<textarea style='height:auto' id='GP_log' rows='");
        s += rows;
        s += F("' disabled></textarea>\n");
        s += F("<script>let _gplog='';\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();\n"
        "xhttp.onreadystatechange=function(){\n"
        "if(this.readyState==4&&this.status==200){\n"
        "_gplog+=this.responseText;\n"
        "var elm=document.getElementById('GP_log');\n"
        "elm.innerHTML=_gplog;elm.scrollTop=elm.scrollHeight;}};\n"
        "xhttp.open('GET','/GP_log?',true);xhttp.send();},1000);</script>\n");
        SEND(s);
    }
    
    // ======================= НАСТРОЙКА =======================
    void CHECK(const String& name, bool state = 0, bool dis = false) {
        String s;
        s.reserve(100);
        s += F("<input type='checkbox' name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("' value='");
        s += state;
        s += "' ";
        if (state) s += F("checked ");
        if (dis) s += F("disabled ");
        s += F("onclick='GP_click(this)'>\n");
        SEND(s);
    }
    void SWITCH(const String& name, bool state = 0, bool dis = false) {
        String s;
        s.reserve(100);
        s += F("<label class='switch'><input type='checkbox' name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("' value='");
        s += state;
        s += "' ";
        if (state) s += F("checked ");
        if (dis) s += F("disabled ");
        s += F("onclick='GP_click(this)'>\n");
        s += F("<span class='slider'></span></label>");
        SEND(s);
    }
    
    void DATE(const String& name, bool dis = false) {
        GPdate d;
        DATE(name, d, dis);
    }
    void DATE(const String& name, GPdate d, bool dis = false) {
        String s;
        s.reserve(100);
        s += F("<input step='any' type='date' name='");
        s += name;
        s += F("' id='");
        s += name;
        if (d.year) {
            s += F("' value='");
            s += d.encode();
        }
        s += "' ";
        if (dis) s += F("disabled ");
        s += F("onchange='GP_click(this)'>\n");
        SEND(s);
    }
    
    void TIME(const String& name, bool dis = false) {
        String s;
        s.reserve(90);
        s += F("<input step='any' type='time' name='");
        s += name;
        s += F("' id='");
        s += name;
        s += "' ";
        if (dis) s += F("disabled ");
        s += F("onchange='GP_click(this)'>\n");
        SEND(s);
    }
    void TIME(const String& name, GPtime t, bool dis = false) {
        String s;
        s.reserve(100);
        s += F("<input step='any' type='time' name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("' value='");
        s += t.encode();
        s += "' ";
        if (dis) s += F("disabled ");
        s += F("onchange='GP_click(this)'>\n");
        SEND(s);
    }
    
    void SLIDER(const String& name, float value, float min, float max, float step = 1, uint8_t dec = 0, GPstyle st = GP_GREEN, bool dis = false) {
        String s;
        s.reserve(220);
        s += F("<input type='range' name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("' value='");
        s += value;
        s += F("' min='");
        s += min;
        s += F("' max='");
        s += max;
        s += F("' step='");
        s += step;
        s += F("' style='background-image:linear-gradient(");
        s += GPgetStyle(st);
        s += ',';
        s += GPgetStyle(st);
        s += F(");' onload='GP_change(this)' onchange='GP_click(this)' oninput='GP_change(this)' onmousewheel='GP_wheel(this);GP_change(this);GP_click(this)' ");
        if (dis) s += F("disabled");
        s += F(">\n");
        s += F("<output id='");
        s += name;
        s += F("_val' ");
        if (st != GP_GREEN) {
            s += F("style='background:");
            s += GPgetStyle(st);
            s += F(";'");
        }
        s += F(">");
        if (!dec) s += round(value);
        else s += String(value, (uint16_t)dec);
        s += F("</output>\n");
        SEND(s);
    }
    
    String SPIN_BTN(const String& name, float step, bool dir, GPstyle st, uint8_t dec) {
        String s;
        s.reserve(70);
        s += F("<input type='button' class='spinBtn' onclick='GP_spin(\"");
        s += name;
        s += F("\",");
        s += dir ? step : -step;
        s += ',';
        s += dec;
        s += F(")' ");
        s += F("value='");
        s += dir ? '+' : '-';
        s += "' ";
        if (st != GP_GREEN) {
            s += F(" style='background:");
            s += GPgetStyle(st);
            s += F(";'");
        }
        s += F(">\n");
        return s;
    }
    void SPINNER(const String& name, float value, float min = NAN, float max = NAN, float step = 1, uint8_t dec = 0, GPstyle st = GP_GREEN, const String& width = "") {
        String s;
        s.reserve(320);
        s += F("<div id='spinner' class='spinner'>\n");
        s += SPIN_BTN(name, step, false, st, dec);
        s += F("<input type='number' name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("' step='");
        s += String(step, (uint16_t)dec);
        if (width.length()) {
            s += F("' style='width:");
            s += width;
        }
        s += F("' onchange='GP_click(this)' onmousewheel='GP_wheel(this);GP_click(this)' value='");
        if (!dec) s += round(value);
        else s += String(value, (uint16_t)dec);
        if (!isnan(min)) {
            s += F("' min='");
            s += min;
        }
        if (!isnan(max)) {
            s += F("' max='");
            s += max;
        }
        s += F("'>\n");
        s += SPIN_BTN(name, step, true, st, dec);
        s += F("</div>\n");
        SEND(s);
    }
    
    void COLOR(const String& name, uint32_t value = 0, bool dis = false) {
        String s;
        s.reserve(100);
        s += F("<input type='color' name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("' value='");
        GPcolor col(value);
        s += col.encode();
        s += "' ";
        if (dis) s += F("disabled ");
        s += F("onchange='GP_click(this)'>\n");
        SEND(s);
    }
    void COLOR(const String& name, GPcolor col, bool dis = false) {
        String s;
        s.reserve(100);
        s += F("<input type='color' name='");
        s += name;
        s += F("' id='");
        s += name;
        s += F("' value='");
        s += col.encode();
        s += "' ";
        if (dis) s += F("disabled ");
        s += F("onchange='GP_click(this)'>\n");
        SEND(s);
    }

    void SELECT(const String& name, const String& list, int8_t sel = 0, bool dis = false) {
        String s;
        s.reserve(250);
        if (sel < 0) sel = 0;
        s += F("<select name='");
        s += name;
        s += F("' id='");
        s += name;
        s += "' ";
        if (dis) s += F("disabled ");
        s += F("onchange='GP_click(this)'>\n");
        GP_parser p;
        while (p.parse(list)) {
            s += F("<option value='");
            s += p.str;
            s += F("'");
            if (p.count == sel) s += F(" selected");
            s += F(">");
            s += p.str;
            s += F("</option>\n");
        }
        s += F("</select>\n");
        SEND(s);
    }
    
    void SELECT_IDX(const String& name, const String& list, int8_t sel = 0, bool dis = false) {
        String s;
        s.reserve(250);
        if (sel < 0) sel = 0;
        s += F("<select name='");
        s += name;
        s += F("' id='");
        s += name;
        s += "' ";
        if (dis) s += F("disabled ");
        s += F("onchange='GP_click(this)'>\n");
        GP_parser p;
        int8_t idx = 0; 
        while (p.parse(list)) {
            s += F("<option value='");
            s += idx;
            s += F("'");
            if (p.count == sel) s += F(" selected");
            s += F(">");
            s += idx;
            s += F(". ");
            s += p.str;
            s += F("</option>\n");
            idx++;
        }
        s += F("</select>\n");
        SEND(s);
    }
    
    // ======================= ГРАФИКИ =======================
    template <int ax, int am>
    void PLOT(const String& id, const char** labels, int16_t vals[ax][am], int dec = 0, const String& furl = "/") {
        String s;
        s.reserve(500);
        s += F("<script type='text/javascript' src='");
        
        if (furl.length() > 1) {
            s += furl;
            s += F("PLOT/loader.js");
        }
        else s += F("https://www.gstatic.com/charts/loader.js");
        
        s += F("'></script>\n"
        "<script type='text/javascript'>\n"
        "google.charts.load('current', {'packages':['corechart']});\n"
        "google.charts.setOnLoadCallback(drawChart);\n"
        "function drawChart() {\n"
        "var data = google.visualization.arrayToDataTable([\n");
        SEND(s);
        s.clear();
        
        s += '[';
        for (int i = 0; i < ax+1; i++) {
            s += '\'';
            if (i) s += labels[i-1];
            s += "',";
        }
        s += "],\n";
        for (int j = 0; j < am; j++) {
            s += '[';
            for (int i = 0; i < ax+1; i++) {
                if (!i) s += '\'';
                if (!i) s += j;
                else {
                    if (dec) s += (float)vals[i-1][j] / dec;
                    else s += vals[i-1][j];
                }
                if (!i) s += '\'';
                s += ',';
            }
            s += F("],\n");
        }
        SEND(s);
        s.clear();
        
        s += F("]);var options = {pointSize:5,curveType:'function','chartArea':{'width':'90%','height':'90%'},\n"
        "backgroundColor:'none',hAxis:{title:''},vAxis:{title:''},\n"
        "legend: {position:'bottom'}};\n"
        "var chart = new google.visualization.LineChart(document.getElementById('");
        s += id;
        s += F("'));\n");
        s += F("chart.draw(data, options);}\n"
        "</script><div id='");
        s += id;
        s += F("' class='chartBlock'></div>\n");
        SEND(s);
    }
    
    template <int ax, int am>
    void PLOT_DARK(const String& id, const char** labels, int16_t vals[ax][am], int dec = 0, const String& furl = "/") {
        String s;
        s.reserve(500);
        s += F("<script type='text/javascript' src='");
        
        if (furl.length() > 1) {
            s += furl;
            s += F("PLOT_DARK/loader.js");
        }
        else s += F("https://www.gstatic.com/charts/loader.js");
        
        s += F("'></script>\n"
        "<script type='text/javascript'>\n"
        "google.charts.load('current', {'packages':['corechart']});\n"
        "google.charts.setOnLoadCallback(drawChart);\n"
        "function drawChart() {\n"
        "var data = google.visualization.arrayToDataTable([\n");
        SEND(s);
        s.clear();
        
        s += '[';
        for (int i = 0; i < ax+1; i++) {
            s += '\'';
            if (i) s += labels[i-1];
            s += "',";
        }
        s += "],\n";
        for (int j = 0; j < am; j++) {
            s += '[';
            for (int i = 0; i < ax+1; i++) {
                if (!i) s += '\'';
                if (!i) s += j;
                else {
                    if (dec) s += (float)vals[i-1][j] / dec;
                    else s += vals[i-1][j];
                }
                if (!i) s += '\'';
                s += ',';
            }
            s += F("],\n");
        }
        SEND(s);
        s.clear();
        
        s += F("]);var options = {pointSize:5,curveType:'function','chartArea':{'width':'90%','height':'90%'},\n"
        "backgroundColor:'none',hAxis:{title:'',titleTextStyle:{color:'#ddd'},textStyle:{color:'#bbb'}},\n"
        "vAxis:{title:'',titleTextStyle:{color:'#ddd'},gridlines:{color:'#777'},textStyle:{color:'#bbb'}},\n"
        "legend: {position:'bottom',textStyle:{color:'#eee'}}};\n"
        "var chart = new google.visualization.LineChart(document.getElementById('");
        s += id;
        s += F("'));\n");
        s += F("chart.draw(data, options);}\n");
        s += F("</script><div id='");
        s += id;
        s += F("' class='chartBlock'></div>\n");
        SEND(s);
    }
    
    void AJAX_PLOT(const String& name, int axes, int xamount = 20, int prd = 1000, const String& furl = "/") {
        String s;
        s.reserve(500);
        s += F("<script src='");
        
        if (furl.length() > 1) {
            s += furl;
            s += F("AJAX_PLOT/highcharts.js");
        }
        else s += F("https://code.highcharts.com/highcharts.js");
        
        s += F("'></script>\n<div id='");
        s += name;
        s += F("' class='chartBlock'></div><script>\n var ");
        s += name;
        s += F("=new Highcharts.Chart({\n"
        "chart:{borderRadius:10,renderTo:'");
        s += name;
        s += F("',style:{fontFamily:'sans-serif'}},\n"
        "title:{text:''},"
        "series:[");
        for (int i = 0; i < axes; i++) {
            s += F("{data:[]}");
            if (i != axes - 1) s += ',';
        }
        s += F("],\n"
        "xAxis:{type:'datetime',dateTimeLabelFormats:{second:'%H:%M:%S'}},\n"
        "yAxis:{title:{enabled:false}},"
        "credits:{enabled:false}});\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();var ch=");
        s += name;
        s += F("\n"
        "xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){\n"
        "var x=(new Date()).getTime();"
        "var arr=this.responseText.split(',');"
        "var move=(ch.series[0].data.length>");
        s += xamount;
        s += F(");\n"
        "for(let i=0;i<arr.length;i++)ch.series[i].addPoint([x,Number(arr[i])],true,move,true);\n"
        "}};xhttp.open('GET','/GP_update?");
        s += name;
        s += F("=',true);xhttp.send();},\n");
        s += prd;
        s += F(");</script>\n");
        SEND(s);
    }
    
    void AJAX_PLOT_DARK(const String& name, int axes, int xamount = 20, int prd = 1000, const String& furl = "/") {
        String s;
        s.reserve(500);
        
        if (furl.length() > 1) {
            s += F("<script src='");
            s += furl;
            s += F("AJAX_PLOT_DARK/highcharts.js'></script>\n");
            s += F("<script src='");
            s += furl;
            s += F("AJAX_PLOT_DARK/dark-unica.js'></script>\n");
        }
        else s += F("<script src='https://code.highcharts.com/highcharts.js'></script>\n"
        "<script src='https://code.highcharts.com/themes/dark-unica.js'></script>\n");
        
        s += F("<div id='");
        s += name;
        s += F("' class='chartBlock'></div><script>\n"
        "var ");
        s += name;
        s += F("=new Highcharts.Chart({\n"
        "chart:{borderRadius:10,renderTo:'");
        s += name;
        s += F("',style:{fontFamily:'sans-serif'}},\n"
        "title:{text:''},"
        "series:[");
        for (int i = 0; i < axes; i++) {
            s += F("{data:[]}");
            if (i != axes - 1) s += ',';
        }
        s += F("],\n"
        "xAxis:{type:'datetime',dateTimeLabelFormats:{second:'%H:%M:%S'}},\n"
        "yAxis:{title:{enabled:false}},"
        "credits:{enabled:false}});\n"
        "setInterval(function(){var xhttp=new XMLHttpRequest();var ch=");
        s += name;
        s += F("\n"
        "xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){\n"
        "var x=(new Date()).getTime();"
        "var arr=this.responseText.split(',');"
        "var move=(ch.series[0].data.length>");
        s += xamount;
        s += F(");\n"
        "for(let i=0;i<arr.length;i++)ch.series[i].addPoint([x,Number(arr[i])],true,move,true);\n"
        "}};xhttp.open('GET','/GP_update?");
        s += name;
        s += F("=',true);xhttp.send();},\n");
        s += prd;
        s += F(");</script>\n");
        SEND(s);
    }
    
    template <int ax, int am>
    void PLOT_STOCK(const String& id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec = 0, const String& furl = "/") {
        String s;
        s.reserve(500);
        s += F("<script src='");
        
        if (furl.length() > 1) {
            s += furl;
            s += F("PLOT_STOCK/highstock.js");
        }
        else s += F("https://code.highcharts.com/stock/highstock.js");
        
        s += F("'></script>\n <div class='chartBlock' id='");
        s += id;
        s += F("'></div>");
        s += F("<script>Highcharts.stockChart('");
        s += id;
        s += F("',{chart:{},\n"
        "rangeSelector:{buttons:[\n"
        "{count:1,type:'minute',text:'1M'},\n"
        "{count:1,type:'hour',text:'1H'},\n"
        "{count:1,type:'day',text:'1D'},\n"
        "{type:'all',text:'All'}],\n"
        "inputEnabled:false,selected:0},\n"
        "time:{useUTC:false},\n"
        "credits:{enabled:false},series:[\n");
        SEND(s);
        s.clear();
        for (int axs = 0; axs < ax; axs++) {
            s += F("{name:'");
            s += labels[axs];
            s += F("',data:[\n");
            for (int ams = 0; ams < am; ams++) {
                s += '[';
                s += times[ams];
                s += F("000");
                s += ',';
                if (dec) s += (float)vals[axs][ams] / dec;
                else s += vals[axs][ams];
                s += "],\n";
            }
            s += "]},\n";
            SEND(s);
            s.clear();
        }
        s += F("]});</script>\n");
        SEND(s);
    }
    
    template <int ax, int am>
    void PLOT_STOCK_DARK(const String& id, const char** labels, uint32_t* times, int16_t vals[ax][am], int dec = 0, const String& furl = "/") {
        String s;
        s.reserve(500);

        if (furl.length() > 1) {
            s += F("<script src='");
            s += furl;
            s += F("PLOT_STOCK_DARK/highstock.js'></script>\n");
            s += F("<script src='");
            s += furl;
            s += F("PLOT_STOCK_DARK/dark-unica.js'></script>\n");
        }
        else s += F("<script src='https://code.highcharts.com/stock/highstock.js'></script>\n"
        "<script src='https://code.highcharts.com/themes/dark-unica.js'></script>");
        
        s += F("<div class='chartBlock' id='");
        s += id;
        s += F("'></div>");
        s += F("<script>Highcharts.stockChart('");
        s += id;
        s += F("',{chart:{},\n"
        "rangeSelector:{buttons:[\n"
        "{count:1,type:'minute',text:'1M'},\n"
        "{count:1,type:'hour',text:'1H'},\n"
        "{count:1,type:'day',text:'1D'},\n"
        "{type:'all',text:'All'}],\n"
        "inputEnabled:false,selected:0},\n"
        "time:{useUTC:false},\n"
        "credits:{enabled:false},series:[\n");
        SEND(s);
        s.clear();
        for (int axs = 0; axs < ax; axs++) {
            s += F("{name:'");
            s += labels[axs];
            s += F("',data:[\n");
            for (int ams = 0; ams < am; ams++) {
                s += '[';
                s += times[ams];
                s += F("000");
                s += ',';
                if (dec) s += (float)vals[axs][ams] / dec;
                else s += vals[axs][ams];
                s += "],\n";
            }
            s += "]},\n";
            SEND(s);
            s.clear();
        }
        s += F("]});</script>\n");
        SEND(s);
    }
};