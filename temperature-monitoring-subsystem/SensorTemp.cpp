#include "SensorTemp.h"

TemperatureSensor::TemperatureSensor(int pin, int offset) {
    this->pin = pin;
    this->offset = offset;
}

float TemperatureSensor::readTemperature() {
    int sensorValue = analogRead(pin);
    float mV = (sensorValue / 4095.0) * 5 * 1000;
    float temperatureC = (mV - (500 + offset)) / 10;
    return temperatureC;
}