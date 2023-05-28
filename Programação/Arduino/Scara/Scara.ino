#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <MobaTools.h>

MoToTimer Pause;
MoToStepper Mot1(800, STEPDIR);
MoToStepper Mot2(800, STEPDIR);
MoToStepper Mot3(800, STEPDIR);

// 0 e 1 é o tx e rx da serial com o computador
#define m1stp 2   //saida para o step do drive do motor 1 
#define m2stp 4   //saida para o step do drive do motor 2 
#define m3stp 6   //saida para o step do drive do motor 3 
#define m4stp 8   //saida para o step do drive do motor 4 
#define m1dir 3   //saida para o dir  do drive do motor 1 
#define m2dir 5   //saida para o dir  do drive do motor 2 
#define m3dir 7   //saida para o dir  do drive do motor 3 
#define m4dir 9   //saida para o dir  do drive do motor 4 
#define efim  A3   //Entrada do fim de curso para calibração do eixo 1, 2 e 3

#define espTx A0  //Tx da comunicação com o esp modbus
#define espRx A1  //Rx da comunicação com o esp modbus

#define ss    10  //comunicação ISP com o leitor de cartão SD
#define miso  12  //comunicação ISP com o leitor de cartão SD
#define mosi  11  //comunicação ISP com o leitor de cartão SD
#define sck   13  //comunicação ISP com o leitor de cartão SD

#define servo A2  //PWM do Servo da garra

extern void configuraIO();
extern void calibra(int modoCal);
extern void configuraIO();
extern void verificaSerial();

void setup() {
  Serial.begin(9600);
  configuraIO();
  //calibra(4);
}

int angulo = 0;
void loop() {
  verificaSerial();
  /*if (Serial.available() > 0) {
    angulo = Serial.parseInt();
    //Mot1.write(angulo);
    //Mot2.write(angulo);
    Mot1.write(angulo);
  }*/
  
  delay(1);
}
