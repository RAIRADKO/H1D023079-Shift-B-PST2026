# Modul 5 – Real-Time Operating System: Multitasking dan Komunikasi Task

## Percobaan 5A: Multitasking

### Deskripsi Program

Program `modul5_multitasking` menjalankan tiga task secara concurrent menggunakan FreeRTOS:

| Task | Pin | Fungsi |
|------|-----|--------|
| `TaskBlink1` | D8 | Kedip LED kuning setiap 200ms |
| `TaskBlink2` | D7 | Kedip LED merah setiap 300ms |
| `Taskprint` | - | Cetak counter ke Serial Monitor setiap 500ms |

### Source Code Lengkap dengan Komentar

```cpp
#include <Arduino_FreeRTOS.h>  // Library FreeRTOS untuk Arduino

// Deklarasi prototype fungsi task
void TaskBlink1( void *pvParameters );
void TaskBlink2( void *pvParameters );
void Taskprint( void *pvParameters );

void setup() {
  Serial.begin(9600);  // Inisialisasi komunikasi serial

  // xTaskCreate(fungsi, nama, stack_size, parameter, prioritas, handle)
  // Stack size 128 word cukup untuk task sederhana
  // Prioritas 1 = sama untuk semua task (round-robin scheduling)
  xTaskCreate(TaskBlink1, "task1", 128, NULL, 1, NULL);
  xTaskCreate(TaskBlink2, "task2", 128, NULL, 1, NULL);
  xTaskCreate(Taskprint,  "task3", 128, NULL, 1, NULL);

  vTaskStartScheduler();  // Memulai RTOS scheduler (opsional di Arduino FreeRTOS)
}

void loop() {
  // Dibiarkan kosong — kontrol sudah diambil alih oleh RTOS scheduler
}

void TaskBlink1(void *pvParameters) {
  pinMode(8, OUTPUT);  // Inisialisasi pin 8 sebagai output (LED kuning)
  while(1) {
    Serial.println("Task1");
    digitalWrite(8, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS);  // Delay non-blocking selama 200ms
    digitalWrite(8, LOW);
    vTaskDelay(200 / portTICK_PERIOD_MS);  // Total periode: 400ms
  }
}

void TaskBlink2(void *pvParameters) {
  pinMode(7, OUTPUT);  // Inisialisasi pin 7 sebagai output (LED merah)
  while(1) {
    Serial.println("Task2");
    digitalWrite(7, HIGH);
    vTaskDelay(300 / portTICK_PERIOD_MS);  // Delay non-blocking selama 300ms
    digitalWrite(7, LOW);
    vTaskDelay(300 / portTICK_PERIOD_MS);  // Total periode: 600ms
  }
}

void Taskprint(void *pvParameters) {
  int counter = 0;
  while(1) {
    counter++;
    Serial.println(counter);               // Cetak nilai counter ke Serial Monitor
    vTaskDelay(500 / portTICK_PERIOD_MS);  // Delay 500ms antara setiap increment
  }
}
```

---

### Jawaban Pertanyaan Praktikum

#### 1. Apakah ketiga task berjalan secara bersamaan atau bergantian? Jelaskan mekanismenya!

Ketiga task berjalan **secara bergantian (concurrent), bukan benar-benar paralel**, karena Arduino Uno hanya memiliki satu core prosesor (ATmega328P).

**Mekanisme:**

FreeRTOS menggunakan **preemptive scheduling** berbasis time-slice. Scheduler secara periodik (setiap satu tick) mengevaluasi task mana yang berhak mendapatkan CPU. Karena ketiga task memiliki prioritas yang sama (prioritas 1), scheduler menerapkan **Round-Robin scheduling** — setiap task mendapat giliran CPU secara berurutan.

Kunci mekanisme ini adalah fungsi `vTaskDelay()`. Ketika sebuah task memanggil `vTaskDelay()`, task tersebut masuk ke state **Blocked** dan melepaskan CPU sepenuhnya. Scheduler kemudian segera menjalankan task lain yang sedang dalam state **Ready**. Ini berbeda dengan `delay()` biasa yang memblokir CPU tanpa memberi kesempatan task lain berjalan.

```
Timeline (ilustrasi):
Task1 [RUN]-->[BLOCKED 200ms]
Task2          [RUN]-------->[BLOCKED 300ms]
Task3               [RUN]------------->[BLOCKED 500ms]
Task1                    [READY]->[RUN] (setelah 200ms selesai)
...
```

Hasilnya, dari sudut pandang pengguna, ketiga LED berkedip dengan frekuensi berbeda seolah-olah berjalan bersamaan.

---

#### 2. Bagaimana cara menambahkan task keempat? Jelaskan langkahnya!

Untuk menambahkan task keempat, ikuti langkah berikut:

**Langkah 1 – Deklarasikan prototype fungsi task:**
```cpp
void TaskKeempat( void *pvParameters );
```

