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
  Junta da Base possui uma redução de     20:1
  Junta do Cotovelo possui uma redução de 16:1
  junta do Punho possui uma redução de     4:1

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

*/

// declaração das funções

MoToStepper Mot1(800, STEPDIR);
MoToStepper Mot2(800, STEPDIR);
MoToStepper Mot3(800, STEPDIR);
MoToStepper Mot4(800, STEPDIR);
MoToServo servo;

// Criação das variaveis

byte selecaoCinematica = 0; // 0 = direta e 1 = inversa

long X = 0;
long Y = 0;
long Z = 0;
int  A = -5; //altura Z
int  B = 0; //Base
int  C = 0; //Cotovelo
int  R = 0; //Rotação Ferramenta
byte F = 100; //abertura da Ferramenta
byte Vel = 100;

long rX = 0;
long rY = 0;
long rZ = 0;
int  rA = 0; //altura Z
int  rB = 0; //Base
int  rC = 0; //Cotovelo
int  rR = 0; //Rotação Ferramenta



#define offset1 -320        //offset para o referenciamento
#define offset2 6600        //offset para o referenciamento
#define offset3 5380        //offset para o referenciamento
#define offset4 -1450       //offset para o referenciamento
#define velocidadeMax1 300  //z
#define velocidadeMax2 400  //base
#define velocidadeMax3 400  //cotovelo
#define velocidadeMax4 300  //punho
#define aceleracaoMax1 200
#define aceleracaoMax2 200
#define aceleracaoMax3 100
#define aceleracaoMax4 100
#define limiteMaxBase 120        //°
#define limiteMaxCotovelo 150    //°
#define limiteMaxPunho 150       //°
#define limiteMaxAltura 0       //°
#define limiteMinBase -120       //°
#define limiteMinCotovelo -145   //°
#define limiteMinPunho -150      //°
#define limiteMinAltura -65     //mm
#define limiteMinFerramenta 0 //%
#define limiteMaxFerramenta 100   //%
#define limiteMinVelocidade 1   //%
#define limiteMaxVelocidade 100 //%



//protótipos funções locais
void ajustaVelocidade(int nivel); //ajusta a velocidade das juntas de 0 a 100%
void configuraCinematica(); //colocado no setup ele configura os motores e seus valores iniciais
void loopCinematica(); //colocado no loop, realiza todas as funções que envolvem a cinemática
void testeMotorSerial(char dadodaserial);
void homing();
void cinematicaDireta(int disA, int angB, int angC, long angR);
void cinematicaInversa(long setX, long setY, int angR, long altura);


extern void cinematicaDireta(int angA, int angB, int angC, long altura);
extern void cinematicainversa(long setX, long setY, int angR, long altura);
extern int registradores[20];

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

  //Garra
  servo.attach(16);
  servo.setSpeed(100);
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
      //Serial.println("Moveu cotovelo");
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
            //Serial.println("Deu ruim");
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
  ajustaVelocidade(100);
  Mot1.writeSteps(0);
  Mot2.writeSteps(0);
  Mot3.writeSteps(0);
  Mot4.writeSteps(0);
}

