#include "SH1107.h"

extern I2C_HandleTypeDef hi2c1; // main.c'deki donanım tanımını çağırıyoruz
uint8_t screen_buffer[2048];    // 128x128 / 8 = 2048 Byte RAM tamponu

// Ekrana komut gönderme
void SH1107_WriteCmd(uint8_t cmd) {
    HAL_I2C_Mem_Write(&hi2c1, SH1107_I2C_ADDR, SH1107_CMD, I2C_MEMADD_SIZE_8BIT, &cmd, 1, HAL_MAX_DELAY);
}

// Ekran Başlatma Komut Dizisi
void SH1107_Init(void) {
    SH1107_WriteCmd(0xAE); // Display OFF
    SH1107_WriteCmd(0xDC); // Set Display Start Line
    SH1107_WriteCmd(0x00);
    SH1107_WriteCmd(0x81); // Kontrast
    SH1107_WriteCmd(0x2F);
    SH1107_WriteCmd(0xA0); // Segment Re-map
    SH1107_WriteCmd(0xC0); // COM Tarama Yönü
    SH1107_WriteCmd(0xA4); // Normal Mod
    SH1107_WriteCmd(0xA6); // Normal (Inverse Off)
    SH1107_WriteCmd(0xD5); // Saat Bölme Oranı
    SH1107_WriteCmd(0x50);
    SH1107_WriteCmd(0xD9); // Ön Şarj Periyodu
    SH1107_WriteCmd(0x22);
    SH1107_WriteCmd(0xDB); // VCOM Seviyesi
    SH1107_WriteCmd(0x35);
    SH1107_WriteCmd(0xAD); // Dahili DC-DC Ayarı
    SH1107_WriteCmd(0x8A); // DC-DC ON
    SH1107_WriteCmd(0xAF); // Display ON
}

// RAM'deki sanal ekranı temizler
void SH1107_ClearBuffer(void) {
    for(uint16_t i = 0; i < 2048; i++) {
        screen_buffer[i] = 0x00;
    }
}

// Buffer içeriğini I2C ile ekrana basar (Adafruit'teki display.display() gibi)
void SH1107_Update(void) {
    for (uint8_t page = 0; page < 16; page++) {
        SH1107_WriteCmd(0xB0 + page); // Sayfa Seçimi
        SH1107_WriteCmd(0x00);        // Alt Sütun Adresi
        SH1107_WriteCmd(0x10);        // Üst Sütun Adresi
        HAL_I2C_Mem_Write(&hi2c1, SH1107_I2C_ADDR, SH1107_DATA, I2C_MEMADD_SIZE_8BIT, &screen_buffer[page * 128], 128, HAL_MAX_DELAY);
    }
}

// Piksel Boyama
void SH1107_DrawPixel(int16_t x, int16_t y, uint8_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;

    uint16_t index = x + (y / 8) * SCREEN_WIDTH;
    if (color) screen_buffer[index] |= (1 << (y % 8));
    else       screen_buffer[index] &= ~(1 << (y % 8));
}

// Çizgi Çizme (Bresenham Algoritması)
void SH1107_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
    int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy, e2;

    while (1) {
        SH1107_DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// Dikdörtgen Çizme
void SH1107_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
    SH1107_DrawLine(x, y, x + w - 1, y, color);
    SH1107_DrawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
    SH1107_DrawLine(x, y, x, y + h - 1, color);
    SH1107_DrawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}
