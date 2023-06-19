/* --------------------------------------------------------------------------------
    Funções referentes a leitura e manipulação das funções usando os botões
   -------------------------------------------------------------------------------- */

/* bibliotecas em uso:
    - MobaTools.h
    - Wire.h
    - Adafruit_MPR121.h
    -
*/
#define PCF8574_ADDRESS 0x20 //endereço do pcf8574 das entradas
#define switchModo  0 //switch para alternar entre modo local e remoto
#define capModo     6 //toque capacitivo do switch
#define capCima     7 //toque capacitivo cima
#define capBaixo    8 //
#define capEsquerda 11 //
#define capDireita  10 //
#define capCentro   9 //
#define capExtra1   5 //toque capacitivo do botão preto da esquerda
#define capExtra2   4 //toque capacitivo do botão preto da direita

// Criação das variaveis
byte funcao = 0;
byte multiplicador = 1;
extern byte telaPrioridades;
extern byte modo; // se igual a 0 comando local, se igual a 1 comando via modbus

extern long X;
extern long Y;
extern long Z;
extern int  A; //altura Z
extern int  B; //Base
extern int  C; //Cotovelo
extern int  R; //Rotação Ferramenta
extern byte F; //abertura da Ferramenta
extern byte Vel;

extern void cinematicaDireta(int disA, int angB, int angC, long angR);
extern byte selecaoCinematica;
extern long buf[8];
void configuraBotoes();
void loopBotoes();
button_t leituraMPR121(void);
byte lastPCF = B00000000;
uint16_t lastMPR = 0;
Adafruit_MPR121 capacitivo = Adafruit_MPR121();

void configuraBotoes() {
  capacitivo.begin(0x5A);
  Wire.begin();
}


