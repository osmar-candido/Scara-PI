/* --------------------------------------------------------------------------------
    Funções referentes a escrita Serial com o computador e o ESP
   -------------------------------------------------------------------------------- */

// Inclusão da bibliotecas
//#include <SoftwareSerial.h>

SoftwareSerial Seria(espTx, espRx); // RX, TX do arduino

// Criação das variaveis

char chegadaSerial;
int situacao = 0; //em que etapa esta do recebimento de dados
String dado = ""; //dado recebido
String endereco = ""; //endereço da memoria a qual sera modificada
String verificador = ""; //valor para identificar se os dados recebidos estão corretos
int registradores[20]; //registro onde sera armazenado as informações a serem trocadas via modbus


void configuraSerial(); // aqui inicia-se as seriais tanto com o pc como tambem com o ESP
void loopSerialEsp(); // função a ser inserida no loop que efetua as funções de comunicação com o ESP


void configuraSerial() {
  Serial.begin(9600);
  Seria.begin(9600);
}

void loopSerialEsp() {
  leSerialEsp();
  escreveSerialEsp();
}

void leSerialEsp() {
  while (Seria.available() > 0) { // verifica se existe algum dado disponivel
    chegadaSerial = Seria.read(); // armazena o primeiro byte
    if (situacao == 2 || situacao == 3) { 
      if (chegadaSerial == '!') { //identifica o terminador da frase, salvando o dado e limpando o buffer
        salvaDado(); 
        limpaBuffer();
      }
      if (chegadaSerial == ',') { //identifica o separador de variaveis, salva o dado lido e reinicia o processo
        salvaDado();
        situacao = 1;
      }
      if (situacao == 3) {
        verificador.concat(chegadaSerial);
      }
      if (chegadaSerial != ',' && chegadaSerial != '!' && situacao != 3) {
        if (chegadaSerial == '/') {
          situacao = 3;
        } else {
          dado.concat(chegadaSerial);
        }
      }
    }
    if (situacao == 1) {
      if (chegadaSerial == ':') {
        situacao = 2;
      } else {
        endereco.concat(chegadaSerial);
      }
    }
    if (chegadaSerial == '|') {
      situacao = 1;
    }
  }
}
void escreveSerialEsp() {
  Seria.print("|");
  for (int cont = 0; cont <= 9; cont = cont + 1) {
    Seria.print(cont);
    Seria.print(':');
    Seria.print(registradores[cont]);
    Seria.print('/');
    Seria.print(20 + cont + registradores[cont]);
    if (cont == 9) {
      Seria.println('!');
    } else {
      Seria.print(',');
    }
  }
}

void salvaDado() {
  for (int cont = 0 ; cont < endereco.length(); cont = cont + 1) { //Verifica se existe algum caracter irregular
    if (endereco.charAt(cont) == ',') {
      endereco.remove(cont, 1);
      cont = endereco.length() + 10;
    }
  }
  if (endereco.toInt() > 9 && endereco.toInt() < 20) {
    if ((endereco.toInt() + dado.toInt() + 20) == verificador.toInt()) {
      registradores[endereco.toInt()] = dado.toInt();
    }
  }
  dado = "";
  endereco = "";
  verificador = "";
  situacao = 1;
}

void limpaBuffer() {
  situacao = 0;
  while (Seria.available()) {
    Seria.read();
  }
}



