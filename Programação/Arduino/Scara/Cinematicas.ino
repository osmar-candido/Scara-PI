/* --------------------------------------------------------------------------------
    Funções referentes ao controle dos motores e os calculos de posicionamento
   -------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------
    Funçoes desejadas
 *    * Configurar entradas e saidas;
 *    * Parametrizar valores responsáveis pelo tempo de aceleração velocidade
        final e tempo de desaceleração do motor;
 *    * Calcular as cinematicas inversa e direta;
 *    * Efetuar o homing;
 *    * atualizar as variaveis da posição atual do braço e seu status
   -------------------------------------------------------------------------------- */
/*
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
*/

// declaração das funções

MoToStepper Mot1(800, STEPDIR);
MoToStepper Mot2(800, STEPDIR);
MoToStepper Mot3(800, STEPDIR);
MoToStepper Mot4(800, STEPDIR);


// Criação das variaveis

int selecaoCinematica = 0; // 0 = direta e 1 = inversa
long X = 0;
long Y = 0;
long Z = 0;
int  A = 0;
int  B = 0;
int  C = 0;
int  R = 0;
byte garra = 0;

//protótipos funções locais
void configuraCinematica();
void loopCinematica();
extern void cinematicaDireta(int angA, int angB, int angC, long altura);
extern void cinematicainversa(long setX, long setY, int angR, long altura);

//protótipos funções externas

void configuraCinematica() {
  //Base
  Mot1.attach(m1stp, m1dir);  // STEPpin, DIRpin
  Mot1.setSpeed(300);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot1.setRampLen(100);       // 500 ms (rampa aceleração)

  //Cotovelo
  Mot2.attach(m2stp, m2dir);  // STEPpin, DIRpin
  Mot2.setSpeed(300);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot2.setRampLen(100);       // 500 ms (rampa aceleração)
  
  //Punho
  Mot3.attach(m3stp, m3dir);  // STEPpin, DIRpin
  Mot3.setSpeed(300);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot3.setRampLen(100);       // 500 ms (rampa aceleração)
  
  //Eixo Z
  Mot3.attach(m4stp, m4dir);  // STEPpin, DIRpin
  Mot3.setSpeed(300);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot3.setRampLen(100);       // 500 ms (rampa aceleração)
}

void homing() {
  //Referencia o motor da base
  if (digitalRead(efim) == LOW) {
    Mot1.setZero();
    Mot1.move(200);
    delay(2000);
  }
  Mot1.setSpeed(50);
  Mot1.setRampLen(1);
  while (digitalRead(efim) == HIGH) {
    if (Mot1.moving() == false) {
      Mot1.move(-10);
    }
    delay(1);
  }
  Mot1.stop();
  Mot1.setZero(1444);
  Serial.println("Referenciado Eixo1 - Base");
  //calibrado1 = true;
  Mot1.setSpeed(300);
  Mot1.setRampLen(80);

  //Referencia motor dos cotovelo
  if (digitalRead(efim) == LOW) {
    Mot2.setZero();
    Mot2.move(200);
    delay(2000);
  }
  Mot3.move(-300);
  Mot2.setSpeed(100);
  Mot2.setRampLen(1);
  while (digitalRead(efim) == HIGH) {
    if (Mot2.moving() == false) {
      Mot2.move(-10);
    }
    delay(1);
  }
  Mot2.stop();
  Mot2.setZero(5389);
  Mot2.move(200);
  Serial.println("referenciado Eixo 2 - Cotovelo");
  //calibrado2 = true;
  Mot2.setSpeed(800);
  Mot2.setRampLen(400);
  //referencia motor do punho
  if (digitalRead(efim) == LOW) {
    Mot3.setZero();
    Mot3.move(-200);
    delay(2000);
  }
  Mot3.setSpeed(100);
  Mot3.setRampLen(1);
  while (digitalRead(efim) == HIGH) {
    if (Mot3.moving() == false) {
      Mot3.move(10);
    }
    delay(1);
  }
  Mot3.stop();
  Mot3.setZero(-3889);
  delay(1000);
  Serial.println("referenciado Eixo 3 - Punho");
  //calibrado3 = true;
  Mot3.setSpeed(300);
  Mot3.setRampLen(100);

  //referencia motor da altura
  if (digitalRead(efim) == LOW) {
    Mot4.setZero();
    Mot4.move(-200);
    delay(2000);
  }
  Mot4.setSpeed(100);
  Mot4.setRampLen(1);
  while (digitalRead(efim) == HIGH) {
    if (Mot4.moving() == false) {
      Mot4.move(10);
    }
    delay(1);
  }
  Mot4.stop();
  Mot4.setZero(-3889);
  delay(1000);
  Serial.println("referenciado Eixo 4 - Eixo Z");
  //calibrado4 = true;
  Mot4.setSpeed(300);
  Mot4.setRampLen(100);



}

void loopCinematica() {
  if(selecaoCinematica == 0){
    //cinematicaDireta();
  }
  if(selecaoCinematica == 1){
    //cinematicaInversa();
  }
}
void ajustaVelocidade() {
  //configuração MoBaTools
  Mot1.attach(m1stp, m1dir);  // STEPpin, DIRpin
  Mot1.setSpeed(300);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot1.setRampLen(100);       // 100 ms (rampa aceleração)
  Mot2.attach(m2stp, m2dir);  // STEPpin, DIRpin
  Mot2.setSpeed(300);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot2.setRampLen(100);       // 100 ms (rampa aceleração)
  Mot3.attach(m3stp, m3dir);  // STEPpin, DIRpin
  Mot3.setSpeed(300);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot3.setRampLen(100);       // 100 ms (rampa aceleração)
}
