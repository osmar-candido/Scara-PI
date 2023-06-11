#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <MobaTools.h>
#include <SoftwareSerial.h>
#include <math.h>

MoToTimer Pause;

// 0 e 1 é o tx e rx da serial com o computador
#define m1stp 2   //saida para o step do drive do motor 1 
#define m2stp 4   //saida para o step do drive do motor 2 
#define m3stp 6   //saida para o step do drive do motor 3 
#define m4stp 8   //saida para o step do drive do motor 4 
#define m1dir 3   //saida para o dir  do drive do motor 1 
#define m2dir 5   //saida para o dir  do drive do motor 2 
#define m3dir 7   //saida para o dir  do drive do motor 3 
#define m4dir 9   //saida para o dir  do drive do motor 4 
#define efim  17  //A3   //Entrada do fim de curso para calibração do eixo 1, 2 e 3

#define espTx 14  //A0  //Tx da comunicação com o esp modbus
#define espRx 15  //A1  //Rx da comunicação com o esp modbus

#define ss    10  //comunicação ISP com o leitor de cartão SD
#define miso  12  //comunicação ISP com o leitor de cartão SD
#define mosi  11  //comunicação ISP com o leitor de cartão SD
#define sck   13  //comunicação ISP com o leitor de cartão SD

#define servo 16  //A2  //PWM do Servo da garra

#define distanciaBaseCotovelo 150   // Distancia entre a base e o cotovelo
#define distanciaCotoveloPunho 150  // Distancia entre o cotovelo e o punho

#define TEMPOLEITURA 100
#define TEMPOESPERA 3000

byte enderecoo;
byte codigoResultado=0;
byte dispositivosEncontrados=0;

extern void testeMotorSerial(char dadodaserial);
extern void configuraIO();
extern void calibra(int modoCal);
extern void configuraIO();
extern void verificaSerial();
extern void configuraDisplay();
extern void configuraBotoes();
extern void configuraSerial();
extern byte lerEntradasPCF8574();
extern void loopBotoes();
extern void configuraCinematica();
extern void loopDisplay();
extern void homing();
extern void loopCinematica();
extern void loopSerialEsp();

byte modo = 0; //0 = local(IHM/Serial) || 1 = remoto(ModbusTCP)

void setup() {
  //Define todos as saidas e entradas do arduino 
  pinMode(m1stp, OUTPUT);
  pinMode(m2stp, OUTPUT);
  pinMode(m3stp, OUTPUT);
  pinMode(m4stp, OUTPUT);
  pinMode(m1dir, OUTPUT);
  pinMode(m2dir, OUTPUT);
  pinMode(m3dir, OUTPUT);
  pinMode(m4dir, OUTPUT);
  pinMode(efim, INPUT_PULLUP);
  pinMode(servo, OUTPUT);
  
  configuraDisplay(); //configura e inicia a comunicação com o display
  configuraBotoes(); //configura e inicia a comunicação com o MPR121 e o PCF8574
  configuraCinematica(); //configura e inicia a biblioteca responsável pelo controle dos motores
  configuraSerial(); //configura e inicia a comunicação serial com o ESP
  homing(); // efetua o homing do robô
}

int angulo = 0;

void loop() {
  loopBotoes();   //efetua a leitura dos botoes e faz os comando conforme as variaves definidas
  loopDisplay();  //efetua a leitura das variaveis de comando e exibe na tela
  loopCinematica(); //controla todo o sistema de posiconamento com base nas variaveis definidas
  loopSerialEsp(); //envia o comando para o esp atualizando os dados no modbus
  
  delay(1); //delay desnecessário mas incluido para reduzir bugs de ruido(funciona n sei pq).
}
