#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

// Estrutura para armazenar temperatura e umidade
typedef struct {
    uint8_t temperature;
    uint8_t humidity;
} dht11_data_t;

// Função para ler temperatura e umidade do DHT11
int dht11_read(uint8_t pin, dht11_data_t *result);

#endif // DHT11_H
