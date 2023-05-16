#include <MobaTools.h>

MoToTimer Pause;
MoToStepper Mot1(800, STEPDIR);
MoToStepper Mot2(800, STEPDIR);
MoToStepper Mot3(800, STEPDIR);

#define m1stp 10   //saida para o step do drive do motor 1 
#define m2stp 11   //saida para o step do drive do motor 2 
#define m3stp 9   //saida para o step do drive do motor 3 
#define m1dir 8   //saida para o dir  do drive do motor 1 
#define m2dir 12   //saida para o dir  do drive do motor 2 
#define m3dir 3   //saida para o dir  do drive do motor 3 
#define m12en 5       //saida para o enable do drive do motor 1 e 2
#define m3en  6       //Saida para o enable do drive do motor 3
#define e1fim 2   //Entrada do fim de curso para calibração do eixo 1
#define e2fim 4   //Entrada do fim de curso para calibração do eixo 2
#define e3fim 7  //Entrada do fim de curso para calibração do eixo 3


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
