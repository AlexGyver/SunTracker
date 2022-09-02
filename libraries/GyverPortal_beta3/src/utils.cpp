#include "utils.h"
#define GP_PGM(name, val) static const char name[] PROGMEM = val

// ==================== COLORS ======================
const uint32_t _GPcolors[] PROGMEM = {
    0xbf1e1e, 0xe11414, 0xbb32aa, 0xdb13c1, 0x802ecb, 0xa60cf5, 0x3b3ebd, 0x353aff,
    0x1f8fa9, 0x06b3db, 0x37a93c, 0x25d52c, 0xb4b700, 0xb37f0d, 0xff4500, 0x5e5e5e,
};

String GPgetStyle(GPstyle s) {
    GPcolor col(pgm_read_dword(&_GPcolors[s]));
    return col.encode();
}

// ================== FILE TYPES ====================
GP_PGM(_gp_types, "image,text,audio,video,application");
GP_PGM(_gp_img, "ico,gif,bmp,jpeg,jpg,png,tif,tiff,svg,webp");
GP_PGM(_gp_txt, "htm,html,css,csv");
GP_PGM(_gp_au, "aac,mp3,wav,weba");
GP_PGM(_gp_vid, "avi,mpeg,mp4,webm,3gp");
GP_PGM(_gp_app, "js,bin,gz,json,pdf,php,tar,xml");
GP_PGM(_gp__img, "x-icon,gif,bmp,jpeg,jpeg,png,tiff,tiff,svg+xml,webp");
GP_PGM(_gp__txt, "html,html,css,csv");
GP_PGM(_gp__au, "aac,mpeg,wav,webm");
GP_PGM(_gp__vid, "x-msvideo,mpeg,mp4,webm,3gpp");
GP_PGM(_gp__app, "javascript,octet-stream,gzip,json,pdf,x-httpd-php,x-tar,xml");
const char* const _gp_ext[] PROGMEM = {_gp_img, _gp_txt, _gp_au, _gp_vid, _gp_app};
const char* const _gp_val[] PROGMEM = {_gp__img, _gp__txt, _gp__au, _gp__vid, _gp__app};

String GPfileType(const String& uri) {
    int div = uri.indexOf('.');
    if (div >= 0) {
        String ext = uri.substring(div + 1, uri.length());
        for (int i = 0; i < 5; i++) {
            int pos = GPinList(ext, FPSTR(pgm_read_dword(_gp_ext + i)));
            if (pos >= 0) {
                ext = GPlistIdx(i, FPSTR(_gp_types));
                ext += '/';
                ext += GPlistIdx(pos, FPSTR(pgm_read_dword(_gp_val + i)));
                return ext;
            }
        }
    }
    return String(F("text/plain"));
}

// ================== ALIGNS ====================
GP_PGM(_gp_al0, "center");
GP_PGM(_gp_al1, "flex-start");
GP_PGM(_gp_al2, "flex-end");
GP_PGM(_gp_al3, "space-between");
const char* const _gp_al[] PROGMEM = {_gp_al0, _gp_al1, _gp_al2, _gp_al3};

PGM_P GPgetAlign(GPalign a) {
    return _gp_al[a];
}

// ===================== DATE-TIME UNIX =====================
uint32_t GPunix(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn, uint8_t s, int8_t gmt) {
    int8_t my = (m >= 3) ? 1 : 0;
    y += my - 1970;
    uint16_t dm = 0;
    for (int i = 0; i < m - 1; i++) dm += (i<7)?((i==1)?28:((i&1)?30:31)):((i&1)?31:30);
    return (((d-1+dm+((y+1)>>2)-((y+69)/100)+((y+369)/100/4)+365*(y-my))*24ul+h-gmt)*60ul+mn)*60ul+s;
}
uint32_t GPunix(GPdate d, GPtime t, int8_t gmt) {
    return GPunix(d.year, d.month, d.day, t.hour, t.minute, t.second, gmt);
}

void GPaddInt(int16_t val, int16_t* arr, uint8_t am) {
    for (int i = 0; i < am - 1; i++) arr[i] = arr[i + 1];
    arr[am - 1] = val;
}
void GPaddUnix(uint32_t val, uint32_t* arr, uint8_t am) {
    for (int i = 0; i < am - 1; i++) arr[i] = arr[i + 1];
    arr[am - 1] = val;
}
void GPaddUnixS(int16_t val, uint32_t* arr, uint8_t am) {
    for (int i = 0; i < am - 1; i++) arr[i] = arr[i + 1];
    arr[am - 1] += val;
}

// ================ STRING UTILS ==================
int GPinList(const String& s, const String& li) {
    int l = s.length();
    if (l > (int)li.length()) return -1;
    if (s == li) return 0;
    int p = 0, t = 0;
    while (1) {
        t = li.indexOf(s, p);
        if (t < 0) return -1;
        if (!t && li[t+l] == ',') break;
        if (t && li[t-1] == ',' && (li[t+l] == ',' || !li[t+l])) break;
        p = t + 1;
    }
    int cnt = 0;
    for (int i = 0; i < t; i++) if (li[i] == ',') cnt++;
    return cnt;
}
String GPlistIdx(int idx, const String& li) {
    int cnt = 0, p = 0, i = 0;
    while (1) {
        if (li[i] == ',' || !li[i]) {
            if (cnt == idx) return li.substring(p, i);
            if (!li[i]) return String();
            cnt++;
            p = i + 1;
        }
        i++;
    }
}