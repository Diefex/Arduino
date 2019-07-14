//Librerias-------------------
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <ListLib.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
//----------------------------

//Inicializacion

//Configuracion LCD ----------
// Los Pines deben estar conectados de la siguente manera:
//      CLK  - Pin D1
//      DIN  - Pin D2
//      DC   - Pin D5
//      CE   - Pin D6
//      RST  - Pin D7
Adafruit_PCD8544 display = Adafruit_PCD8544(D1, D2, D5, D6, D7);
// Los Pines deben estar conectados de la siguente manera:
//      Boton Izquierdo  - Pin D8
//      Boton Derecho  - Pin SD3
const int pinBtnI = D8;
const int pinBtnD = 10;
int btnI = LOW;
int btnD = LOW;
//----------------------------

//Constantes------------------
int const LCDX = 84;
int const LCDY = 48;
int const TAMJ = 7;
int const NUMALIENS = 5;
//----------------------------

//Funciones
boolean btnDPress() {
  if (digitalRead(pinBtnD) == HIGH) {
    return true;
  } else {
    return false;
  }
}
boolean btnIPress() {
  if (digitalRead(pinBtnI) == HIGH) {
    return true;
  } else {
    return false;
  }
}

//Clases

//Logica----------------------
class Entidad {
  protected:
    float posX;
    float posY;
  public:
    Entidad();
    Entidad(int, int);
    virtual void mover(float) = 0;
};
Entidad :: Entidad () {}
Entidad :: Entidad(int _posX, int _posY) {
  posX = _posX;
  posY = _posY;
}

class Jugador : public Entidad {
  public:
    Jugador(int, int);
    void mover(float);
    float getPosX();
};
Jugador :: Jugador (int _posX, int _posY) : Entidad(_posX, _posY) {}
void Jugador :: mover(float vel) {
  posX += vel;
}
float Jugador :: getPosX() {
  return posX;
}

class Alien : public Entidad {
    boolean vivo = false;
  public:
    Alien();
    Alien(int, int);
    void mover(float);
    boolean vive();
    void revivir();
    void kill();
    float getPosX();
    float getPosY();
};
Alien :: Alien (int _posX, int _posY) : Entidad(_posX, _posY) {}
void Alien :: mover(float vel) {
  posY += vel;
}
Alien :: Alien () {
  posX = random(LCDX - 7);
  posY = 0;
}
boolean Alien :: vive() {
  return vivo;
}
void Alien :: revivir() {
  posY = 0;
  posX = random(LCDX - 7);
  vivo = true;
}
void Alien :: kill() {
  posY = 0;
  vivo = false;
}
float Alien :: getPosX() {
  return posX;
}
float Alien :: getPosY() {
  return posY;
}

class AlienFactory {
    Alien alienPool[NUMALIENS];
  public:
    AlienFactory();
    Alien crear();
    void destruir(int);
    void reiniciar();
};
AlienFactory :: AlienFactory () {
  for (int i = 0; i < NUMALIENS; i++) {
    Alien a = Alien(random(LCDX - 7), 0);
    alienPool[i] = a;
  }
}
Alien AlienFactory :: crear() {
  for (int i = 0; i < NUMALIENS; i++) {
    if (!alienPool[i].vive()) {
      alienPool[i].revivir();
      return alienPool[i];
    }
  }
}
void AlienFactory :: destruir(int i) {
  alienPool[i].kill();
}

//----------------------------

//Persistencia----------------
class Conexion {
    unsigned long myChannelNumber = 823268; // ID del canal en ThingSpeak
    const char * myWriteAPIKey = "C9SU5WNQB5P9Z5LX"; // Key del canal en ThingSpeak
    String ssid = "ALEJANDRO";   // SSID de la Red
    String pass = "28349523";   // Contraseña de la Red
  private:
    static Conexion* wifi;
    Conexion();
  public:
    static Conexion* conectarWiFi();
    void subirPuntaje(int);
};
Conexion* Conexion :: wifi = 0;
Conexion :: Conexion() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
}
Conexion* Conexion :: conectarWiFi() {
  if(wifi == 0){
    wifi = new Conexion;
  }
  return wifi;
}
void Conexion :: subirPuntaje (int puntaje) {
  int attp = 0;
  int err = 0;
  do {
    int httpCode = ThingSpeak.writeField(myChannelNumber, 1, puntaje, myWriteAPIKey);
    if (httpCode == 200) {
      Serial.print("Puntaje Subido: ");
      Serial.print(puntaje);
      Serial.println("");
    }
    else {
      Serial.println("No se puedo subir el puntaje. HTTP error code " + String(httpCode));
    }
    delay(500);
    attp++;
    err = httpCode;
  } while (err == -401 && attp < 10);
}
//----------------------------

//Presentacion----------------
class VistaInicial {
  public:
    void titulo();
    void conectando();
};
void VistaInicial :: titulo() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 0);
  display.println("Alien");
  display.setCursor(15, 20);
  display.println("Dodge");
  display.setTextSize(1);
  display.setCursor(10, 40);
  display.println("Press Start");
  display.display();
}
void VistaInicial :: conectando() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(5, 5);
  display.println("Conectanto a");
  display.setCursor(20, 15);
  display.println("red WiFi");
  display.setCursor(15, 25);
  display.println("Por Favor");
  display.setCursor(15, 35);
  display.println("Espere...");
  display.display();
}

