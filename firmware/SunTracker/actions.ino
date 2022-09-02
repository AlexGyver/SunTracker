void action() {
  if (ui.update()) {
    ui.updateFloat("curpos", pos);
    ui.updateFloat("tarpos", target);
  }

  // ================== CONFIG ==================
  if (ui.form()) {
    if (ui.form("/cfg")) {
      eemem.update();
      ui.copyStr("ssid", data.ssid);
      ui.copyStr("pass", data.pass);
      ui.copyInt("gmt", data.gmt);
      ui.copyFloat("lat", data.lat);
      ui.copyFloat("lon", data.lon);

      ui.copyInt("centr", data.center);
      ui.copyInt("min", data.min);
      ui.copyInt("max", data.max);
    }
  }

  if (ui.click()) {
    eemem.update();
    // ================== MOTOR ==================
    if (ui.copyInt("mdu", data.mdu)) motor.setMinDuty(data.mdu);
    ui.copyInt("vel", data.vel);
    ui.copyInt("acc", data.acc);
    ui.copyInt("minv", data.minv);
    
    ui.copyInt("spd", duty);
    if (ui.clickDown("frw")) motor.setSpeed(duty), logSerial = 1;
    if (ui.clickDown("bkw")) motor.setSpeed(-duty), logSerial = 1;
    if (ui.clickUp("frw") || ui.clickUp("bkw")) motor.stop(), logSerial = 0;

    if (ui.copyFloat("kp", data.kp)) pid.kp = data.kp;
    if (ui.copyFloat("ki", data.ki)) pid.ki = data.ki;
    if (ui.copyFloat("fil", data.fil)) kalm.q = data.fil;
    if (ui.copyFloat("fil2", data.fil2)) kalm.errMeas = data.fil2;

    if (ui.copyCheck("spdOn", spdOn)) {
      logSerial = spdOn;
      pid.init();
      manualSpeed = 0;
      motor.stop();
    }
    
    ui.copyInt("spdPid", spdPid);
    if (ui.clickDown("frwPid")) manualSpeed = spdPid;
    if (ui.clickDown("bkwPid")) manualSpeed = -spdPid;
    if (ui.clickUp("frwPid") || ui.clickUp("bkwPid")) {
      manualSpeed = 0;
      pid.init();
    }

    if (ui.copyCheck("planOn", planOn)) {
      pid.init();
      logSerial = planOn;
      if (!planOn) moveFlag = 0;
      motor.stop();
    }
    if (ui.click("tar") && planOn) move(ui.getInt("tar"));
    ui.copyInt("deadz", data.deadz);

    // ================== HOME ==================
    ui.copyCheck("track", data.track);
    if (ui.click("force")) moveToSun();
    if (ui.click("brake")) motor.stop(), moveFlag = 0;
  }
}
