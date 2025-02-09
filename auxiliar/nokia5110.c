#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "nokia5110.h"

struct LCD_att lcd;
struct LCD_GPIO lcd_gpio;

/*----- Funções GPIO -----*/
/*
 * @brief Configura os pinos GPIO usados para o LCD
 * @param PIN: pino do GPIO usado
 */
void LCD_setRST(uint gpio){
    lcd_gpio.RSTPIN = gpio;
    gpio_init(lcd_gpio.RSTPIN);
    gpio_set_dir(lcd_gpio.RSTPIN, GPIO_OUT);
}

void LCD_setCE(uint gpio){
    lcd_gpio.CEPIN = gpio;
    gpio_init(lcd_gpio.CEPIN);
    gpio_set_dir(lcd_gpio.CEPIN, GPIO_OUT);
}

void LCD_setDC(uint gpio){
    lcd_gpio.DCPIN = gpio;
    gpio_init(lcd_gpio.DCPIN);
    gpio_set_dir(lcd_gpio.DCPIN, GPIO_OUT);
}

void LCD_setDIN(uint gpio){
    lcd_gpio.DINPIN = gpio;
    gpio_init(lcd_gpio.DINPIN);
    gpio_set_dir(lcd_gpio.DINPIN, GPIO_OUT);
}

void LCD_setCLK(uint gpio){
    lcd_gpio.CLKPIN = gpio;
    gpio_init(lcd_gpio.CLKPIN);
    gpio_set_dir(lcd_gpio.CLKPIN, GPIO_OUT);
}

/*----- Funções da biblioteca -----*/

/*
 * @brief Envia dados para o LCD usando os pinos GPIO configurados
 * @param val: valor a ser enviado
 */
void LCD_send(uint8_t val){
    uint8_t i;
    for(i = 0; i < 8; i++){
        gpio_put(lcd_gpio.DINPIN, !!(val & (1 << (7 - i))));
        gpio_put(lcd_gpio.CLKPIN, true);
        gpio_put(lcd_gpio.CLKPIN, false);
    }
}

/*
 * @brief Escreve dados no LCD
 * @param data: dados a serem escritos
 * @param mode: comando ou dado
 */
void LCD_write(uint8_t data, uint8_t mode){
    if(mode == LCD_COMMAND){
        gpio_put(lcd_gpio.DCPIN, false);
        gpio_put(lcd_gpio.CEPIN, false);
        LCD_send(data);
        gpio_put(lcd_gpio.CEPIN, true);
    }
    else{
        gpio_put(lcd_gpio.DCPIN, true);
        gpio_put(lcd_gpio.CEPIN, false);
        LCD_send(data);
        gpio_put(lcd_gpio.CEPIN, true);
    }
}

/*
 * @brief Inicializa o LCD com valores predeterminados
 */
void LCD_init(){
    gpio_put(lcd_gpio.RSTPIN, false);
    gpio_put(lcd_gpio.RSTPIN, true);
    LCD_write(0x21, LCD_COMMAND); // Comandos estendidos do LCD
    LCD_write(0xB8, LCD_COMMAND); // Ajusta o contraste do LCD
    LCD_write(0x04, LCD_COMMAND); // Configura o coeficiente de temperatura
    LCD_write(0x14, LCD_COMMAND); // Modo de viés 1:40
    LCD_write(0x20, LCD_COMMAND); // Comandos básicos do LCD
    LCD_write(LCD_DISPLAY_NORMAL, LCD_COMMAND); // LCD normal
    LCD_clrScr();
    lcd.inverttext = false;
}

/*
 * @brief Inverte a cor exibida no display
 * @param mode: true = invertido / false = normal
 */
void LCD_invert(bool mode){
    if(mode == true){
        LCD_write(LCD_DISPLAY_INVERTED, LCD_COMMAND);
    }
    else{
        LCD_write(LCD_DISPLAY_NORMAL, LCD_COMMAND);
    }
}

/*
 * @brief Inverte a cor de qualquer texto enviado para o display
 * @param mode: true = invertido / false = normal
 */
void LCD_invertText(bool mode){
    if(mode == true){
        lcd.inverttext = true;
    }
    else{
        lcd.inverttext = false;
    }
}

