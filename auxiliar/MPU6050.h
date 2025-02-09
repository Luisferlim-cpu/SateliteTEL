#ifndef MPU6050_H
#define MPU6050_H

#include "hardware/i2c.h"

// Endereço do MPU6050 no barramento I2C
#define MPU6050_ADDR 0x68  // Endereço I2C do MPU6050

// Registradores do MPU6050
#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75

// Variáveis globais para armazenar os dados brutos
extern int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
extern int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;

// Função para inicializar o MPU6050
void MPU6050_init(i2c_inst_t *i2c);

// Função para ler os dados do acelerômetro
void MPU6050_Read_Accel(i2c_inst_t *i2c);

// Função para ler os dados do giroscópio
void MPU6050_Read_Gyro(i2c_inst_t *i2c);

#endif // MPU_H