float distanciaBasePunho = 0.0;
float angInterno = 0.0;
void setGarra(int setPointGarra){
  servo.write(setPointGarra);
}
void cinematicaDireta(int disA, int angB, int angC, int angR) { //altura, base, cotovelo, punho

  //Verifica Limites de segurança
   
  if (angB <= limiteMaxBase && angB >= limiteMinBase) {
    Mot2.writeSteps(round(((800 * 20) / 360)* angB));  
  }
  if (angC <= limiteMaxCotovelo && angC >= limiteMinCotovelo) {
    Mot3.writeSteps(round(((800 * 16) / 360)* angC));  
  }
  if (angR <= limiteMaxPunho && angR >= limiteMinPunho) {
    Mot4.writeSteps(round(((800 * 4) / 360)* angR));  
  }
  if (disA <= limiteMaxAltura && disA >= limiteMinAltura) {
    Mot1.writeSteps(round(100 * disA));  
  }

  //Calcula a distancia entre a base e o Punho
  //float distanciaBasePunho = sqrt((distanciaBaseCotovelo ^ 2) + (distanciaCotoveloPunho ^ 2));
  //float distanciaBasePunho = sqrt(pow(distanciaBaseCotovelo, 2) + pow(distanciaCotoveloPunho, 2) - 2 * distanciaBaseCotovelo * distanciaCotoveloPunho * cos(angC+180));
  distanciaBasePunho = sqrt(pow(distanciaBaseCotovelo, 2) + pow(distanciaCotoveloPunho, 2) - (2 * distanciaBaseCotovelo * distanciaCotoveloPunho * cos(radians(angC + 180))));
  registradores[0] = round(distanciaBasePunho);
  //Calcula o angulo entre a hipotenusa e o elo Base:Cotovelo
  angInterno = acos((pow(distanciaBaseCotovelo, 2) + pow(distanciaBasePunho, 2) - (pow(distanciaCotoveloPunho, 2))) / (2 * (distanciaBaseCotovelo * distanciaBasePunho)));
  registradores[1] = round(degrees(angInterno));
  
  //se o cotovelo tiver angulo positivo e soma da base com o angulo interno for maior que 0 Situação = 3
  //se o cotovelo tiver angulo positivo e soma da base com o angulo interno for menor que 0 Situação = 1
  //se o cotovelo tiver angulo negativo e subtração da base com o angulo interno for maior que 0 Situação = 4
  //se o cotovelo tiver angulo negativo e subtração da base com o angulo interno for menor que 0 Situação = 2

  
  //situação 1
  if(angC >= 0 && (angB + degrees(angInterno)) < 0){
    registradores[4] = 1;
    rX = round(cos(radians(angB) + radians(90) + angInterno) * distanciaBasePunho);
    rY = round(sin(radians(angB) + radians(90) + angInterno) * distanciaBasePunho);
  }
  //situação 2
  if(angC < 0 && (angB - degrees(angInterno)) < 0){
    registradores[4] = 2;
    rX = round(cos(radians(angB) - angInterno + radians(90) ) * distanciaBasePunho);
    rY = round(sin(radians(angB) + radians(90) - angInterno) * distanciaBasePunho);
  }
  //situação 3
  if(angC >= 0 && (angB + degrees(angInterno)) >= 0){
    registradores[4] = 3;
    rX = round( cos(radians(180) - (radians(angB) + radians(90) + angInterno)) * distanciaBasePunho * (-1));
    rY = round(sin(radians(180) - (radians(angB) + radians(90) + angInterno)) * distanciaBasePunho);
  }
  //situação 4
  if(angC < 0 && (angB - degrees(angInterno)) >= 0){
    registradores[4] = 4;
    rX = round(cos(radians(180)-(radians(angB) + radians(90) - angInterno)) * distanciaBasePunho * (-1));
    rY = round(sin(radians(180)-(radians(angB) + radians(90) - angInterno)) * distanciaBasePunho);
  }
  registradores[8] = rX;
  registradores[9] = rY;
  registradores[3] = disA * -1;
  rZ = disA * 100;
  rR = rX + rY;
}

