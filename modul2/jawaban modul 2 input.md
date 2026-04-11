### 1. Rangkaian Schematic Percobaan
![pushButton (1)](https://github.com/user-attachments/assets/8da3f589-6d69-41b3-b464-c3d0cef4bffc)

---

### 2. Penggunaan `INPUT_PULLUP` pada Arduino Uno
[cite_start]Mode `INPUT_PULLUP` digunakan untuk mengaktifkan resistor *pull-up* internal yang sudah ada di dalam mikrokontroler Arduino[cite: 6]. 

**Keuntungannya dibandingkan rangkaian biasa:**
* **Menghemat Komponen:** Kamu tidak perlu menambahkan resistor eksternal secara fisik pada breadboard.
* **Mencegah *Floating*:** Tanpa resistor pull-up/pull-down, pin input yang tidak terhubung ke mana-mana akan berada pada state *floating* (mengambang), sehingga nilainya bisa berubah-ubah antara `HIGH` dan `LOW` karena noise elektromagnetik.
* **Rangkaian Lebih Sederhana:** Kamu cukup menghubungkan tombol langsung ke pin Arduino dan GND. Saat tombol tidak ditekan, pin membaca `HIGH`. Saat tombol ditekan, arus mengalir ke GND, dan pin membaca `LOW` (dikenal sebagai *Active LOW*). [cite_start]Hal ini terlihat pada kodemu di mana kondisi maju terjadi saat `upState == LOW`[cite: 8].

---

### 3. Kemungkinan Penyebab Satu LED Segmen Tidak Menyala
Jika ada satu segmen (misalnya segmen 'b') yang mati sementara yang lain menyala, berikut adalah proses *troubleshooting*-nya:

**Dari Sisi Hardware:**
* **LED Segmen Putus/Rusak:** LED di dalam komponen 7-segment untuk bagian tersebut sudah terbakar atau rusak.
* **Kabel Jumper Longgar:** Kabel yang menghubungkan pin Arduino ke pin segmen tersebut tidak menancap dengan baik di breadboard.
* **Resistor Rusak:** Jika kamu menggunakan resistor individu untuk masing-masing kaki segmen, resistor pada jalur tersebut mungkin rusak atau kakinya patah.
* **Pin Arduino Rusak:** Pin I/O pada Arduino yang bertugas mengirim sinyal ke segmen tersebut rusak (bisa dites dengan memindahkan ke pin lain).

**Dari Sisi Software:**
* [cite_start]**Kesalahan Deklarasi Pin:** Terjadi salah ketik (typo) pada array `segmentPins` sehingga pin yang dideklarasikan tidak sesuai dengan kabel yang dipasang[cite: 1].
* [cite_start]**Kesalahan Array Pola:** Nilai biner pada array `digitPattern` untuk angka tertentu salah ketik menjadi `0` padahal seharusnya `1`[cite: 3].
* [cite_start]**Gagal Inisialisasi OUTPUT:** Terjadi kesalahan pada blok `setup()` sehingga pin tersebut tidak di-set sebagai `OUTPUT`, namun ini jarang terjadi jika kamu menggunakan *looping* array seperti di kodemu[cite: 6].

---

### 4. Modifikasi Rangkaian dan Program (Increment & Decrement)

**Modifikasi Hardware:**
Tambahkan satu push button lagi untuk fungsi *decrement* (mundur). Hubungkan satu kakinya ke **Pin 3** dan kaki lainnya ke **GND**.

**Modifikasi Kode & Penjelasan (README.md):**

```markdown
# 7-Segment Counter Hexadecimal (0-F) dengan 2 Tombol

Proyek ini adalah sistem counter heksadesimal (0-9, A-F) menggunakan 7-segment display (Common Cathode) dan dua buah push button yang berfungsi sebagai increment (tambah) dan decrement (kurang).

## Komponen Hardware
- 1x Arduino Uno
- 1x 7-Segment Display (Common Cathode)
- 2x Push Button
- Kabel Jumper & Resistor 220Ω

## Penjelasan Kode (Line by Line)

```cpp
#include <Arduino.h>

// ========== PIN CONFIGURATION ==========
// Deklarasi pin Arduino yang terhubung ke 7-segment (urutan: a, b, c, d, e, f, g, dp)
const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4};

