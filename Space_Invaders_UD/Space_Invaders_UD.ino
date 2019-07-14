//Librerias ------------------------------------------------
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <ListLib.h>

using namespace std;

//Configuracion lcd ----------------------------------------
// Los Pines deben estar conectados de la siguente manera:
//      CLK  - Pin D1
//      DIN  - Pin D2
//      DC   - Pin D5
//      CE   - Pin D6
//      RST  - Pin D7
Adafruit_PCD8544 display = Adafruit_PCD8544(D1, D2, D5, D6, D7);

//Constantes-----------------------------------------------
const int LCDX = 84; //Tamaño horizontal del lcd
const int LCDY = 48; //Tamaño vertical del lcd
const int TAMJ = 7; //Tamaño del jugador
const int NUMALIENS = 4; //Numero maximo de aliens vivos al mismo tiempo

//Configuracion de Botones --------------------------------
// Los Botones deben estar conectados de la siguente manera:
//      BtnDerecho  - Pin 10 (SD3)
//      BtnIzquierdo  - Pin D8
const int pinBtnD = 10;
const int pinBtnI = D8;
int btnD = HIGH;
int btnI = HIGH;

//Configuracion del Juego----------------------------------


//Prototipos de funciones ---------------------------------
void lcdInit();
void gameInit();

//Clases---------------------------------------------------
class Objeto {
  protected:
    float posX; //Posicion
    int posY; //Altura
  public :
    Objeto();
    Objeto(int, int);
    virtual void dibujar();
    int getPosX();
    int getPosY();
};

class Nave : public Objeto {
  protected:
    int tam; //Tamaño
  public:
    Nave(int, int, int);
    virtual void dibujar();
    void mover(int);
};

class Jugador : public Nave {
  public:
    Jugador(int, int, int);
    virtual void dibujar();
};

class Alien : public Nave {
    float dir = 1;
    boolean vivo = true;
  public:
    Alien(int, int, int);
    virtual void dibujar ();
    void moverA();
    void turn();
    boolean isVivo();
};

class AlienFactory {
    boolean aliensVivos[NUMALIENS];
  public:
    void genAlien();
    void killAlien(int);
};

class Laser : public Objeto {
  public:
    Laser (int, int);
    virtual void dibujar();
    void mover();
};

//Declaracion de objetos y demas----------------------------
List<Alien*> aliens; //Lista de aliens en pantalla
List<Laser*> lasers; //Lasers en pantalla
AlienFactory af;
Jugador j(LCDX / 2, LCDY - TAMJ, TAMJ); //Jugador

//Metodos ---------------------------------------------------
//De Objeto---------------------------------------
Objeto :: Objeto (int posInit, int _h) {
  posX = posInit;
  posY = _h;
}

int Objeto :: getPosX() {
  return posX;
}
int Objeto :: getPosY() {
  return posY;
}
//De Nave-----------------------------------------
Nave :: Nave (int posInit, int _h, int _tam) : Objeto(posInit, _h) {
  tam = _tam;
}

void Nave :: mover(int x) {
  posX += x;
}

//De Jugador------------------------------------------
Jugador :: Jugador (int posInit, int _h, int _tam) : Nave (posInit, _h, _tam) {}

void Jugador :: dibujar () {
  int X = posX;
  display.drawPixel(X + 3, posY, BLACK);
  display.drawRect(X + 2, posY + 1, 3, 2, BLACK);
  display.drawRect(X + 1, posY + 3, 5, 2, BLACK);
  display.drawRect(X, posY + 4, 2, 2, BLACK);
  display.drawRect(X + 5, posY + 4, 2, 2, BLACK);
  display.drawPixel(X, posY + 6, BLACK);
  display.drawPixel(X + 6, posY + 6, BLACK);
}

//De Alien--------------------------------------------
Alien :: Alien (int posInit, int _h, int _tam) : Nave (posInit, _h, _tam) {}

void Alien :: dibujar () {
  int X = posX;
  display.drawRect(X + 1, posY + 1, 5, 3, BLACK);
  display.drawRect(X, posY + 3, 1, 2, BLACK);
  display.drawRect(X + 6, posY + 3, 1, 3, BLACK);
  display.drawPixel(X + 3, posY + 2, BLACK);
  display.drawPixel(X + 2, posY, BLACK);
  display.drawPixel(X + 4, posY, BLACK);
}

