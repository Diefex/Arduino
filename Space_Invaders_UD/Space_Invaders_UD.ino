//Librerias ------------------------------------------------
#include <LCD5110_Graph.h>
#include <ListLib.h>

using namespace std;

//Configuracion lcd ----------------------------------------
//Pines
LCD5110 lcd(8, 9, 10, 12, 11);
// Los Pines deben estar conectados de la siguente manera:
//      CLK  - Pin 8
//      DIN  - Pin 9
//      DC   - Pin 10
//      CE   - Pin 11
//      RST  - Pin 12

//fuente
extern uint8_t SmallFont[];

//Constantes-----------------------------------------------
const int LCDX = 84; //Tamaño horizontal del lcd
const int LCDY = 48; //Tamaño vertical del lcd
const int TAMJ = 7; //Tamaño del jugador
const int NUMALIENS = 4; //Numero maximo de aliens vivos al mismo tiempo
//Configuracion de Botones --------------------------------
//Pines
const int pinBtnD = 3;
const int pinBtnI = 4;
//Variables
int btn1 = HIGH;
int btn2 = HIGH;

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
  public:
    Alien(int, int, int);
    virtual void dibujar ();
    void moverA();
    void turn();
};

class AlienFactory {
    boolean aliensVivos[NUMALIENS];
  public:
    Alien* genAlien();
    void killAlien(int);
};

class Laser : public Objeto {
  public:
    Laser (int, int);
    virtual void dibujar();
    void mover();
};
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
  lcd.setPixel(posX + 3, posY);
  lcd.drawRect(posX + 2, posY + 1, posX + 4, posY + 2);
  lcd.drawRect(posX + 1, posY + 3, posX + 5, posY + 4);
  lcd.drawRect(posX, posY + 4, posX + 1, posY + 5);
  lcd.drawRect(posX + 5, posY + 4, posX + 6, posY + 5);
  lcd.setPixel(posX, posY + 6);
  lcd.setPixel(posX + 6, posY + 6);
}

//De Alien--------------------------------------------
Alien :: Alien (int posInit, int _h, int _tam) : Nave (posInit, _h, _tam) {}

void Alien :: dibujar () {
  lcd.drawRect(posX + 1, posY + 1, posX + 5, posY + 3);
  lcd.drawRect(posX, posY + 3, posX, posY + 4);
  lcd.drawRect(posX + 6, posY + 3, posX + 6, posY + 4);
  lcd.setPixel(posX + 3, posY + 2);
  lcd.setPixel(posX + 2, posY);
  lcd.setPixel(posX + 4, posY);
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
//De Laser-------------------------------------------
Laser :: Laser (int x, int y) : Objeto(x, y) {}

void Laser :: dibujar () {
  lcd.drawRect(posX, posY, posX + 1, posY + 3);
}

void Laser :: mover () {
  if (posY >= 0) {
    posY--;
  }
}

//De AlienFactory-------------------------------------
Alien* AlienFactory :: genAlien () {
  int y = 0;
  int x = 10;
  for (int i = 0; i < NUMALIENS; i++) {
    x = 5 + (i * 10);
    y = 5 + (i * 5);
    if (aliensVivos[i] == false) {
      Alien* a = new Alien(x, y, 5);
      aliensVivos[i] = true;
      return a;
    }
  }
  return new Alien(-1, -1, 0);
}

void AlienFactory :: killAlien(int i) {
  aliensVivos[i] = false;
}

//Inicializacion del LCD------------------------------
void lcdInit() {
  lcd.InitLCD();
  lcd.setFont(SmallFont);
  randomSeed(analogRead(7));
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
List<Alien*> aliens; //Lista de aliens en pantalla
List<Laser*> lasers; //Lasers en pantalla
AlienFactory af;
Jugador j(LCDX / 2, LCDY - TAMJ, TAMJ); //Jugador
int t = 0;
void setup() {
  pinMode(pinBtnD, INPUT);
  pinMode(pinBtnI, INPUT);
  lcdInit();

  //aliens.Add(new Alien(10, 10, 7));
  //aliens.Add(new Alien(20, 20, 7));
  for (int i = 0; i < NUMALIENS; i++) {
    aliens.Add(af.genAlien());
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
    Alien* a = af.genAlien();
    if (a->getPosX() >= 0) {
      aliens.Add(a);
    }
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
  lcd.clrScr();
  for (int i = 0; i < aliens.Count(); i++) {
    aliens[i]->dibujar();

  }
  for (int i = 0; i < lasers.Count(); i++) {
    lasers[i]->dibujar();
  }
  j.dibujar();
  lcd.update();
  t++;
}

void eliminarLaser(int i) {
  lasers.Remove(i);
}

void eliminarAlien(int i) {
  aliens.Remove(i);
  af.killAlien(i);
}
