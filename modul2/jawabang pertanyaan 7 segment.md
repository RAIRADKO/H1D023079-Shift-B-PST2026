### 1. Rangkaian Schematic Percobaan
![sevenSegment (1)](https://github.com/user-attachments/assets/7bc21f8b-cae1-49ff-89ac-5758a517f9a5)

---

### 2. Apa yang terjadi jika nilai `num` lebih dari 15?

Jika nilai `num` lebih dari 15, program akan mengalami *Undefined Behavior* akibat **Membaca di Luar Batas Array (*Array Out of Bounds*)**. 
* [cite_start]Array `digitPattern` hanya dideklarasikan memiliki 16 baris (indeks 0 hingga 15)[cite: 1].
* [cite_start]Fungsi `displayDigit(int num)` langsung menggunakan variabel `num` sebagai penentu indeks baris eksekusi: `digitPattern[num][i]`[cite: 2].
* Jika `num` bernilai 16 atau lebih, Arduino akan mencoba membaca data di alamat memori acak setelah array tersebut berakhir. Akibatnya, 7-segment akan menampilkan pola lampu yang acak (nyala tidak beraturan) karena membaca *garbage data*, atau dalam kasus tertentu bisa menyebabkan *microcontroller* mengalami *crash* sementara.

---

### 3. Common Cathode atau Common Anode?

Program ini menggunakan **Common Cathode (Katoda Bersama)**.

**Alasannya:**
[cite_start]Kita bisa membuktikannya melalui baris kode pembentuk angka 0 yaitu `{1,1,1,1,1,1,0,0}`[cite: 1].
Untuk menampilkan angka '0' pada 7-segment, segmen a, b, c, d, e, dan f harus menyala, sedangkan segmen g dan dp mati. [cite_start]Kode tersebut menggunakan angka `1` (logika HIGH) untuk menyalakan segmen dan `0` (logika LOW) untuk mematikan segmen[cite: 1]. Menyala saat diberi arus positif (HIGH) merupakan karakteristik mutlak dari konfigurasi Common Cathode. (Jika ini Common Anode, logikanya akan terbalik, yaitu 0 untuk menyala).

---

### 4. Modifikasi Program (F ke 0)

[cite_start]Untuk membuat tampilan berjalan mundur dari F ke 0, modifikasi utama dilakukan pada fungsi `loop()` [cite: 4] dengan membalik urutan iterasi perulangan `for`. 

Berikut adalah modifikasi kodenya, disusun lengkap dalam bentuk `README.md`:

```markdown
# Modul 2: 7-Segment Display Countdown (F ke 0)

Kode ini adalah modifikasi dari program dasar 7-segment untuk melakukan hitung mundur (*countdown*) dalam format heksadesimal dari F (15) hingga 0.

## Kode Program

```cpp
#include <Arduino.h>

// Mendefinisikan pin Arduino yang terhubung ke segmen a-g dan dp
const int segmentPins[8] = {7,6,5,11,10,8,9,4};

// Array 2 dimensi yang menyimpan pola nyala/mati (1/0) untuk angka 0-F
// Konfigurasi ini disesuaikan untuk 7-segment Common Cathode
byte digitPattern[16][8] = {
  {1,1,1,1,1,1,0,0}, // Indeks 0: Pola angka 0
  {0,1,1,0,0,0,0,0}, // Indeks 1: Pola angka 1
  {1,1,0,1,1,0,1,0}, // Indeks 2: Pola angka 2
  {1,1,1,1,0,0,1,0}, // Indeks 3: Pola angka 3
  {0,1,1,0,0,1,1,0}, // Indeks 4: Pola angka 4
  {1,0,1,1,0,1,1,0}, // Indeks 5: Pola angka 5
  {1,0,1,1,1,1,1,0}, // Indeks 6: Pola angka 6
  {1,1,1,0,0,0,0,0}, // Indeks 7: Pola angka 7
  {1,1,1,1,1,1,1,0}, // Indeks 8: Pola angka 8
  {1,1,1,1,0,1,1,0}, // Indeks 9: Pola angka 9
  {1,1,1,0,1,1,1,0}, // Indeks 10: Pola huruf A
  {0,0,1,1,1,1,1,0}, // Indeks 11: Pola huruf b
  {1,0,0,1,1,1,0,0}, // Indeks 12: Pola huruf C
  {0,1,1,1,1,0,1,0}, // Indeks 13: Pola huruf d
  {1,0,0,1,1,1,1,0}, // Indeks 14: Pola huruf E
  {1,0,0,0,1,1,1,0}  // Indeks 15: Pola huruf F
};

// Fungsi kustom untuk menampilkan karakter berdasarkan indeks dari parameter num
void displayDigit(int num){
  // Melakukan looping sebanyak 8 kali untuk menyalakan/mematikan ke-8 pin segmen
  for(int i = 0; i < 8; i++){
    // Menuliskan logika (HIGH/LOW) ke pin spesifik berdasarkan array digitPattern
    digitalWrite(segmentPins[i], digitPattern[num][i]);
  }
}

// Fungsi setup berjalan sekali saat mikrokontroler dinyalakan
void setup(){
  // Menetapkan semua pin yang terhubung ke 7-segment sebagai OUTPUT
  for(int i = 0; i < 8; i++){
    pinMode(segmentPins[i], OUTPUT);
  }
}

// Fungsi loop berjalan berulang secara terus-menerus
void loop(){
  // MODIFIKASI: Looping dibalik agar dimulai dari 15 (F) dan berkurang hingga 0
  for(int i = 15; i >= 0; i--){
    displayDigit(i);  // Memanggil fungsi untuk menampilkan karakter ke-i
    delay(1000);      // Memberikan jeda selama 1000 milidetik (1 detik) sebelum angka berubah
  }
}
```
```
