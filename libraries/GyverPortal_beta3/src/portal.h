#pragma once

#include "utils.h"
#include "list.h"
#include "log.h"
#include "objects.h"

#ifdef ESP8266
#include <ESP8266WebServer.h>
extern ESP8266WebServer* _gp_s;
#else
#include <WebServer.h>
extern WebServer* _gp_s;
#endif
extern String* _gp_uri;
/*
#ifndef GP_UPLOAD_PAGE_ERROR
#define GP_UPLOAD_PAGE_ERROR F("<h2>Upload Error, <a href='/'>return back</a></h2>")
#endif
*/
#ifndef GP_OTA_URL
#define GP_OTA_URL F("/ota_update")
#endif

// ============================= CLASS ===========================
class GyverPortal {
public:
    // ======================= КОНСТРУКТОР =======================
    GyverPortal() {
        _uri.reserve(15);
    }

    ~GyverPortal() {
        stop();
    }
    
    
    // ========================= СИСТЕМА =========================
    // запустить портал. Можно передать имя MDNS (оставь пустым "" если MDNS не нужен) и порт
    void start(__attribute__((unused)) const String& mdns, uint16_t port = 80) {
        _active = 1;
        _dns = (WiFi.getMode() == WIFI_AP);
        
        #ifndef GP_NO_MDNS
        if (mdns.length()) {
            _mdnsF = 1;
            MDNS.begin(mdns.c_str());  
            MDNS.addService("http", "tcp", port);
        }
        #endif
        
        server.begin(port);
        
        #ifndef GP_NO_DNS
        if (_dns) dnsServer.start(53, "*", WiFi.softAPIP());
        #endif
        
        server.onNotFound([this]() {
            if (_auth && !server.authenticate(_login, _pass)) return server.requestAuthentication();
            _showPage = 0;
            _uri = server.uri();
            if (_uri.startsWith(F("/favicon.ico"))) {       // иконка
                server.send(200, "text/plane");             // игнорируем
                return;
            } else if (_uri.startsWith(F("/GP_click"))) {   // клик
                _clickF = 1;
                checkList();
                server.send(200, "text/plane");             // отвечаем ничем
            } else if (_uri.startsWith(F("/GP_update"))) {  // апдейт
                _updateF = 1;
                _answerF = 1;                               // запомнили что апдейт
            } else if (_uri.startsWith(F("/GP_log"))) {     // лог
                if (log.available()) server.send(200, "text/plane", log.read());
                else server.send(200);
                return;
            } else if (_uri.startsWith(F("/GP_upload"))) {
                server.send(200, "text/html", F("<meta http-equiv='refresh' content='0; url=/'/>"));
                return;
                #if defined(FS_H) && !defined(GP_NO_DOWNLOAD)
            } else if (!server.args() && _uri[0] == '/' && _uri.indexOf('.') > 0) {   // файл
                #ifdef GP_DOWNLOAD_PROVIDER
                if (_autoD) {                   // авто скачивание
                    File f = GP_DOWNLOAD_PROVIDER.open(_uri, "r");
                    if (f) {
                        server.streamFile(f, GPfileType(_uri));
                        f.close();
                    } else server.send(200);    // ошибка открытия
                } else _fileDF = 1;             // ручное скачивание (ниже)
                #else
                _fileDF = 1;
                #endif
                #endif
            } else {                            // форма или любая страница
                if (server.args()) {            // есть данные в запросе, это форма
                    _formF = 1;
                    checkList();
                }
                _showPage = 1;
                _reqF = 1;
            }

            if (_action || _actionR) {          // подключен новый обработчик действия
                if (_action) _action();         // вызов обычного
                if (_actionR) _actionR(*this);  // вызов с объектом
                if (_answerF) server.send(200); // юзер не ответил на update - отвечаем за него
                
                #if defined(FS_H) && !defined(GP_NO_DOWNLOAD)
                if (_fileDF) {
                    if (file) {
                        server.streamFile(file, GPfileType(_uri));
                        file.close();
                    } else server.send(200);
                }
                #endif
                
                if (_showPage) show();  // отправляем страницу
                _reqF = _fileDF = _answerF = _updateF = _clickF = _formF = 0;  // скидываем флаги
            } else {
                #ifdef ESP32
                if (_showPage) show();  // затычка для esp32, отправляем страницу даже без обработчика
                #endif
            }
        });
        
        #if defined(FS_H) && !defined(GP_NO_UPLOAD)
        server.on("/GP_upload", HTTP_POST, [this]() {
            //server.send(200);
            server.send(200, "text/html", F("<script>setInterval(function(){if (history.length > 0) window.history.back(); else window.location.href = \"/\";},500);</script>"));
        }, [this]() {
            if (_uploadM || _uploadMR) {
                if (_uploadM) _uploadM(server.upload());
                else _uploadMR(server.upload(), *this);
            } else if (_uploadA || _uploadAR) {
                HTTPUpload& upl = server.upload();
                switch (upl.status) {
                case UPLOAD_FILE_START:
                    if (!file) {
                        if (_uploadA) _uploadA(upl);
                        else _uploadAR(upl, *this);
                    }
                    break;
                case UPLOAD_FILE_WRITE:
                    if (file) file.write(upl.buf, upl.currentSize);
                    break;
                case UPLOAD_FILE_END:
                    if (file) file.close();
                    //show();
                    break;
                case UPLOAD_FILE_ABORTED:
                    if (file) file.close();
                    //server.send(200, "text/html", GP_UPLOAD_PAGE_ERROR);
                    break;
                }
            }
        });
        #endif
    }
    
