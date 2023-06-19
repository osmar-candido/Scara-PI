//codigo de teste para validar os calculos da cinematica direta e inversa
#define distanciaBaseCotovelo 150   // Distancia entre a base e o cotovelo em milimetro
#define distanciaCotoveloPunho 100  // Distancia entre o cotovelo e o punho em milimetro

#define limiteMaxBase 120        //°
#define limiteMaxCotovelo 150    //°
#define limiteMaxPunho 150       //°
#define limiteMaxAltura 0       //°
#define limiteMinBase -120       //°
#define limiteMinCotovelo -145   //°
#define limiteMinPunho -150      //°
#define limiteMinAltura -65     //mm

int registradores[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
long X = 0;
long Y = 0;
long Z = 0;
int  A = -5; //altura Z
int  B = -45; //Base
int  C = -45; //Cotovelo
int  R = 45; //Rotação Ferramenta
byte F = 100; //abertura da Ferramenta
byte Vel = 100;
long rX = 0;
long rY = 0;
long rZ = 0;
int  rA = 0; //altura Z
int  rB = 0; //Base
int  rC = 0; //Cotovelo
int  rR = 0; //Rotação Ferramenta
float distanciaBasePunho = 0.0;
float angInterno = 0.0;
float vX, vY;

float angOrigemBase;
float angInternoCotovelo;
float angInternoBase;
float angInternoFerramenta;
float sol1_Base;
float sol1_Cotovelo;
float sol1_Punho;
float sol2_Base;
float sol2_Cotovelo;
float sol2_Punho;



void setup() {
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  Serial.println("X = 9 Y = 246 R = 0"); // B = -10 C = 20
  cinematicaInversa(9, 246, 0, 0);
  Serial.println("");
  Serial.println("X = -115 Y = 196 R = 0"); // B = 50  C = -50
  cinematicaInversa(-115, 196, 0, 0);
  Serial.println("");
  Serial.println("X = 213 Y = 79 R = 0"); // B = -50  C = -50
  cinematicaInversa(213, 79, 0, 0);
  Serial.println("");
  while (true) {
    delay(10000);
  }
}

void cinematicaDireta(int disA, int angB, int angC, int angR) { //altura, base, cotovelo, punho //Funcionando
  //Calcula a distancia entre a base e o Punho
  distanciaBasePunho = sqrt(pow(distanciaBaseCotovelo, 2) + pow(distanciaCotoveloPunho, 2) - (2 * distanciaBaseCotovelo * distanciaCotoveloPunho * cos(radians(angC + 180))));
  //Envia o valor para o regirstrador
  registradores[0] = round(distanciaBasePunho); //valor aferido
  Serial.print("Distancia Base Punho: ");
  Serial.print(distanciaBasePunho);
  Serial.print(" Registrada: ");
  Serial.println(registradores[0]);


  //Calcula o angulo entre a hipotenusa e o elo Base:Cotovelo
  angInterno = acos((pow(distanciaBaseCotovelo, 2) + pow(distanciaBasePunho, 2) - (pow(distanciaCotoveloPunho, 2))) / (2 * (distanciaBaseCotovelo * distanciaBasePunho)));
  //Envia o valor para o regirstrador
  registradores[1] = round(degrees(angInterno));
  Serial.print("Angulo Interno: ");
  Serial.print(degrees(angInterno));
  Serial.print(" Registrada: ");
  Serial.println(registradores[1]);


  //Situação 1
  if (angC >= 0 && (angB + degrees(angInterno)) < 0) {
    Serial.println("Situacao 1");
    // registradores[4] = 1;
    //rX = round(cos(radians(angB) + radians(90) + angInterno) * distanciaBasePunho);
    //rY = round(sin(radians(angB) + radians(90) + angInterno) * distanciaBasePunho);
  }
  //situação 2
  if (angC < 0 && (angB - degrees(angInterno)) < 0) {
    Serial.println("Situacao 2");
    //registradores[4] = 2;
    // rX = round(cos(radians(angB) - angInterno + radians(90) ) * distanciaBasePunho);
    //rY = round(sin(radians(angB) + radians(90) - angInterno) * distanciaBasePunho);
  }
  //situação 3
  if (angC >= 0 && (angB + degrees(angInterno)) >= 0) {
    Serial.println("Situacao 3");
    // registradores[4] = 3;
    //rX = round( cos(radians(180) - (radians(angB) + radians(90) + angInterno)) * distanciaBasePunho * (-1));
    // rY = round(sin(radians(180) - (radians(angB) + radians(90) + angInterno)) * distanciaBasePunho);
  }
  //situação 4
  if (angC < 0 && (angB - degrees(angInterno)) >= 0) {
    Serial.println("Situacao 4");
    //registradores[4] = 4;
    // rX = round(cos(radians(180) - (radians(angB) + radians(90) - angInterno)) * distanciaBasePunho * (-1));
    // rY = round(sin(radians(180) - (radians(angB) + radians(90) - angInterno)) * distanciaBasePunho);
  }

  //Calculo da Situação 1
  vX = cos(radians(angB) + radians(90) + angInterno) * distanciaBasePunho;
  vY = sin(radians(angB) + radians(90) + angInterno) * distanciaBasePunho;
  rX = round(vX);
  rY = round(vY);

  Serial.print("X: ");
  Serial.print(vX);
  Serial.print(" registrada: ");
  Serial.println(rX);
  Serial.print("Y: ");
  Serial.print(vY);
  Serial.print(" registrada: ");
  Serial.println(rY);

  //Calculo da situação 2
  vX = cos(radians(angB) - angInterno + radians(90) ) * distanciaBasePunho;
  vY = sin(radians(angB) + radians(90) - angInterno) * distanciaBasePunho;
  rX = round(vX);
  rY = round(vY);
  Serial.print("X: ");
  Serial.print(vX);
  Serial.print(" registrada: ");
  Serial.println(rX);
  Serial.print("Y: ");
  Serial.print(vY);
  Serial.print(" registrada: ");
  Serial.println(rY);

  //Calculo da situação 3
  vX =  cos(radians(180) - (radians(angB) + radians(90) + angInterno)) * distanciaBasePunho * (-1);
  vY = sin(radians(180) - (radians(angB) + radians(90) + angInterno)) * distanciaBasePunho;
  rX = round(vX);
  rY = round(vY);
  Serial.print("X: ");
  Serial.print(vX);
  Serial.print(" registrada: ");
  Serial.println(rX);
  Serial.print("Y: ");
  Serial.print(vY);
  Serial.print(" registrada: ");
  Serial.println(rY);

  //Calculo da situação 4
  vX = cos(radians(180) - (radians(angB) + radians(90) - angInterno)) * distanciaBasePunho * (-1);
  vY = sin(radians(180) - (radians(angB) + radians(90) - angInterno)) * distanciaBasePunho;
  rX = round(vX);
  rY = round(vY);
  Serial.print("X: ");
  Serial.print(vX);
  Serial.print(" registrada: ");
  Serial.println(rX);
  Serial.print("Y: ");
  Serial.print(vY);
  Serial.print(" registrada: ");
  Serial.println(rY);

}

void cinematicaInversa(long setX, long setY, int angR, long altura) {
  if (setX == 0) { //o valor não pode ser 0 devido aos calculos
    setX = 1;
  }
  if (setY == 0) { //o valor não pode ser 0 devido aos calculos
    setY = 1;
  }

  distanciaBasePunho = sqrt(pow(setX, 2) + pow(setY, 2));
  angOrigemBase = degrees(atan(setY / sqrt(pow(setX, 2)))); //radianos
  angInternoCotovelo = degrees(acos(( pow(distanciaBaseCotovelo, 2) + pow(distanciaCotoveloPunho, 2) - pow(distanciaBasePunho, 2)) / (2 * distanciaBaseCotovelo * distanciaCotoveloPunho)));
  angInternoBase = degrees(acos(( pow(distanciaBaseCotovelo, 2) + pow(distanciaBasePunho, 2) - pow(distanciaCotoveloPunho, 2)) / (2 * distanciaBaseCotovelo * distanciaBasePunho)));
  angInternoFerramenta = 180 - angInternoBase - angInternoCotovelo;

  Serial.print("Distancia Base Punho:      ");
  Serial.println(distanciaBasePunho);
  Serial.print("Angulo Origem Base:        ");
  Serial.println(angOrigemBase);
  Serial.print("Angulo Interno Cotovelo:   ");
  Serial.println(angInternoCotovelo);
  Serial.print("Angulo Interno  Base:      ");
  Serial.println(angInternoBase);
  Serial.print("Angulo Interno Ferramenta: ");
  Serial.println(angInternoFerramenta);
  Serial.println("");
  /*sol1_Base;
    sol1_Cotovelo;
    sol1_Punho;
    sol2_Base;
    sol2_Cotovelo;
    sol2_Punho;
  */

  if (setX > 0) {
    //solução 1 X positivo
    //angulo Base
    sol1_Base = angOrigemBase - angInternoBase - 90;
    
    //angulo Cotovelo
    sol1_Cotovelo = 180 - angInternoCotovelo;

    //angulo Punho
    sol1_Punho = angR - angInternoFerramenta - degrees(atan(setX / setY)) - 90;

    //solução 2 X positivo
    //angulo Base
    sol2_Base = angOrigemBase + angInternoBase - 90;

    //angulo Cotovelo
    sol2_Cotovelo = 0 - angInternoCotovelo;

    //angulo Punho
    sol2_Punho = angR + angInternoFerramenta + degrees(atan(setX / setY)) - 90;
    Serial.println("X positivo");
    Serial.println("Solucao 1");
    Serial.print("Base:     ");
    Serial.println(sol1_Base);
    Serial.print("Cotovelo: ");
    Serial.println(sol1_Cotovelo);
    Serial.print("Punho:    ");
    Serial.println(sol1_Punho);
    Serial.println("Solucao 2");
    Serial.print("Base:     ");
    Serial.println(sol2_Base);
    Serial.print("Cotovelo: ");
    Serial.println(sol2_Cotovelo);
    Serial.print("Punho:    ");
    Serial.println(sol2_Punho);
    
    
    

  } else {
    //solução 3 X negativo
    //angulo Base
    sol1_Base = 90 - angOrigemBase - angInternoBase - 90;

    //angulo Cotovelo
    sol1_Cotovelo = 0 - angInternoCotovelo;

    //angulo Punho
    sol1_Punho = angR + angInternoFerramenta + degrees(atan((setX * -1) / setY)) - 90;

    //solução 4 X negativo
    //angulo Base
    sol2_Base = 90 - angOrigemBase + angInternoBase - 90;

    //angulo Cotovelo
    sol2_Cotovelo = 180 - angInternoCotovelo;

    //angulo Punho
    sol2_Punho = angR - angInternoFerramenta - degrees(atan((setX * -1) / setY)) - 90;
    Serial.println("X Negativo");
    Serial.println("Solucao 1");
    Serial.print("Base:     ");
    Serial.println(sol1_Base);
    Serial.print("Cotovelo: ");
    Serial.println(sol1_Cotovelo);
    Serial.print("Punho:    ");
    Serial.println(sol1_Punho);
    Serial.println("Solucao 2");
    Serial.print("Base:     ");
    Serial.println(sol2_Base);
    Serial.print("Cotovelo: ");
    Serial.println(sol2_Cotovelo);
    Serial.print("Punho:    ");
    Serial.println(sol2_Punho);
  }

  /*para decidir qual das 2 opções vao ser utilizadas poderia ser decidido pela mais proxima, como também pela mais rapida, 
   *mas simplificando ao maximo será decidido pela qual não esta alem dos limites configurados, sendo priorizado a opção 1
  */
  
  //escrita nos motores
  if (rB <= limiteMaxBase && rB >= limiteMinBase) {
    /*
        registradores[0] = rB;
        Mot2.writeSteps(round(((800 * 20) / 360)* rB));
    */
  }
  if (rC <= limiteMaxCotovelo && rC >= limiteMinCotovelo) {
    /*
        registradores[1] = rC;
        Mot3.writeSteps(round(((800 * 16) / 360)* rC));
    */
  }
  if (rR <= limiteMaxPunho && rR >= limiteMinPunho) {
    /*
        registradores[2] = rR;
        Mot4.writeSteps(round(((800 * 4) / 360)* rR));
    */
  }
  if (altura <= limiteMaxAltura && altura >= limiteMinAltura) {
    /*
        registradores[3] = altura;
        Mot1.writeSteps(round(100 * altura));
    */
  }
}
