//Librerias ------------------------------------------------
//LCD
#include <LCD5110_Graph.h>
//ESP
#include <SoftwareSerial.h>
//Configuracion ESP ----------------------------------------
//Pines
// Los Pines deben estar conectados de la siguente manera:
//      TDX  - Pin 6
//      RDX  - Pin 7
SoftwareSerial ESP(7, 6);
//Configuracion LCD ----------------------------------------
//Pines
// Los Pines deben estar conectados de la siguente manera:
//      CLK  - Pin 8
//      DIN  - Pin 9
//      DC   - Pin 10
//      CE   - Pin 11
//      RST  - Pin 12
LCD5110 myGLCD(8, 9, 10, 12, 11);
//fuente para el LCD
extern uint8_t SmallFont[];
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
const int tamPel = 2;
//---------------------------------------------------------
//Configuracion de Botones --------------------------------
//Pines
const int pinBtn1 = 3;
const int pinBtn2 = 4;
//Variables
int btn1 = HIGH;
int btn2 = HIGH;
//---------------------------------------------------------
//Setup pelY Loop -------------------------------------------
void setup() {
  Serial.begin(9600);
  ESP.begin(9600);
  LCDinit();
  gameInit();
}

void loop() {
  // Repetir lo recibido por el ESP8266 hacia el monitor serial
  if (ESP.available())
  { char c = ESP.read() ;
    Serial.print(c);
  }

  // Repetir lo recibido por el monitor serial hacia el ESP8266
  if (Serial.available())
  { char c = Serial.read();
    ESP.print(c);
  }
  leerBtns();
  movPelota();
  movOponente();
  dibujar();
}
//---------------------------------------------------------
//Funciones -----------------------------------------------

//inicializacion del LCD
void LCDinit() {
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  randomSeed(analogRead(7));
}

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
  btn1 = digitalRead(pinBtn1);
  btn2 = digitalRead(pinBtn2);
  //boton1 mueve hacia arriba
  if (btn1 == LOW && posJ1 > limSup + 1) {
    posJ1--;
  }
  //boton2 mueve hacia abajo
  if (btn2 == LOW && posJ1 < limInf - tamJ - 1) {
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
      myGLCD.print("Jugador 1 Gana", CENTER, (limInf - limSup) / 2);
      myGLCD.update();
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
      myGLCD.print("Jugador 2 Gana", CENTER, (limInf - limSup) / 2);
      myGLCD.update();
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
    if (btn1 == LOW) {
      DirPel = 7 * PI / 4;
    } else if (btn2 == LOW) {
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
  myGLCD.clrScr();
  //dibuja el campo
  myGLCD.drawLine(42, 0, 42, 47);
  myGLCD.drawLine(0, limSup, 83, limSup);
  myGLCD.drawLine(0, limInf, 83, limInf);
  //dibuja los puntJs
  myGLCD.printNumI(puntJ2, 48, 0);
  myGLCD.printNumI(puntJ1, 30, 0);
  //dibuja los jugadores
  myGLCD.drawRect(2, posJ1, 3, (posJ1 + tamJ));
  myGLCD.drawRect(81, posJ2, 80, (posJ2 + tamJ));
  //dibuja la Pelota
  myGLCD.drawRect(pelX, pelY, pelX + tamPel, pelY + tamPel);
  //Muestra en panatalla
  myGLCD.update();
}
//---------------------------------------------------------
