#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <Wire.h>

#define MPU6050_ADDR 0x68
#define ACCEL_XOUT_H 0x3B
#define PWR_MGMT_1   0x6B

class SensorMPU {
public:
    void begin();
    void leerDatos(float &ax, float &ay, float &az, float &gx, float &gy, float &gz);

private:
    int16_t leerDatoBruto(uint8_t addr);
};

#endif
