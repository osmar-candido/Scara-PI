
//   '|' - iniciador
//   ',' - separador
//   ':' - Pós endereço
//   '!' - Terminador
// Exemplo apenas 1 dado   : "|A:100!"
// Exemplo multiplos dados : "|A:100,B:50!"

char chegadaSerial;
int situacao = 0; //em que etapa esta do recebimento de dados
String dado = ""; //dado recebido
String endereco = ""; //
String verificador = "";
int registradores[20];
bool enviaDado = false;
int ip[4] = {0,0,0,0};
String ipPart = "";

/*void separaIp() {
  parte = 1;
  for (int cont = 0; cont < meuIp.length() ; cont = cont + 1) {
    if (parte == 1 && meuIp.charAt(cont) != '.'){
      ipPart.concat(meuIp.charAt(cont))  
    }else{
      if(parte == 1){
        parte = 2;
        ip[0] = ipPart.toInt();
        ipPart = "";
      }
    }
    if (parte == 2 && meuIp.charAt(cont) != '.'){
      ipPart.concat(meuIp.charAt(cont))  
    }else{
      if(parte == 2){
        parte = 3;
        ip[1] = ipPart.toInt();
        ipPart = "";
      }
    }
    if (parte == 2 && meuIp.charAt(cont) != '.'){
      ipPart.concat(meuIp.charAt(cont))  
    }else{
      if(parte == 2){
        parte = 3;
        ip[1] = ipPart.toInt();
        ipPart = "";
      }
    }
    if (parte == 2 && cont < meuIp.length()){
      ipPart.concat(meuIp.charAt(cont))  
    }else{
      if(parte == 2){
        parte = 3;
        ip[1] = ipPart.toInt();
        ipPart = "";
      }
    }
  }
}*/

void daleSerial() {
  while (Serial.available() > 0) {
    //aqui onde avaliamos o que vem da serial, onde pode ser separado em 4 situações
    //situação 0 - é quando não foi recebido o char de inicio de comando |
    //situação 1 - armazena os bytes referentes ao endereço até receber o char :
    //situação 2 - armazena os bytes referentes ao dado até receber o char /
    //situação 3 - armazena os bytes referentes ao verificador até receber o terminador!
    
    chegadaSerial = Serial.read();
    if (situacao == 2 || situacao == 3) {
      if (chegadaSerial == '!') {
        salvaDado();
        limpaBuffer();
        enviaDado = true;
      }
      if (chegadaSerial == ',') {
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
    if (enviaDado == true) {
      Serial.print("|");
      for (int cont = 10; cont < 20; cont = cont + 1) {
        Serial.print(cont);
        Serial.print(':');
        Serial.print(MBHoldingRegister[cont]);
        Serial.print('/');
        Serial.print(MBHoldingRegister[cont] + cont + 20);
        if (cont == 19) {
          Serial.println('!');
        } else {
          Serial.print(',');
        }
        registradores[cont] = MBHoldingRegister[cont];
      }
      enviaDado = false;
    }
  }
}

void salvaDado() {
  for (int cont = 0 ; cont < endereco.length(); cont = cont + 1) {
    if (endereco.charAt(cont) == ',') {
      endereco.remove(cont, 1);
      cont = endereco.length() + 10;
    }
  }
  if (endereco.toInt() >= 0 && endereco.toInt() <= 9) {
    if ((endereco.toInt() + dado.toInt() + 20) == verificador.toInt()) {
      registradores[endereco.toInt()] = dado.toInt();
    }
  }
  verificador = "";
  dado = "";
  endereco = "";
  situacao = 1;
}

void limpaBuffer() {
  situacao = 0;
  while(Serial.available() > 0){
    Serial.read();
  }
}