    // запустить портал
    void start(__attribute__((unused)) uint8_t mode = WIFI_STA) {
        start("");
    }
    
    // остановить портал
    void stop() {
        _active = 0;
        #ifndef GP_NO_DNS
        if (_dns) dnsServer.stop();
        #endif
        server.stop();
    }
    
    // проверить, запущен ли портал
    bool state() {
        return _active;
    }
    
    
    // ================== IP REMOTE CLIENT ==================
    // вернёт IP адрес клиента
	IPAddress clientIP() {
		return server.client().remoteIP();
	}
    
    // вернёт true, если IP адрес клиента принадлежит указанной сети
    bool clientFromNet(IPAddress NetIP, int8_t mask) {
		IPAddress RclIP = clientIP();                   // адрес клинета
		uint8_t netmask[4] = {255, 255, 255, 255};      // Делаем маску /32
		for (int r = 0; r < (32 - mask); r++) bitClear(netmask[3 - r / 8],r % 8);    // Конвертируем формат маски
		for (int r = 0; r < 4; r++) {       // Вычисляем адреса сетей относительно маски
			RclIP[r] &= netmask[r];         // сперва клиента
			NetIP[r] &= netmask[r];         // потом адрес сети IP переданного в функцию
		}
		return (NetIP == RclIP);            // Если адреса сетей совпадают, значит они из одной сети
	}
    
    
    // ========================= AUTH =========================
    // включить авторизацию по логину-паролю
    void enableAuth(const char* login, const char* pass) {
        _auth = 1;
        _login = login;
        _pass = pass;
    }
    
    // отключить авторизацию
    void disableAuth() {
        _auth = 0;
    }
    
    
    // ========================== OTA ==========================
    // включить OTA обновление
    void enableOTA() {
        #ifndef GP_NO_OTA
        httpUpdater.setup(&server, GP_OTA_URL);
        #endif
    }
    
    // включить OTA обновление с авторизацией
    void enableOTA(__attribute__((unused)) const String& login, __attribute__((unused)) const String& pass) {
        #ifndef GP_NO_OTA
        httpUpdater.setup(&server, GP_OTA_URL, login, pass);
        #endif
    }
    
    
    // ======================= АТТАЧИ =======================
    // подключить функцию-обработчик действия
    void attach(void (*handler)()) {
        _action = *handler;
    }
    void attach(void (*handler)(GyverPortal& p)) {
        _actionR = *handler;
    }
    
    // отключить функцию-обработчик загрузки файла
    void detach() {
        _action = nullptr;
        _actionR = nullptr;
    }
    
    // подключить функцию-билдер страницы
    void attachBuild(void (*handler)()) {
        _build = *handler;
    }
    void attachBuild(void (*handler)(GyverPortal& p)) {
        _buildR = *handler;
    }
    
    // отключить функцию-билдер страницы
    void detachBuild() {
        _build = nullptr;
        _buildR = nullptr;
    }
    
    // подключить функцию-обработчик загрузки файла
    void attachUploadManual(void (*handler)(HTTPUpload& upl)) {
        _uploadM = *handler;
    }
    void attachUploadManual(void (*handler)(HTTPUpload& upl, GyverPortal& p)) {
        _uploadMR = *handler;
    }
    
