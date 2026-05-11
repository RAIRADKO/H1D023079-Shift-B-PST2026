#include <Arduino.h>

const int carGreenPin = A0;
const int carYellowPin = A1;
const int carRedPin = A2;
const int pedRedPin = A3;
const int pedGreenPin = A4;
const int buttonPin = 12;

const int segA = 2;
const int segB = 3;
const int segC = 4;
const int segD = 5;
const int segE = 6;
const int segF = 7;
const int segG = 8;

const byte angkaPola[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1},
  {1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 1, 1}
};

void setup() {
  pinMode(carRedPin, OUTPUT);
  pinMode(carYellowPin, OUTPUT);
  pinMode(carGreenPin, OUTPUT);
  pinMode(pedRedPin, OUTPUT);
  pinMode(pedGreenPin, OUTPUT);

  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);

  digitalWrite(carGreenPin, HIGH);
  digitalWrite(carYellowPin, LOW);
  digitalWrite(carRedPin, LOW);
  digitalWrite(pedRedPin, HIGH);
  digitalWrite(pedGreenPin, LOW);
  
  matikanLayar();
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    mulaiPenyeberangan();
  }
}

void mulaiPenyeberangan() {
  digitalWrite(carGreenPin, LOW);
  digitalWrite(carYellowPin, HIGH);
  delay(2000); 

  digitalWrite(carYellowPin, LOW);
  digitalWrite(carRedPin, HIGH);
  delay(1000);

  digitalWrite(pedRedPin, LOW);
  digitalWrite(pedGreenPin, HIGH);

  for (int i = 9; i >= 0; i--) {
    tampilkanAngka(i);
    delay(1000);
  }

  matikanLayar();
  digitalWrite(pedGreenPin, LOW);
  digitalWrite(pedRedPin, HIGH);
  
  delay(2000);

  digitalWrite(carRedPin, LOW);
  digitalWrite(carGreenPin, HIGH);

  delay(5000); 
}

void tampilkanAngka(int angka) {
  digitalWrite(segA, angkaPola[angka][0]);
  digitalWrite(segB, angkaPola[angka][1]);
  digitalWrite(segC, angkaPola[angka][2]);
  digitalWrite(segD, angkaPola[angka][3]);
  digitalWrite(segE, angkaPola[angka][4]);
  digitalWrite(segF, angkaPola[angka][5]);
  digitalWrite(segG, angkaPola[angka][6]);
}

void matikanLayar() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
}