void cinematicaInversa(long setX, long setY, int angR, long altura) {
  if(setX == 0){ //o valor não pode ser 0 devido aos calculos
    setX = 1;
  }
  if(setY == 0){ //o valor não pode ser 0 devido aos calculos
    setY = 1;
  }  
  float distanciaBasePunho = sqrt(pow(setX, 2) + pow(setY, 2));
  float angOrigemBase = degrees(atan(setY/sqrt(pow(setX,2)))); //radianos
  
  float angInternoCotovelo = degrees(acos(( pow(distanciaBaseCotovelo, 2) + pow(distanciaCotoveloPunho, 2) - pow(distanciaBasePunho, 2)) / (2 * distanciaBaseCotovelo * distanciaCotoveloPunho)));
  float angInternoBase = degrees(acos(( pow(distanciaBaseCotovelo, 2) + pow(distanciaBasePunho, 2) - pow(distanciaCotoveloPunho, 2)) / (2 * distanciaBaseCotovelo * distanciaBasePunho)));
  float angInternoFerramenta = 180 - angInternoBase - angInternoCotovelo;
  float sol1_Base;
  float sol1_Cotovelo;
  float sol1_Punho;
  float sol2_Base;
  float sol2_Cotovelo;
  float sol2_Punho;
  

  if(setX > 0){
    //solução 1 X positivo
    //angulo Base
    sol1_Base = angOrigemBase - angInternoBase - 90; 
    //angulo Cotovelo
    sol1_Cotovelo = 180 - angInternoCotovelo;
    //angulo Punho
    sol1_Punho = angR - angInternoFerramenta - degrees(atan(setX/setY)) - 90;
    //solução 2 X positivo
    //angulo Base
    sol2_Base = angOrigemBase + angInternoBase - 90;
    //angulo Cotovelo
    sol2_Cotovelo = 0 - angInternoCotovelo;
    //angulo Punho
    sol2_Punho = angR + angInternoFerramenta + degrees(atan(setX/setY)) - 90;
  }else{
    //solução 3 X negativo
    //angulo Base
    sol1_Base = 90 - angOrigemBase - angInternoBase - 90; 
    //angulo Cotovelo
    sol1_Cotovelo = 0 - angInternoCotovelo;
    //angulo Punho
    sol1_Punho = angR + angInternoFerramenta + degrees(atan((setX * -1)/setY)) - 90;

    //solução 4 X negativo
    //angulo Base
    sol2_Base = 90 - angOrigemBase + angInternoBase - 90; 
    //angulo Cotovelo
    sol2_Cotovelo = 180 - angInternoCotovelo;
    //angulo Punho
    sol2_Punho = angR - angInternoFerramenta - degrees(atan((setX * -1)/setY)) - 90;
  }
  
  //avalia qual das 2 soluções é a mais proxima 
  //é possivel definir de 2 formas, pela soma de todos os passos ou pela soma do que necessita de mais passos
  long passos[4] = {0, 0, 0, 0};
  long comparador[2] = {0, 0}; 
  
  /*if(Mot1.readSteps() >= round(100 * altura)){ // passos Z não preciso saber disso pois é igual para ambos
    passos[0] = Mot1.readSteps() - round(100 * altura);
  }else{
    passos[0] = round(100 * altura) - Mot1.readSteps(); 
  }*/

  if(Mot2.readSteps() >= round(((800 * 20) / 360) * sol1_Base)){ // Passos na Base
    passos[1] = Mot2.readSteps() - round(((800 * 20) / 360) * sol1_Base);
  }else{
    passos[1] = round(((800 * 20) / 360) * sol1_Base) - Mot2.readSteps(); 
  }

  if(Mot3.readSteps() >= round(((800 * 16) / 360) * sol1_Cotovelo)){ // Passos no Cotovelo
    passos[2] = Mot3.readSteps() - round(((800 * 16) / 360) * sol1_Cotovelo);
  }else{
    passos[2] = round(((800 * 16) / 360) * sol1_Cotovelo) - Mot3.readSteps(); 
  }

  if(Mot4.readSteps() >= round(((800 * 4) / 360) * sol1_Punho)){
    passos[3] = Mot4.readSteps() - round(((800 * 4) / 360) * sol1_Punho);
  }else{
    passos[3] = round(((800 * 4) / 360) * sol1_Punho) - Mot4.readSteps(); 
  }

  if(passos[1] >= passos[2] && passos[1] >= passos[3]){
    comparador[0] = passos[1];
  }else if(passos[2] >= passos[3]){
    comparador[0] = passos[2];
  }else{
    comparador[0] = passos[3];
  }

  //aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

  if(Mot2.readSteps() >= round(((800 * 20) / 360) * sol2_Base)){ // Passos na Base
    passos[1] = Mot2.readSteps() - round(((800 * 20) / 360) * sol2_Base);
  }else{
    passos[1] = round(((800 * 20) / 360) * sol2_Base) - Mot2.readSteps(); 
  }

  if(Mot3.readSteps() >= round(((800 * 16) / 360) * sol2_Cotovelo)){ // Passos no Cotovelo
    passos[2] = Mot3.readSteps() - round(((800 * 16) / 360) * sol2_Cotovelo);
  }else{
    passos[2] = round(((800 * 16) / 360) * sol2_Cotovelo) - Mot3.readSteps(); 
  }

  if(Mot4.readSteps() >= round(((800 * 4) / 360) * sol2_Punho)){
    passos[3] = Mot4.readSteps() - round(((800 * 4) / 360) * sol2_Punho);
  }else{
    passos[3] = round(((800 * 4) / 360) * sol2_Punho) - Mot4.readSteps(); 
  }

  if(passos[1] >= passos[2] && passos[1] >= passos[3]){
    comparador[1] = passos[1];
  }else if(passos[2] >= passos[3]){
    comparador[1] = passos[2];
  }else{
    comparador[1] = passos[3];
  }

  if(comparador[0]>=comparador[1]){
    //solução 1
    rA = altura; //altura
    rB = sol1_Base; //base
    rC = sol1_Cotovelo; //cotovelo
    rR = sol1_Punho; //punho
  }else{
    //solução 2
    rA = altura; //altura
    rB = sol2_Base; //base
    rC = sol2_Cotovelo; //cotovelo
    rR = sol2_Punho; //punho
  }

  //escrita nos motores
  if (rB <= limiteMaxBase && rB >= limiteMinBase) {
    registradores[0] = rB;
    Mot2.writeSteps(round(((800 * 20) / 360)* rB));  
  }
  if (rC <= limiteMaxCotovelo && rC >= limiteMinCotovelo) {
    registradores[1] = rC;
    Mot3.writeSteps(round(((800 * 16) / 360)* rC));  
  }
  if (rR <= limiteMaxPunho && rR >= limiteMinPunho) {
    registradores[2] = rR;
    Mot4.writeSteps(round(((800 * 4) / 360)* rR));  
  }
  if (altura <= limiteMaxAltura && altura >= limiteMinAltura) {
    registradores[3] = altura;
    Mot1.writeSteps(round(100 * altura));  
  }
}

