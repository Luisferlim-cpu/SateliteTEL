#ifndef BMP180_H
#define BMP180_H

#include "hardware/i2c.h"

// Endereço I2C do BMP180
#define BMP180_ADDRESS 0x77   

// Inicializa o sensor e lê os coeficientes de calibração
void BMP180_Start(i2c_inst_t *i2c);

// Obtém a temperatura em graus Celsius (°C)
float BMP180_GetTemp(i2c_inst_t *i2c);

// Obtém a pressão em hectopascais (hPa)
float BMP180_GetPress(i2c_inst_t *i2c, int oss);

// Obtém a altitude estimada com base na pressão medida
float BMP180_GetAlt(i2c_inst_t *i2c, int oss);

#endif // BMP180_H