#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "bmp180.h"

// Endereço I2C do BMP180
#define BMP180_ADDRESS 0x77

// Variáveis de calibração
static int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;
static uint16_t AC4, AC5, AC6;
static int32_t B5;

// Pressão atmosférica padrão em Pascals
#define ATM_PRESSURE 101325

// Função auxiliar para escrever no sensor
static void bmp180_write(uint8_t reg, uint8_t data) {
    uint8_t buffer[2] = {reg, data};
    i2c_write_blocking(i2c_default, BMP180_ADDRESS, buffer, 2, false);
}

// Função auxiliar para ler do sensor
static void bmp180_read(uint8_t reg, uint8_t *buffer, uint8_t len) {
    i2c_write_blocking(i2c_default, BMP180_ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c_default, BMP180_ADDRESS, buffer, len, false);
}

// Lê os coeficientes de calibração do BMP180
void BMP180_ReadCalibration() {
    uint8_t buffer[22];
    bmp180_read(0xAA, buffer, 22);

    AC1 = (buffer[0] << 8) | buffer[1];
    AC2 = (buffer[2] << 8) | buffer[3];
    AC3 = (buffer[4] << 8) | buffer[5];
    AC4 = (buffer[6] << 8) | buffer[7];
    AC5 = (buffer[8] << 8) | buffer[9];
    AC6 = (buffer[10] << 8) | buffer[11];
    B1 = (buffer[12] << 8) | buffer[13];
    B2 = (buffer[14] << 8) | buffer[15];
    MB = (buffer[16] << 8) | buffer[17];
    MC = (buffer[18] << 8) | buffer[19];
    MD = (buffer[20] << 8) | buffer[21];
}

// Obtém a temperatura não compensada
static int32_t bmp180_get_utemp() {
    bmp180_write(0xF4, 0x2E);
    sleep_ms(5); // Espera 4,5 ms

    uint8_t buffer[2];
    bmp180_read(0xF6, buffer, 2);
    return (buffer[0] << 8) | buffer[1];
}

// Calcula a temperatura real
float BMP180_GetTemp(i2c_inst_t *i2c) {
    int32_t UT = bmp180_get_utemp();

    int32_t X1 = ((UT - AC6) * AC5) / 32768;
    int32_t X2 = (MC * 2048) / (X1 + MD);
    B5 = X1 + X2;
    int32_t T = (B5 + 8) / 16;

    return T / 10.0; // Retorna em graus Celsius
}

// Obtém a pressão não compensada
static int32_t bmp180_get_upress(int oss) {
    uint8_t command = 0x34 + (oss << 6);
    bmp180_write(0xF4, command);

    // Tempo de espera baseado no oversampling
    switch (oss) {
        case 0: sleep_ms(5); break;
        case 1: sleep_ms(8); break;
        case 2: sleep_ms(14); break;
        case 3: sleep_ms(26); break;
    }

    uint8_t buffer[3];
    bmp180_read(0xF6, buffer, 3);
    return ((buffer[0] << 16) | (buffer[1] << 8) | buffer[2]) >> (8 - oss);
}

// Calcula a pressão real
float BMP180_GetPress(i2c_inst_t *i2c, int oss) {
    int32_t UP = bmp180_get_upress(oss);

    int32_t B6 = B5 - 4000;
    int32_t X1 = (B2 * (B6 * B6 / 4096)) / 2048;
    int32_t X2 = (AC2 * B6) / 2048;
    int32_t X3 = X1 + X2;
    int32_t B3 = (((AC1 * 4 + X3) << oss) + 2) / 4;

    X1 = (AC3 * B6) / 8192;
    X2 = (B1 * (B6 * B6 / 4096)) / 65536;
    X3 = ((X1 + X2) + 2) / 4;
    uint32_t B4 = AC4 * (uint32_t)(X3 + 32768) / 32768;
    uint32_t B7 = ((uint32_t)UP - B3) * (50000 >> oss);

    int32_t P;
    if (B7 < 0x80000000) {
        P = (B7 * 2) / B4;
    } else {
        P = (B7 / B4) * 2;
    }

    X1 = (P / 256) * (P / 256);
    X1 = (X1 * 3038) / 65536;
    X2 = (-7357 * P) / 65536;
    P = P + (X1 + X2 + 3791) / 16;

    return P / 100.0; // Retorna pressão em hPa
}

// Calcula a altitude aproximada
float BMP180_GetAlt(i2c_inst_t *i2c, int oss) {
    float pressure = BMP180_GetPress(i2c, oss);
    return 44330.0 * (1.0 - pow((pressure / ATM_PRESSURE), 0.19029495718));
}

// Inicializa o sensor BMP180
void BMP180_Start(i2c_inst_t *i2c) {
    BMP180_ReadCalibration();
}