void Alien :: moverA () {
  if (posX <= 0 || posX >= LCDX - tam) {
    turn();
  }
  posX += dir / 4;
}

void Alien :: turn() {
  dir *= -1;
}

boolean Alien :: isVivo(){
  return vivo;
}
//De Laser-------------------------------------------
Laser :: Laser (int x, int y) : Objeto(x, y) {}

void Laser :: dibujar () {
  display.drawRect(posX, posY, 1, 3, BLACK);
}

void Laser :: mover () {
  if (posY >= 0) {
    posY--;
  }
}

//De AlienFactory-------------------------------------
void AlienFactory :: genAlien () {
  int y = 0;
  int x = 10;
  for (int i = 0; i < aliens.Count(); i++) {
    x = 5 + (i * 10);
    y = 5 + (i * 5);
    if (aliens[i]->isVivo()) {
      Alien* a = new Alien(x, y, 5);
      aliens[i] = a;
    }
  }
}

void AlienFactory :: killAlien(int i) {
  aliensVivos[i] = false;
}

//Inicializacion del LCD------------------------------
void lcdInit() {
  display.begin();
  display.setContrast(70);
}

//Inicializacion del juego----------------------------
void gameInit() {
  //inicializacion de Jugadores

  //inicializacion de puntajes

}

//input de botones------------------------------------
boolean BtnDerPress() {
  //lectura de pines
  int btn = digitalRead(pinBtnD);
  //boton1 mueve hacia la derecha
  if (btn == LOW) {
    return true;
  } else {
    return false;
  }
}

boolean BtnIzqPress() {
  //lectura de pines
  int btn = digitalRead(pinBtnI);
  //boton2 mueve hacia la izquierda
  if (btn == LOW) {
    return true;
  } else {
    return false;
  }
}

//Setup Y Loop --------------------------------------------

int t = 0;
void setup() {
  pinMode(pinBtnD, INPUT);
  pinMode(pinBtnI, INPUT);
  lcdInit();

  //aliens.Add(new Alien(10, 10, 7));
  //aliens.Add(new Alien(20, 20, 7));
  for (int i = 0; i < NUMALIENS; i++) {
    af.genAlien();
  }
}

void loop() {
  //Leer Botones
  if (BtnDerPress()) {
    j.mover(1);
  }
  if (BtnIzqPress()) {
    j.mover(-1);

  }
  //Disparar laser
  if (t >= 70) {
    lasers.Add(new Laser(j.getPosX() + 3, j.getPosY()));
    t -= 70;
  }
  //Mover Aliens
  for (int i = 0; i < aliens.Count(); i++) {
    aliens[i]->moverA();
    if (aliens[i]->getPosX() < 0 || aliens[i]->getPosY() < 0) {
      eliminarAlien(i);
    }
  }
  //Mover Lasers
  for (int i = 0; i < lasers.Count(); i++) {
    if (lasers[i]->getPosY() < 0) {
      eliminarLaser(i);
    } else {
      lasers[i]->mover();
      for (int j = 0; j < aliens.Count(); j++) {
        if ((aliens[j]->getPosX() <= lasers[i]->getPosX() && aliens[j]->getPosX() + 5 >= lasers[i]->getPosX()) && (aliens[j]->getPosY() <= lasers[i]->getPosY() && aliens[j]->getPosY() + 5 >= lasers[i]->getPosY())) {
          eliminarAlien(j);
        }
      }
    }
  }
  //Dibujar
  display.clearDisplay();
  for (int i = 0; i < aliens.Count(); i++) {
    aliens[i]->dibujar();

  }
  for (int i = 0; i < lasers.Count(); i++) {
    lasers[i]->dibujar();
  }
  j.dibujar();
  display.display();
  t++;
}

void eliminarLaser(int i) {
  lasers.Remove(i);
}

void eliminarAlien(int i) {
  aliens.Remove(i);
  af.killAlien(i);
}
