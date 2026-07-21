/*
 * matrixLED.c
 *
 *  Created on: 08.07.2026
 *      Author: Dana Ahmadifar
 */
#include "matrixLED.h"

static SPI_HandleTypeDef *max_spi;

// Zahlen-Array
static const uint8_t nummern[10][8] = {
    // 0
    {0x00, 0x7E, 0xFF, 0x99, 0x8D, 0xFF, 0x7E, 0x00},
    // 1
    {0x00, 0x00, 0x82, 0xFF, 0xFF, 0x80, 0x00, 0x00},
    // 2
    {0x00, 0xC2, 0xE3, 0xB1, 0x99, 0x8F, 0x86, 0x00},
    // 3
    {0x00, 0x42, 0xC3, 0x89, 0x89, 0xFF, 0x76, 0x00},
    // 4
    {0x30, 0x38, 0x2C, 0x26, 0xFF, 0xFF, 0x20, 0x00},
    // 5
    {0x00, 0x47, 0xC7, 0x85, 0x85, 0xFD, 0x40, 0x00},
    // 6
    {0x00, 0x7E, 0xFF, 0x89, 0x89, 0xFB, 0x72, 0x00},
    // 7
    {0x00, 0x01, 0xF1, 0xF9, 0x0D, 0x07, 0x03, 0x00},
    // 8
    {0x00, 0x76, 0xFF, 0x89, 0x89, 0xFF, 0x76, 0x00},
    // 9
    {0x00, 0x46, 0xCF, 0x89, 0x89, 0xFF, 0x7E, 0x00}
};

// Symbole Array
static const uint8_t symbole[4][8] = {
    // °C
    {0x06, 0x09, 0x09, 0x66, 0x90, 0x90, 0x90, 0x00},
    // °F
    {0x06, 0x09, 0x09, 0xF6, 0x50, 0x50, 0x10, 0x00},
    // %
    {0x03, 0x23, 0x10, 0x08, 0x04, 0xC2, 0xC1, 0x00},
    // b
    {0xFF, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00}
};

static void MAX7219_Write(uint8_t addr1, uint8_t data1) {
    uint8_t buffer[2] = {addr1, data1};

    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(max_spi, buffer, 2, 10);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

static void MAX7219_Write_Dual(uint8_t addr1, uint8_t data1, uint8_t addr2, uint8_t data2) {
    uint8_t buffer[4] = {addr2, data2, addr1, data1};

    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(max_spi, buffer, 4, 10);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

// Sendet Befehle an alle 3 Matrizen
// Reihenfolge MCU -> Matrix 1 -> Matrix 2 -> Matrix 3
static void MAX7219_Write_Triple(uint8_t addr1, uint8_t data1, uint8_t addr2, uint8_t data2, uint8_t addr3, uint8_t data3) {
    uint8_t buffer[6] = {addr3, data3, addr2, data2, addr1, data1};

    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(max_spi, buffer, 6, 10);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

void MAX7219_Init(SPI_HandleTypeDef *hspi) {
    max_spi = hspi; // SPI Bus

    uint8_t config[5][2] = {
        {0x09, 0x00}, // Decode mode: Aus
        {0x0A, 0x02}, // Helligkeit
        {0x0B, 0x07}, // Scan limit: 8 Zeilen
        {0x0C, 0x01}, // Shutdown: Normalbetrieb
        {0x0F, 0x00}  // Display test: Aus
    };

    // Konfiguration an alle 3 Displays senden
    for(uint8_t i = 0; i < 5; i++) {
        MAX7219_Write_Triple(config[i][0], config[i][1], config[i][0], config[i][1], config[i][0], config[i][1]);
    }

    // Alle 3 Displays leeren
    for(uint8_t i = 1; i <= 8; i++) {
        MAX7219_Write_Triple(i, 0x00, i, 0x00, i, 0x00);
    }
}

// Kompatibilitätsfunktion für 2 Matrizen
void Display_Zahl_99(uint8_t info_zahl) {
    uint8_t zehner = info_zahl / 10;
    uint8_t einer  = info_zahl % 10;

    for (uint8_t zeile = 0; zeile < 8; zeile++) {
        MAX7219_Write_Dual(zeile + 1, nummern[zehner][zeile],
        				   zeile + 1, nummern[einer][zeile]);
    }
}

void Display_Zahl_999(uint8_t info_zahl) {
	uint8_t hundert = info_zahl / 100;
    uint8_t zehner = (info_zahl / 10) % 10;
    uint8_t einer  = info_zahl % 10;

    for (uint8_t zeile = 0; zeile < 8; zeile++) {
        MAX7219_Write_Triple(zeile + 1, nummern[hundert][zeile],
        					 zeile + 1, nummern[zehner][zeile],
							 zeile + 1, nummern[einer][zeile]);
    }
}

void Display_Wert_Mit_Symbol(uint8_t wert, uint8_t symbol_index) {
    uint8_t zehner = wert / 10;
    uint8_t einer  = wert % 10;

    if (symbol_index > 3) symbol_index = 0;

    for (uint8_t zeile = 0; zeile < 8; zeile++) {
        MAX7219_Write_Triple(
            zeile + 1, nummern[zehner][zeile],      // Matrix 1 Zehner
            zeile + 1, nummern[einer][zeile],       // Matrix 2 Einer
            zeile + 1, symbole[symbol_index][zeile] // Matrix 3 Symbol
        );
    }
}