**Langkah 2 – Daftarkan task di dalam `setup()` menggunakan `xTaskCreate()`:**
```cpp
xTaskCreate(
  TaskKeempat,   // Pointer ke fungsi task
  "task4",       // Nama task (untuk debugging)
  128,           // Stack size (word)
  NULL,          // Parameter yang dikirim ke task (NULL jika tidak ada)
  1,             // Prioritas task
  NULL           // Handle task (NULL jika tidak diperlukan)
);
```

**Langkah 3 – Implementasikan fungsi task:**
```cpp
void TaskKeempat(void *pvParameters) {
  // Inisialisasi jika diperlukan
  while(1) {
    // Tulis logika task di sini
    Serial.println("Task4");
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 detik
  }
}
```

> **Catatan:** Perhatikan total penggunaan RAM. Setiap task membutuhkan stack memori (minimal 128 word = 256 byte). Arduino Uno hanya memiliki 2KB RAM, sehingga jumlah task yang dapat dibuat terbatas.

---

#### 3. Modifikasi Program: Potensiometer Mengontrol Kecepatan LED

Program dimodifikasi dengan menambahkan task pembaca potensiometer yang nilainya digunakan untuk menentukan kecepatan kedip LED secara dinamis.

```cpp
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // Untuk mutex (proteksi shared variable)

// Shared variable: nilai delay LED (dalam ms)
volatile int blinkDelay = 200;
SemaphoreHandle_t xMutex;

void TaskBlink(void *pvParameters);
void TaskReadPot(void *pvParameters);

void setup() {
  Serial.begin(9600);
  xMutex = xSemaphoreCreateMutex();  // Buat mutex untuk proteksi shared variable

  xTaskCreate(TaskBlink,   "blink",  128, NULL, 1, NULL);
  xTaskCreate(TaskReadPot, "pot",    128, NULL, 1, NULL);
}

void loop() {}

// Task membaca nilai potensiometer dari pin A0
void TaskReadPot(void *pvParameters) {
  while(1) {
    int potValue = analogRead(A0);        // Baca nilai ADC (0–1023)
    int newDelay = map(potValue, 0, 1023, 50, 1000);  // Peta ke rentang 50–1000ms

    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      blinkDelay = newDelay;             // Update shared variable dengan aman
      xSemaphoreGive(xMutex);
    }

    Serial.print("Pot value: ");
    Serial.print(potValue);
    Serial.print(" | Blink delay: ");
    Serial.println(blinkDelay);

    vTaskDelay(100 / portTICK_PERIOD_MS);  // Baca setiap 100ms
  }
}

// Task mengkedipkan LED berdasarkan nilai delay dari potensiometer
void TaskBlink(void *pvParameters) {
  pinMode(8, OUTPUT);
  while(1) {
    int currentDelay;

    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      currentDelay = blinkDelay;         // Baca shared variable dengan aman
      xSemaphoreGive(xMutex);
    }

    digitalWrite(8, HIGH);
    vTaskDelay(currentDelay / portTICK_PERIOD_MS);
    digitalWrite(8, LOW);
    vTaskDelay(currentDelay / portTICK_PERIOD_MS);
  }
}
```

**Hasil:** Ketika potensiometer diputar ke kiri (nilai mendekati 0), LED berkedip sangat cepat (~50ms). Ketika diputar ke kanan (nilai mendekati 1023), LED berkedip lambat (~1000ms). Perubahan terjadi secara real-time tanpa menghentikan sistem.

**Mutex digunakan** untuk mencegah race condition saat kedua task mengakses variabel `blinkDelay` secara bersamaan.

---

## Percobaan 5B: Komunikasi Task (Queue)

### Deskripsi Program

Program `modul5_taskqueue` mendemonstrasikan komunikasi antar-task menggunakan **FreeRTOS Queue**. Task `read_data` berperan sebagai *producer* yang mengirim data struct berisi suhu dan kelembaban ke queue, sedangkan task `display` berperan sebagai *consumer* yang mengambil dan menampilkan data tersebut ke Serial Monitor.

### Source Code Lengkap dengan Komentar

