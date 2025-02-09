#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "dht11.h"

// Tempo máximo de espera para uma mudança de estado no pino
#define TIMEOUT 10000 

// Leitura de dados do DHT11
static int dht11_read_raw(uint8_t pin, uint8_t *data) {
    uint32_t start_time;
    uint8_t last_state = 1;
    uint8_t counter = 0;
    uint8_t j = 0, i;

    // Preparação do pino: seta para saída e força nível baixo
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
    sleep_ms(18);  // Aguarda pelo menos 18ms para iniciar a comunicação

    gpio_put(pin, 1);
    sleep_us(40);

    // Configura o pino para entrada (esperando resposta do sensor)
    gpio_set_dir(pin, GPIO_IN);

    // Captura os 40 bits do DHT11
    for (i = 0; i < 85; i++) {  // Tempo suficiente para capturar 40 bits
        counter = 0;
        start_time = time_us_32();
        
        while (gpio_get(pin) == last_state) {
            counter++;
            if (counter == TIMEOUT) return -1; // Timeout
        }

        last_state = gpio_get(pin);

        // Pulso inicial de 80µs do sensor, ignoramos os primeiros 3 pulsos
        if ((i >= 4) && (i % 2 == 0)) {
            data[j / 8] <<= 1;
            if (counter > 30)  // Pulso longo representa "1"
                data[j / 8] |= 1;
            j++;
        }
    }

    return (j >= 40) ? 0 : -1; // Retorna erro se não capturou todos os bits
}

// Função principal para ler temperatura e umidade do DHT11
int dht11_read(uint8_t pin, dht11_data_t *result) {
    uint8_t data[5] = {0};

    if (dht11_read_raw(pin, data) != 0)
        return -1; // Falha na leitura

    // Verifica o checksum
    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
        return -1; // Checksum inválido

    // Preenche a estrutura com os valores lidos
    result->humidity = data[0];
    result->temperature = data[2];

    return 0; // Sucesso
}
