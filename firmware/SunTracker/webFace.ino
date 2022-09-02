void build() {
  GP.BUILD_BEGIN(700);
  GP.THEME(GP_DARK);

  GP.NAV_TABS_LINKS("/,/motor,/cfg,", "Home,Motor,Config");

  if (ui.uri() == "/motor") {
    // ===================== MOTOR =====================
    GP.GRID_RESPONSIVE(700);
    GP_MAKE_GRID(
      GP_MAKE_BLOCK_TAB(
        "MOTOR CONFIG",
        GP_MAKE_BOX(GP.LABEL("Min duty"); GP.SLIDER("mdu", data.mdu, 0, 1024););
        GP_MAKE_BOX(GP.LABEL("Velocity"); GP.SLIDER("vel", data.vel, 0, 30););
        GP_MAKE_BOX(GP.LABEL("Accel."); GP.SLIDER("acc", data.acc, 0, 30););
        GP_MAKE_BOX(GP.LABEL("Min vel"); GP.SLIDER("minv", data.minv, 0, 10, 0.5););
      );
      GP_MAKE_BLOCK_TAB(
        "MANUAL",
        GP_MAKE_BOX(GP.LABEL("Duty"); GP.SLIDER("spd", duty, 0, 1023););
        GP_MAKE_BOX(GP.BUTTON("bkw", "◄"); GP.BUTTON("frw", "►"););
      );
    );

    GP_MAKE_GRID(
      GP_MAKE_BLOCK_TAB(
        "PID CONFIG",
        GP_MAKE_BOX(GP.LABEL("Kp"); GP.SPINNER("kp", data.kp, 0, 40, 0.1, 1););
        GP_MAKE_BOX(GP.LABEL("Ki"); GP.SPINNER("ki", data.ki, 0, 40, 0.1, 1););
        GP_MAKE_BOX(GP.LABEL("Filt q."); GP.SLIDER("fil", data.fil, 0, 1, 0.1););
        GP_MAKE_BOX(GP.LABEL("Filt er."); GP.SLIDER("fil2", data.fil2, 0, 100, 1););
      );

      GP_MAKE_BLOCK_TAB(
        "SPEED",
        GP.SWITCH("spdOn"); GP.BREAK();
        GP_MAKE_BOX( GP.LABEL("Speed"); GP.SLIDER("spdPid", spdPid, 0, 50); );
        GP_MAKE_BOX( GP.BUTTON("bkwPid", "◄"); GP.BUTTON("frwPid", "►"); );
      );
    );

    GP_MAKE_BLOCK_TAB(
      "PLANNER",
      GP.SWITCH("planOn"); GP.BREAK();
      GP_MAKE_BOX(GP.LABEL("Target"); GP.SLIDER("tar", pos, data.min, data.max););
      GP_MAKE_BOX(GP.LABEL("Stop zone"); GP.SLIDER("deadz", data.deadz, 0, 20););
    );
  } else if (ui.uri() == "/cfg") {

    // ===================== CFG =====================
    GP.UPDATE("curpos");

    GP_MAKE_FORM(
      "/cfg", "SAVE",
      GP_MAKE_BLOCK_TAB(
        "GEO DATA",
        GP_MAKE_BOX(GP.LABEL("GMT"); GP.NUMBER("gmt", "GMT", data.gmt));
        GP_MAKE_BOX(GP.LABEL("Lat"); GP.NUMBER_F("lat", "latitude", data.lat));
        GP_MAKE_BOX(GP.LABEL("Lon"); GP.NUMBER_F("lon", "longitude", data.lon));
      );

      GP_MAKE_BLOCK_TAB(
        "POSITION",
        GP.LABEL("Current: "); GP.LABEL_BLOCK(String(pos), "curpos");
        GP_MAKE_BOX(GP.LABEL("Min"); GP.SLIDER("min", data.min, 0, 270));
        GP_MAKE_BOX(GP.LABEL("Center"); GP.SLIDER("centr", data.center, 0, 270));
        GP_MAKE_BOX(GP.LABEL("Max"); GP.SLIDER("max", data.max, 0, 270));
      );

      GP_MAKE_BLOCK_TAB(
        "WIFI",
        GP_MAKE_BOX(GP.LABEL("SSID: "); GP.TEXT("ssid", "SSID", data.ssid));
        GP_MAKE_BOX(GP.LABEL("Pass: "); GP.TEXT("pass", "Pass", data.pass));
      );
    );
  } else {

    // ===================== HOME =====================
    GP.UPDATE("curpos,tarpos");
    SunPosition sun(data.lat, data.lon, ntp.unix(), data.gmt);

    GP_MAKE_BLOCK_TAB(
      "SUN DATA",
      GP.LABEL("Altitude: "); GP.LABEL_BLOCK(String(sun.altitude())); GP.BREAK();
      GP.LABEL("Azimuth: "); GP.LABEL_BLOCK(String(sun.azimuth())); GP.BREAK();
      GP.LABEL("Angle90: "); GP.LABEL_BLOCK(String(sun.angle90())); GP.BREAK();
    );

    GP_MAKE_BLOCK_TAB(
      "TRACKER",
      GP.LABEL("State: "); GP.SWITCH("track", data.track); GP.BREAK();
      GP.LABEL("Position: "); GP.LABEL_BLOCK(String(pos), "curpos"); GP.BREAK();
      GP.LABEL("Target: "); GP.LABEL_BLOCK(String(target)); GP.BREAK();
      GP.BUTTON("force", "FORCE");
      GP.BUTTON("brake", "BRAKE");
    );

    GP_MAKE_BLOCK_TAB(
      "CONNECTION",
      GP.LABEL("WiFi mode: ");
      GP.LABEL_BLOCK(WiFi.getMode() == WIFI_AP ? "AP" : "STA"); GP.BREAK(); GP.BREAK();

      GP.LABEL("Time synced");
      GP.LED("leds", ntp.synced()); GP.BREAK();
      GP.LABEL("Time: " + ntp.timeString()); GP.BREAK();
      GP.LABEL("Date: " + ntp.dateString()); GP.BREAK();
    );
  }

  GP.BUILD_END();
}
