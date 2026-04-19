# Modul 3 - Praktikum Komunikasi UART Arduino
## Laporan Jawaban Pertanyaan Praktikum

---

## 1. Proses Input Keyboard hingga LED Menyala/Mati

Berikut adalah alur lengkap dari input keyboard hingga LED bereaksi:

```
[User ketik '1'/'0'] 
        │
        ▼
[Serial Monitor PC]
        │  (dikirim via USB / UART TX)
        ▼
[RX Buffer Arduino]
        │
        ▼
[Serial.available() > 0 → ada data]
        │
        ▼
[Serial.read() → baca 1 karakter]
        │
        ├── data == '1' → digitalWrite(PIN_LED, HIGH) → LED NYALA
        │
        └── data == '0' → digitalWrite(PIN_LED, LOW)  → LED MATI
        │
        ▼
[Serial.println() → kirim konfirmasi balik ke Serial Monitor]
```

**Penjelasan langkah per langkah:**

| Langkah | Proses |
|---------|--------|
| 1 | User mengetik karakter `'1'` atau `'0'` di Serial Monitor pada komputer |
| 2 | Data dikirim melalui kabel USB ke Arduino menggunakan protokol UART dengan baud rate 9600 |
| 3 | Arduino menerima data dan menyimpannya di **RX buffer** (buffer penerima serial) |
| 4 | `Serial.available() > 0` mendeteksi bahwa ada data yang siap dibaca di buffer |
| 5 | `Serial.read()` mengambil 1 karakter dari buffer dan menyimpannya di variabel `data` |
| 6 | Kondisi `if` mengecek nilai `data`: jika `'1'` maka `HIGH` (nyala), jika `'0'` maka `LOW` (mati) |
| 7 | `digitalWrite()` mengirimkan sinyal tegangan ke pin 12 untuk mengontrol LED secara fisik |
| 8 | Arduino mengirim pesan konfirmasi `"LED ON"` / `"LED OFF"` kembali ke Serial Monitor via UART TX |

---

## 2. Fungsi `Serial.available()`

### Mengapa `Serial.available()` harus digunakan sebelum membaca data?

`Serial.available()` berfungsi sebagai **penjaga (gatekeeper)** yang mengecek jumlah byte yang tersedia di RX buffer serial sebelum `Serial.read()` dipanggil.

### Apa yang terjadi jika baris tersebut dihilangkan?

```cpp
// ❌ TANPA Serial.available() — BERMASALAH
void loop() {
  char data = Serial.read(); // Langsung baca tanpa cek buffer
  if (data == '1') { ... }
}
```

Ketika buffer kosong, `Serial.read()` mengembalikan nilai **-1**. Dalam C++, nilai -1 diinterpretasikan sebagai karakter `'\xFF'`. Karakter ini tidak cocok dengan `'1'`, `'0'`, `'\n'`, maupun `'\r'`, sehingga program akan terus-menerus masuk ke kondisi `else` dan mencetak:

```
Perintah tidak dikenal
Perintah tidak dikenal
Perintah tidak dikenal
... (ribuan kali per detik)
```

### Perbandingan

| Kondisi | Dengan `Serial.available()` | Tanpa `Serial.available()` |
|---------|-----------------------------|----------------------------|
| Buffer kosong | Program lewati blok serial | `Serial.read()` return -1, spam output |
| Buffer ada data | Baca dan proses karakter | Baca dan proses karakter |
| Stabilitas output | Stabil dan terkontrol | Output tidak terkontrol |
| Beban CPU | Efisien | Boros, loop terus eksekusi read |

---

## 3. Modifikasi Program: Mode BLINK dengan Input `'2'`

### Kode Program Lengkap

