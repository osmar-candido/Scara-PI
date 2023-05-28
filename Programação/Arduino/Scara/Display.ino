/* --------------------------------------------------------------------------------
    Funções referentes a seleção de telas e configuração do display
   -------------------------------------------------------------------------------- */

// Inclusão da bibliotecas
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Variaveis e declarações

LiquidCrystal_I2C lcd(0x27, 16, 2); // endereço do LCD em 0x27 16 colunas 2 linhas

int tela_A = 0; // referente a tela principal

void configuraDisplay();
void loopDisplay();

void configuraDisplay() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0); //coluna,linha
  lcd.print(" Projeto  Scara ");
  lcd.setCursor(0, 1);
  lcd.print(" Thomas e Osmar ");
}

void loopDisplay() {
  switch(tela_A){
    case 0:
        
    break; 
    case 1:
    break;
    case 2:
    break;
    case 3:
    break;
    case 4:
    break;
    case 5:
    break;
    case 6:
    break;
    case 7:
    break;
  }
}
