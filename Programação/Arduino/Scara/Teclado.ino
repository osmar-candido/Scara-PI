/* --------------------------------------------------------------------------------
    Funções referentes a leitura e manipulação das funções usando os botões
   -------------------------------------------------------------------------------- */

/* bibliotecas em uso:
    - MobaTools.h
    - Wire.h
    - Adafruit_MPR121.h
    -
*/
#define PCF8574_ADDRESS 0x20
// Criação das variaveis



void configuraBotoes();
void loopBotoes();
button_t leituraMPR121(void);

Adafruit_MPR121 capacitivo = Adafruit_MPR121();
//MoToButtons botoes(leituraMPR121(), 20, 500);



void configuraBotoes() {
  capacitivo.begin(0x5A);
  Wire.begin();
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

void loopBotoes() { //usa em conjunto a biblioteca mobatools para as funções dos botoes e a biblioteca do MPR121 para o toque capacitivo
  //botoes.processButtons();
  byte testepcf = lerEntradasPCF8574();
  Serial.print(testepcf, BIN);
  Serial.print("   ");
  Serial.println(capacitivo.touched(), BIN);
  /*for (byte i = 0; i < 16; i++) {
    // verifica todos os 16 botoes
    if ( botoes.longPress(i) ) {
      Serial.print("clique longo: ");
      Serial.println(i);
    }
    switch ( botoes.clicked(i) ) {
      case SINGLECLICK :
        Serial.print("clique unico: ");
        Serial.println(i);
        break;
      case DOUBLECLICK :
        Serial.print("clique duplo: ");
        Serial.println(i);
        break;
      case NOCLICK:
        // a very short press is also a click!
        // usually you have to decide wether to check for a click or a short press
        if (botoes.shortPress(i)) {
          Serial.print("clique rapido: ");
          Serial.println(i);
        }
        break;
    }

    }*/
}

button_t leituraMPR121(void) {
  button_t leituraTemp = 0;
  leituraTemp = capacitivo.touched();
  return leituraTemp;
}
