#define SPEED_LOOP_PRD 15
#define SAFE_ZONE 20

#include <SunPosition.h>

#include <GyverNTP.h>
GyverNTP ntp(3);

#include <GyverPortal.h>
GyverPortal ui;

#include <GyverMotor2.h>
//GMotor2<DRIVER3WIRE, 10> motor(D5, D6, D7);
GMotor2<DRIVER2WIRE_PWM, 10> motor(D5, D6);

#include "filters.h"
MedianF<float> mfil;
SimpleKalman kalm;

#include "PID.h"
PID pid;

float target;
float speed = 0;
float pos;

int s2;
float curMaxV;
int startPos;
float tgVS;
bool moveFlag = 0;
bool dir = 0;
int manualSpeed = 0;
bool logSerial = 0;
bool spdOn = 0;
bool planOn = 0;
int spdPid = 0;
int duty = 0;

struct Data {
  char ssid[20] = "";
  char pass[20] = "";
  int gmt = 3;
  float lat = NAN;
  float lon = NAN;
  int center = 270/2;
  int min = 0;
  int max = 270;
  float fil = 0.25;
  float fil2 = 50;
  int deadz = 2;
  int acc = 10;
  int vel = 10;
  int mdu = 0;
  int minv = 1;
  float kp = 0;
  float ki = 0;
  bool track = 0;
};

Data data;

#include <EEManager.h>
EEManager eemem(data);
