#include <Wire.h>
#include "MPU9250.h"
#include "queue.h"
#include <HijelHID_BLEKeyboard.h>

#define POS_THRESHOLD 2.0f
#define NEG_THRESHOLD 1.2f
#define RESET_THRESHOLD 0.6f
#define SAMPLE_WINDOW 4

MPU9250 accelerometer;
Queue* data = nullptr;
HijelHID_BLEKeyboard bleKeyboard("Motion Keyboard", "Hijel", 100);

bool gestureArmed = true;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    delay(2000);

    data = createQueue();
    if (data == nullptr) {
        Serial.println("Queue creation failed");
        return;
    }

    bleKeyboard.begin();

    if (!accelerometer.setup(0x68)) {
        Serial.println("MPU9250 setup failed");
        return;
    }

    delay(1000);

}

void loop() {
    if (data == nullptr) {
        return;
    }

    if (accelerometer.update()) {
        enqueue(
            data,
            accelerometer.getLinearAccX(),
            accelerometer.getLinearAccY(),
            accelerometer.getLinearAccZ()
        );
    }

    if (data->count >= SAMPLE_WINDOW && bleKeyboard.isConnected()) {
        float sumX = 0.0f;
        float sumY = 0.0f;
        float out[3];
        int samples = 0;

        while (dequeue(data, out)) {
            sumX += out[X];
            sumY += out[Y];
            samples++;
        }

        if (samples == 0) {
            return;
        }

        float avgX = sumX / samples;
        float avgY = sumY / samples;

        // re-arm only when motion settles back down
        if (!gestureArmed) {
            if (abs(avgX) < RESET_THRESHOLD && abs(avgY) < RESET_THRESHOLD) {
                gestureArmed = true;
            }
            return;
        }
        if ( abs(avgX)>abs(avgY)){
            if (avgX > POS_THRESHOLD) {
                bleKeyboard.tap(KEY_D);
                gestureArmed = false;
                Serial.println("D");
            } 
            else if (avgX < -NEG_THRESHOLD) {
                bleKeyboard.tap(KEY_A);
                gestureArmed = false;
                Serial.println("A");
            }
        }
        else
        {
            if (avgY > POS_THRESHOLD) {
                bleKeyboard.tap(KEY_W);
                gestureArmed = false;
                Serial.println("W");
            } 
            else if (avgY < -NEG_THRESHOLD) {
                bleKeyboard.tap(KEY_S);
                gestureArmed = false;
                Serial.println("S");
            }
        }
    }
}