void loopBotoes() { //usa em conjunto a biblioteca mobatools para as funções dos botoes e a biblioteca do MPR121 para o toque capacitivo
  //botoes.processButtons();

  byte getPCF = lerEntradasPCF8574();
  uint16_t getMPR = lerMPR121();

  //verifica se o switch teve o toque capacitivo e mostra a tela


  if (bitRead(getPCF, switchModo) != bitRead(lastPCF, switchModo)) {
    if (bitRead(getPCF, switchModo) == 1) {
      modo = 1;
      funcao = 0;
      //Serial.println("Modo Remoto");
    } else {
      //Serial.println("Modo Local");
      modo = 0;
    }
  }
  if (bitRead(getMPR, capModo) != bitRead(lastMPR, capModo)) {
    if (bitRead(getMPR, capModo) == 1) {
      bitWrite(telaPrioridades, 0, 1);
      //Serial.println("Modo Pressionado");
    } else {
      //Serial.println("Modo Solto");
      bitWrite(telaPrioridades, 0, 0);
      buf[0] = 300;
      buf[1] = 300;
      buf[2] = 300;
      buf[3] = 300;
    }
  }
  if (bitRead(getMPR, capCima) != bitRead(lastMPR, capCima)) {
    if (bitRead(getMPR, capCima) == 1) {
      //Serial.println("Cima Pressionado");
      if(modo == 0){
        switch (funcao){
          case 1:
            if(selecaoCinematica == 0){ //direta
              A = A + (1*multiplicador);
            }else{ //inversa
              Y = Y + (1*multiplicador);
            }
          break;
          case 2:
            if(selecaoCinematica == 0){ //direta
              C = C + (1*multiplicador);
            }else{ //inversa
              Z = Z + (1*multiplicador);
            }
          break;
          case 3:
            if(selecaoCinematica == 0){ //direta
              Vel = Vel + (1*multiplicador);
            }else{ //inversa
              Vel = Vel + (1*multiplicador);
            }
          break;
        }
      }
    } else {
      //Serial.println("Cima Solto");
    }
  }
  if (bitRead(getMPR, capBaixo) != bitRead(lastMPR, capBaixo)) {
    if (bitRead(getMPR, capBaixo) == 1) {
      //Serial.println("Baixo Pressionado");
      if(modo == 0){
        switch (funcao){
          case 1:
            if(selecaoCinematica == 0){ //baixo
              A = A - (1*multiplicador);
            }else{ //inversa
              Y = Y - (1*multiplicador);
            }
          break;
          case 2:
            if(selecaoCinematica == 0){ //baixo
              C = C - (1*multiplicador);
            }else{ //inversa
              Z = Z - (1*multiplicador);
            }
          break;
          case 3:
            if(selecaoCinematica == 0){ //baixo
              Vel = Vel - (1*multiplicador);
            }else{ //inversa
              Vel = Vel - (1*multiplicador);
            }
          break;
        }
      }
    } else {
      //Serial.println("Baixo Solto");
    }
  }
  if (bitRead(getMPR, capEsquerda) != bitRead(lastMPR, capEsquerda)) {
    if (bitRead(getMPR, capEsquerda) == 1) {
      //Serial.println("Esquerda Pressionado");
      if(modo == 0){
        switch (funcao){
          case 1:
            if(selecaoCinematica == 0){ //esquerda
              B = B - (1*multiplicador);
            }else{ //inversa
              X = X - (1*multiplicador);
            }
          break;
          case 2:
            if(selecaoCinematica == 0){ //esquerda
              R = R - (1*multiplicador);
            }else{ //inversa
              R = R - (1*multiplicador);
            }
          break;
          case 3:
            if(selecaoCinematica == 0){ //esquerda
              F = F - (1*multiplicador);
            }else{ //inversa
              F = F - (1*multiplicador);
            }
          break;
        }
      }
    } else {
      //Serial.println("Esquerda Solto");
    }
  }
  if (bitRead(getMPR, capDireita) != bitRead(lastMPR, capDireita)) {
    if (bitRead(getMPR, capDireita) == 1) {
      //Serial.println("Direita Pressionado");
      if(modo == 0){
        switch (funcao){
          case 1:
            if(selecaoCinematica == 0){ //direta
              B = B + (1 * multiplicador);
            }else{ //inversa
              X = X + (1*multiplicador);
            }
          break;
          case 2:
            if(selecaoCinematica == 0){ //direta
              R = R + (1*multiplicador);
            }else{ //inversa
              R = R + (1*multiplicador);
            }
          break;
          case 3:
            if(selecaoCinematica == 0){ //direta
              //F = F + (1*multiplicador);
            }else{ //inversa
              //F = F + (1*multiplicador);
            }
          break;
        }
      }
    } else {
      //Serial.println("Direita Solto");
    }
  }
  if (bitRead(getMPR, capCentro) != bitRead(lastMPR, capCentro)) {
    if (bitRead(getMPR, capCentro) == 1) {
      //Serial.println("Centro Pressionado");
      if(funcao == 3){
        funcao = 0; 
        //0 tela normal
        //1 edita altura(cima Baixo) e base(esquerda Direita) em direto
        //1 edita X(esquerda Direita) e Y(cima Baixo) em inverso

        //2 edita cotovelo(cima Baixo) e punho(esquerda Direita) em direto
        //2 edita Z(cima Baixo) e punho(esquerda Direita) em inverso

        //3 edita ferramenta(esquerda Direita) Velocidade(Cima Baixo) Direto
        //3 edita ferramenta(esquerda Direita) Velocidade(Cima Baixo) Direto
        
      }else{
        funcao = funcao + 1;
      }
    } else {
      //Serial.println("Centro Solto");
    }
  }
  if (bitRead(getMPR, capExtra1) != bitRead(lastMPR, capExtra1)) {
    if (bitRead(getMPR, capExtra1) == 1) {
      //Serial.println("Extra1 Pressionado");
      if(selecaoCinematica == 0){
        selecaoCinematica = 1;
      }else{
        selecaoCinematica = 0;
      }
    } else {
      //Serial.println("Extra1 Solto");
    }
  }
  if (bitRead(getMPR, capExtra2) != bitRead(lastMPR, capExtra2)) {
    if (bitRead(getMPR, capExtra2) == 1) {
      //Serial.println("Extra2 Pressionado");
      //selecaoCinematica = 1;
      if(multiplicador == 1){
        multiplicador = 5;
      }else if(multiplicador == 5){
        multiplicador = 10;
      }else if(multiplicador == 10){
        multiplicador = 1;
      }
    } else {
      //Serial.println("Extra2 Solto");
    }
  }

  lastMPR = getMPR;
  lastPCF = getPCF; //atualiza a variavel com o ultimo valor lido
  //Serial.print(testepcf, BIN);
  //Serial.print("   ");
  //Serial.println(capacitivo.touched(), BIN);
}

uint16_t lerMPR121() {
  uint16_t leituraTemp = 0;
  leituraTemp = capacitivo.touched();
  return leituraTemp;
}

byte lerEntradasPCF8574() {
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.requestFrom(PCF8574_ADDRESS, 1);  // Solicita 1 byte do PCF8574
  if (Wire.available()) {
    byte status = Wire.read();  // Lê o byte recebido do PCF8574
    return status;
  }
  return B00000000;  // Retorna 0 caso não tenha sido possível ler os dados
}
