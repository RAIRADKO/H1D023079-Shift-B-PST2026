## Percobaan 4A (modul4_adc.ino)

**1. Apa fungsi perintah analogRead() pada rangkaian praktikum ini?**

`analogRead(potensioPin)` membaca tegangan analog dari pin A0 yang terhubung ke potensiometer. Arduino mengonversi tegangan tersebut (0–5V) menjadi nilai digital 10-bit (0–1023) menggunakan ADC internal ATmega328P. Nilai ini kemudian disimpan ke variabel `val` untuk diproses lebih lanjut.

**2. Mengapa diperlukan fungsi map() dalam program tersebut?**

Karena ADC menghasilkan nilai `val` dalam rentang 0–1023, sedangkan fungsi `myservo.write()` hanya menerima nilai sudut 0–180 derajat. Tanpa `map()`, nilai ADC akan jauh melampaui batas servo. Fungsi `map(val, 0, 1023, 0, 180)` memetakan secara linear sehingga potensiometer minimum (ADC=0) → servo 0°, dan maksimum (ADC=1023) → servo 180°.

**3. Modifikasi agar servo hanya bergerak 30° hingga 150°:**

Cukup ubah satu baris pada bagian `map()`:
```cpp
pos = map(val,
           0,    // nilai minimum ADC
           1023, // nilai maksimum ADC
           30,   // sudut minimum servo (diubah dari 0)
           150   // sudut maksimum servo (diubah dari 180)
           );
```
Dengan ini, meskipun potensiometer tetap menghasilkan ADC 0–1023 secara penuh, output `pos` dibatasi antara 30–150. Saat ADC=0 maka pos=30, saat ADC=511 maka pos≈90, saat ADC=1023 maka pos=150.

---

## Percobaan 4B (modul4_pwm.ino)

**1. Jelaskan mengapa LED dapat diatur kecerahannya menggunakan analogWrite()!**

`analogWrite(ledPin, pwm)` menghasilkan sinyal PWM pada pin 9 dengan frekuensi ~490 Hz. LED menyala dan mati bergantian sangat cepat sehingga mata manusia tidak menangkap kedipan, melainkan melihat rata-rata kecerahan. Saat `pwm=0` duty cycle 0% → LED mati total. Saat `pwm=255` duty cycle 100% → LED menyala penuh. Nilai di antaranya menghasilkan kecerahan proporsional.

**2. Apa hubungan antara nilai ADC (0–1023) dan nilai PWM (0–255)?**

Keduanya dihubungkan oleh `map(nilaiADC, 0, 1023, 0, 255)` yang melakukan penskalaan linear: `pwm = (nilaiADC / 1023) × 255`. Perbedaan rentang terjadi karena ADC Arduino beresolusi 10-bit (2¹⁰ = 1024 nilai) sedangkan register Timer PWM beresolusi 8-bit (2⁸ = 256 nilai).

**3. Modifikasi agar LED hanya menyala pada PWM 50–200:**

Tambahkan kondisi if-else setelah baris `map()`, menggantikan baris `analogWrite()`:
```cpp
// Ganti baris analogWrite(ledPin, pwm) dengan:
if (pwm >= 50 && pwm <= 200) {
  analogWrite(ledPin, pwm);  // nyala sesuai kecerahan
} else {
  analogWrite(ledPin, 0);    // mati di luar rentang
}
```
Saat potensiometer menghasilkan `pwm` < 50 atau > 200, LED dimatikan paksa dengan `analogWrite(ledPin, 0)`. Hanya ketika `pwm` berada di rentang 50–200 LED menyala dengan kecerahan sesuai nilai tersebut.
