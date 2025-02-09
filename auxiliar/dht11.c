#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "dht11.h"

// Tempo máximo de espera para uma mudança de estado no pino
#define TIMEOUT 10000 

// Função de delay (em microsegundos)
void microDelay(uint32_t delayUs) {
    busy_wait_us(delayUs);
}

uint8_t DHT11_Read(uint8_t DHT11_PIN) {
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

uint8_t DHT11_Start(uint8_t DHT11_PIN) {
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