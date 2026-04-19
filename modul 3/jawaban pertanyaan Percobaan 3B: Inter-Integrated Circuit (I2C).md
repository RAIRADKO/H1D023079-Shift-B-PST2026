# Modul 3 – Komunikasi I2C dengan LCD dan Potensiometer

## 3.6.4 Pertanyaan Praktikum

---

### 1. Jelaskan bagaimana cara kerja komunikasi I2C antara Arduino dan LCD pada rangkaian tersebut!

Komunikasi I2C (Inter-Integrated Circuit) bekerja menggunakan **dua jalur sinyal** saja:

- **SDA (Serial Data)** – jalur untuk mengirim dan menerima data
- **SCL (Serial Clock)** – jalur clock yang mengatur sinkronisasi pengiriman data

Pada rangkaian ini, Arduino bertindak sebagai **master** dan modul I2C LCD bertindak sebagai **slave**. Cara kerjanya:

1. Library `Wire.h` menginisialisasi komunikasi I2C pada pin **A4 (SDA)** dan **A5 (SCL)** di Arduino Uno.
2. LCD dengan modul I2C memiliki alamat unik, pada kode ini adalah `0x27`:
   ```cpp
   LiquidCrystal_I2C lcd(0x27, 16, 2);
   ```
3. Saat `lcd.init()` dipanggil di `setup()`, Arduino mengirim sinyal inisialisasi ke alamat `0x27` melalui bus I2C.
4. Setiap perintah `lcd.print()` atau `lcd.setCursor()` menyebabkan Arduino mengirimkan paket data melalui SDA secara sinkron dengan clock SCL.
5. Modul I2C pada LCD (biasanya PCF8574) menerima data serial tersebut dan mengonversinya menjadi sinyal paralel 4-bit untuk menggerakkan layar LCD.

Keunggulan I2C: hanya butuh **2 kabel** untuk mengendalikan LCD, dibanding antarmuka paralel yang membutuhkan 6–10 pin.

---

### 2. Apakah pin potensiometer harus seperti itu? Jelaskan yang terjadi apabila pin kiri dan pin kanan tertukar!

Potensiometer memiliki 3 pin:
- **Pin kiri** → VCC (5V)
- **Pin tengah** → output sinyal (terhubung ke A0)
- **Pin kanan** → GND (0V)

Pada kode ini, pin tengah potensiometer terhubung ke pin **A0** Arduino:
```cpp
const int pinPot = A0;
int nilai = analogRead(pinPot);
```

**Jika pin kiri dan pin kanan tertukar** (VCC dan GND dibalik), maka:

- Arah pembacaan ADC akan **terbalik**. Ketika potensiometer diputar ke kiri (yang seharusnya menghasilkan nilai 0), justru menghasilkan nilai **1023**, dan sebaliknya.
- Di LCD, bar indicator akan **penuh ketika puter kiri** dan **kosong ketika putar kanan** — kebalikan dari perilaku yang diharapkan.
- Secara elektrikal, rangkaian **tidak rusak** karena potensiometer hanya berfungsi sebagai pembagi tegangan resistif; hanya logika pembacaan yang terbalik.
- Nilai Volt yang ditampilkan juga terbalik: posisi paling kiri menampilkan ~5V dan paling kanan ~0V.

---

### 3. Modifikasi Program: Menggabungkan UART dan I2C (Keduanya sebagai Output)

Program dimodifikasi sehingga data ditampilkan **sekaligus di LCD dan Serial Monitor**.

#### Spesifikasi Output Serial Monitor:
```
ADC: 0   0% | setCursor(0, 0) dan Bar (level) | setCursor(0, 1)
```

#### Kode yang Dimodifikasi (`modul3_I2C.ino`):

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

// Ganti alamat jika perlu (0x27 / 0x20)
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinPot = A0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
}

