#include "data.h"

void setup() {
  startup();

  pos = analogRead(0) * 270.0 / 1023;
  pid.minOut = -1023;
  pid.maxOut = 1023;
  pid.kp = data.kp;
  pid.ki = data.ki;

  kalm.q = data.fil;
  kalm.errMeas = data.fil2;
  motor.setMinDuty(data.mdu);


  ntp.begin();
  ntp.setGMT(data.gmt);
  byte count = 0;
  while (!ntp.synced()) {
    ntp.updateNow();
    delay(1000);
    if (++count > 10) break;
  }
}

void speedLoop() {
  static uint32_t tmr;
  uint32_t dt = millis() - tmr;
  if (dt >= SPEED_LOOP_PRD) {
    tmr = millis();
    float rawPos = analogRead(0) * 270.0 / 1023;
    float medianPos = mfil.get(rawPos);
    pos += (medianPos - pos) * 0.5;

    static float prevPos;
    speed = (pos - prevPos) * 1000.0 / dt;
    prevPos = pos;
    speed = kalm.get(speed);

    pid.input = speed;

    if (spdOn) {
      pid.setpoint = manualSpeed;
      pid.compute(dt / 1000.0);
      motor.setSpeed(pid.output);
    } else if (moveFlag) {
      if (abs(target - pos) < data.deadz) {
        moveFlag = 0;
        motor.stop();
      } else {
        float minVel = data.minv;
        if (dir) {
          if (pos < s2) pid.setpoint = (pos - startPos) * tgVS;
          else pid.setpoint = curMaxV - (pos - s2) * tgVS;
          pid.setpoint = max(minVel, pid.setpoint);
        } else {
          if (pos > s2) pid.setpoint = (startPos - pos) * tgVS;
          else pid.setpoint = curMaxV - (s2 - pos) * tgVS;
          pid.setpoint = min(-minVel, -pid.setpoint);
        }
        pid.compute(dt / 1000.0);
        motor.setSpeed(pid.output);
      }
    }

    if (moveFlag || logSerial) {
      Serial.print(pid.output);
      Serial.print(',');
      Serial.print(speed);
      Serial.print(',');
      Serial.print(pid.setpoint);
      Serial.print(',');
      Serial.print(target);
      Serial.print(',');
      Serial.println(pos);
    }

    if ((motor.getSpeed() > 0 && rawPos > data.max) || (motor.getSpeed() < 0 && rawPos < data.min)) motor.stop();
  }
}

void move(int tar) {
  tar = constrain(tar, 0, 270);
  target = tar;
  moveFlag = 1;
  startPos = pos;             // текущий угол - начальная позиция
  dir = target > startPos;    // направление движения
  s2 = (target + pos) / 2;    // середина траектории (как сред. арифм)
  int err2 = abs(target - pos) / 2;       // длина половины траектории
  curMaxV = sqrt(2.0 * data.acc * err2);  // макс. скорость при заданном ускорением
  if (curMaxV > data.vel) curMaxV = data.vel;   // ограничиваем по макс. скорости
  tgVS = curMaxV / err2;      // tg угла наклона графика для дальнейших расчётов
  pid.init();
}

void moveToSun() {
  SunPosition sun(data.lat, data.lon, ntp.unix(), data.gmt);
  move(sun.angle90() + data.center);
}

void trackerTick() {
  if (!ntp.synced() || !data.track) return;
  static uint32_t tmr;
  if (millis() - tmr >= 45000) {
    tmr = millis();
    if (ntp.minute()) return;
    SunPosition sun(data.lat, data.lon, ntp.unix() + 30 * 60, data.gmt);
    if (ntp.hour() < sun.sunrise() / 60 || ntp.hour() > sun.sunset() / 60) return;
    move(sun.angle90() + data.center);
  }
}

void loop() {
  ntp.tick();
  eemem.tick();
  ui.tick();
  speedLoop();
  trackerTick();
}
