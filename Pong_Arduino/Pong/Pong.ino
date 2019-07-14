//Librerias ------------------------------------------------
//LCD
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
//Configuracion LCD ----------------------------------------
//Pines
// Los Pines deben estar conectados de la siguente manera:
//      CLK  - Pin D1
//      DIN  - Pin D2
//      DC   - Pin D5
//      CE   - Pin D6
//      RST  - Pin D7
Adafruit_PCD8544 display = Adafruit_PCD8544(D1, D2, D5, D6, D7);
//---------------------------------------------------------
//Configuracion de Jugadores ------------------------------
//posiciones
int posJ1;
int posJ2;
const int limSup = 8;
const int limInf = 47;
//puntajes
int puntJ1;
int puntJ2;
//tamaño
const int tamJ = 10;
//---------------------------------------------------------
//Configuracion de Pelota --------------------------------
//Posicion
float pelX;
float pelY;
//Velocidad
const float VelPel = 0.5;
//Direccion
float DirPel;
//tamaño
const int tamPel = 3;
//---------------------------------------------------------
//Configuracion de Botones --------------------------------
//Pines
const int pinBtnI = D8;
const int pinBtnD = 10;
//Variables
int btnI = LOW;
int btnD = LOW;
//---------------------------------------------------------
//Setup pelY Loop -------------------------------------------
void setup() {
  pinMode(pinBtnI, INPUT);
  pinMode(pinBtnD, INPUT);
  Serial.begin(9600);
  display.begin();
  display.setContrast(70);
  gameInit();
}

void loop() {
  leerBtns();
  movPelota();
  movOponente();
  dibujar();
}
//---------------------------------------------------------
//Funciones -----------------------------------------------

//inicializacion del juego
void gameInit() {
  //inicializacion de Jugadores
  posJ1 = (limSup + limInf - tamJ) / 2;
  posJ2 = (limSup + limInf - tamJ) / 2;
  //inicializacion de Pelota
  pelX = 42;
  pelY = (limSup + limInf) / 2;
  DirPel = (3 * PI) / 4;
  //inicializacion de puntajes
  puntJ1 = 0;
  puntJ2 = 0;
}

//input de botones
void leerBtns() {
  //lectura de pines
  btnI = digitalRead(pinBtnI);
  btnD = digitalRead(pinBtnD);
  //boton1 mueve hacia arriba
  if (btnI == HIGH && posJ1 > limSup + 1) {
    posJ1--;
  }
  //boton2 mueve hacia abajo
  if (btnD == HIGH && posJ1 < limInf - tamJ - 1) {
    posJ1++;
  }
}

//Movimiento de la Pelota
void movPelota() {
  if (DirPel >= 2 * PI) {
    DirPel -= 2 * PI;
  }
  //Punto para jugador 1
  if (pelX > 83 - tamPel) {
    puntJ1++;
    if (puntJ1 >= 5) {
      display.setCursor(5, (limInf - limSup) / 2);
      display.println("Jugador 1 Gana");
      display.display();
      delay(3000);
      setup();
    }
    pelX = 42;
    pelY = (limSup + limInf) / 2;
    DirPel = (5 * PI) / 4;
  }
  //Punto para jugador 2
  if (pelX < 0) {
    puntJ2++;
    if (puntJ2 >= 5) {
      display.setCursor(5, (limInf - limSup) / 2);
      display.println("Jugador 2 Gana");
      display.display();
      delay(3000);
      setup();
    }
    pelX = 42;
    pelY = (limSup + limInf) / 2;
    DirPel = (7 * PI) / 4;
  }
  //Colision con limites superior e inferior
  if (pelY <= limSup + 1 || pelY >= limInf - tamPel) {
    DirPel += PI / 2;
  }
  //Colision con jugador 1
  if (pelX <= 4 && pelY >= posJ1 - tamPel && pelY <= posJ1 + tamJ) {
    if (btnI == LOW) {
      DirPel = 7 * PI / 4;
    } else if (btnD == LOW) {
      DirPel = PI / 4;
    } else {
      DirPel += PI / 2;
    }
  }
  //Colision con jugador 2
  if (pelX >= 80 - tamPel && pelY >= posJ2 - tamPel && pelY <= posJ2 + tamJ) {
    DirPel += PI / 2;
  }
  pelX += cos(DirPel) * VelPel;
  pelY += sin(DirPel) * VelPel;
}

//Movimiento del oponente
void movOponente() {
  float H = (limSup + limInf - tamJ) / 2;
  //Calcula direccion de la pelota
  if (pelX >= 42) {
    H = pelY - ((84 - pelX) * tan(DirPel));
  }
  //Mueve hacia arriba
  if (H < posJ2 && posJ2 > limSup + 1) {
    posJ2--;
  }
  //Mueve hacia abajo
  if (H > posJ2 && posJ2 < limInf - tamJ - 1) {
    posJ2++;
  }
}

//dibujado en el LCD
void dibujar() {
  //limpia pantalla
  display.clearDisplay();
  //dibuja el campo
  display.drawLine(42, 0, 42, 47,BLACK);
  display.drawLine(0, limSup, 83, limSup, BLACK);
  display.drawLine(0, limInf, 83, limInf, BLACK);
  //dibuja los puntajes
  display.setCursor(46,0);
  display.println(puntJ2);
  display.setCursor(34,0);
  display.println(puntJ1);
  //dibuja los jugadores
  display.drawRect(2, posJ1, 2, tamJ, BLACK);
  display.drawRect(81, posJ2, 2, tamJ, BLACK);
  //dibuja la Pelota
  display.drawRect(pelX, pelY, tamPel, tamPel, BLACK);
  //Muestra en panatalla
  display.display();
}
//---------------------------------------------------------