void loop() {
  int nilai = analogRead(pinPot);

  // Mapping ke bar (0-16) dan persen (0-100)
  int panjangBar = map(nilai, 0, 1023, 0, 16);
  int persen      = map(nilai, 0, 1023, 0, 100);
  float volt      = nilai * (5.0 / 1023.0);

  // ── UART Output ──────────────────────────────────────────────
  // Format: ADC: 0   0.00 V   0%
  Serial.print("ADC: ");
  Serial.print(nilai);
  Serial.print("   Volt: ");
  Serial.print(volt, 2);
  Serial.print(" V");
  Serial.print("   Persen: ");
  Serial.print(persen);
  Serial.println("%");

  // ── I2C / LCD Output ─────────────────────────────────────────
  // Baris 0: nilai ADC, Volt, Persen
  lcd.setCursor(0, 0);
  lcd.print("ADC:");
  lcd.print(nilai);
  lcd.print(" ");
  lcd.print(volt, 1);
  lcd.print("V ");

  // Baris 1: bar visual
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    if (i < panjangBar) {
      lcd.print((char)255);  // blok penuh
    } else {
      lcd.print(" ");
    }
  }

  delay(200);
}
```

#### Penjelasan Setiap Baris Kode:

| Baris | Kode | Penjelasan |
|---|---|---|
| 1 | `#include <Wire.h>` | Library untuk komunikasi I2C |
| 2 | `#include <LiquidCrystal_I2C.h>` | Library untuk LCD dengan modul I2C |
| 6 | `LiquidCrystal_I2C lcd(0x27, 16, 2)` | Inisialisasi LCD di alamat I2C `0x27`, ukuran 16 kolom × 2 baris |
| 8 | `const int pinPot = A0` | Mendefinisikan pin analog A0 untuk potensiometer |
| 11 | `Serial.begin(9600)` | Inisialisasi UART dengan baud rate 9600 bps |
| 12 | `lcd.init()` | Inisialisasi LCD via I2C |
| 13 | `lcd.backlight()` | Menyalakan backlight LCD |
| 17 | `analogRead(pinPot)` | Membaca tegangan ADC 10-bit (0–1023) dari potensiometer |
| 20 | `map(nilai, 0, 1023, 0, 16)` | Mengubah skala ADC (0–1023) menjadi panjang bar (0–16) |
| 21 | `map(nilai, 0, 1023, 0, 100)` | Mengubah skala ADC menjadi persen (0–100%) |
| 22 | `nilai * (5.0 / 1023.0)` | Mengonversi nilai ADC ke tegangan dalam Volt |
| 25–31 | `Serial.print(...)` | Mengirim data ke Serial Monitor (UART output) dengan format tabel |
| 34 | `lcd.setCursor(0, 0)` | Memindahkan kursor ke kolom 0, baris 0 (baris atas LCD) |
| 35–38 | `lcd.print(...)` | Menampilkan nilai ADC, Volt ke baris atas LCD |
| 41 | `lcd.setCursor(0, 1)` | Memindahkan kursor ke kolom 0, baris 1 (baris bawah LCD) |
| 42–48 | `for (int i = 0; i < 16; i++)` | Looping untuk menggambar bar di 16 kolom baris bawah |
| 44 | `lcd.print((char)255)` | Mencetak karakter blok penuh (█) untuk mengisi bar |
| 47 | `lcd.print(" ")` | Mencetak spasi untuk bagian bar yang kosong |
| 51 | `delay(200)` | Jeda 200ms sebelum pembacaan berikutnya |

---

### 4. Tabel Pengamatan Serial Monitor

> **Catatan:** Nilai Volt dan Persen dihitung dengan rumus:
> - `Volt = ADC × (5.0 / 1023.0)`
> - `Persen = ADC × (100 / 1023)`

| ADC | Volt (V) | Persen (%) |
|:---:|:--------:|:----------:|
| 1   | 0.00 V   | 0%         |
| 21  | 0.10 V   | 2%         |
| 49  | 0.24 V   | 5%         |
| 74  | 0.36 V   | 7%         |
| 96  | 0.47 V   | 9%         |

> Nilai di atas merupakan hasil kalkulasi berdasarkan rumus konversi ADC. Nilai aktual dapat sedikit berbeda tergantung toleransi potensiometer dan tegangan referensi Arduino.