    // отключить функцию-обработчик загрузки файла
    void detachUploadManual() {
        _uploadM = nullptr;
        _uploadMR = nullptr;
    }
    
    // подключить функцию-обработчик загрузки файла
    void attachUpload(void (*handler)(HTTPUpload& upl)) {
        _uploadA = *handler;
    }
    void attachUpload(void (*handler)(HTTPUpload& upl, GyverPortal& p)) {
        _uploadAR = *handler;
    }
    
    // отключить функцию-обработчик загрузки файла
    void detachUpload() {
        _uploadA = nullptr;
        _uploadAR = nullptr;
    }
    
    // ======================= ТИКЕР =======================
    // тикер портала. Вернёт true, если портал запущен
    bool tick() {
        if (!_active) return 0;
        
        // deprecated
        #ifdef ESP8266
        if (!_action && !_actionR) {
            if (_showPage) {_showPage = 0; show();}
            if (_updateF) server.send(200, "text/plane");
            _updateF = _clickF = _formF = 0;
        }
        #endif
        // deprecated
        
        #ifndef GP_NO_DNS
        if (_dns) dnsServer.processNextRequest();
        #endif
        #if !defined(GP_NO_MDNS) && defined(ESP8266)
        if (_mdnsF) MDNS.update();
        #endif
        server.handleClient();
        
        // deprecated
        #ifdef ESP8266
        if (!_action && !_actionR) {
            if (_formF && _form) _form(this);
            if (_clickF && _click) _click(this);
            if (_updateF && _update) _update(this);
        }
        #endif
        // deprecated
        
        yield();
        return 1;
    }
    
    
    // ================== DOWNLOAD FILE ==================
    // вернёт true, если был запрос на загрузку файла
    bool download() {
        return _fileDF;
    }
    
    // автоматическое скачивание файла по uri (по умолч. выкл, false). ТРЕБУЕТ ДЕФАЙНА GP_DOWNLOAD_PROVIDER
    void autoDownload(bool mode) {
        _autoD = mode;
    }
    
    
    // ======================= REQUEST =======================
    // вернёт true, если был http запрос
    bool request() {
        return _reqF;
    }
    
    // вернёт true, если был запрос
    bool request(const String& name) {
        return _reqF ? _uri.equals(name) : 0;
    }

    
    // ======================= FORM =======================
    // вернёт true, если было нажатие на любой submit
    bool form() {
        return _formF;
    }
    
    // вернёт true, если был submit с указанной формы
    bool form(const String& name) {
        return _formF ? _uri.equals(name) : 0;
    }
    
    // вернёт имя теукщей submit формы
    String& formName() {
        return _uri;
    }
    
    // вернёт true, если был submit с форм, имя которых начинмется с name
    bool formSub(const String& name) {
        return _formF ? _uri.startsWith(name) : 0;
    }
    

    // ======================= CLICK =======================
    // вернёт true, если был клик по (кнопка, чекбокс, свитч, слайдер, селектор)
    bool click() {
        return _clickF;
    }
    
    // вернёт true, если был клик по указанному элементу (кнопка, чекбокс, свитч, слайдер, селектор)
    bool click(const String& name) {
        return _clickF ? (server.argName(0).equals(name) && server.args() == 1) : 0;
    }
    
    // вернёт имя теукщего кликнутого компонента
    String clickName() {
        return String(server.argName(0));
    }
    
    // вернёт true, если кнопка была нажата
    bool clickDown(const String& name) {
        return _clickF ? (server.argName(0).equals(name) && server.args() == 2 && server.arg(1)[0] == '0') : 0;
    }
    
    // вернёт true, если кнопка была отпущена
    bool clickUp(const String& name) {
        return _clickF ? (server.argName(0).equals(name) && server.args() == 2 && server.arg(1)[0] == '1') : 0;
    }

    
    // ===================== CLICK OBJ ======================
    bool clickDown(GP_BUTTON& btn) {
        return clickDown(btn.name);
    }
    
    bool clickUp(GP_BUTTON& btn) {
        return clickUp(btn.name);
    }
    
    bool clickDown(GP_BUTTON_MINI& btn) {
        return clickDown(btn.name);
    }
    
