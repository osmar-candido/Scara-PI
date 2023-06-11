/* --------------------------------------------------------------------------------
    Funções referentes a seleção de telas e configuração do display
   -------------------------------------------------------------------------------- */
//Comunicação Testada

// Inclusão da bibliotecas
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Variaveis e declarações

LiquidCrystal_I2C lcd(0x22, 16, 2); // endereço do LCD em 0x27 16 colunas 2 linhas

//variaveis locais
int tela_A = 0; // referente a tela principal
byte grau = B11011111;
long buf[4] = {0, 0, 0, 0};
int prev_tela_A = 1000;

//aqui tem 8 variaveis de prioridades, as quais podem interromper o funcionamento norma do display
//primeiro bit é referente a tela da seleção do modo (local ou remoto)
byte telaPrioridades = B00000000;


//variaveis externas
extern byte selecaoCinematica; //0 = direta e 1 = inversa
extern long X;      //recebe o valor tratato de -99999 a 10000 milimetros
extern long Y;      //recebe o valor tratato de -99999 a 10000 milimetros
extern long Z;      //recebe o valor tratato de -99999 a 10000 milimetros
extern int  A;      //recebe o valor tratato de -360 a 360 graus
extern int  B;      //recebe o valor tratato de -360 a 360 graus
extern int  C;      //recebe o valor tratato de -360 a 360 graus
extern int  R;      //recebe o valor tratato de -360 a 360 graus
extern byte garra;  //recebe o valor tratato de 0 a 100%
extern byte modo;   //0 = local(IHM/Serial) || 1 = remoto(ModbusTCP)

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
  if (bitRead(telaPrioridades, 0) == 1) {
    tela_A = 3;
  } else {
    tela_A = 1;
  }

  if (tela_A != prev_tela_A) { //verifica se necessita mudar o plano de fundo
    lcd.clear(); //na teoria não sera necessário
    prev_tela_A = tela_A; // essa variavel serve para evitar atualização desnecessária do display
    switch (tela_A) { //insere o plano de fundo da tela
      case 0: //tela inicial quando no modo cin. inversa
        lcd.setCursor(0, 0);
        lcd.print("X:      Y:      ");
        lcd.setCursor(0, 1);
        lcd.print("Z:      R:      ");
        break;
      case 1: //tela inicial quando no modo cin. direta
        lcd.setCursor(0, 0);
        lcd.print("A:      B:      ");
        lcd.setCursor(0, 1);
        lcd.print("R:      C:      ");
        break;
      case 2: //tela mostrando a situação da garra
        lcd.setCursor(0, 0);
        lcd.print("     Garra:     ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        break;
      case 3: //tela mostrando a situação local ou remota
        lcd.setCursor(0, 0);
        lcd.print("Modo de Operacao");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        buf[0] = 100;
        break;
    }
  }
  switch (tela_A) { // atualiza os dados a serem exibidos
    case 0:
      if (buf[0] != X) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(2, 0);
        lcd.print("     ");
        lcd.setCursor(2, 0);
        lcd.print(X);
      }
      if (buf[1] != Y) {
        lcd.setCursor(10, 0);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print(Y);
      }
      if (buf[2] != Z) {
        lcd.setCursor(2, 1);
        lcd.print("     ");
        lcd.setCursor(2, 1);
        lcd.print(Z);
      }
      if (buf[3] != R) {
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 1);
        lcd.print(R);
      }
      buf[0] = X;
      buf[1] = Y;
      buf[2] = Z;
      buf[3] = R; // atualiza as variaveis para aferir se houve mudança
      break;
    case 1:
      if (buf[0] != A) {
        lcd.setCursor(2, 0);
        lcd.print("     ");
        lcd.setCursor(2, 0);
        lcd.print(A);
      }
      if (buf[1] != B) {
        lcd.setCursor(10, 0);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print(B);
      }
      if (buf[2] != R) {
        lcd.setCursor(2, 1);
        lcd.print("     ");
        lcd.setCursor(2, 1);
        lcd.print(R);
      }
      if (buf[3] != C) {
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 1);
        lcd.print(C);
      }
      buf[0] = A;
      buf[1] = B;
      buf[2] = R;
      buf[3] = C;
      break;
    case 2:
      if (buf[0] != garra) {
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(5, 1);
        lcd.print(garra);
        lcd.write(B00100101); //escreve o simbolo de %
      }
      buf[0] = garra;
      buf[1] = 0;
      buf[2] = 0;
      buf[3] = 0;
      break;
    case 3:
      if (buf[0] != modo) {
        if (modo == 0) { //Operacao local
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print("Local IHM/Serial");
        }
        if (modo == 1) { //Operacao Remota
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print("Remoto ModbusTCP");

        }
        buf[0] = modo;
      }
  }
}
