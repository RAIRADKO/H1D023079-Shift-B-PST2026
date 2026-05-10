#include <Arduino_FreeRTOS.h>
#include <queue.h>

struct readings {
  int temp;
  int h;
};

QueueHandle_t my_queue;

void setup() {
  Serial.begin(9600);
  
  // Membuat queue untuk 1 item dengan ukuran struct readings
  my_queue = xQueueCreate(1, sizeof(struct readings));
  
  // Membuat task
  xTaskCreate(read_data, "read sensors", 128, NULL, 0, NULL);
  xTaskCreate(display, "display", 128, NULL, 0, NULL);
}

void loop() {
  // Biarkan kosong karena RTOS scheduler yang akan mengambil alih
}

void read_data(void *pvParameters) {
  struct readings x;
  for(;;) {
    x.temp = 54;
    x.h = 30;
    
    // Mengirim data ke queue
    xQueueSend(my_queue, &x, portMAX_DELAY);
    
    // Menggunakan portTICK_PERIOD_MS agar delay akurat dalam milidetik
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void display(void *pvParameters) {
  struct readings x;
  for(;;) {
    // Menunggu dan menerima data dari queue
    if(xQueueReceive(my_queue, &x, portMAX_DELAY) == pdPASS) {
      Serial.print("temp = ");
      Serial.println(x.temp);
      Serial.print("humidity = ");
      Serial.println(x.h);
    }
  }
}