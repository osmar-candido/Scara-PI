
double theta1, theta2, L1, L2, phi;
bool calibrado1 = false;
bool calibrado2 = false;
bool calibrado3 = false;
void calibra(int modoCal);
void configuraIO();

void configuraIO() {
  //Saidas diretas
  
  pinMode(m3en,  OUTPUT);

  //Entradas diretas
  pinMode(e1fim, INPUT);
  pinMode(e2fim, INPUT);
  pinMode(e3fim, INPUT);

  //configuração MoBaTools
  Mot1.attach(m1stp, m1dir);  // STEPpin, DIRpin
  Mot1.setSpeed(300);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot1.setRampLen(100);       // 500 ms (rampa aceleração)


  Mot2.attach(m2stp, m2dir);  // STEPpin, DIRpin
  Mot2.setSpeed(300);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot2.setRampLen(100);       // 500 ms (rampa aceleração)


  Mot3.attach(m3stp, m3dir);  // STEPpin, DIRpin
  Mot3.setSpeed(300);         // = 80/20 = 4 U/Min (velocidade maxima)
  Mot3.setRampLen(100);       // 500 ms (rampa aceleração)
}

void calibra(int modoCal) {
  switch (modoCal) {
    case 1: //calibra motor 1
      if (digitalRead(e1fim) == LOW) {
        Mot1.setZero();
        Mot1.move(200);
        delay(2000);
      }
      Mot1.setSpeed(50);
      Mot1.setRampLen(1);
      while (digitalRead(e1fim) == HIGH) {
        if (Mot1.moving() == false) {
          Mot1.move(-10);
        }
        delay(1);
      }
      Mot1.stop();
      Mot1.setZero(1444);
      Serial.println("Calibrado E1");
      calibrado1 = true;
      Mot1.setSpeed(300);
      Mot1.setRampLen(80);
      break;

    case 2: //calibra motor 2
      if (digitalRead(e2fim) == LOW) {
        Mot2.setZero();
        Mot2.move(200);
        delay(2000);
      }
      Mot3.move(-300);
      Mot2.setSpeed(100);
      Mot2.setRampLen(1);
      while (digitalRead(e2fim) == HIGH) {
        if (Mot2.moving() == false) {
          Mot2.move(-10);
        }
        delay(1);
      }
      Mot2.stop();
      Mot2.setZero(5389);
      Mot2.move(200);
      Serial.println("Calibrado E2");
      calibrado2 = true;
      Mot2.setSpeed(800);
      Mot2.setRampLen(400);
      break;
    case 3: //calibra motor 3
      if (digitalRead(e3fim) == LOW) {
        Mot3.setZero();
        Mot3.move(-200);
        delay(2000);
      }
      Mot3.setSpeed(100);
      Mot3.setRampLen(1);
      while (digitalRead(e3fim) == HIGH) {
        if (Mot3.moving() == false) {
          Mot3.move(10);
        }
        delay(1);
      }
      Mot3.stop();
      Mot3.setZero(-3889);
      delay(1000);
      Serial.println("Calibrado E3");
      calibrado3 = true;
      Mot3.setSpeed(300);
      Mot3.setRampLen(100);
      break;
    default: //calibra todos os motores
      calibra(1);
      calibra(2);
      calibra(3);
      break;
  }
}


void inverseKinematics(float x, float y) {
  theta2 = acos((sq(x) + sq(y) - sq(L1) - sq(L2)) / (2 * L1 * L2));
  if (x < 0 & y < 0) {
    theta2 = (-1) * theta2;
  }

  theta1 = atan(x / y) - atan((L2 * sin(theta2)) / (L1 + L2 * cos(theta2)));

  theta2 = (-1) * theta2 * 180 / PI;
  theta1 = theta1 * 180 / PI;

  // Ajuste de angulo dependendo do quadrante em que a o posicionamento for setado
  if (x >= 0 & y >= 0) {       // 1 primeiro quadrante
    theta1 = 90 - theta1;
  }
  if (x < 0 & y > 0) {       // 2 segundo quadrante
    theta1 = 90 - theta1;
  }
  if (x < 0 & y < 0) {       // 3 terceiro quadrante
    theta1 = 270 - theta1;
    phi = 270 - theta1 - theta2;
    phi = (-1) * phi;
  }
  if (x > 0 & y < 0) {       // 4 quarto quadrante
    theta1 = -90 - theta1;
  }
  if (x < 0 & y == 0) {
    theta1 = 270 + theta1;
  }

  // Calcula "phi" angulo para que o eixo ferramenta esteja paralelo ao eixo X
  phi = 90 + theta1 + theta2;
  phi = (-1) * phi;

  // Ajuste de angulo dependendo do quadrante em que a o posicionamento for setado
  if (x < 0 & y < 0) {       // 3 Terceiro quadrante
    phi = 270 - theta1 - theta2;
  }
  if (abs(phi) > 165) {
    phi = 180 + phi;
  }

  theta1 = round(theta1);
  theta2 = round(theta2);
  phi = round(phi);
  Serial.println(theta1);
  Serial.println(theta2);
  Serial.println(phi);

}