```cpp
#include <Arduino.h>

// === DEKLARASI PIN DAN VARIABEL GLOBAL ===

const int PIN_LED = 12;
// Mendefinisikan pin LED sebagai konstanta agar tidak bisa diubah secara tidak sengaja

bool blinkMode = false;
// Flag boolean untuk menandai apakah mode kedip sedang aktif
// true  = LED berkedip terus-menerus
// false = LED dalam mode normal (ON/OFF statis)

unsigned long previousMillis = 0;
// Menyimpan waktu (ms) saat terakhir kali LED berpindah state
// Tipe unsigned long karena millis() bisa menghasilkan nilai sangat besar

const long interval = 500;
// Interval waktu antar kedip dalam milidetik
// 500ms = LED berganti state setiap 0,5 detik (1 siklus kedip = 1 detik)

bool ledState = LOW;
// Menyimpan state LED saat ini untuk keperluan toggle
// HIGH = nyala, LOW = mati

// =========================================

void setup() {
  Serial.begin(9600);
  // Inisialisasi komunikasi serial dengan baud rate 9600 bit/detik
  // Nilai ini harus sama antara Arduino dan Serial Monitor

  Serial.println("Ketik '1' untuk menyalakan LED");
  Serial.println("Ketik '0' untuk mematikan LED");
  Serial.println("Ketik '2' untuk mode BLINK (berkedip terus)");
  // Menampilkan petunjuk penggunaan ke Serial Monitor saat pertama kali dinyalakan

  pinMode(PIN_LED, OUTPUT);
  // Mengkonfigurasi pin 12 sebagai output digital
}

void loop() {

  // === BAGIAN 1: CEK INPUT SERIAL ===

  if (Serial.available() > 0) {
  // Cek apakah ada data yang diterima di buffer penerima serial (RX buffer)
  // Jika > 0, berarti ada karakter yang siap dibaca

    char data = Serial.read();
    // Membaca satu karakter dari buffer serial dan menyimpannya ke variabel data

    if (data == '1') {
      blinkMode = false;
      // Matikan mode kedip agar LED tidak terus berkedip
      digitalWrite(PIN_LED, HIGH);
      // Nyalakan LED secara permanen
      Serial.println("LED ON");
      // Kirim konfirmasi ke Serial Monitor
    }
    else if (data == '0') {
      blinkMode = false;
      // Matikan mode kedip
      digitalWrite(PIN_LED, LOW);
      // Padamkan LED
      Serial.println("LED OFF");
    }
    else if (data == '2') {
      blinkMode = true;
      // Aktifkan mode kedip — LED akan berkedip terus sampai perintah lain diberikan
      Serial.println("LED BLINK MODE ON - Ketik '0' atau '1' untuk berhenti");
    }
    else if (data != '\n' && data != '\r') {
      // Abaikan karakter newline (\n) dan carriage return (\r) yang dikirim otomatis
      // oleh Serial Monitor setelah setiap baris — tampilkan pesan hanya untuk karakter lain
      Serial.println("Perintah tidak dikenal. Gunakan '0', '1', atau '2'.");
    }
  }

  // === BAGIAN 2: EKSEKUSI BLINK MENGGUNAKAN MILLIS() ===

  if (blinkMode) {
  // Blok ini hanya dieksekusi jika mode kedip sedang aktif

    unsigned long currentMillis = millis();
    // Mengambil nilai waktu saat ini sejak Arduino dinyalakan (dalam milidetik)

    if (currentMillis - previousMillis >= interval) {
    // Cek apakah sudah lewat 500ms sejak toggle terakhir
    // Teknik ini NON-BLOCKING: program tidak berhenti, hanya mengecek selisih waktu

      previousMillis = currentMillis;
      // Perbarui waktu referensi ke waktu sekarang untuk perhitungan interval berikutnya

      ledState = (ledState == LOW) ? HIGH : LOW;
      // Toggle state LED menggunakan operator ternary:
      // jika saat ini LOW → ganti ke HIGH, jika HIGH → ganti ke LOW

      digitalWrite(PIN_LED, ledState);
      // Terapkan state baru ke pin LED secara fisik
    }
  }
}
```

### Diagram Alur Program

