#include <Servo.h> // library untuk servo motor

Servo myservo; // membuat objek servo

// ===================== PIN SETUP =====================
// Tentukan pin yang digunakan untuk potensiometer dan servo
const int potensioPin = A0;   // Pin analog input A0
const int servoPin = 9;       // Pin digital PWM 9

// ===================== VARIABEL =====================
// Variabel untuk menyimpan data ADC dan sudut servo
int pos = 0; // variabel untuk menyimpan posisi sudut
int val = 0; // variabel untuk menyimpan nilai pembacaan analog

void setup() {

  // Hubungkan servo ke pin yang sudah ditentukan
  myservo.attach(servoPin); 

  // Aktifkan komunikasi serial untuk monitoring
  Serial.begin(9600); // baud rate standar 9600

}

void loop() {

  // ===================== PEMBACAAN ADC =====================
  // Baca nilai dari potensiometer (rentang 0–1023)
  val = analogRead(potensioPin); 

  // ===================== KONVERSI DATA =====================
  // Ubah nilai ADC menjadi sudut servo (0–180 derajat)
  pos = map(val,
             0,      // nilai minimum ADC
             1023,   // nilai maksimum ADC
             0,      // sudut minimum servo
             180     // sudut maksimum servo
             );  

  // ===================== OUTPUT SERVO =====================
  // Gerakkan servo sesuai hasil mapping
  myservo.write(pos); 

  // ===================== MONITORING DATA =====================
  // Tampilkan data ADC dan sudut servo ke Serial Monitor
  Serial.print("ADC Potensio: ");
  Serial.print(val); 

  Serial.print(" | Sudut Servo: ");
  Serial.println(pos); 

  // ===================== STABILISASI =====================
  // Delay untuk memberi waktu servo bergerak stabil
  delay(5); // delay 15ms sudah cukup untuk kestabilan servo standar
}