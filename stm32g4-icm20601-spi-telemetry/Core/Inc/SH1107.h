#ifndef INC_SH1107_H_
#define INC_SH1107_H_

#include "stm32g4xx_hal.h" // NUCLEO-G431RB HAL sürücüleri
#include <stdlib.h>

// Ekran Ayarları
#define SH1107_I2C_ADDR         (0x3C << 1) // 0x78
#define SH1107_CMD              0x00
#define SH1107_DATA             0x40
#define SCREEN_WIDTH            128
#define SCREEN_HEIGHT           128

// Fonksiyon Protokolleri
void SH1107_Init(void);
void SH1107_Update(void);
void SH1107_ClearBuffer(void);
void SH1107_DrawPixel(int16_t x, int16_t y, uint8_t color);
void SH1107_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
void SH1107_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);


void SH1107_DrawChar(int16_t x, int16_t y, char ch, uint8_t color);
void SH1107_DrawString(int16_t x, int16_t y, const char* str, uint8_t color);



void SH1107_Draw_Telemetry(float ax, float ay, float az, float temp, float gz);




#endif /* INC_SH1107_H_ */
