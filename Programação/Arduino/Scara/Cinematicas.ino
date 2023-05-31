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

byte selecaoCinematica = 0; // 0 = direta e 1 = inversa
long X = 0;
long Y = 0;
long Z = 0;
int  A = 0;
int  B = 0;
int  C = 0;
int  R = 0;
byte garra = 0;
int offset1 = 0;  //offset para o referenciamento
int offset2 = 0;  //offset para o referenciamento
int offset3 = 0;  //offset para o referenciamento
int offset4 = 0;  //offset para o referenciamento
int velocidadeMax1 = 300;
int velocidadeMax2 = 300;
int velocidadeMax3 = 300;
int velocidadeMax4 = 300;
int aceleracaoMax1 = 100;
int aceleracaoMax2 = 100;
int aceleracaoMax3 = 100;
int aceleracaoMax4 = 100;

//protótipos funções locais
void ajustaVelocidade(int nivel); //ajusta a velocidade das juntas de 0 a 100%
void configuraCinematica(); //colocado no setup ele configura os motores e seus valores iniciais
void loopCinematica(); //colocado no loop, realiza todas as funções que envolvem a cinemática
void testeMotorSerial(char dadodaserial); 
void homing();

extern void cinematicaDireta(int angA, int angB, int angC, long altura);
extern void cinematicainversa(long setX, long setY, int angR, long altura);

//protótipos funções externas

void configuraCinematica() {
  //Base
  Mot1.attach(m1stp, m1dir);  // STEPpin, DIRpin
  Mot1.setSpeed(200);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot1.setRampLen(70);       // 500 ms (rampa aceleração)

  //Cotovelo
  Mot2.attach(m2stp, m2dir);  // STEPpin, DIRpin
  Mot2.setSpeed(200);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot2.setRampLen(70);       // 500 ms (rampa aceleração)

  //Punho
  Mot3.attach(m3stp, m3dir);  // STEPpin, DIRpin
  Mot3.setSpeed(200);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot3.setRampLen(70);       // 500 ms (rampa aceleração)

  //Eixo Z
  Mot4.attach(m4stp, m4dir);  // STEPpin, DIRpin
  Mot4.setSpeed(200);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot4.setRampLen(70);       // 500 ms (rampa aceleração)
}

void homing() {
  //Referencia o motor da base
  //atualiza a velocidade de todos os motores para ser seguro efetuar o homing
  Mot1.setSpeed(100); //z
  Mot1.setRampLen(1);
  Mot2.setSpeed(100); //base
  Mot2.setRampLen(1);
  Mot3.setSpeed(100); //cotovelo
  Mot3.setRampLen(1);
  Mot4.setSpeed(50); //punho
  Mot4.setRampLen(1);

  //Verifica se nenhum fim de curso está pressionado e corrige caso estiver

  if (digitalRead(efim) == LOW) {
    //gira motor da base antihorário 100 passos
    Mot2.move(100);
    delay(3000);
    if (digitalRead(efim) == LOW) {
      //gira cotovelo sentido antihorário 100 passos
      Mot3.move(100);
      delay(3000);
      Serial.println("Moveu cotovelo");
      if (digitalRead(efim) == LOW) {
        //gira punho sentido antihorário -50 passos
        Mot4.move(-50);
        delay(3000);
        if (digitalRead(efim) == LOW) {
          //desce a base -200 passos
          Mot1.move(-200);
          delay(3000);
          if (digitalRead(efim) == LOW) {
            //deu Ruim
            Serial.println("Deu ruim");
            while (true) {
              delay(1000);
            }
          }
        }
      }
    }
  }
  //Referenciar a altura
  while (digitalRead(efim) == 1) {
    if (Mot1.moving() == false) {
      Mot1.move(10);
    }
    delay(1);
  }
  Mot1.stop();
  Mot1.setZero(offset1);
  delay(1000);
  Mot1.move(-300);
  delay(3000);
  //efetuar o zero do eixo Z

  //referenciar cotovelo
  while (digitalRead(efim) == 1) {
    if (Mot3.moving() == false) {
      Mot3.move(-10);
    }
    delay(1);
  }
  Mot3.stop();
  Mot3.setZero(offset3);
  delay(1000);
  Mot3.setSpeed(300);
  Mot3.setRampLen(100);
  Mot3.move(9000);
  delay(3000);
  //efetuar o zero do Cotovelo

  //referenciar Punho
  while (digitalRead(efim) == 1) {
    if (Mot4.moving() == false) {
      Mot4.move(10);
    }
    delay(1);
  }
  Mot4.stop();
  Mot4.setZero(offset4);
  delay(1000);
  Mot4.move(-150);
  delay(3000);
  //efetuar o zero do Punho

  //referenciar base
  while (digitalRead(efim) == 1) {
    if (Mot2.moving() == false) {
      Mot2.move(-10);
    }
    delay(1);
  }
  Mot2.stop();
  Mot2.setZero(offset2);
  delay(1000);
  Mot2.move(3000);
  delay(3000);
}

void loopCinematica() {
  if (selecaoCinematica == 0) {
    //cinematicaDireta();
  }
  if (selecaoCinematica == 1) {
    //cinematicaInversa();
  }
}
void ajustaVelhocidade(int nivel) {
  //velocidades préconfiguradas

  Mot1.setSpeed(map(nivel, 1, 100, 1, velocidadeMax1));     // = 80/20 = 4 U/Min (velocidade maxima)
  Mot1.setRampLen(map(nivel, 1, 100, 1, aceleracaoMax1));       // 100 ms (rampa aceleração)
  Mot2.setSpeed(map(nivel, 1, 100, 1, velocidadeMax2));     // = 80/20 = 4 U/Min (velocidade maxima)
  Mot2.setRampLen(map(nivel, 1, 100, 1, aceleracaoMax1));       // 100 ms (rampa aceleração)
  Mot3.setSpeed(map(nivel, 1, 100, 1, velocidadeMax3));     // = 80/20 = 4 U/Min (velocidade maxima)
  Mot3.setRampLen(map(nivel, 1, 100, 1, aceleracaoMax1));       // 100 ms (rampa aceleração)
  Mot4.setSpeed(map(nivel, 1, 100, 1, velocidadeMax4));     // = 80/20 = 4 U/Min (velocidade maxima)
  Mot4.setRampLen(map(nivel, 1, 100, 1, aceleracaoMax1));       // 100 ms (rampa aceleração)

}





void testeMotorSerial(char dadodaserial) {
  switch (dadodaserial) {
    case 'a':
      Mot1.move(500); // motor eixo z subindo
      break;
    case 'z':
      Mot1.move(-500);
      break;
    case 's':
      Mot2.move(100); //gira Base antihorario
      break;
    case 'x':
      Mot2.move(-100);
      break;
    case 'd':
      Mot3.move(100); // motor cotovelo sentido antihoraario
      break;
    case 'c':
      Mot3.move(-100);
      break;
    case 'f':
      Mot4.move(50); // motor punho sentido horario
      break;
    case 'v':
      Mot4.move(-100);
      break;
  }
}
