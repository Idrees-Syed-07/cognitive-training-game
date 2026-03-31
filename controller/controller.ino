#include <Wire.h>
#include "MPU9250.h"
#include "queue.h"
#include <HijelHID_BLEKeyboard.h>

#define THRESHOLD 1.01f
#define COOLDOWN 200
#define SAMPLE_WINDOW 10

MPU9250 accelerometer;
Queue* data;
HijelHID_BLEKeyboard bleKeyboard("Motion Keyboard", "Hijel", 100);
SemaphoreHandle_t dataMutex;
TickType_t lastTriggerTime = 0;

void readAccelerometer(void* parameter) {
    while(true) {
        if (accelerometer.update()) {
            xSemaphoreTake(dataMutex, portMAX_DELAY);
            enqueue(data, accelerometer.getLinearAccX(), accelerometer.getLinearAccY(), accelerometer.getLinearAccZ());
            xSemaphoreGive(dataMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void processData(void* parameter) {
    while(true) {
        xSemaphoreTake(dataMutex, portMAX_DELAY);
        bool hasEnoughData = data->count >= SAMPLE_WINDOW;
        xSemaphoreGive(dataMutex);

        if (hasEnoughData && bleKeyboard.isConnected()) {
            float average[3] = {0, 0, 0};

            for (int i = 0; i < SAMPLE_WINDOW; i++) {
                float out[3];

                xSemaphoreTake(dataMutex, portMAX_DELAY);
                bool ok = dequeue(data, out);
                xSemaphoreGive(dataMutex);

                if (ok) {
                    average[X] += out[X];
                    average[Y] += out[Y];
                    average[Z] += out[Z];
                }
            }

            for (int i = 0; i < 3; i++) {
                average[i] /= SAMPLE_WINDOW;
                Serial.printf("%f ", average[i]);
            }
            Serial.println();

            TickType_t time = xTaskGetTickCount();

            if ((time - lastTriggerTime) > pdMS_TO_TICKS(COOLDOWN)) {
                if (abs(average[X]) > abs(average[Y])) {
                    if (average[X] > THRESHOLD) {
                        bleKeyboard.tap(KEY_D);
                        lastTriggerTime = time;
                    }
                    else if (average[X] < -THRESHOLD) {
                        bleKeyboard.tap(KEY_A);
                        lastTriggerTime = time;
                    }
                } else {
                    if (average[Y] > THRESHOLD) {
                        bleKeyboard.tap(KEY_W);
                        lastTriggerTime = time;
                    }
                    else if (average[Y] < -THRESHOLD) {
                        bleKeyboard.tap(KEY_S);
                        lastTriggerTime = time;
                    }
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    bleKeyboard.begin();
    delay(1000);

    dataMutex = xSemaphoreCreateMutex();
    if (dataMutex == NULL) {
        Serial.println("Mutex creation failed");
        return;
    }

    data = createQueue();
    if (data == NULL) {
        Serial.println("Queue creation failed");
        return;
    }

    if (!accelerometer.setup(0x68)) {
        Serial.println("MPU9250 setup failed");
        return;
    }
    delay(1000);

    xTaskCreate(readAccelerometer, "Read Accelerometer", 4096, NULL, 1, NULL);
    xTaskCreate(processData, "Process Data", 4096, NULL, 2, NULL);
}

void loop() {

}