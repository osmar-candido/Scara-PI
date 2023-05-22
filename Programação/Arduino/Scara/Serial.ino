/* --------------------------------------------------------------------------------
 *  Funções referentes a escrita Serial com o computador e o ESP
 * -------------------------------------------------------------------------------- */

// Inclusão da bibliotecas

// Criação das variaveis

void configuraSerial();
void loopSerialEsp();


void configuraSerial(){
  
}
void loopSerialEsp(){
  
}











// --- Funções externas ---
extern void calibra(int modoCal);


// --- Funções locais ---
void configuraSerial();
void verificaSerial();

// --- variaveis externas ---
extern bool calibrado1;
extern bool calibrado2;
extern bool calibrado3;


void configuraSerial() {
  Serial.begin(9600);
}

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
          Serial.print(-1 *(Mot3.read() / 20));
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
}
