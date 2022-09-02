struct SimpleKalman {
  float errMeas;  // примерный шум измерений
  float q;        // скорость изменения значений 0.001-1, варьировать самому

  SimpleKalman(float err = 1, float nq = 0.5) {
    errEst = errMeas = err;
    q = nq;
  }
  float get(float val) {
    float gain = (float)errEst / (errEst + errMeas);
    float curEst = prevEst + (float)gain * (val - prevEst);
    errEst = (1.0 - gain) * errEst + fabs(prevEst - curEst) * q;
    prevEst = curEst;
    return curEst;
  }
  float errEst, prevEst;
};

template <typename T>
struct MedianF {
  T get(T val) {
    buf[count] = val;
    if (++count >= 3) count = 0;
    return (max(buf[0], buf[1]) == max(buf[1], buf[2])) ? max(buf[0], buf[2]) : max(buf[1], min(buf[0], buf[2]));
  }
  T buf[3] = {0, 0, 0};
  byte count = 0;
};
