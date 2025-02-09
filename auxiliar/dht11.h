#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

// Função para ler temperatura e umidade do DHT11
uint8_t DHT11_Read(uint8_t DHT11_PIN);

//funcao de start do dht11
uint8_t DHT11_Start(uint8_t DHT11_PIN);

void microDelay(uint32_t delayUs);


#endif // DHT11_H
