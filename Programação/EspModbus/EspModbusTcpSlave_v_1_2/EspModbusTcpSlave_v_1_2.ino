/***********************
 * Autor:
 *     Jhon Valencia - TrialCommand
 * Editor: 
 *     Osmar Candido Junior
 * 
 * Versão: 
 *     1.2 Estável
 * Descrição: 
 *     ESP MODBUS TCP, lê 10 registradores e escreve 10 registradores. efetua a troca 
 * de dados via serial, sendo necessário receber um dado antes, a conexão é realizada 
 * usando ip dinâmico no wifi especificado na variavel ssid e password
 *     .
 * Fazer nas proximas versões:
 *     Permitir a conexão com IP estático;
 *     alterar a quantidade de variaveis e seus tipos a serem manipulados;
 *     receber via serial as configurações wifi, ip, status e permitir configurar o numero de registradores e seus tipos.
***********************/
// Inclusão de bibliotecas
#include <ESP8266WiFi.h>


// Configuração dos dados Wifi
const char* ssid = "Scara_ModbusTCP";
const char* password = "projetoscara";
//const char* ssid = "FAMILIA CANDIDO_2.4G";
//const char* password = "954901jc";


int ModbusTCP_port = 502;

// Requerido para Modbus TCP/IP
#define maxInputRegister 20
#define maxHoldingRegister 20

#define MB_FC_NONE 0
#define MB_FC_READ_REGISTERS 3 //implemented
#define MB_FC_WRITE_REGISTER 6 //implemented
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16 //implemented
//
// MODBUS Error Codes
//
#define MB_EC_NONE 0
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3
#define MB_EC_SLAVE_DEVICE_FAILURE 4
//
// MODBUS MBAP offsets
//
#define MB_TCP_TID 0
#define MB_TCP_PID 2
#define MB_TCP_LEN 4
#define MB_TCP_UID 6
#define MB_TCP_FUNC 7
#define MB_TCP_REGISTER_START 8
#define MB_TCP_REGISTER_NUMBER 10

byte ByteArray[260];
unsigned int MBHoldingRegister[maxHoldingRegister];

extern int registradores[20];
extern void daleSerial();
//////////////////////////////////////////////////////////////////////////

WiFiServer MBServer(ModbusTCP_port);

void setup() {

  Serial.begin(9600);
  //inputString.reserve(MAX_VAR_COUNT * MAX_VAR_LENGTH); // Aloca espaço para a string de entrada
  delay(10000) ;

  WiFi.begin(ssid, password);
  delay(100);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  MBServer.begin();
  /*Serial.print("|P:");
  Serial.print(WiFi.localIP());
  Serial.print("!");*/
}


void loop() {

  boolean flagClientConnected = 0;
  byte byteFN = MB_FC_NONE;
  int Start;
  int WordDataLength;
  int ByteDataLength;
  int MessageLength;
  WiFiClient client = MBServer.available();

  // Modbus TCP/IP
  while (client.connected()) {

    if (client.available())
    {
      flagClientConnected = 1;
      int i = 0;
      while (client.available())
      {
        ByteArray[i] = client.read();
        i++;
      }

      client.flush();


      ///// code here --- codigo aqui

      daleSerial();

      ///////// Holding Register [0] A [9] = 10 Holding Registers Escritura
      ///////// Holding Register [0] A [9] = 10 Holding Registers Writing
      for (int cont = 0; cont < 10; cont = cont + 1) {
        /*Serial.print(String(cont));
          Serial.print("   ");
          Serial.println(String(registradores[cont]));
        */
        MBHoldingRegister[cont] = registradores[cont];
      }




      ///////// Holding Register [10] A [19] = 10 Holding Registers Lectura
      ///// Holding Register [10] A [19] = 10 Holding Registers Reading

      int Temporal[10];

      

      Temporal[0] = MBHoldingRegister[10];
      Temporal[1] = MBHoldingRegister[11];
      Temporal[2] = MBHoldingRegister[12];
      Temporal[3] = MBHoldingRegister[13];
      Temporal[4] = MBHoldingRegister[14];
      Temporal[5] = MBHoldingRegister[15];
      Temporal[6] = MBHoldingRegister[16];
      Temporal[7] = MBHoldingRegister[17];
      Temporal[8] = MBHoldingRegister[18];
      Temporal[9] = MBHoldingRegister[19];

      //// end code - fin


      //// rutine Modbus TCP
      byteFN = ByteArray[MB_TCP_FUNC];
      Start = word(ByteArray[MB_TCP_REGISTER_START], ByteArray[MB_TCP_REGISTER_START + 1]);
      WordDataLength = word(ByteArray[MB_TCP_REGISTER_NUMBER], ByteArray[MB_TCP_REGISTER_NUMBER + 1]);
    }

    // Handle request

    switch (byteFN) {
      case MB_FC_NONE:
        break;

      case MB_FC_READ_REGISTERS: // 03 Read Holding Registers
        ByteDataLength = WordDataLength * 2;
        ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
        ByteArray[8] = ByteDataLength; //Number of bytes after this one (or number of bytes of data).
        for (int i = 0; i < WordDataLength; i++)
        {
          ByteArray[ 9 + i * 2] = highByte(MBHoldingRegister[Start + i]);
          ByteArray[10 + i * 2] = lowByte(MBHoldingRegister[Start + i]);
        }
        MessageLength = ByteDataLength + 9;
        client.write((const uint8_t *)ByteArray, MessageLength);

        byteFN = MB_FC_NONE;

        break;


      case MB_FC_WRITE_REGISTER: // 06 Write Holding Register
        MBHoldingRegister[Start] = word(ByteArray[MB_TCP_REGISTER_NUMBER], ByteArray[MB_TCP_REGISTER_NUMBER + 1]);
        ByteArray[5] = 6; //Number of bytes after this one.
        MessageLength = 12;
        client.write((const uint8_t *)ByteArray, MessageLength);
        byteFN = MB_FC_NONE;
        break;

      case MB_FC_WRITE_MULTIPLE_REGISTERS: //16 Write Holding Registers
        ByteDataLength = WordDataLength * 2;
        ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
        for (int i = 0; i < WordDataLength; i++)
        {
          MBHoldingRegister[Start + i] = word(ByteArray[ 13 + i * 2], ByteArray[14 + i * 2]);
        }
        MessageLength = 12;
        client.write((const uint8_t *)ByteArray, MessageLength);
        byteFN = MB_FC_NONE;

        break;
    }
  }




}