/* CODIGO DA PRIMEIRA VERSÃO ONDE AS FUNÇÕES OCORRIA VIA SERIAL




// --- Funções externas ---
extern void calibra(int modoCal);


// --- Funções locais ---
//void configuraSerial();
void verificaSerial();

// --- variaveis externas ---
extern bool calibrado1;
extern bool calibrado2;
extern bool calibrado3;

void verificaSerial() {

  char comando;
  int fcomando;

  if (Serial.available() > 0) {
    comando = Serial.read();
    switch (comando) {
      case 'a': //referente a posição do eixo 1
        fcomando = Serial.parseInt();
        if (fcomando < 190 && fcomando > -135) {
          fcomando = -1 * (fcomando * 4);
          Mot1.write(fcomando);
        } else {
          Serial.println("ERRO: Angulo excede o limite");
        }

        break;
      case 'b': //referente a posição do eixo 2
        fcomando = Serial.parseInt();
        if (fcomando < 155 && fcomando > -145) {
          fcomando = fcomando * 16;
          Mot2.write(fcomando);
        } else {
          Serial.println("ERRO: Angulo excede o limite");
        }
        break;
      case 'c': //referente a posição do eixo 3
        fcomando = Serial.parseInt();
        if (fcomando < 90 && fcomando > -90) {
          fcomando = fcomando * 20;
          Mot3.write(fcomando);

        } else {
          Serial.println("ERRO: Angulo excede o limite");
        }

        break;
      case 'd': //Calibra a posição do eixo 1
        calibra(1);
        break;
      case 'e': //Calibra a posição do eixo 2
        calibra(2);
        break;
      case 'f': //Calibra a posição do eixo 3
        calibra(3);
        break;
      case 'g': //Calibra todos os eixos
        calibra(4);
        break;
      case 'r': //alinha para a posição reta
        Mot1.write(0);
        Mot2.write(0);
        Mot3.write(0);
        break;
      case 'h': //Ajuda Serial
        Serial.println("\n \n \n");
        Serial.println("Comandos: ");
        Serial.println("  h - Abre o menu de ajuda dos comandos");
        Serial.println("  s - Abre o menu com informacoes dos eixos");
        Serial.println("  a - Efetua o posicionamento do eixo 1 (Ferramenta) \n     Ex: 'a45' ou 'a-45' ira mover o eixo para 45 e -45 graus respectivamente ");
        Serial.println("  b - Efetua o posicionamento do eixo 2 (Cotovelo) \n     Ex: 'b45' ou 'b-45' ira mover o eixo para 45 e -45 graus respectivamente ");
        Serial.println("  c - Efetua o posicionamento do eixo 3 (Base) \n     Ex: 'c45' ou 'c-45' ira mover o eixo para 45 e -45 graus respectivamente ");
        Serial.println("  d - Efetua a calibracao do eixo 1");
        Serial.println("  e - Efetua a calibracao do eixo 2");
        Serial.println("  f - Efetua a calibracao do eixo 3");
        Serial.println("  g - Efetua a calibracao de todos os eixos");
        Serial.println("  r - move todos os eixos para 0°");
        break;
      case 's':
        Serial.println("\n \n \n");
        Serial.println("Status: ");
        //----------------------------------
        Serial.println("    Motor 1: ");
        Serial.print(  "      Calibrado: ");
        if (calibrado1) {
          Serial.println("Sim");
          Serial.print("Posicao: ");
          Serial.print(Mot1.read() / 4);
          Serial.println("°");
        } else {
          Serial.println("Não");
          Serial.println("Posicao: ?");
        }
        //----------------------------------
        Serial.println("    Motor 2: ");
        Serial.print(  "      Calibrado: ");
        if (calibrado2) {
          Serial.println("Sim");
          Serial.print("Posicao: ");
          Serial.print(Mot2.read() / 16);
          Serial.println("°");
        } else {
          Serial.println("Não");
          Serial.println("Posicao: ?");
        }
        //----------------------------------
        Serial.println("    Motor 3: ");
        Serial.print(  "      Calibrado: ");
        if (calibrado3) {
          Serial.println("Sim");
          Serial.print("Posicao: ");
          Serial.print(-1 * (Mot3.read() / 20));
          Serial.println("°");
        } else {
          Serial.println("Não");
          Serial.println("Posicao: ?");
        }
        break;
      case 'x': //Calibra a posição do eixo C
        break;
      case 'y': //Calibra a posição do eixo C
        break;
      default:
        Mot1.stop();
        Mot2.stop();
        Mot3.stop();
        break;
    }
    //limpa Serial
  }
  Serial.flush();
}*/