/*
 * @brief Coloca um caractere na posição atual do cursor do LCD
 * @param c: caractere a ser impresso
 */
void LCD_putChar(char c){
    for(int i = 0; i < 6; i++){
        if(lcd.inverttext != true)
            LCD_write(ASCII[c - 0x20][i], LCD_DATA);
        else
            LCD_write(~(ASCII[c - 0x20][i]), LCD_DATA);
    }
}

/*
 * @brief Imprime uma string no LCD
 * @param str: string a ser impressa
 * @param x: ponto inicial no eixo x (coluna)
 * @param y: ponto inicial no eixo y (linha)
 */
void LCD_print(char *str, uint8_t x, uint8_t y){
    LCD_goXY(x, y);
    while(*str){
        LCD_putChar(*str++);
    }
}

/*
 * @brief Limpa a tela
 */
void LCD_clrScr(){
    for(int i = 0; i < 504; i++){
        LCD_write(0x00, LCD_DATA);
        lcd.buffer[i] = 0;
    }
}

/*
 * @brief Define o cursor do LCD para a posição X,Y
 * @param x: posição no eixo x (coluna)
 * @param y: posição no eixo y (linha)
 */
void LCD_goXY(uint8_t x, uint8_t y){
    LCD_write(0x80 | x, LCD_COMMAND); // Coluna
    LCD_write(0x40 | y, LCD_COMMAND); // Linha
}

/*
 * @brief Atualiza toda a tela conforme o conteúdo do lcd.buffer
 */
void LCD_refreshScr(){
    LCD_goXY(LCD_SETXADDR, LCD_SETYADDR);
    for(int i = 0; i < 6; i++){
        for(int j = 0; j < LCD_WIDTH; j++){
            LCD_write(lcd.buffer[(i * LCD_WIDTH) + j], LCD_DATA);
        }
    }
}

/*
 * @brief Atualiza uma área específica da tela
 * @param xmin: ponto inicial no eixo x
 * @param xmax: ponto final no eixo x
 * @param ymin: ponto inicial no eixo y
 * @param ymax: ponto final no eixo y
 */
void LCD_refreshArea(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax){
    for(int i = 0; i < 6; i++){
        if(i * 8 > ymax){
            break;
        }
        LCD_write(LCD_SETYADDR | i, LCD_COMMAND);
        LCD_write(LCD_SETXADDR | xmin, LCD_COMMAND);
        for(int j = xmin; j <= xmax; j++){
            LCD_write(lcd.buffer[(i * LCD_WIDTH) + j], LCD_DATA);
        }
    }
}

/*
 * @brief Define um pixel na tela
 * @param x: posição no eixo x
 * @param y: posição no eixo y
 * @param pixel: true para acender o pixel, false para apagar
 */
void LCD_setPixel(uint8_t x, uint8_t y, bool pixel){
    if(x >= LCD_WIDTH)
        x = LCD_WIDTH - 1;
    if(y >= LCD_HEIGHT)
        y = LCD_HEIGHT - 1;
    
    if(pixel != false){
        lcd.buffer[x + (y / 8) * LCD_WIDTH] |= 1 << (y % 8);
    }
    else{
        lcd.buffer[x + (y / 8) * LCD_WIDTH] &= ~(1 << (y % 8));
    }
}

/*
 * @brief Desenha uma linha horizontal
 * @param x: ponto inicial no eixo x
 * @param y: ponto inicial no eixo y
 * @param l: comprimento da linha
 */
void LCD_drawHLine(int x, int y, int l){
    int by, bi;
    if ((x>=0) && (x<LCD_WIDTH) && (y>=0) && (y<LCD_HEIGHT)){
        for (int cx=0; cx<l; cx++){
            by=((y/8)*84)+x;
            bi=y % 8;
            lcd.buffer[by+cx] |= (1<<bi);
        }
    }
}

/*
 * @brief Desenha uma linha vertical
 * @param x: ponto inicial no eixo x
 * @param y: ponto inicial no eixo y
 * @param l: comprimento da linha
 */
void LCD_drawVLine(int x, int y, int l){
    if ((x>=0) && (x<84) && (y>=0) && (y<48)){
        for (int cy=0; cy<= l; cy++){
            LCD_setPixel(x, y+cy, true);
        }
    }
}