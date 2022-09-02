#include <Arduino.h>

struct PID {
  float setpoint = 0, output = 0;
  int16_t minOut = 0, maxOut = 255;
  float input = 0;
  float kp, ki, kd;

  float compute(float dts) {
    float err = setpoint - input;           // ошибка регулирования
    //float di = prInp - input;               // изменение входного сигнала за dt
    float di = err - prErr;
    prErr = err;
    if (rev) {
      err = -err;
      di = -di;
    }
    prInp = input;                          // запомнили предыдущее
    output = onrate ? 0 : (err * kp);       // пропорциональая составляющая
    output += -di * kd / dts;                // дифференциальная составляющая
    sum += err * ki * dts;                  // суммирование инт. суммы
    if (onrate) sum += di * kp;             // режим пропорционально скорости
    sum = constrain(sum, minOut, maxOut);   // ограничиваем инт. сумму
    output += sum;                          // интегральная составляющая
    output = constrain(output, minOut, maxOut);     // ограничиваем выход
    return output;
  }

  void init() {
    sum = 0;
    prInp = 0;
    prErr = 0;
    input = 0;
    setpoint = 0;
  }

  void direction(bool d) {  // reverse 1
    rev = d;
  }
  void mode(bool m) {   // on rate 1
    onrate = m;
  }

  bool rev = 0;
  float prInp = 0, prErr = 0, sum = 0;
  bool onrate = 0;
  uint32_t tmr = 0;
};
