/* --------------------------------------------------------------------------------
    Funções referentes a escrita Serial com o computador e o ESP
   -------------------------------------------------------------------------------- */

// Inclusão da bibliotecas
//#include <SoftwareSerial.h>

SoftwareSerial Seria(espTx, espRx); // RX, TX do arduino

// Criação das variaveis

// Definições

//posição da variavel desejada dentro do array registradores[]
//escrita(variaveis manipuladas pelo Arduino)
#define fbAnguloBase 0 //ok
#define fbAnguloCotovelo 1 //ok
#define fbAnguloPunho 2 //ok
#define fbAlturaZ 3 //ok dentro do void cinematicaDireta em cinematicas
#define fbFerramenta 4 
#define fbVelocidade 5 //ok dentro do void ajustavelocidade em cinematicas
#define fbModoPosionamento 6 
#define fbJuntasMovendo 7 //ok dentro do void loopcinematicas em cinematicas
#define fbX 8 //ok dentro do void cinematicaDireta em cinematicas
#define fbY 9 //ok dentro do void cinematicaDireta em cinematicas

//9
//leitura(variaveis manipuladas pelo mestreModbus)
#define setAnguloBase 10 //ok
#define setAnguloCotovelo 11 //ok
#define setAnguloPunho 12 //ok
#define setAlturaZ 13 //ok
#define setAberturaFerramenta 14
#define setModoPosicionamento 15 
#define setX 16
#define setY 17
#define setEmergencia 18
#define setVelocidade 19 

char chegadaSerial;
int situacao = 0; //em que etapa esta do recebimento de dados
String dado = ""; //dado recebido
String endereco = ""; //endereço da memoria a qual sera modificada
String verificador = ""; //valor para identificar se os dados recebidos estão corretos
int registradores[20]; //registro onde sera armazenado as informações a serem trocadas via modbus

extern void ajustaVelocidade(int nivel);
extern byte selecaoCinematica; // 0 = direta 1 = inversa
extern byte modo; // se igual a 0 comando local, se igual a 1 comando via modbus
extern long X;
extern long Y;
extern long Z;
extern int  A;    //altura Z
extern int  B;    //Base
extern int  C;    //Cotovelo
extern int  R;    //Rotação Ferramenta
extern byte F;    //abertura Ferramenta
extern byte Vel;  //velocidade de movimento;



void configuraSerial(); // aqui inicia-se a serial com o  ESP
void loopSerialEsp(); // função a ser inserida no loop que efetua as funções de comunicação com o ESP


void configuraSerial() {
  Serial.begin(9600);
  //Seria.begin(9600);
}

void loopSerialEsp() {
  leSerialEsp();
  escreveSerialEsp();
  atualizaVariaveis();
}

void atualizaVariaveis(){
  //Leitura ModBus
  //Modo Modbus
  if (modo == 1) { //comandos são recebidos via Modbus
    // angulo da base
    if (registradores[setAnguloBase] != B) { 
      if (registradores[setAnguloBase] <= limiteMaxBase && registradores[setAnguloBase] >= limiteMinBase) {
        B = registradores[setAnguloBase];
      }
    }
    // angulo cotovelo
    if (registradores[setAnguloCotovelo] != C) { 
      if (registradores[setAnguloCotovelo] <= limiteMaxCotovelo && registradores[setAnguloCotovelo] >= limiteMinCotovelo) {
        C = registradores[setAnguloCotovelo];
      }
    }
    // angulo Punho
    if (registradores[setAnguloPunho] != R) { 
      if (registradores[setAnguloPunho] <= limiteMaxPunho && registradores[setAnguloPunho] >= limiteMinPunho) {
        R = registradores[setAnguloPunho]*-1;
      }
    }
    // altura Z
    if (registradores[setAlturaZ] != A) { 
      if ((registradores[setAlturaZ] * -1) <= limiteMaxAltura && (registradores[setAlturaZ] * -1) >= limiteMinAltura) {
        A = registradores[setAlturaZ] * -1;
      }
    }
    // AberturaFerramenta
    if (registradores[setAberturaFerramenta] != F) { 
      if ((registradores[setAberturaFerramenta]) <= limiteMaxFerramenta && (registradores[setAberturaFerramenta]) >= limiteMinFerramenta) {
        F = registradores[setAberturaFerramenta];
      }
    }
    // Velocidade
    if (registradores[setVelocidade] != F) { 
      if ((registradores[setVelocidade]) <= limiteMaxVelocidade && (registradores[setVelocidade]) >= limiteMinVelocidade) {
        Vel = registradores[setVelocidade];
      }
      if(Vel == 0){
        Vel = 1;
      }
      ajustaVelocidade(Vel);
    }
    
    // Posição X
    if (registradores[setX] != X) { 
      X = registradores[setX];
    }
    // Posição Y
    if (registradores[setY] != Y) { 
      Y = registradores[setY];
    }
    // Modo Posicionamento
    if (registradores[setModoPosicionamento] != selecaoCinematica) { 
      selecaoCinematica = registradores[setModoPosicionamento];
    }
  }
  //Ambos
  if (registradores[setEmergencia] != Y) { 
      emergenciaModbus = registradores[setEmergencia];
  }
  //Escrita Modbus
  
  //anguloBase
  //AnguloCotovelo
  //Angulo Punho
  //Altura Z
  //Abertura Ferramenta
  //Velocidade
  //Emergencia
  //Juntas Movendo
  //Posição X
  //Posição Y
}

void leSerialEsp() {
  while (Serial.available() > 0) { // verifica se existe algum dado disponivel
    chegadaSerial = Serial.read(); // armazena o primeiro byte
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
  //atualiza variaveis importantes
  
}
void escreveSerialEsp() {//envia os valores armazenados no registro para o esp
  Serial.print("|");
  for (int cont = 0; cont <= 9; cont = cont + 1) {
    Serial.print(cont);
    Serial.print(':');
    Serial.print(registradores[cont]);
    Serial.print('/');
    Serial.print(20 + cont + registradores[cont]);
    if (cont == 9) {
      Serial.println('!');
    } else {
      Serial.print(',');
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
  while (Serial.available()) {
    Serial.read();
  }
}
