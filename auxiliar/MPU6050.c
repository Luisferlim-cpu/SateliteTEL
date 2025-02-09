#include "MPU6050.h"
#include "nokia5110.h"
#include "hardware/i2c.h"
#include <stdio.h>

#define MPU6050_ADDR 0x68  // Endereço I2C do MPU6050
#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75

// Variáveis globais para armazenar os dados brutos
int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;

// Função de inicialização do MPU6050
void MPU6050_init(i2c_inst_t *i2c) {
    uint8_t check;
    uint8_t data;

    // Ler o WHO_AM_I para verificar a comunicação
    i2c_read_blocking(i2c, MPU6050_ADDR, &check, 1, false);
    
    if (check == 0x68) {  // Verifica se o ID do MPU6050 é 0x68
        // Acordar o sensor (desligar o modo de sono)
        data = 0;
        i2c_write_blocking(i2c, MPU6050_ADDR, &data, 1, false);

        // Configurar a taxa de amostragem para 1KHz
        data = 0x07;
        i2c_write_blocking(i2c, MPU6050_ADDR, &data, 1, false);

        // Configuração do acelerômetro (sem ganho)
        data = 0x00;
        i2c_write_blocking(i2c, MPU6050_ADDR, &data, 1, false);

        // Configuração do giroscópio (sem ganho)
        data = 0x00;
        i2c_write_blocking(i2c, MPU6050_ADDR, &data, 1, false);
    }
}

// Função para ler os dados do acelerômetro
void MPU6050_Read_Accel(i2c_inst_t *i2c) {
    uint8_t Rec_Data[6];

    // Ler os dados do acelerômetro (6 bytes)
    i2c_read_blocking(i2c, MPU6050_ADDR, Rec_Data, 6, false);

    // Converter os dados para inteiros de 16 bits
    Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
    Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);

    // Exibir os dados no LCD
    char buf[8];
    LCD_print("Acelerometro:", 1, 0);

    // Converte os valores brutos em unidades de g
    float X = Accel_X_RAW * 100 / 16384.0;
    float Y = Accel_Y_RAW * 100 / 16384.0;
    float Z = Accel_Z_RAW * 100 / 16384.0;
    
    sprintf(buf, "%.2f", X);
    LCD_print("Ax: ", 1, 1);
    LCD_print(buf, 20, 1);

    sprintf(buf, "%.2f", Y);
    LCD_print("Ay: ", 1, 2);
    LCD_print(buf, 20, 2);

    sprintf(buf, "%.2f", Z);
    LCD_print("Az: ", 1, 3);
    LCD_print(buf, 20, 3);
}

// Função para ler os dados do giroscópio
void MPU6050_Read_Gyro(i2c_inst_t *i2c) {
    uint8_t Rec_Data[6];

    // Ler os dados do giroscópio (6 bytes)
    i2c_read_blocking(i2c, MPU6050_ADDR, Rec_Data, 6, false);

    // Converter os dados para inteiros de 16 bits
    Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
    Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);

    // Exibir os dados no LCD
    char buf[6];
    LCD_print("Giroscopio:", 1, 0);


    // Converter os valores brutos em graus por segundo
    sprintf(buf, "%.2f", Gyro_X_RAW / 131.0);
    LCD_print("Gx: ", 1, 1);
    LCD_print(buf, 20, 1);

    sprintf(buf, "%.2f", Gyro_Y_RAW / 131.0);
    LCD_print("Gy: ", 1, 2);
    LCD_print(buf, 20, 2);

    sprintf(buf, "%.2f", Gyro_Z_RAW / 131.0);
    LCD_print("Gz: ", 1, 3);
    LCD_print(buf, 20, 3);
}
