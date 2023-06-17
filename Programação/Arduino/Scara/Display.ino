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
long buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
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
extern byte F;  //recebe o valor tratato de 0 a 100%
extern byte modo;   //0 = local(IHM/Serial) || 1 = remoto(ModbusTCP)
extern byte funcao;
extern byte multiplicador;

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

  //tela_A = tela_A + (funcao * 5);
  if(selecaoCinematica == 0){
    if(funcao == 1){
      tela_A = 5;
    }
    if(funcao == 2){
      tela_A = 6;
    }
    if(funcao == 3){
      tela_A = 7;
    }
  }else{
    if(funcao == 1){
      tela_A = 10;
    }
    if(funcao == 2){
      tela_A = 11;
    }
    if(funcao == 3){
      tela_A = 12;
    }
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
        break;
      case 5: //tela mostrando a funcao 1
        lcd.setCursor(0, 0);
        lcd.print("   Altur:        ");
        lcd.setCursor(0, 1);
        lcd.print("   Base:         ");
        buf[0] = 1000;
        buf[1] = 1000;
        buf[2] = 1000;
        buf[3] = 1000;
        buf[4] = 1000;
        buf[5] = 1000;
        buf[6] = 1000;
        buf[7] = 1000;
        buf[8] = 
        //buf[5] = 0;
        break;
      case 6: //tela mostrando a funcao 2
        lcd.setCursor(0, 0);
        lcd.print("   Cotov:        ");
        lcd.setCursor(0, 1);
        lcd.print("   Punho:        ");
        buf[5] = 0;
        break;
      case 7: //tela mostrando a funcao 3
        lcd.setCursor(0, 0);
        lcd.print("   Garra:        ");
        lcd.setCursor(0, 1);
        lcd.print("   Vel..:        ");
        buf[5] = 0;
        break;
      case 10: //tela mostrando a funcao 1
        lcd.setCursor(0, 0);
        lcd.print("   Y:            ");
        lcd.setCursor(0, 1);
        lcd.print("   X:            ");
        buf[5] = 0;
        break;
      case 11: //tela mostrando a funcao 2
        lcd.setCursor(0, 0);
        lcd.print("   Z:            ");
        lcd.setCursor(0, 1);
        lcd.print("   Punho:        ");
        buf[5] = 0;
        break;
      case 12: //tela mostrando a funcao 3
        lcd.setCursor(0, 0);
        lcd.print("   Garra:        ");
        lcd.setCursor(0, 1);
        lcd.print("   Vel..:        ");
        buf[5] = 0;
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
      if (buf[0] != F) {
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(5, 1);
        lcd.print(F);
        lcd.write(B00100101); //escreve o simbolo de %
      }
      buf[0] = F;
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
      break;
    case 5:
      if (buf[0] != A) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 0);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print(A);
        buf[0] = A;
      }
      if (buf[1] != B) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 1);
        lcd.print(B);
        buf[1] = B;
      }
      if (buf[5] != multiplicador) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(0, 1);
        lcd.print("   ");
        lcd.setCursor(0, 1);
        lcd.print(multiplicador);
        buf[5] = multiplicador;
      }
    break; 
    case 6:
      if (buf[0] != C) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 0);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print(C);
        buf[0] = C;
      }
      if (buf[1] != R) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 1);
        lcd.print(R);
        buf[1] = R;
      }
      if (buf[5] != multiplicador) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(0, 1);
        lcd.print("   ");
        lcd.setCursor(0, 1);
        lcd.print(multiplicador);
        buf[5] = multiplicador;
      }
    break; 
    case 7:
      if (buf[0] != X) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 0);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print("sla");
        buf[0] = X;
      }
      if (buf[1] != Vel) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 1);
        lcd.print(Vel);
        buf[1] = Vel;
      }
      if (buf[5] != multiplicador) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(0, 1);
        lcd.print("   ");
        lcd.setCursor(0, 1);
        lcd.print(multiplicador);
        buf[5] = multiplicador;
      }
    break; 
    case 10:
      if (buf[0] != Y) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 0);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print(Y);
        buf[0] = Y;
      }
      if (buf[1] != X) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 1);
        lcd.print(X);
        buf[1] = X;
      }
      if (buf[5] != multiplicador) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(0, 1);
        lcd.print("   ");
        lcd.setCursor(0, 1);
        lcd.print(multiplicador);
        buf[5] = multiplicador;
      }
    break; 
    case 11:
      if (buf[0] != Z) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 0);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print(Z);
        buf[0] = Z;
      }
      if (buf[1] != R) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 1);
        lcd.print(R);
        buf[1] = R;
      }
      if (buf[5] != multiplicador) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(0, 1);
        lcd.print("   ");
        lcd.setCursor(0, 1);
        lcd.print(multiplicador);
        buf[5] = multiplicador;
      }
    break; 
    case 12:
      if (buf[0] != X) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 0);
        lcd.print("     ");
        lcd.setCursor(10, 0);
        lcd.print("Sla");
        buf[0] = X;
      }
      if (buf[5] != multiplicador) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(0, 1);
        lcd.print("   ");
        lcd.setCursor(0, 1);
        lcd.print(multiplicador);
        buf[5] = multiplicador;
      }
      if (buf[1] != Vel) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(10, 1);
        lcd.print("     ");
        lcd.setCursor(10, 1);
        lcd.print(Vel);
        buf[1] = Vel;
      }
      if (buf[5] != multiplicador) { //verifica se houve alteração na variavel e reescreve >> serve para todos os proximos
        lcd.setCursor(0, 1);
        lcd.print("   ");
        lcd.setCursor(0, 1);
        lcd.print(multiplicador);
        buf[5] = multiplicador;
      }
    break;  
  }
}
