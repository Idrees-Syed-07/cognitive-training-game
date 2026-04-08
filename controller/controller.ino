#include <Wire.h>
#include "MPU9250.h"
#include "queue.h"
#include <HijelHID_BLEKeyboard.h>

#define THRESHOLD 0.9f
#define COOLDOWN 750
#define SAMPLE_WINDOW 10

MPU9250 accelerometer;
Queue* data;
HijelHID_BLEKeyboard bleKeyboard("Motion Keyboard", "Hijel", 100);
SemaphoreHandle_t dataMutex; //mutex stops both tasks from accessing queue at same time to stop race conditions
TickType_t lastTriggerTime = 0;

//reading accelerometer data and adding it to queue
void readAccelerometer(void* parameter) {
    while(true) {
        if (accelerometer.update()) {
            xSemaphoreTake(dataMutex, portMAX_DELAY); //taking mutex to stop processData from accessing queue
            enqueue(data, accelerometer.getLinearAccX(), accelerometer.getLinearAccY(), accelerometer.getLinearAccZ());
            xSemaphoreGive(dataMutex); //giving mutex back
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

//checking queue for any swipes and in which dirction
void processData(void* parameter) {
    while(true) {
        xSemaphoreTake(dataMutex, portMAX_DELAY); //taking mutex to stop readAccelerometer from accessing queue
        bool hasEnoughData = data->count >= SAMPLE_WINDOW;
        xSemaphoreGive(dataMutex); //giving mutex back

        if (hasEnoughData && bleKeyboard.isConnected()) {
            float average[3] = {0, 0, 0};

            //dequeueing last bit of data and adding to total sum
            for (int i = 0; i < SAMPLE_WINDOW; i++) {
                float out[3];

                xSemaphoreTake(dataMutex, portMAX_DELAY); //taking mutex
                bool ok = dequeue(data, out);
                xSemaphoreGive(dataMutex); //giving mutex

                if (ok) {
                    average[X] += out[X];
                    average[Y] += out[Y];
                    average[Z] += out[Z];
                }
            }

            for (int i = 0; i < 3; i++) {
                average[i] /= SAMPLE_WINDOW; //dividing sum byy #samples to get average
                Serial.printf("%f ", average[i]);
            }
            Serial.println();

            TickType_t time = xTaskGetTickCount(); //getting current time

            //checking if enough time has passed since last input 
            //stops opposite input after first due to deacceleration
            if ((time - lastTriggerTime) > pdMS_TO_TICKS(COOLDOWN)) {
                //checking for direction with largest acceleration, 
                //sending corresponding input if above the threshold
                //and updating last input time to current time if so
                if (fabs(average[Y]) > fabs(average[Z])) {
                    if (average[Y] > THRESHOLD) {
                        bleKeyboard.tap(KEY_A);
                        lastTriggerTime = time;
                    }
                    else if (average[Y] < -THRESHOLD) {
                        bleKeyboard.tap(KEY_D);
                        lastTriggerTime = time;
                    }
                } else {
                    if (average[Z] > THRESHOLD) {
                        bleKeyboard.tap(KEY_S);
                        lastTriggerTime = time;
                    }
                    else if (average[Z] < -THRESHOLD) {
                        bleKeyboard.tap(KEY_W);
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

    //creating mutex
    dataMutex = xSemaphoreCreateMutex();
    if (dataMutex == NULL) {
        Serial.println("Mutex creation failed");
        return;
    }

    //creating queue
    data = createQueue();
    if (data == NULL) {
        Serial.println("Queue creation failed");
        return;
    }

    //setting up accelerometer
    if (!accelerometer.setup(0x68)) {
        Serial.println("MPU9250 setup failed");
        return;
    }
    delay(1000);

    accelerometer.calibrateAccelGyro();
    delay(10000);

    //running the two tasks (readAccelerometer and processData) on separate threads
    //so they can run at the same time
    xTaskCreate(readAccelerometer, "Read Accelerometer", 4096, NULL, 1, NULL);
    xTaskCreate(processData, "Process Data", 4096, NULL, 2, NULL);
}

//unneeded since we are using tasks
void loop() {

}
