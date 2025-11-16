#include "funciones.h"

void SensorMPU::begin() {
    Wire.begin();
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(PWR_MGMT_1);
    Wire.write(0); // Iniciar el MPU6050
    Wire.endTransmission();
}

int16_t SensorMPU::leerDatoBruto(uint8_t addr) {
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(addr);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, 2, true);
    int16_t alto = Wire.read();
    int16_t bajo = Wire.read();
    return (alto << 8) | bajo;
}

void SensorMPU::leerDatos(float &ax, float &ay, float &az, float &gx, float &gy, float &gz) {
    int16_t AcX = leerDatoBruto(ACCEL_XOUT_H);
    int16_t AcY = leerDatoBruto(ACCEL_XOUT_H + 2);
    int16_t AcZ = leerDatoBruto(ACCEL_XOUT_H + 4);
    int16_t GyX = leerDatoBruto(0x43);
    int16_t GyY = leerDatoBruto(0x45);
    int16_t GyZ = leerDatoBruto(0x47);

    ax = (float)AcX / 16384.0;
    ay = (float)AcY / 16384.0;
    az = (float)AcZ / 16384.0;
    gx = (float)GyX / 131.0;
    gy = (float)GyY / 131.0;
    gz = (float)GyZ / 131.0;
}
