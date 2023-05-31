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
#define switchModo   7  //switch para alternar entre modo local e remoto
#define capModo    7    //toque capacitivo do switch
#define capCima    0    //toque capacitivo cima
#define capBaixo   1
#define capEsqueda 2
#define capDireita 3
#define capCentro  4
#define capExtra1  5    //toque capacitivo do botão preto da esquerda
#define capExtra2  6    //toque capacitivo do botão preto da direita

// Criação das variaveis
extern byte telaPrioridades;
extern byte modo;

void configuraBotoes();
void loopBotoes();
button_t leituraMPR121(void);

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
  if (bitRead(getMPR, capModo) == 1) {
    bitWrite(telaPrioridades, 0, 1); //seta o bit 0 em 1
  } else {
    bitWrite(telaPrioridades, 0, 0); //seta o bit 0 em 0
  }
  if(bitRead(getPCF,0) == 1){
    modo = 1;
  }else{
    modo = 0;
  }
  

  Serial.print(testepcf, BIN);
  Serial.print("   ");
  Serial.println(capacitivo.touched(), BIN);


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