    bool clickUp(GP_BUTTON_MINI& btn) {
        return clickUp(btn.name);
    }
    
    bool click(GP_BUTTON& btn) {
        return click(btn.name);
    }
    bool click(GP_BUTTON_MINI& btn) {
        return click(btn.name);
    }
    
    bool click(GP_NUMBER& num) {
        return click(num.name);
    }
    bool click(GP_NUMBER_F& num) {
        return click(num.name);
    }
    
    bool click(GP_TEXT& txt) {
        return click(txt.name);
    }
    bool click(GP_PASS& pas) {
        return click(pas.name);
    }
    
    bool click(GP_AREA& ar) {
        return click(ar.name);
    }
    
    bool click(GP_CHECK& ch) {
        return click(ch.name);
    }
    bool click(GP_SWITCH& sw) {
        return click(sw.name);
    }
    
    bool click(GP_DATE& d) {
        return click(d.name);
    }
    bool click(GP_TIME& t) {
        return click(t.name);
    }
    bool click(GP_COLOR& c) {
        return click(c.name);
    }
    
    bool click(GP_SPINNER& s) {
        return click(s.name);
    }
    bool click(GP_SLIDER& s) {
        return click(s.name);
    }
    
    bool click(GP_SELECT& s) {
        return click(s.name);
    }
    bool click(GP_SELECT_IDX& s) {
        return click(s.name);
    }
    
    
    // ======================= UPDATE =======================
    // вернёт true, если было обновление
    bool update() {
        return _updateF;
    }
    
    // вернёт true, если было update с указанного компонента
    bool update(const String& name) {
        return _updateF ? server.argName(0).equals(name) : 0;
    }
    
    // вернёт имя обновлённого компонента
    String updateName() {
        return String(server.argName(0));
    }
    
    
    // ===================== UPDATE OBJ =====================
    bool update(GP_TITLE& title) {
        return update(title.name);
    }
    bool update(GP_LABEL& label) {
        return update(label.name);
    }
    bool update(GP_LABEL_BLOCK& label) {
        return update(label.name);
    }
    
    bool update(GP_LED& led) {
        return update(led.name);
    }
    bool update(GP_LED_RED& led) {
        return update(led.name);
    }
    bool update(GP_LED_GREEN& led) {
        return update(led.name);
    }
    
    bool update(GP_NUMBER& num) {
        return update(num.name);
    }
    bool update(GP_NUMBER_F& num) {
        return update(num.name);
    }
    
    bool update(GP_TEXT& txt) {
        return update(txt.name);
    }
    bool update(GP_PASS& pas) {
        return update(pas.name);
    }
    
    bool update(GP_AREA& ar) {
        return update(ar.name);
    }
    
    bool update(GP_CHECK& ch) {
        return update(ch.name);
    }
    bool update(GP_SWITCH& sw) {
        return update(sw.name);
    }
    
    bool update(GP_DATE& d) {
        return update(d.name);
    }
    bool update(GP_TIME& t) {
        return update(t.name);
    }
    bool update(GP_COLOR& c) {
        return update(c.name);
    }
    
    bool update(GP_SPINNER& s) {
        return update(s.name);
    }
    bool update(GP_SLIDER& s) {
        return update(s.name);
    }
    
    // ======================= ANSWER =======================
    // отправить ответ на обновление
    bool answer(const String& s) {
        _updateF = 0;
        _answerF = 0;
        server.send(200, "text/plane", s);
        return 1;
    }
    bool answer(int v) {
        return answer(String(v));
    }
    bool answer(float v, uint8_t dec) {
        return answer(String(v, (uint16_t)dec));
    }
    bool answer(int* v, int am, int dec = 0) {
        String s;
        s.reserve(am * 4);
        for (int i = 0; i < am; i++) {
            if (dec) s += (float)v[i] / dec;
            else s += v[i];
            if (i != am - 1) s += ',';
        }
        return answer(s);
    }
    
    bool answer(GPcolor& col) {
        return answer(col.encode());
    }
    bool answer(GPdate& date) {
        return answer(date.encode());
    }
    bool answer(GPtime& time) {
        return answer(time.encode());
    }
    