// Deklarasi pin untuk tombol increment (maju) dan decrement (mundur)
const int btnUp = 2;
const int btnDown = 3;

// ========== DATA PATTERN ==========
// Array 2D untuk menyimpan pola nyala LED 7-segment dari angka 0 hingga F.
// Menggunakan logika 1 untuk menyalakan LED (Common Cathode) dan 0 untuk mematikan.
byte digitPattern[16][8] = {
  {1,1,1,1,1,1,0,0}, // 0
  {0,1,1,0,0,0,0,0}, // 1
  {1,1,0,1,1,0,1,0}, // 2
  {1,1,1,1,0,0,1,0}, // 3
  {0,1,1,0,0,1,1,0}, // 4
  {1,0,1,1,0,1,1,0}, // 5
  {1,0,1,1,1,1,1,0}, // 6
  {1,1,1,0,0,0,0,0}, // 7
  {1,1,1,1,1,1,1,0}, // 8
  {1,1,1,1,0,1,1,0}, // 9
  {1,1,1,0,1,1,1,0}, // A
  {0,0,1,1,1,1,1,0}, // b
  {1,0,0,1,1,1,0,0}, // C
  {0,1,1,1,1,0,1,0}, // d
  {1,0,0,1,1,1,1,0}, // E
  {1,0,0,0,1,1,1,0}  // F
};

// Variabel untuk menyimpan angka yang sedang ditampilkan saat ini
int currentDigit = 0;

// Variabel untuk menyimpan status tombol sebelumnya (untuk mendeteksi kapan tombol DITEKAN, bukan sekadar DITAHAN)
bool lastUpState = HIGH;
bool lastDownState = HIGH;

// ========== FUNCTION ==========
// Fungsi khusus untuk menampilkan angka ke 7-segment berdasarkan nilai parameter 'num'
void displayDigit(int num)
{
  for(int i=0; i<8; i++)
  {
    // Mengirim sinyal (HIGH/LOW) ke setiap pin segmen sesuai pola di dalam array digitPattern
    digitalWrite(segmentPins[i], digitPattern[num][i]);
  }
}

// ========== SETUP ==========
void setup()
{
  // Mengatur semua pin yang terhubung ke 7-segment sebagai OUTPUT
  for(int i=0; i<8; i++)
  {
    pinMode(segmentPins[i], OUTPUT);
  }

  // Mengatur pin tombol sebagai INPUT dengan resistor pull-up internal.
  // Default state pin akan menjadi HIGH, dan menjadi LOW saat tombol ditekan.
  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);

  // Menampilkan angka awal (0) saat perangkat pertama kali dinyalakan
  displayDigit(currentDigit);
}

// ========== LOOP ==========
void loop()
{
  // Membaca status (HIGH/LOW) dari kedua tombol secara real-time
  bool upState = digitalRead(btnUp);
  bool downState = digitalRead(btnDown);

  // ====== LOGIKA INCREMENT (TOMBOL UP) ======
  // Mengecek "Falling Edge" -> Tombol berubah dari tidak ditekan (HIGH) menjadi ditekan (LOW)
  if(lastUpState == HIGH && upState == LOW)
  {
    currentDigit++; // Tambah angka
    if(currentDigit > 15) currentDigit = 0; // Jika melebihi F (15), kembali ke 0
    displayDigit(currentDigit); // Tampilkan angka baru
  }

  // ====== LOGIKA DECREMENT (TOMBOL DOWN) ======
  // Mengecek "Falling Edge" untuk tombol mundur
  if(lastDownState == HIGH && downState == LOW)
  {
    currentDigit--; // Kurangi angka
    if(currentDigit < 0) currentDigit = 15; // Jika kurang dari 0, melompat ke F (15)
    displayDigit(currentDigit); // Tampilkan angka baru
  }

  // Memperbarui status tombol untuk pembacaan di siklus loop berikutnya
  lastUpState = upState;
  lastDownState = downState;
  
  // Memberikan sedikit jeda untuk *debounce* sederhana agar tombol tidak terbaca menekan berkali-kali
  delay(50); 
}
```
```
