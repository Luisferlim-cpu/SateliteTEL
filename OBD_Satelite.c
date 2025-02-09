#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "auxiliar/nokia5110.h"
#include "auxiliar/MPU6050.h"
#include "auxiliar/bmp180.h"
#include "auxiliar/DHT11.h"
#include <stdio.h>
#include <string.h>

/*Embarcatech 2024 - Luis Fernando Lima da Silva - obd para satelite*/

// Configuração de GPIO
#define DHT11_PIN 15 

// Configuração do I2C
#define I2C_PORT i2c1
#define I2C_SDA 4
#define I2C_SCL 5

// Função de delay (em microsegundos)
void microDelay(uint32_t delayUs) {
    busy_wait_us(delayUs);
}

void setup();

// Função de leitura do DHT11
uint8_t DHT11_Start(void) {
    uint8_t resposta = 0;
    gpio_set_dir(DHT11_PIN, GPIO_OUT);  // DHT11 como saída
    gpio_put(DHT11_PIN, 0);  // Pulsa o pino para 0
    microDelay(20000);  // Espera por 20ms
    gpio_put(DHT11_PIN, 1);  // Seta o pino para 1
    microDelay(30);  // Espera por 30us
    gpio_set_dir(DHT11_PIN, GPIO_IN);  // DHT11 como entrada
    microDelay(40);

    if (!(gpio_get(DHT11_PIN))) {
        microDelay(80);
        if ((gpio_get(DHT11_PIN))) resposta = 1;
    }
    return resposta;
}

// Função para ler os dados do DHT11
uint8_t DHT11_Read(void) {
    uint8_t byte = 0;
    for (uint8_t a = 0; a < 8; a++) {
        microDelay(40); // Atraso de 40us
        if (!(gpio_get(DHT11_PIN))) {
            byte &= ~(1 << (7 - a));
        } else {
            byte |= (1 << (7 - a));
        }
    }
    return byte;
}

// Função principal
int main() {
    setup();

    while (true) {
        // Leitura do MPU6050
        MPU6050_Read_Accel(I2C_PORT);
        MPU6050_Read_Gyro(I2C_PORT);

        // Leitura do DHT11
        if (DHT11_Start()) {
            uint8_t umidadeInteira = DHT11_Read(); // Umidade relativa (parte inteira)
            uint8_t umidadeDecimal = DHT11_Read(); // Umidade relativa (parte decimal)
            uint8_t tempInteira = DHT11_Read(); // Temperatura (Celsius - parte inteira)
            uint8_t tempDecimal = DHT11_Read(); // Temperatura (Celsius - parte decimal)
            uint8_t somaVerificacao = DHT11_Read(); // Soma de verificação

            if (umidadeInteira + umidadeDecimal + tempInteira + tempDecimal == somaVerificacao) {
                float tempCelsius = (float)tempInteira + (float)(tempDecimal / 10.0);
                float tempFahrenheit = tempCelsius * 9 / 5 + 32;
                float umidadeRelativa = (float)umidadeInteira + (float)(umidadeDecimal / 10.0);

                // Exibe no LCD
                LCD_clrScr();
                LCD_print("Temp:", 0, 0);
                char buffer[16];  // Buffer para armazenar o número convertido
                sprintf(buffer, "%.2f", tempCelsius);  // Converte float para string com 2 casas decimais
                LCD_print(buffer, 1, 0);
                LCD_print("Humidity:", 0, 1);
                char buffer2[16];  // Buffer para armazenar o número convertido
                sprintf(buffer2, "%.2f", umidadeRelativa);  // Converte float para string com 2 casas decimais
                LCD_print(buffer2, 1, 1);
            }
        }

        // Leitura do BMP180
        float pressao = BMP180_GetPress(I2C_PORT, 3);
        float altitude = BMP180_GetAlt(I2C_PORT, 3);

        // Exibe os dados no LCD
        LCD_print("Pressure:", 0, 2);
        char pressaoStr[10];
        sprintf(pressaoStr, "%.2f Pa", pressao);
        LCD_print(pressaoStr, 1, 2);
        LCD_print("Altitude:", 0, 3);
        char altitudeStr[10];
        sprintf(altitudeStr, "%.2f m", altitude);
        LCD_print(altitudeStr, 1, 3);

        // Leitura do ADC (exemplo de medição de corrente)
        uint16_t valorRaw = adc_read();
        float tensaoRaw = (float)valorRaw * 3.3f / (1 << 12);  // ADC de 12 bits
        float corrente = (tensaoRaw - 2.5f) / 0.1f;

        // Exibe corrente no LCD
        char correnteStr[6];
        sprintf(correnteStr, "%.2f mA", corrente);
        LCD_print("Current:", 0, 4);
        LCD_print(correnteStr, 1, 4);

        // Exibe a tensão no LCD
        // sprintf(tensaoStr, "%.2f V", tensaoRaw);
        // LCD_print("Voltage:", 0, 5);
        // LCD_print(tensaoStr, 1, 5);

        sleep_ms(2000); // Atraso de 2 segundos
    }

    return 0;
}

// Função de inicialização dos periféricos
void setup() {
    // Inicializa o sistema de GPIO
    gpio_init(DHT11_PIN);
    gpio_set_dir(DHT11_PIN, GPIO_IN); // DHT11 como entrada

    // Inicializa I2C
    i2c_init(I2C_PORT, 100000); // 100kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o ADC
    adc_init();
    adc_gpio_init(26); // Configuração do pino de leitura analógica
    adc_select_input(0); // Canal 0 do ADC

    // Inicializa o LCD e sensores
    LCD_init();
    MPU6050_init(I2C_PORT);
    BMP180_Start(I2C_PORT);
}