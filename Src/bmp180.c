/*
 * bmp180.c
 *
 *  Created on: 06.07.2026
 *      Author: Dana Ahmadifar
 */


#include "bmp180.h"
#include <math.h>

#define BMP180_ADDR (0x77 << 1)

static I2C_HandleTypeDef *bmp_i2c;

// Kalibrierung
static struct {
    short AC1, AC2, AC3, B1, B2, MB, MC, MD;
    unsigned short AC4, AC5, AC6;
    long B5;
} bmp180_cal;

void BMP180_Init(I2C_HandleTypeDef *hi2c) {
    bmp_i2c = hi2c;
    uint8_t calib_data[22];

    // 22 Bytes
    HAL_I2C_Mem_Read(bmp_i2c, BMP180_ADDR, 0xAA, 1, calib_data, 22, 100);

    // Bytes zu 16-Bit Werten zusammensetzen (Bitshift)
    bmp180_cal.AC1 = (calib_data[0] << 8) | calib_data[1];
    bmp180_cal.AC2 = (calib_data[2] << 8) | calib_data[3];
    bmp180_cal.AC3 = (calib_data[4] << 8) | calib_data[5];
    bmp180_cal.AC4 = (calib_data[6] << 8) | calib_data[7];
    bmp180_cal.AC5 = (calib_data[8] << 8) | calib_data[9];
    bmp180_cal.AC6 = (calib_data[10] << 8) | calib_data[11];
    bmp180_cal.B1  = (calib_data[12] << 8) | calib_data[13];
    bmp180_cal.B2  = (calib_data[14] << 8) | calib_data[15];
    bmp180_cal.MB  = (calib_data[16] << 8) | calib_data[17];
    bmp180_cal.MC  = (calib_data[18] << 8) | calib_data[19];
    bmp180_cal.MD  = (calib_data[20] << 8) | calib_data[21];
}

float BMP180_GetTemperature(void) {
    uint8_t cmd = 0x2E; // Kommando Temperaturmessung
    uint8_t raw_data[2];
    long UT, X1, X2;

    HAL_I2C_Mem_Write(bmp_i2c, BMP180_ADDR, 0xF4, 1, &cmd, 1, 100);
    HAL_Delay(5); // Sensor braucht min. 4.5ms

    // Rohe Temperaturwert
    HAL_I2C_Mem_Read(bmp_i2c, BMP180_ADDR, 0xF6, 1, raw_data, 2, 100);
    UT = (raw_data[0] << 8) | raw_data[1];

    // Datasheet Formel fuer Temperaturkompensation
    X1 = ((UT - bmp180_cal.AC6) * bmp180_cal.AC5) >> 15;
    X2 = (bmp180_cal.MC << 11) / (X1 + bmp180_cal.MD);
    bmp180_cal.B5 = X1 + X2;

    // Temperatur berechnen
    float temp = ((bmp180_cal.B5 + 8) >> 4) / 10.0f;
    return temp;
}

long BMP180_GetPressure(void) {
    uint8_t cmd = 0x34; // Kommando Druck
    uint8_t raw_data[3];
    long UP, B6, X1, X2, X3, B3;
    unsigned long B4, B7;
    long p;

    // Kommando schreiben
    HAL_I2C_Mem_Write(bmp_i2c, BMP180_ADDR, 0xF4, 1, &cmd, 1, 100);
    HAL_Delay(5);

    // Rohen Druckwert lesen
    HAL_I2C_Mem_Read(bmp_i2c, BMP180_ADDR, 0xF6, 1, raw_data, 3, 100);
    UP = ((raw_data[0] << 16) | (raw_data[1] << 8) | raw_data[2]) >> (8 - 0);

    // Datasheet Formel fuer Druck
    B6 = bmp180_cal.B5 - 4000;
    X1 = (bmp180_cal.B2 * (B6 * B6 >> 12)) >> 11;
    X2 = (bmp180_cal.AC2 * B6) >> 11;
    X3 = X1 + X2;
    B3 = (((bmp180_cal.AC1 * 4 + X3) << 0) + 2) >> 2;

    X1 = (bmp180_cal.AC3 * B6) >> 13;
    X2 = (bmp180_cal.B1 * (B6 * B6 >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    B4 = (bmp180_cal.AC4 * (unsigned long)(X3 + 32768)) >> 15;

    B7 = ((unsigned long)UP - B3) * (50000 >> 0);
    if (B7 < 0x80000000) { p = (B7 << 1) / B4; }
    else { p = (B7 / B4) << 1; }

    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;
    p = p + ((X1 + X2 + 3791) >> 4);

    return p / 100; // Rueckgabe in hPa
}

float BMP180_GetAltitude(long current_pressure) {
    float sea_level_pressure = 1013.25f;
    // Formel fuer Altitude
    float altitude = 44330.0f * (1.0f - powf((float)current_pressure / sea_level_pressure, 0.1902949f));

    return altitude;
}
