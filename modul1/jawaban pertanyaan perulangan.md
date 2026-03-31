### 1. Skema Rangkaian (Schematic) 5 LED
<img width="1303" height="662" alt="image" src="https://github.com/user-attachments/assets/43a9a06b-5188-4ad7-8629-4699580ee93f" />


### 2. Bagaimana program membuat efek LED berjalan dari kiri ke kanan?
Efek berjalan dari kiri ke kanan (atau pin rendah ke tinggi) dicapai menggunakan **perulangan `for` pertama**: `for (int ledPin = 2; ledPin < 5; ledPin++)`. 
Program memulai variabel `ledPin` dari angka 2. Ia menyalakan LED di pin 2 (`HIGH`), menunggu sesaat sesuai `delay(timer)`, lalu mematikannya (`LOW`). Setelah itu, nilai `ledPin` ditambah 1 (`ledPin++`) menjadi 3. LED di pin 3 menyala, delay, mati, dan seterusnya. Karena proses nyala-mati ini berpindah berurutan ke pin yang lebih besar secara cepat, mata kita menangkapnya sebagai efek cahaya yang "berjalan" maju.

### 3. Bagaimana program membuat LED kembali dari kanan ke kiri?
Efek berjalan mundur dari kanan ke kiri dicapai menggunakan **perulangan `for` kedua**: `for (int ledPin = 7; ledPin >= 2; ledPin--)`.
Konsepnya sama persis dengan yang maju, namun arah perhitungannya dibalik. Program memulai dari pin paling tinggi (misalnya pin 7), menyalakan LED, memberi jeda, lalu mematikannya. Kemudian nilai `ledPin` dikurangi 1 (`ledPin--`) sehingga berpindah ke pin 6, lalu 5, dan seterusnya hingga menyentuh batas pin 2 (`>= 2`). Ini menciptakan efek cahaya yang "berjalan" mundur kembali ke titik awal.

---

### 4. Modifikasi Kode

Untuk membuat efek 3 LED kiri dan 3 LED kanan menyala bergantian, kita akan menggunakan 6 pin berurutan (misalnya Pin 2, 3, 4 untuk kelompok Kiri, dan Pin 5, 6, 7 untuk kelompok Kanan).

kodenya:

```cpp
int timer = 500; 

void setup() { 
  for (int pin = 2; pin <= 7; pin++) { 
    pinMode(pin, OUTPUT); 
  } 
} 

void loop() { 
  // Nyalakan 3 LED Kiri, Matikan 3 LED Kanan
  digitalWrite(2, HIGH); 
  digitalWrite(3, HIGH); 
  digitalWrite(4, HIGH); 
  digitalWrite(5, LOW); 
  digitalWrite(6, LOW); 
  digitalWrite(7, LOW); 
  delay(timer); 

  // Matikan 3 LED Kiri, Nyalakan 3 LED Kanan
  digitalWrite(2, LOW); 
  digitalWrite(3, LOW); 
  digitalWrite(4, LOW); 
  digitalWrite(5, HIGH); 
  digitalWrite(6, HIGH); 
  digitalWrite(7, HIGH); 
  delay(timer); 
}
```

penjelasan setiap baris kodenya:

```markdown
# Dokumentasi Program LED: 3 Kiri dan 3 Kanan Bergantian

Program ini mengontrol 6 buah LED yang terhubung ke pin 2 hingga 7 pada Arduino. Tiga LED pertama (pin 2, 3, 4) bertindak sebagai kelompok "Kiri", sedangkan tiga LED berikutnya (pin 5, 6, 7) bertindak sebagai kelompok "Kanan". Kedua kelompok ini akan menyala dan mati secara bergantian mirip seperti lampu sirine.

## Penjelasan Kode Baris per Baris

- `int timer = 500;`
  Mendeklarasikan variabel `timer` dengan nilai 500 sebagai durasi jeda (dalam milidetik). Semakin besar angkanya, perpindahan nyala LED akan semakin lambat.

- `void setup() {`
  Fungsi bawaan Arduino yang hanya dijalankan satu kali saat perangkat pertama kali dihidupkan untuk konfigurasi awal.

- `for (int pin = 2; pin <= 7; pin++) {`
  Memulai perulangan untuk mengonfigurasi banyak pin secara efisien. Variabel `pin` dimulai dari angka 2, dan akan terus diulang dan ditambah 1 hingga mencapai angka 7.

- `pinMode(pin, OUTPUT);`
  Menetapkan pin yang saat ini sedang diproses dalam perulangan (dari 2 sampai 7) agar berfungsi sebagai pin keluaran (OUTPUT).

- `}`
  Penutup dari perulangan `for` pada fungsi setup.

- `}`
  Penutup dari fungsi `setup()`.

- `void loop() {`
  Fungsi utama Arduino yang akan mengeksekusi perintah di dalamnya secara terus-menerus (berulang-ulang) selama papan Arduino menyala.

- `digitalWrite(2, HIGH);`
  Memberikan perintah agar pin 2 (LED Kiri 1) dialiri arus listrik sehingga menyala.

- `digitalWrite(3, HIGH);`
  Memberikan perintah agar pin 3 (LED Kiri 2) dialiri arus listrik sehingga menyala.

- `digitalWrite(4, HIGH);`
  Memberikan perintah agar pin 4 (LED Kiri 3) dialiri arus listrik sehingga menyala.

- `digitalWrite(5, LOW);`
  Memberikan perintah agar arus listrik pada pin 5 (LED Kanan 1) diputus sehingga mati.

- `digitalWrite(6, LOW);`
  Memberikan perintah agar arus listrik pada pin 6 (LED Kanan 2) diputus sehingga mati.

- `digitalWrite(7, LOW);`
  Memberikan perintah agar arus listrik pada pin 7 (LED Kanan 3) diputus sehingga mati.

- `delay(timer);`
  Menahan status LED saat ini (Kiri menyala, Kanan mati) selama 500 milidetik sesuai variabel `timer`.

- `digitalWrite(2, LOW);`
  Memutus arus listrik pada pin 2 (LED Kiri 1) sehingga mati.

- `digitalWrite(3, LOW);`
  Memutus arus listrik pada pin 3 (LED Kiri 2) sehingga mati.

- `digitalWrite(4, LOW);`
  Memutus arus listrik pada pin 4 (LED Kiri 3) sehingga mati.

- `digitalWrite(5, HIGH);`
  Mengalirkan arus listrik pada pin 5 (LED Kanan 1) sehingga menyala.

- `digitalWrite(6, HIGH);`
  Mengalirkan arus listrik pada pin 6 (LED Kanan 2) sehingga menyala.

- `digitalWrite(7, HIGH);`
  Mengalirkan arus listrik pada pin 7 (LED Kanan 3) sehingga menyala.

- `delay(timer);`
  Menahan status LED saat ini (Kiri mati, Kanan menyala) selama 500 milidetik. Setelah jeda ini selesai, program akan kembali ke baris pertama dalam `loop()`.

- `}`
  Penutup dari fungsi `loop()`.
```
