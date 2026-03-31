#include "MPU9250.h"
#include "queue.h"
#include <BleKeyboard.h>

#define MOVEMENT_THRESHOLD 1.5
#define SAMPLE_WINDOW 10

MPU9250 accelerometer;
Queue* data;
BleKeyboard bleKeyboard;

void setup() {
    data = createQueue();
    if (data == NULL) return;

    Serial.begin(115200);
    Wire.begin();
    delay(2000);

    bleKeyboard.begin();

    accelerometer.setup(0x68);
    delay(5000);
    accelerometer.calibrateAccelGyro();
}

void loop() {
    if (accelerometer.update()) {
        enqueue(data, accelerometer.getLinearAccX(), accelerometer.getLinearAccY(), accelerometer.getLinearAccZ());
    }

    if (data->count >= SAMPLE_WINDOW && bleKeyboard.isConnected()) {
        float sumX = 0, sumY = 0;
        float out[3];
        int samples = 0;

        while (dequeue(data, out)) {
            sumX += out[X];
            sumY += out[Y];
            samples++;
        }

        float avgX = sumX / samples;
        float avgY = sumY / samples;

        if      (avgX >  MOVEMENT_THRESHOLD) bleKeyboard.write('d');
        else if (avgX < -MOVEMENT_THRESHOLD) bleKeyboard.write('a');
        else if (avgY >  MOVEMENT_THRESHOLD) bleKeyboard.write('w');
        else if (avgY < -MOVEMENT_THRESHOLD) bleKeyboard.write('s');
    }
}