class VistaJuego {
  public:
    void iniciarJuego();
    void dibujarJugador(int, int);
    void dibujarAlien(int, int);
    void dibujarPuntaje(int);
    void gameOver(int);
};
void VistaJuego :: iniciarJuego() {
  display.setTextSize(2);

  for (int i = 3; i > 0; i--) {
    display.clearDisplay();
    display.setCursor(40, 20);
    display.print(i);
    display.display();
    delay(1000);
  }
}
void VistaJuego :: dibujarJugador(int x, int y) {
  display.drawPixel(x + 3, y, BLACK);
  display.drawRect(x + 2, y + 1, 3, 2, BLACK);
  display.drawRect(x + 1, y + 3, 5, 2, BLACK);
  display.drawRect(x, y + 4, 2, 2, BLACK);
  display.drawRect(x + 5, y + 4, 2, 2, BLACK);
  display.drawPixel(x, y + 6, BLACK);
  display.drawPixel(x + 6, y + 6, BLACK);
}
void VistaJuego :: dibujarAlien(int x, int y) {
  display.drawRect(x + 1, y + 1, 5, 3, BLACK);
  display.drawRect(x, y + 3, 1, 2, BLACK);
  display.drawRect(x + 6, y + 3, 1, 2, BLACK);
  display.drawPixel(x + 3, y + 2, BLACK);
  display.drawPixel(x + 2, y, BLACK);
  display.drawPixel(x + 4, y, BLACK);
}
void VistaJuego :: dibujarPuntaje(int puntos) {
  display.setTextSize(1);
  display.print(puntos);
}
void VistaJuego :: gameOver(int puntos) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(20, 0);
  display.println("Game");
  display.setCursor(20, 20);
  display.println("Over");
  display.setTextSize(1);
  display.setCursor(15, 40);
  display.print("Puntaje= ");
  display.print(puntos);
  display.display();
}

class Modelo {
    Conexion* conexion;
    VistaJuego vista;
    int puntos = 0;
  public:
    void iniciar();
    void jugar();
    void gameOver();
};
void Modelo :: iniciar() {
  VistaInicial vistaInit;
  vistaInit.conectando();
  conexion = Conexion::conectarWiFi();
  vistaInit.titulo();
}
void Modelo :: jugar() {
  puntos = 0;
  List<Alien> aliens;
  AlienFactory aF;
  Jugador j((LCDX - TAMJ) / 2, LCDY - TAMJ);
  vista.iniciarJuego();
  boolean enJuego = true;
  unsigned long ts1 = millis();
  unsigned long ts2 = 0;
  unsigned long tf1 = millis();
  unsigned long tf2 = 0;
  int dif = 1000;
  while (enJuego) {
    ts2 = millis();
    tf2 = millis();
    if (ts2 > (ts1 + dif)) { //Va aumentando la velocidad del juego
      ts1 = millis();
      for (int i = 0; i < aliens.Count(); i++) {
        if (!aliens[i].vive()) {
          aliens[i] = aF.crear();
        }
      }
      if (aliens.Count() < NUMALIENS) {
        aliens.Add(aF.crear());
      }
      if (dif > 300) {
        dif -= 10;
      }
      puntos++;
    }
    if (tf2 > (tf1 + 16)) { //Entra 60 veces por segundo
      tf1 = millis();
      display.clearDisplay();
      for (int i = 0; i < aliens.Count(); i++) {
        if (aliens[i].vive()) {
          vista.dibujarAlien(aliens[i].getPosX(), aliens[i].getPosY());
        }
      }
      vista.dibujarJugador(j.getPosX(), LCDY - TAMJ);
      vista.dibujarPuntaje(puntos);
      display.display();
      for (int i = 0; i < aliens.Count(); i++) {
        aliens[i].mover(1);
        if ((aliens[i].getPosX() + 7 > j.getPosX()) && (aliens[i].getPosX() < j.getPosX() + TAMJ) && (aliens[i].getPosY() + 5 > LCDY - TAMJ)) {
          enJuego = false;
          gameOver();
        }
        if (aliens[i].getPosY() > LCDY) {
          aF.destruir(i);
          aliens.Remove(i);
        }
      }
      if (btnDPress() && j.getPosX() < LCDX - TAMJ) {
        j.mover(1);
      }
      if (btnIPress() && j.getPosX() > 0) {
        j.mover(-1);
      }
    }
  }
}
void Modelo :: gameOver() {
  delay(1000);
  vista.gameOver(puntos);
  delay(1000);
  conexion->subirPuntaje(puntos);
  VistaInicial vistaInit;
  vistaInit.titulo();
  loop();
}
//----------------------------

//Main------------------------
Modelo modelo;
void setup() {
  //Init WiFi
  WiFiClient  client;
  Serial.begin(115200);
  delay(100);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  //Init pines
  pinMode(pinBtnI, INPUT);
  pinMode(pinBtnD, INPUT);
  //Init LCD
  display.begin();
  display.setTextSize(1);
  display.setContrast(70);
  //Iniciar
  modelo.iniciar();
}

void loop() {
  if (btnDPress() || btnIPress()) {
    modelo.jugar();
  }
}
//---------------------------