void loopCinematica() {
  //caso o controle for via Cinemática Direta
  if (selecaoCinematica == 0) {
    cinematicaDireta(A, B, C, R);
  }
  //caso o controle for via Cinemática Inversa
  if (selecaoCinematica == 1) {
    cinematicaInversa(X, Y, R, Z);
  }
  //Se o motor estiver em movimento
  if(Mot1.moving() || Mot2.moving() || Mot3.moving() || Mot4.moving()){
    registradores[7] = 1;
  }else{
    registradores[7] = 0;
  }
}





void ajustaVelocidade(int nivel) { //efetua a configuração da velocidade das juntas de forma proporcional
  if(nivel < 10){
    nivel = 10;
  }
  registradores[5] = nivel;
  Mot1.setSpeed(map(nivel, 1, 100, 1, velocidadeMax1));     // = 80/20 = 4 U/Min (velocidade maxima)
  Mot1.setRampLen(map(nivel, 1, 100, 1, aceleracaoMax1));       // 100 ms (rampa aceleração)
  Mot2.setSpeed(map(nivel, 1, 100, 1, velocidadeMax2));     // = 80/20 = 4 U/Min (velocidade maxima)
  Mot2.setRampLen(map(nivel, 1, 100, 1, aceleracaoMax1));       // 100 ms (rampa aceleração)
  Mot3.setSpeed(map(nivel, 1, 100, 1, velocidadeMax3));     // = 80/20 = 4 U/Min (velocidade maxima)
  Mot3.setRampLen(map(nivel, 1, 100, 1, aceleracaoMax1));       // 100 ms (rampa aceleração)
  Mot4.setSpeed(map(nivel, 1, 100, 1, velocidadeMax4));     // = 80/20 = 4 U/Min (velocidade maxima)
  Mot4.setRampLen(map(nivel, 1, 100, 1, aceleracaoMax1));       // 100 ms (rampa aceleração)
}


int getData(int dataIndex){
  switch (dataIndex){
  case 0: //retorna a posição atual da base
    return 0;
    break;
  case 1:
    return 0;
    break;
  default:
    return 0;
    break;
  }
}
/*void testeMotorSerial(char dadodaserial) {
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
    case 'u':
      //Serial.print("Motor 1: ");
      //Serial.println(Mot1.readSteps()); // motor cotovelo sentido antihoraario
      break;
    case 'j':
      //Serial.print("Motor 2: ");
      // Serial.println(Mot2.readSteps());
      break;
    case 'i':
      // Serial.print("Motor 3: ");
      //Serial.println(Mot3.readSteps()); // motor punho sentido horario
      break;
    case 'k':
      ///Serial.print("Motor 4: ");
      //Serial.println(Mot4.readSteps());
      break;
  }
}*/