```cpp
#include <Arduino_FreeRTOS.h>
#include <queue.h>          // Library queue FreeRTOS

// Struct untuk menyimpan data bacaan sensor
struct readings {
  int temp;  // Suhu
  int h;     // Kelembaban (humidity)
};

QueueHandle_t my_queue;  // Handle untuk queue

void setup() {
  Serial.begin(9600);

  // Membuat queue dengan kapasitas 1 item, setiap item berukuran sizeof(struct readings)
  // Kapasitas 1 berarti producer harus menunggu consumer mengambil data sebelum bisa kirim lagi
  my_queue = xQueueCreate(1, sizeof(struct readings));

  // Prioritas 0 untuk kedua task → scheduler memilih secara round-robin
  xTaskCreate(read_data, "read sensors", 128, NULL, 0, NULL);
  xTaskCreate(display,   "display",      128, NULL, 0, NULL);
}

void loop() {}

// Task Producer: membaca (simulasi) dan mengirim data ke queue
void read_data(void *pvParameters) {
  struct readings x;
  for(;;) {
    x.temp = 54;   // Data statis (simulasi sensor)
    x.h    = 30;

    // xQueueSend: kirim data ke queue
    // portMAX_DELAY: tunggu selamanya jika queue penuh (blocking)
    xQueueSend(my_queue, &x, portMAX_DELAY);

    vTaskDelay(100 / portTICK_PERIOD_MS);  // Kirim data setiap 100ms
  }
}

// Task Consumer: menerima data dari queue dan menampilkan ke Serial Monitor
void display(void *pvParameters) {
  struct readings x;
  for(;;) {
    // xQueueReceive: ambil data dari queue
    // portMAX_DELAY: tunggu selamanya jika queue kosong (blocking)
    // Mengembalikan pdPASS jika berhasil menerima data
    if (xQueueReceive(my_queue, &x, portMAX_DELAY) == pdPASS) {
      Serial.print("temp = ");
      Serial.println(x.temp);
      Serial.print("humidity = ");
      Serial.println(x.h);
    }
  }
}
```

---

### Jawaban Pertanyaan Praktikum

#### 1. Apakah kedua task berjalan secara bersamaan atau bergantian? Jelaskan mekanismenya!

Kedua task berjalan **secara bergantian (concurrent)**, bukan paralel, karena prosesor hanya memiliki satu core.

**Mekanisme dengan Queue:**

Queue pada FreeRTOS bertindak sebagai **mekanisme sinkronisasi otomatis**. Alur eksekusinya adalah sebagai berikut:

1. **`read_data` (producer)** berjalan, mengisi struct `x`, lalu memanggil `xQueueSend()`. Karena queue berkapasitas 1 dan langsung terisi, task ini masuk ke state **Blocked** sambil menunggu consumer mengambil data (atau setelah delay 100ms).
2. **`display` (consumer)** mendapat giliran CPU, memanggil `xQueueReceive()`. Data tersedia di queue, sehingga langsung diterima dan ditampilkan ke Serial Monitor. Setelah selesai, task ini kembali ke `xQueueReceive()` dan **Blocked** menunggu data baru.
3. Setelah `read_data` selesai delay 100ms, ia kembali **Ready** dan mengirim data baru, membangunkan `display`.

Pola ini disebut **producer-consumer pattern** — kedua task saling menunggu satu sama lain melalui queue, bukan timer. Queue sekaligus berfungsi sebagai buffer data dan mekanisme sinkronisasi.

---

#### 2. Apakah program ini berpotensi mengalami race condition? Jelaskan!

**Tidak**, program ini **tidak berpotensi mengalami race condition**, justru karena menggunakan queue.

**Penjelasan:**

Race condition terjadi ketika dua atau lebih task mengakses dan memodifikasi *shared resource* (seperti variabel global) secara bersamaan tanpa mekanisme proteksi. Akibatnya, nilai akhir variabel bergantung pada urutan eksekusi yang tidak bisa diprediksi.

Pada program ini, data antara kedua task **tidak dibagikan secara langsung**. Data hanya berpindah melalui `xQueueSend()` dan `xQueueReceive()`, yang merupakan operasi **thread-safe** (atomic) yang sudah diproteksi secara internal oleh FreeRTOS menggunakan mekanisme critical section. Hanya satu task yang bisa mengakses queue pada satu waktu.

Sebaliknya, jika data suhu dan kelembaban disimpan dalam **variabel global** tanpa mutex seperti berikut, *maka* race condition bisa terjadi:

```cpp
// BERBAHAYA — potensi race condition:
int globalTemp = 0;
int globalHumidity = 0;

// Task 1 menulis:
globalTemp = 54;        // CPU direbut di sini oleh scheduler
globalHumidity = 30;    // Task 2 sudah membaca globalTemp tapi globalHumidity belum diupdate
```

Penggunaan queue pada program ini sudah merupakan praktik yang benar dan aman.

---

#### 3. Modifikasi Program: Menggunakan Sensor DHT Sesungguhnya

Program dimodifikasi untuk membaca data nyata dari sensor DHT11/DHT22 melalui library `DHT.h`.

**Wiring:** Pin DATA sensor DHT → Pin D2 Arduino, VCC → 5V, GND → GND.

