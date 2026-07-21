/*
 * matrixLED.h
 *
 *  Created on: 08.07.2026
 *      Author: Dana Ahmadifar
 */

#ifndef INC_MATRIXLED_H_
#define INC_MATRIXLED_H_

#include "main.h"

void MAX7219_Init(SPI_HandleTypeDef *hspi);
void Display_Zahl_99(uint8_t info_zahl);
void Display_Zahl_999(uint8_t info_zahl);
void Display_Wert_Mit_Symbol(uint8_t wert, uint8_t symbol_index);

#endif /* INC_MATRIXLED_H_ */