    // ==================== UPDATE AUTO =====================
    // автоматическое обновление. Отправит значение из указанной переменной
    // Вернёт true в момент обновления
    bool updateString(const String& n, String& f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateInt(const String& n, int& f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateFloat(const String& n, float& f, int dec = 2) {
        return update(n) ? (answer(f, dec), 1) : 0;
    }
    bool updateCheck(const String& n, bool& f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateDate(const String& n, GPdate& f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateTime(const String& n, GPtime& f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateColor(const String& n, GPcolor& f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateSelected(const String& n, int8_t& f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    bool updateSelectedIdx(const String& n, int8_t& f) {
        return update(n) ? (answer(f), 1) : 0;
    }
    
    
    // ===================== ANSWER OBJ =====================
    bool answer(GP_TITLE& title) {
        return answer(title.text);
    }
    bool answer(GP_LABEL& label) {
        return answer(label.text);
    }
    bool answer(GP_LABEL_BLOCK& label) {
        return answer(label.text);
    }
    
    bool answer(GP_LED& led) {
        return answer(led.state);
    }
    bool answer(GP_LED_RED& led) {
        return answer(led.state);
    }
    bool answer(GP_LED_GREEN& led) {
        return answer(led.state);
    }
    
    bool answer(GP_NUMBER& num) {
        return answer(num.value);
    }
    bool answer(GP_NUMBER_F& num) {
        return answer(num.value, num.decimals);
    }
    
    bool answer(GP_TEXT& txt) {
        return answer(txt.text);
    }
    bool answer(GP_PASS& pas) {
        return answer(pas.text);
    }
    
    bool answer(GP_AREA& ar) {
        return answer(ar.text);
    }
    
    bool answer(GP_CHECK& ch) {
        return answer(ch.state);
    }
    bool answer(GP_SWITCH& sw) {
        return answer(sw.state);
    }
    
    bool answer(GP_DATE& d) {
        return answer(d.date);
    }
    bool answer(GP_TIME& t) {
        return answer(t.time);
    }
    bool answer(GP_COLOR& c) {
        return answer(c.color);
    }
    
    bool answer(GP_SPINNER& s) {
        return answer(s.value, s.decimals);
    }
    bool answer(GP_SLIDER& s) {
        return answer(s.value, s.decimals);
    }
    
    
    // ================== UPDATE AUTO OBJ ===================
    bool updateObj(GP_TITLE& title) {
        return (update(title) ? answer(title) : 0);
    }
    bool updateObj(GP_LABEL& label) {
        return (update(label) ? answer(label) : 0);
    }
    bool updateObj(GP_LABEL_BLOCK& label) {
        return (update(label) ? answer(label) : 0);
    }
    
    bool updateObj(GP_LED& led) {
        return (update(led) ? answer(led) : 0);
    }
    bool updateObj(GP_LED_RED& led) {
        return (update(led) ? answer(led) : 0);
    }
    bool updateObj(GP_LED_GREEN& led) {
        return (update(led) ? answer(led) : 0);
    }
    
    bool updateObj(GP_NUMBER& num) {
        return (update(num) ? answer(num) : 0);
    }
    bool updateObj(GP_NUMBER_F& num) {
        return (update(num) ? answer(num) : 0);
    }
    
    bool updateObj(GP_TEXT& txt) {
        return (update(txt) ? answer(txt) : 0);
    }
    bool updateObj(GP_PASS& pas) {
        return (update(pas) ? answer(pas) : 0);
    }
    
    bool updateObj(GP_AREA& ar) {
        return (update(ar) ? answer(ar) : 0);
    }
    
    bool updateObj(GP_CHECK& ch) {
        return (update(ch) ? answer(ch) : 0);
    }
    bool updateObj(GP_SWITCH& sw) {
        return (update(sw) ? answer(sw) : 0);
    }
    
    bool updateObj(GP_DATE& d) {
        return (update(d) ? answer(d) : 0);
    }
    bool updateObj(GP_TIME& t) {
        return (update(t) ? answer(t) : 0);
    }
    bool updateObj(GP_COLOR& c) {
        return (update(c) ? answer(c) : 0);
    }
    
    bool updateObj(GP_SPINNER& s) {
        return (update(s) ? answer(s) : 0);
    }
    bool updateObj(GP_SLIDER& s) {
        return (update(s) ? answer(s) : 0);
    }
    
    
    // ======================= MISC =======================
    // вернёт true, если открыта главная страница (/)
    bool root() {
        return (_uri[0] == '/' && _uri[1] == '\0');
    }
    // адрес запроса
    String& uri() {
        return _uri;
    }
    
    // true если uri совпадает
    bool uri(const String& s) {
        return _uri.equals(s);
    }
    
    
    // ======================= ПАРСЕРЫ =======================
    // ОПАСНЫЕ ФУНКЦИИ (не проверяют есть ли запрос). Конвертируют и возвращают значение
    
    // получить String строку с компонента
    String getString(const String& n) {
        return String(server.arg(n));
    }

    // получить число с компонента
    int getInt(const String& n) {
        return server.arg(n).toInt();
    }
    
    // получить float с компонента
    float getFloat(const String& n) {
        return server.arg(n).toFloat();
    }

    // получить состояние чекбокса
    bool getCheck(const String& n) {
        return (server.arg(n)[0] == '1' || server.arg(n)[0] == 'o');
    }

    // получить дату с компонента
    GPdate getDate(const String& n) {
        return GPdate(server.arg(n));
    }

    // получить время с компонента
    GPtime getTime(const String& n) {
        return GPtime(server.arg(n));
    }

    // получить цвет с компонента
    GPcolor getColor(const String& n) {
        return GPcolor(server.arg(n));
    }
    
    // получить номер выбранного пункта в дроплисте
    int8_t getSelected(const String& n, const String& list) {
        return GPinList(server.arg(n), list);
    }
    
    // получить номер выбранного пункта в дроплисте
    int8_t getSelectedIdx(const String& n) {
        return server.arg(n).toInt();
    }
    
    // ===================== COPY-ПАРСЕРЫ =====================
    // БЕЗОПАСНЫЕ ФУНКЦИИ (проверяют запрос). Копируют данные из запроса в переменную
    bool copyStr(const String& n, char* t) {
        return server.hasArg(n) ? (strcpy(t, server.arg(n).c_str()), 1) : 0;
    }
    bool copyString(const String& n, String& t) {
        return server.hasArg(n) ? (t = server.arg(n), 1) : 0;
    }
    bool copyInt(const String& n, int& t) {
        return server.hasArg(n) ? (t = getInt(n), 1) : 0;
    }
    bool copyFloat(const String& n, float& t) {
        return server.hasArg(n) ? (t = getFloat(n), 1) : 0;
    }
    bool copyCheck(const String& n, bool& t) {
        return server.hasArg(n) ? (t = getCheck(n), 1) : 0;
    }
    bool copyDate(const String& n, GPdate& t) {
        return server.hasArg(n) ? (t = getDate(n), 1) : 0;
    }
    bool copyTime(const String& n, GPtime& t) {
        return server.hasArg(n) ? (t = getTime(n), 1) : 0;
    }
    bool copyColor(const String& n, GPcolor& t) {
        return server.hasArg(n) ? (t = getColor(n), 1) : 0;
    }
    bool copySelected(const String& n, const String& list, int8_t& t) {
        return server.hasArg(n) ? (t = getSelected(n, list), 1) : 0;
    }
    bool copySelectedIdx(const String& n, int8_t& t) {
        return server.hasArg(n) ? (t = getInt(n), 1) : 0;
    }
    
    
    // ===================== COPY OBJ =====================
    bool copyObj(GP_NUMBER& num) {
        return copyInt(num.name, num.value);
    }
    bool copyObj(GP_NUMBER_F& num) {
        return copyFloat(num.name, num.value);
    }
    
    bool copyObj(GP_TEXT& txt) {
        return copyString(txt.name, txt.text);
    }
    bool copyObj(GP_PASS& pas) {
        return copyString(pas.name, pas.text);
    }
    
    bool copyObj(GP_AREA& ar) {
        return copyString(ar.name, ar.text);
    }
    
    bool copyObj(GP_CHECK& ch) {
        return copyCheck(ch.name, ch.state);
    }
    bool copyObj(GP_SWITCH& sw) {
        return copyCheck(sw.name, sw.state);
    }
    
    bool copyObj(GP_DATE& d) {
        return copyDate(d.name, d.date);
    }
    bool copyObj(GP_TIME& t) {
        return copyTime(t.name, t.time);
    }
    bool copyObj(GP_COLOR& c) {
        return copyColor(c.name, c.color);
    }
    
    bool copyObj(GP_SPINNER& s) {
        return copyFloat(s.name, s.value);
    }
    bool copyObj(GP_SLIDER& s) {
        return copyFloat(s.name, s.value);
    }
    
    bool copyObj(GP_SELECT& s) {
        return copySelected(s.name, s.list, s.selected);
    }
    bool copyObj(GP_SELECT_IDX& s) {
        return copySelectedIdx(s.name, s.selected);
    }
    
    
    // ======================= ПРОЧЕЕ =======================    
    // вызвать конструктор и показать страницу
    void show() {
        if (!_build && !_buildR) server.send(200);
        else {
            _gp_uri = &_uri;
            server.sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
            server.sendHeader(F("Pragma"), F("no-cache"));
            server.sendHeader(F("Expires"), F("-1"));
            server.setContentLength(CONTENT_LENGTH_UNKNOWN);
            server.send(200, "text/html", "");
            _gp_s = &server;
            if (_build) _build();
            else _buildR(*this);
            server.sendContent("");
            server.client().stop();
        }
    }

    GPlist list;
    GPlog log;

#ifdef ESP8266
    ESP8266WebServer server;
#else
    WebServer server;
#endif

#ifndef GP_NO_OTA
#ifdef ESP8266
    ESP8266HTTPUpdateServer httpUpdater;
#else
    HTTPUpdateServer httpUpdater;
#endif
#endif

#if defined(FS_H) && !defined(GP_NO_DOWNLOAD)
    File file;
    
    // установить файл для загрузки и скачивания
    void setFile(File f) {
        file = f;
    }
#endif
    
    // ======================= DEPRECATED =======================
    void attachClick(void (*handler)(GyverPortal* p)) {_click = *handler;}
    void attachForm(void (*handler)(GyverPortal* p)) {_form = *handler;}
    void attachUpdate(void (*handler)(GyverPortal* p)) {_update = *handler;}
    int clickValue() {return server.arg(0).toInt();}
    String clickText() {return String(server.arg(0));}
    
    // ======================= PRIVATE =======================
private:
    void checkList() {
        if (!list.idx) return;
        for (int i = 0; i < list.idx; i++) {
            if (list.skips[i] || form(list.forms[i])) {
                switch (list.types[i]) {
                case T_CSTR:    copyStr(list.names[i], (char*)list.vals[i]);        break;
                case T_STRING:  *(String*)list.vals[i] = getString(list.names[i]);  break;
                case T_TIME:    *(GPtime*)list.vals[i] = getTime(list.names[i]);    break;
                case T_DATE:    *(GPdate*)list.vals[i] = getDate(list.names[i]);    break;
                case T_CHECK:   *(bool*)list.vals[i] = getCheck(list.names[i]);     break;
                case T_BYTE:    *(int8_t*)list.vals[i] = (int8_t)getInt(list.names[i]); break;
                case T_INT:     *(long*)list.vals[i] = getInt(list.names[i]);       break;
                case T_FLOAT:   *(float*)list.vals[i] = getFloat(list.names[i]);    break;
                case T_COLOR:   *(GPcolor*)list.vals[i] = getColor(list.names[i]);  break;
                }
            }
        }
    }
    
    String _uri;
    
    bool _mdnsF = false;
    bool _dns = false;
    bool _active = false;
    bool _showPage = 0;
    
    bool _auth = false;
    const char* _login;
    const char* _pass;
    
    bool _fileDF = 0;
    bool _formF = 0;
    bool _updateF = 0;
    bool _clickF = 0;
    bool _answerF = 0;
    bool _reqF = 0;
    bool _autoD = 0;
    
    void (*_build)() = nullptr;
    void (*_buildR)(GyverPortal& p) = nullptr;
    void (*_action)() = nullptr;
    void (*_actionR)(GyverPortal& p) = nullptr;
    void (*_uploadM)(HTTPUpload& upl) = nullptr;
    void (*_uploadMR)(HTTPUpload& upl, GyverPortal& p) = nullptr;
    void (*_uploadA)(HTTPUpload& upl) = nullptr;
    void (*_uploadAR)(HTTPUpload& upl, GyverPortal& p) = nullptr;
    
#ifndef GP_NO_DNS
    DNSServer dnsServer;
#endif

    // ======================= DEPRECATED =======================
    void (*_click)(GyverPortal* p) = nullptr;
    void (*_form)(GyverPortal* p) = nullptr;
    void (*_update)(GyverPortal* p) = nullptr;
};