```cpp
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <DHT.h>         // Library sensor DHT

#define DHTPIN  2        // Pin data sensor DHT terhubung ke D2
#define DHTTYPE DHT11    // Ganti ke DHT22 jika menggunakan DHT22

DHT dht(DHTPIN, DHTTYPE);

struct readings {
  float temp;   // Ubah ke float untuk presisi desimal
  float h;
};

QueueHandle_t my_queue;

void read_data(void *pvParameters);
void display(void *pvParameters);

void setup() {
  Serial.begin(9600);
  dht.begin();  // Inisialisasi sensor DHT

  my_queue = xQueueCreate(1, sizeof(struct readings));

  xTaskCreate(read_data, "read sensors", 256, NULL, 1, NULL);  // Stack lebih besar untuk DHT
  xTaskCreate(display,   "display",      128, NULL, 0, NULL);
}

void loop() {}

void read_data(void *pvParameters) {
  struct readings x;
  for(;;) {
    x.h    = dht.readHumidity();     // Baca kelembaban (%)
    x.temp = dht.readTemperature();  // Baca suhu (°C)

    // Validasi: sensor DHT bisa mengembalikan NaN jika gagal baca
    if (!isnan(x.temp) && !isnan(x.h)) {
      xQueueSend(my_queue, &x, portMAX_DELAY);
    } else {
      Serial.println("Gagal membaca sensor DHT!");
    }

    // DHT membutuhkan jeda minimal 2 detik antar pembacaan
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void display(void *pvParameters) {
  struct readings x;
  for(;;) {
    if (xQueueReceive(my_queue, &x, portMAX_DELAY) == pdPASS) {
      Serial.print("Suhu     : ");
      Serial.print(x.temp);
      Serial.println(" °C");
      Serial.print("Kelembaban: ");
      Serial.print(x.h);
      Serial.println(" %");
      Serial.println("----------");
    }
  }
}
```

**Hasil:** Data yang ditampilkan di Serial Monitor bersifat dinamis — berubah mengikuti kondisi lingkungan nyata. Ketika tangan didekatkan ke sensor, nilai suhu dan kelembaban meningkat secara real-time, membuktikan bahwa pipeline queue produsen-konsumen bekerja dengan data sensor yang sesungguhnya.

---

## Pertanyaan Umum (5.7)

#### 1. Jelaskan perbedaan antara `loop()` pada Arduino biasa dengan sistem yang menggunakan RTOS!

| Aspek | Arduino Biasa (`loop()`) | Arduino dengan RTOS |
|-------|--------------------------|---------------------|
| **Model eksekusi** | Single-threaded, sekuensial | Multi-task, concurrent |
| **Timing** | Bergantung pada durasi kode di dalam loop | Dikontrol oleh scheduler (deterministik) |
| **Blocking** | `delay()` memblokir seluruh program | `vTaskDelay()` hanya memblokir task tersebut |
| **Respons real-time** | Sulit tanpa interrupt | Mudah dengan prioritas task |
| **Kompleksitas** | Sederhana, cocok untuk tugas tunggal | Lebih kompleks, cocok untuk sistem multifungsi |

Pada Arduino biasa, `loop()` adalah inti program dan semua logika dijalankan di sana secara berurutan. Pada RTOS, `loop()` menjadi tidak relevan karena scheduler mengambil alih setelah `setup()` selesai.

---

#### 2. Mengapa fungsi `loop()` dibiarkan kosong?

Karena setelah `vTaskStartScheduler()` dipanggil (atau secara otomatis pada Arduino FreeRTOS), **RTOS scheduler mengambil alih kendali CPU** sepenuhnya. Scheduler tidak pernah mengembalikan kontrol ke `loop()`.

Semua logika program sudah didelegasikan ke task-task yang dibuat dengan `xTaskCreate()`. Menempatkan kode di `loop()` akan sia-sia karena `loop()` praktis tidak pernah dieksekusi, atau jika dieksekusi, akan mengganggu manajemen stack RTOS.

---

#### 3. Apa insight utama yang dipelajari dari praktikum ini?

Insight utama dari praktikum ini adalah pemahaman bahwa **RTOS bukan sekadar library, melainkan paradigma pemrograman yang berbeda fundamental**.

Pada Arduino biasa, programmer harus "memikirkan waktu" secara manual — mengatur urutan pemanggilan fungsi, menghitung kapan setiap aksi harus dilakukan, dan menghindari `delay()` agar sistem tetap responsif. Ini cepat menjadi kompleks dan tidak terstruktur seiring bertambahnya fitur.

Dengan RTOS, programmer dapat **mendefinisikan setiap perilaku sebagai task mandiri** dengan tanggung jawabnya masing-masing, lalu membiarkan scheduler yang mengelola waktu. Hasilnya adalah kode yang lebih modular, mudah di-debug, dan lebih mudah dikembangkan. Penggunaan queue juga mengajarkan bahwa komunikasi antar-task harus dilakukan melalui mekanisme yang thread-safe, bukan melalui variabel global sembarangan, untuk mencegah bug yang sulit dilacak seperti race condition.
