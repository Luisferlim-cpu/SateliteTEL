#ifndef NOKIA5110_LCD_H
#define NOKIA5110_LCD_H

#include <stdint.h>
#include <stdbool.h>
#include "font.h"

#define LCD_WIDTH      84
#define LCD_HEIGHT     48
#define LCD_COMMAND    0
#define LCD_DATA       1
#define LCD_DISPLAY_NORMAL    0x0C
#define LCD_DISPLAY_INVERTED  0x0D
#define LCD_SETXADDR  0x80
#define LCD_SETYADDR  0x40

// Definições para o display ASCII
extern const uint8_t ASCII[96][6];

// Estrutura para as configurações do LCD
struct LCD_GPIO {
    uint RSTPIN;
    uint CEPIN;
    uint DCPIN;
    uint DINPIN;
    uint CLKPIN;
};

// Estrutura para armazenar o estado do LCD
struct LCD_att {
    uint8_t buffer[504]; // Buffer para armazenar a tela
    bool inverttext;     // Flag para texto invertido
};

// Funções para configuração dos pinos
void LCD_setRST(uint gpio);
void LCD_setCE(uint gpio);
void LCD_setDC(uint gpio);
void LCD_setDIN(uint gpio);
void LCD_setCLK(uint gpio);

// Funções principais da biblioteca
void LCD_send(uint8_t val);
void LCD_write(uint8_t data, uint8_t mode);
void LCD_init();
void LCD_invert(bool mode);
void LCD_invertText(bool mode);
void LCD_putChar(char c);
void LCD_print(char *str, uint8_t x, uint8_t y);
void LCD_clrScr();
void LCD_goXY(uint8_t x, uint8_t y);
void LCD_refreshScr();
void LCD_refreshArea(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax);
void LCD_setPixel(uint8_t x, uint8_t y, bool pixel);
void LCD_drawHLine(int x, int y, int l);
void LCD_drawVLine(int x, int y, int l);
void LCD_drawLine(int x1, int y1, int x2, int y2);
void LCD_drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

#endif // NOKIA5110_LCD_H