#include <Arduino.h> // library dasar Arduino

// ===================== PIN SETUP =====================
// Tentukan pin yang digunakan untuk potensiometer dan LED PWM
const int potPin = A0;   // Pin analog input dari potensiometer
const int ledPin = 9;    // Pin digital PWM (mendukung analogWrite)

// ===================== VARIABEL =====================
// Variabel untuk menyimpan hasil pembacaan dan konversi PWM
int nilaiADC = 0;  // Nilai awal pembacaan analog
int pwm = 0;       // Nilai awal untuk output PWM

void setup() {

  // ===================== OUTPUT SETUP =====================
  // Atur pin LED sebagai output
  pinMode(ledPin, OUTPUT);

  // ===================== SERIAL MONITOR =====================
  // Aktifkan komunikasi serial untuk melihat data pembacaan
  Serial.begin(9600); // Baud rate standar 9600
}

void loop() {

  // ===================== PEMBACAAN SENSOR =====================
  // Baca nilai analog dari potensiometer (rentang 0–1023)
  nilaiADC = analogRead(potPin); 

  // ===================== PEMROSESAN DATA (SCALING) =====================
  // Ubah nilai ADC (0–1023) menjadi nilai PWM (0–255)
  // ADC Arduino memiliki resolusi 10-bit, sedangkan PWM 8-bit
  pwm = map(nilaiADC,
            0,      // nilai minimum ADC
            1023,   // nilai maksimum ADC
            0,      // PWM minimum (LED mati)
            255     // PWM maksimum (LED paling terang)
            );  

  // ===================== OUTPUT PWM =====================
  // Kirim sinyal PWM ke LED (mengatur kecerahan)
  analogWrite(ledPin, pwm); 

  // ===================== MONITORING DATA =====================
  // Tampilkan data ADC dan PWM ke Serial Monitor
  Serial.print("ADC: ");
  Serial.print(nilaiADC); 

  Serial.print(" | PWM: ");
  Serial.println(pwm); 

  // ===================== STABILISASI SISTEM =====================
  // Delay untuk menstabilkan pembacaan dan tampilan data
  delay(50); 
}