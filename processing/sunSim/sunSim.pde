// Набросок/Импортировать библиотеку/Установить библиотеку
// поиском найти и поставить PeasyCam
import peasy.*;
PeasyCam cam;

// вертикальная позиция мыши - день
// горизонтальная позиция мыши - угол солнечной панели

// широта и долгота
float lat = 55, lon = 37;

void compute(float lat, float lon, float unix, int gmt) {
  lat = radians(lat);
  float day = (unix / 86400.0) % 365.25;
  float hours = (unix % 86400) / 86400.0;       // decimal hours
  float t = radians(279.342 + 0.985647 * day);    // temp
  float eqTime = (-104.9 * sin(t) + 596.2 * sin(2 * t) + 4.3 * sin(3 * t) - 12.7 * sin(4 * t) - 429.3 * cos(t) - 2.0 * cos(2 * t) + 19.3 * cos(3 * t)) / 60.0;  // time equation
  t = (day + hours / 24 - 1.5) * TWO_PI / 365;    // temp
  decl = 0.006918 - 0.4 * cos(t) + 0.070257 * sin(t) - 0.006758 * cos(t * 2) + 0.000907 * sin(t * 2) - 0.002697 * cos(t * 3) + 0.00148 * sin(t * 3);  // declination
  float ha = degrees(acos(-0.01454 / (cos(lat) * cos(decl)) - tan(lat) * tan(decl)));   // HA sunrise
  if (abs(gmt) <= 12) gmt *= 60;          // gmt -> minutes
  noon = 720 - 4 * lon - eqTime + gmt;    // Solar Noon (min) + gmt
  rise = noon - ha * 4;                   // Sunrise Time (min) + gmt
  set = noon + ha * 4;                    // Sunset Time (min) + gmt
  sunl = 8 * ha;                          // Sunlight Time (min)
  float hrAngl = ((hours * 1440 + eqTime + 4 * lon) % 1440) / 4;   // True Solar Time (min)
  hrAngl = hrAngl + (hrAngl < 0 ? 180 : -180);                      // Hour Angle
  zen = acos(sin(lat) * sin(decl) + cos(lat) * cos(decl) * cos(radians(hrAngl)));   // Zenith
  azm = degrees(acos(((sin(lat) * cos(zen)) - sin(decl)) / (cos(lat) * sin(zen)))); // Azimuth
  decl = degrees(decl);
  alt = 90 - degrees(zen);
  azm = (hrAngl > 0) ? (azm + 180) : (540 - azm);
  azm = azm % 360;
  zen = 90 - degrees(lat) + decl;
  angle = round(sunl / 8.0);  // * 360 / 60 / 24 / 2
  int maxan = int(min(angle, 90));
  angle = map(azm, 180-angle, 180+angle, 90-maxan, 90+maxan);
}

float alt = 0;
float azm = 0;
float decl = 0;
float zen = 0;
float noon = 0;
float rise = 0;
float set = 0;
float sunl = 0;
float angle = 0;

void setup() {
  size(800, 600, P3D);
  cam = new PeasyCam(this, 100);
  cam.setMinimumDistance(150);
  cam.setMaximumDistance(150);
}
int i = 0, il = 0;
float mul = 10;

void draw() {
  int unix = 1655802000 + (mouseY)*20000;
  
  rotateX(PI/3);
  rotateZ(PI/4);
  background(0);
  stroke(color(255, 255, 255));
  sphere(1);
  int g = 50;
  for (int x = -g; x <= g; x+=10) line(x, -g, 0, x, g, 0);
  for (int y = -g; y <= g; y+=10) line(-g, y, 0, g, y, 0);


  if (++i > 24*mul) i = 0;
  stroke(color(255, 0, 0));  
  pushMatrix();
  compute(lat, lon, unix + i * 3600 / mul, 3);
  rotateZ(radians(azm));
  rotateY(radians(-alt));
  translate(g, 0, 0);
  sphere(1);
  popMatrix();

  stroke(color(150, 0, 0));  
  for (int i = 0; i < 24 * mul; i++) {
    compute(lat, lon, unix + i * 3600 / mul, 3);
    if (alt > 0) {
      pushMatrix();
      rotateZ(radians(azm));
      rotateY(radians(-alt));
      line(0, 0, 0, g, 0, 0);
      popMatrix();
    }
  }


  // вращение по наклонной оси как у Илюхи
  stroke(color(0, 150, 0));  
  for (int i = 0; i < 24 * mul; i++) {
    compute(lat, lon, unix + i * 3600 / mul, 3);
    if (alt > 0) {
      pushMatrix();
      rotateY(radians(zen+(mouseX-width/2)/10));
      //rotateY(radians(50));
      //rotateZ(radians(azm));
      //rotateY(radians(60));
      rotateZ(radians(90+angle));
      line(0, 0, 0, g, 0, 0);
      popMatrix();
    }
  }


  if (++il > 24*mul) il = 0;
  stroke(color(0, 255, 0));
  pushMatrix();
  compute(lat, lon, unix + i * 3600 / mul, 3);
  rotateY(radians(zen+(mouseX-width/2)/10));
  //rotateY(radians(50));
  //rotateZ(radians(azm));
  //rotateY(radians(60));
  rotateZ(radians(90+angle));
  translate(g, 0, 0);
  if (alt > 0) sphere(1);
  popMatrix();


  // ориент в зенит, вращение по азимуту
  /*
  stroke(color(0, 255, 0));  
   for (int i = 0; i < 24 * mul; i++) {
   get_sun_pos(i / mul - 3);
   if (alt > 0) {
   pushMatrix();
   rotateZ(radians(az));
   rotateY(radians(-decl));
   line(0, 0, 0, g, 0, 0);
   translate(g, 0, 0);
   popMatrix();
   }
   }
   */
}