```
[loop() mulai]
       │
       ▼
[Serial.available() > 0?]
       │
      Ya ──► [Baca karakter]
       │            │
       │      ┌─────┴─────┬──────────┐
       │     '1'          '0'        '2'
       │      │            │          │
       │  blinkMode=F  blinkMode=F  blinkMode=T
       │  LED HIGH     LED LOW      (tidak ubah LED)
       │
      Tidak
       │
       ▼
[blinkMode == true?]
       │
      Ya ──► [currentMillis - previousMillis >= 500?]
       │                    │
       │                   Ya ──► [Toggle ledState → digitalWrite]
       │
      Tidak
       │
       ▼
[Kembali ke awal loop()]
```

---

## 4. Perbandingan `delay()` vs `millis()`

### Pilihan yang Digunakan: `millis()` ✅

### Perbandingan Mendalam

| Aspek | `delay()` | `millis()` |
|-------|-----------|------------|
| Sifat eksekusi | **Blocking** — program berhenti total | **Non-blocking** — program terus berjalan |
| Responsivitas input serial | ❌ Input bisa terlewat saat delay aktif | ✅ Input tetap terbaca setiap saat |
| Kemampuan multitasking | ❌ Tidak bisa menjalankan tugas lain | ✅ Bisa menjalankan tugas paralel |
| Kemudahan penulisan kode | Lebih sederhana | Sedikit lebih kompleks |
| Cocok untuk sistem kompleks | Tidak | Ya |

### Ilustrasi Masalah dengan `delay()`

```cpp
// ❌ Implementasi dengan delay() — BERMASALAH untuk kasus ini
void loop() {
  if (blinkMode) {
    digitalWrite(PIN_LED, HIGH);
    delay(500);                  // ← Program BEKU selama 500ms
                                 //   Input '0' atau '1' yang dikirim
                                 //   saat ini akan TERLEWAT
    digitalWrite(PIN_LED, LOW);
    delay(500);                  // ← Program BEKU lagi 500ms
  }
  if (Serial.available() > 0) { // ← Baris ini TIDAK PERNAH dicek
    ...                          //   selama delay berlangsung
  }
}
```

### Solusi dengan `millis()` — Non-Blocking

```cpp
// ✅ Implementasi dengan millis() — BENAR untuk kasus ini
void loop() {
  // Kedua blok ini dieksekusi BERGANTIAN setiap iterasi loop
  // tanpa ada yang menghalangi satu sama lain

  if (Serial.available() > 0) {         // Selalu siap baca input
    char data = Serial.read();
    ...
  }

  if (blinkMode) {
    if (millis() - previousMillis >= 500) { // Cek waktu tanpa berhenti
      previousMillis = millis();
      // Toggle LED
    }
  }
}
```

### Kesimpulan

> Gunakan **`millis()`** setiap kali ada kebutuhan timing yang harus berjalan **bersamaan** dengan tugas lain seperti membaca serial, membaca sensor, atau menjalankan logika lainnya.
>
> Gunakan **`delay()`** hanya untuk program sederhana satu-tugas di mana **responsivitas sistem tidak menjadi prioritas**.

Pada program ini, penggunaan `millis()` memastikan bahwa meskipun LED sedang dalam mode berkedip, Arduino **tetap responsif** terhadap perintah baru dari Serial Monitor kapan saja.

---

## Skema Koneksi Hardware

```
Arduino Pin 12  ──►  [Resistor 220Ω]  ──►  [LED Anoda (+)]
                                            [LED Katoda (−)]  ──►  GND
```

---

## Cara Penggunaan

1. Upload sketch `modul3_UART_modified.ino` ke Arduino.
2. Buka **Serial Monitor** (baud rate: **9600**).
3. Pastikan line ending diatur ke **"Both NL & CR"** atau **"Newline"**.
4. Ketik `1` → LED menyala permanen.
5. Ketik `0` → LED mati.
6. Ketik `2` → LED berkedip terus-menerus.
7. Ketik `1` atau `0` untuk menghentikan mode kedip kapan saja.
