/*
 * bmp180.h
 *
 *  Created on: 07.07.2026
 *      Author: Dana Ahmadifar
 */

#ifndef INC_BMP180_H_
#define INC_BMP180_H_

#include "main.h" // Wichtig, damit der STM32 HAL (z.B. I2C_HandleTypeDef) bekannt ist

// Funktions-Prototypen, die wir in der main.c aufrufen können
void BMP180_Init(I2C_HandleTypeDef *hi2c);
float BMP180_GetTemperature(void);
long BMP180_GetPressure(void);
float BMP180_GetAltitude(long current_pressure);

#endif /* INC_BMP180_H_ */
