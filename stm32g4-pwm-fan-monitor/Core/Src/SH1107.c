#include "SH1107.h"

#include <stdio.h>  // <-- BU SATIRI EKLE (sprintf uyarısını çözer)
#include <inttypes.h> // <-- BU SATIRI EKLE (uint32_t formatı için)



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
// Buffer içeriğini I2C ile ekrana basar (SH1107 128x128 için optimize edildi)
void SH1107_Update(void) {
    for (uint8_t page = 0; page < 16; page++) {
        SH1107_WriteCmd(0xB0 + page); // Sayfa Seçimi

        // SH1107 128x128 ekranlarda genellikle 2 veya 4 sütunluk bir RAM offseti olur.
        // Başlangıç sütun adresini 0 yerine 2 veya 4 yapmak kaymayı ve harf bozulmalarını sıfırlar.
        uint8_t column = 2;

        SH1107_WriteCmd(column & 0x0F);         // Alt Sütun Adresi (Lower 4-bit)
        SH1107_WriteCmd(0x10 | (column >> 4));   // Üst Sütun Adresi (Higher 4-bit)

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


// 5x7 ASCII Standart Font Tablosu (0x20 Space karakterinden başlar)
static const uint8_t ASCII_Font[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Space (0x20)
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x7F, 0x41, 0x41, 0x00}, // [
	{0x02, 0x04, 0x08, 0x10, 0x20},
    {0x00, 0x41, 0x41, 0x7F, 0x00}, // ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x0C, 0x52, 0x52, 0x52, 0x3E}, // g
    {0x7F, 0x08, 0x04, 0x04, 0x7F}, // h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x20, 0x40, 0x44, 0x3D, 0x00}, // j
    {0x7F, 0x10, 0x28, 0x44, 0x00}, // k
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // l
    {0x7C, 0x04, 0x18, 0x04, 0x7C}, // m
    {0x7C, 0x08, 0x04, 0x04, 0x7C}, // n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0x7C, 0x14, 0x14, 0x14, 0x08}, // p
    {0x08, 0x14, 0x14, 0x14, 0x7C}, // q
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, // y
    {0x44, 0x64, 0x54, 0x4C, 0x44}  // z
};

// Ekrana Tek Bir Karakter Çizer
void SH1107_DrawChar(int16_t x, int16_t y, char ch, uint8_t color) {
    if (ch < 32 || ch > 126) return; // Geçersiz ASCII kontrolü

    uint8_t font_index = ch - 32;

    for (uint8_t i = 0; i < 5; i++) { // Her karakter 5 sütun genişliğinde
        uint8_t line = ASCII_Font[font_index][i];
        for (uint8_t j = 0; j < 8; j++) { // Her sütun 8 piksel yüksekliğinde
            if (line & (1 << j)) {
                SH1107_DrawPixel(x + i, y + j, color);
            }
        }
    }
}

// Ekrana String (Metin dizisi) Çizer
void SH1107_DrawString(int16_t x, int16_t y, const char* str, uint8_t color) {
    while (*str) {
        SH1107_DrawChar(x, y, *str, color);
        x += 6; // Bir sonraki karakter için 6 piksel sağa kay (5 piksel karakter + 1 piksel boşluk)
        if (x > SCREEN_WIDTH - 6) { // Ekran sınırına gelirse alt satıra geç
            x = 0;
            y += 9;
        }
        str++;
    }
}




// Kullanıcı Dostu Gelişmiş Telemetri Arayüzü
void SH1107_Draw_Telemetry(float ax, float ay, float az, float temp, float gz) {
    char local_msg[32];

    // 1. Üst Başlık ve Dekoratif Çizgiler
    SH1107_DrawRect(0, 0, 128, 128, 1); // Ekranın dışına tam bir çerçeve çiz
    SH1107_DrawLine(0, 18, 128, 18, 1); // Başlığı ayıran yatay çizgi
    SH1107_DrawString(20, 5, "NUCLEO IMU UI", 1);

    // 2. İVME ÖLÇER BÖLÜMÜ (Sol tarafa hizalı)
    SH1107_DrawString(8, 25, "[ACCELEROMETER]", 1);

    int ax_i = (int)ax, ax_f = abs((int)((ax - ax_i) * 100));
    sprintf(local_msg, "X: %d.%02d g", ax_i, ax_f);
    SH1107_DrawString(12, 38, local_msg, 1);

    int ay_i = (int)ay, ay_f = abs((int)((ay - ay_i) * 100));
    sprintf(local_msg, "Y: %d.%02d g", ay_i, ay_f);
    SH1107_DrawString(12, 49, local_msg, 1);

    int az_i = (int)az, az_f = abs((int)((az - az_i) * 100));
    sprintf(local_msg, "Z: %d.%02d g", az_i, az_f);
    SH1107_DrawString(12, 60, local_msg, 1);

    // Orta Ayrım Çizgisi
    SH1107_DrawLine(0, 73, 128, 73, 1);

    // 3. JİROSKOP BÖLÜMÜ
    SH1107_DrawString(8, 80, "[GYROSCOPE Z]", 1);
    int gz_i = (int)gz, gz_f = abs((int)((gz - gz_i) * 10));
    sprintf(local_msg, "Rate: %d.%d dps", gz_i, gz_f);
    SH1107_DrawString(12, 93, local_msg, 1);

    // Alt Ayrım Çizgisi
    SH1107_DrawLine(0, 107, 128, 107, 1);

    // 4. SICAKLIK BÖLÜMÜ (En altta şık bir bar şeklinde)
    int t_i = (int)temp, t_f = abs((int)((temp - t_i) * 10));
    sprintf(local_msg, "SYSTEM TEMP: %d.%d C", t_i, t_f);
    SH1107_DrawString(12, 113, local_msg, 1);
}



// Fan Telemetrisi Görsel Arayüzü
// Sevdiğin Orijinal Sade Tasarım (SH1107 Ekran Genişliğine Optimize Edildi)
void SH1107_Draw_Fan_Telemetry(uint32_t frequency, uint32_t rpm, uint8_t duty_percent) {
    char local_msg[32];

    // Sağ sınır kaymasını engellemek için sığma limitini 120 yapıyoruz
    uint16_t max_w = 120;

    // 1. Üst Başlık ve Dekoratif Çizgiler
    SH1107_DrawRect(0, 0, max_w, 128, 1); // Ekranın dışına tam bir çerçeve çiz
    SH1107_DrawLine(0, 22, max_w, 22, 1); // Başlığı ayıran yatay çizgi
    SH1107_DrawString(16, 7, "PWM FAN MONITOR", 1); // Başlık metni

    // 2. PWM GİRİŞ BÖLÜMÜ
    SH1107_DrawString(8, 32, "[CONTROL STATUS]", 1);
    sprintf(local_msg, "Target PWM: %%%d", duty_percent);
    SH1107_DrawString(12, 47, local_msg, 1);

    // Orta Ayrım Çizgisi
    SH1107_DrawLine(0, 64, max_w, 64, 1);

    // 3. TAKOMETRE FEEDBACK BÖLÜMÜ
    SH1107_DrawString(8, 74, "[TACHO FEEDBACK]", 1);

    // Frekans Gösterimi
    sprintf(local_msg, "Freq : %lu Hz", (unsigned long)frequency);
    SH1107_DrawString(12, 89, local_msg, 1);

    // RPM Gösterimi
    sprintf(local_msg, "Speed: %lu RPM", (unsigned long)rpm);
    SH1107_DrawString(12, 101, local_msg, 1);

    // Alt Ayrım Çizgisi
    SH1107_DrawLine(0, 114, max_w, 114, 1);

    // 4. DURUM BÖLÜMÜ
    if (rpm > 0) {
        SH1107_DrawString(20, 118, "STATUS: RUNNING", 1);
    } else {
        SH1107_DrawString(20, 118, "STATUS: STOPPED", 1);
    }
}
