### 1. Kapan program masuk ke blok `if`?
Program masuk ke dalam blok `if` ketika variabel `timeDelay` memiliki nilai **kurang dari atau sama dengan 100** (`<= 100`). Ini terjadi setelah beberapa kali siklus perulangan di mana nilai `timeDelay` terus dikurangi hingga kedipan LED mencapai titik tercepatnya.

### 2. Kapan program masuk ke blok `else`?
Program masuk ke dalam blok `else` selama variabel `timeDelay` masih bernilai **lebih besar dari 100**. Selama kondisi ini terpenuhi, program akan mengeksekusi perintah di dalam `else` (yaitu mengurangi nilai `timeDelay` sebanyak 100 ms) agar kedipan LED secara bertahap menjadi semakin cepat.

### 3. Apa fungsi dari perintah `delay(timeDelay)`?
Perintah `delay(timeDelay)` berfungsi untuk **menghentikan sementara (menjeda) eksekusi program** selama waktu yang ditentukan oleh variabel `timeDelay` dalam satuan milidetik (ms). 
* Pada baris pertama kemunculannya, ia mengatur seberapa lama LED tetap dalam keadaan menyala. 
* Pada baris kedua, ia mengatur seberapa lama LED tetap dalam keadaan mati sebelum melanjutkan siklus berikutnya.

---

### 4. Modifikasi Kode

Untuk mengubah alur agar kedipan tidak langsung di-reset melainkan melambat ke tahap "sedang" lalu "mati", kita bisa menggunakan variabel bantuan (contoh: boolean) untuk melacak apakah siklus sedang dipercepat atau diperlambat. 

Berikut adalah kode yang sudah dimodifikasi:

```cpp
const int ledPin = 13; 
int timeDelay = 1000; 
bool isSpeedingUp = true; 

void setup() { 
  pinMode(ledPin, OUTPUT); 
} 

void loop() { 
  // Nyalakan LED 
  digitalWrite(ledPin, HIGH); 
  delay(timeDelay); 
  
  // Matikan LED 
  digitalWrite(ledPin, LOW); 
  delay(timeDelay); 
  
  // Logika pengatur alur (Lambat -> Cepat -> Sedang -> Mati)
  if (isSpeedingUp) { 
    if (timeDelay <= 100) { 
      isSpeedingUp = false; 
    } else { 
      timeDelay -= 100; 
    }
  } else { 
    if (timeDelay >= 500) { 
      delay(3000);        
      timeDelay = 1000;   
      isSpeedingUp = true;
    } else { 
      timeDelay += 100;   
    }
  }
}
```

penjelasan setiap baris kodenya:

```markdown
# Dokumentasi Program LED: Lambat → Cepat → Sedang → Mati

Program ini mengontrol LED pada pin 13 Arduino. Kedipan akan dimulai dari sangat lambat, kemudian secara bertahap menjadi sangat cepat. Bukannya langsung mati/reset, kedipan akan berangsur-angsur melambat menjadi fase "sedang", lalu mati selama 3 detik sebelum mengulang siklus.

## Penjelasan Kode Baris per Baris

- `const int ledPin = 13;`
  Mendeklarasikan variabel konstan untuk menetapkan bahwa komponen LED terhubung ke pin digital 13.
  
- `int timeDelay = 1000;`
  Mendeklarasikan variabel `timeDelay` dengan nilai awal 1000 ms (1 detik) sebagai durasi jeda kedipan (fase lambat).
  
- `bool isSpeedingUp = true;`
  Mendeklarasikan variabel penanda (flag) untuk menentukan apakah saat ini siklus kedipan sedang dipercepat (`true`) atau sedang diperlambat (`false`).

- `void setup() {`
  Fungsi bawaan Arduino yang dieksekusi hanya satu kali saat alat dihidupkan (diberi daya).

- `pinMode(ledPin, OUTPUT);`
  Mengatur pin 13 (ledPin) agar berfungsi sebagai pin keluaran arus listrik (OUTPUT).

- `}`
  Penutup dari fungsi `setup()`.

- `void loop() {`
  Fungsi utama Arduino yang akan dieksekusi secara berulang-ulang (looping) selama alat menyala.

- `digitalWrite(ledPin, HIGH);`
  Memberikan tegangan (HIGH) pada pin 13 sehingga LED menyala.

- `delay(timeDelay);`
  Memberikan jeda waktu sesuai dengan besaran nilai variabel `timeDelay` agar LED menyala dalam durasi tertentu.

- `digitalWrite(ledPin, LOW);`
  Menghentikan tegangan (LOW) pada pin 13 sehingga LED padam/mati.

- `delay(timeDelay);`
  Memberikan jeda waktu sesuai variabel `timeDelay` agar LED tetap mati dalam durasi tersebut.

- `if (isSpeedingUp) {`
  Mengecek apakah alur program saat ini berada pada fase percepatan (dari lambat menuju cepat).

- `if (timeDelay <= 100) {`
  Jika sedang dipercepat, cek kembali apakah durasi `timeDelay` sudah menyentuh batas tercepatnya (100 ms).

- `isSpeedingUp = false;`
  Jika kedipan sudah paling cepat, ubah tanda menjadi `false` agar putaran kode selanjutnya memasuki fase perlambatan.

- `} else {`
  Kondisi apabila jeda waktu masih di atas 100 ms (belum mencapai batas tercepat).

- `timeDelay -= 100;`
  Kurangi nilai `timeDelay` sebesar 100 ms agar kedipan LED perlahan bertambah cepat di perulangan berikutnya.

- `}`
  Penutup untuk blok if-else dalam fase percepatan.

- `} else {`
  Masuk ke blok ini jika `isSpeedingUp` bernilai `false`, yang berarti LED sedang dalam fase dari cepat menuju "sedang".

- `if (timeDelay >= 500) {`
  Mengecek apakah proses perlambatan sudah mencapai tahap "sedang", yang ditandai dengan durasi menyentuh/melewati angka 500 ms.

- `delay(3000);`
  Jika sudah mencapai tahap sedang, jeda program selama 3000 ms (3 detik). Posisi terakhir LED adalah mati (karena eksekusi terakhir sebelum blok logika ini adalah `LOW`), sehingga tercipta fase mati yang lama.

- `timeDelay = 1000;`
  Mengembalikan nilai jeda waktu ke 1000 ms untuk mempersiapkan siklus reset ke keadaan "lambat" seperti semula.

- `isSpeedingUp = true;`
  Mengembalikan penanda arah menjadi `true` agar perulangan berikutnya kembali ke fase percepatan.

- `} else {`
  Kondisi jika fase "sedang" (500 ms) masih belum tercapai saat program sedang melambat.

- `timeDelay += 100;`
  Tambahkan durasi jeda sebesar 100 ms agar kecepatan kedipan berangsur-angsur melambat dari tahap cepat ke tahap sedang.

- `}`
  Penutup dari blok perlambatan dan fase mati.

- `}`
  Penutup dari fungsi logika utama arah kecepatan.

- `}`
  Penutup dari fungsi `loop()`.
```
