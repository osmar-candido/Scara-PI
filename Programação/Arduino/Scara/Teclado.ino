/* --------------------------------------------------------------------------------
    Funções referentes a leitura e manipulação das funções usando os botões
   -------------------------------------------------------------------------------- */

/* bibliotecas em uso:
    - MobaTools.h
    - Wire.h
    - Adafruit_MPR121.h
    -
*/

// Criação das variaveis

Adafruit_MPR121 capacitivo = Adafruit_MPR121();
MoToButtons botoes(leituraMPR121(), 20, 500);


void configuraBotoes();
void loopBotoes();

void configuraBotoes() {
  capacitivo.begin(0x5A);
}


void loopBotoes() {
  botoes.processButtons();
  for (byte i = 0; i < 16; i++) {
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
        if (buttonPad.shortPress(i)) {
          Serial.print("clique rapido: "); 
          Serial.println(i);
        }
        break;
    }
  }
}

button_t leituraMPR121( void ) {
  button_t leituraTemp = 0;
  leituraTemp = capacitivo.touched();
  return leituraTemp;
}
