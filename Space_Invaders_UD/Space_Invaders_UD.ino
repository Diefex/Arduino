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
//Configuracion de Botones --------------------------------
//Pines
const int pinBtn1 = 3;
const int pinBtn2 = 4;
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
    int posX; //Posicion
    int posY; //Altura
  public :
    Objeto();
    Objeto(int, int);
    virtual void dibujar();
};

class Nave : public Objeto {
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
  public:
    Alien(int, int, int);
    virtual void dibujar ();
};

class Laser : public Objeto {
  public:
    Laser (int, int);
    virtual void dibujar();
};

class VistaJuego {
  public:
    //impresion del buffer en la pantalla
    void imprimir(List<Objeto*>);
};
//Metodos ---------------------------------------------------
//De Vista----------------------------------------
void VistaJuego :: imprimir(List<Objeto*> Obj) {
  //limpia pantalla
  lcd.clrScr();
  //dibuja el jugador
  for (int i = 0; i < Obj.Count(); i++) {
    Obj[i]->dibujar();
  }
  //Muestra en panatalla
  lcd.update();
}
//De Objeto---------------------------------------
Objeto :: Objeto (int posInit, int _h) {
  posX = posInit;
  posY = _h;
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
  lcd.drawRect(posX, posy);
  lcd.drawRect(posX + 2, posy + 2);
}

//Del Laser-------------------------------------------
Laser :: Laser (int x, int y) : Objeto(x, y) {}

void Laser :: dibujar () {
  lcd.drawLine(posX, posY, posY, posY + 3);
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
  int btn = digitalRead(pinBtn1);
  //boton1 mueve hacia la derecha
  if (btn == LOW) {
    return true;
  } else {
    return false;
  }
}

boolean BtnIzqPress() {
  //lectura de pines
  int btn = digitalRead(pinBtn2);
  //boton1 mueve hacia la izquierda
  if (btn == LOW) {
    return true;
  } else {
    return false;
  }
}

//Setup Y Loop --------------------------------------------
VistaJuego vista;
List<Alien*> aliens; //Lista de aliens en pantalla
Jugador j(LCDX / 2, LCDY - TAMJ, TAMJ); //Jugador
void setup() {
  pinMode(pinBtn1, INPUT);
  pinMode(pinBtn2, INPUT);
  lcdInit();

  aliens.Add(new Alien(10, 10, 7));
  aliens.Add(new Alien(20, 20, 7));
}

void loop() {
  //Leer Botones
  if (BtnDerPress()) {
    j.mover(1);
  }
  if (BtnIzqPress()) {
    j.mover(-1);
  }
  //Dibujar
  lcd.clrScr();
  for (int i = 0; i < Obj.Count(); i++) {
    aliens[i]->dibujar();
  }
  j.dibujar();
  lcd.update();
}
