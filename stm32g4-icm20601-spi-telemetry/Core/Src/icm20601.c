#include "icm20601.h"

extern volatile uint8_t sensor_id; // main.c'deki sensor_id değişkenini bu dosyaya tanıtıyoruz


uint8_t ICM20601_Init(SPI_HandleTypeDef *hspi) {
    uint8_t tx_init_buf[2] = {0};
    uint8_t rx_init_buf[2] = {0};
    uint8_t tx_buf[2];

    // CS High yap (Hat başlangıçta pasif)
    HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_SET);
    HAL_Delay(10);

    // 1. TAM SENKRON WHO_AM_I SORGUSU (Hata Çözümü)
    tx_init_buf[0] = ICM20601_REG_WHO_AM_I | 0x80; // Okuma biti eklendi
    tx_init_buf[1] = 0x00; // Dummy byte (Alıcı clock vursun diye)

    HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(hspi, tx_init_buf, rx_init_buf, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_SET);

    // Gelen veri rx_init_buf[1] hüfresine düşer
    sensor_id = rx_init_buf[1];

    if (sensor_id == 0x00 || sensor_id == 0xFF) {
        return 0; // Sensör hatta değilse başlatma başarısız
    }

    // 2. I2C Arayüzünü tamamen kapat (Sadece SPI aktif kalır)
    tx_buf[0] = ICM20601_REG_USER_CTRL;
    tx_buf[1] = 0x10; // I2C_IF_DIS = 1
    HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi, tx_buf, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_SET);
    HAL_Delay(10);

    // 3. Cihazı uykudan uyandır ve en iyi gyro saatini seç
    tx_buf[0] = ICM20601_REG_PWR_MGMT_1;
    tx_buf[1] = 0x01;
    HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi, tx_buf, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_SET);
    HAL_Delay(50);

    return 1; // Başarılı
}

void ICM20601_Read_All(SPI_HandleTypeDef *hspi, ICM20601_Data_t *sensor_data) {
    uint8_t tx_buf[15] = {0};
    uint8_t rx_buf[15] = {0};

    // İlk byte okuma adresi, gerisi dummy (boş) veri
    tx_buf[0] = ICM20601_REG_ACCEL_XOUT | 0x80;

    HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_RESET);

    // Transmit ve Receive işlemlerini eş zamanlı yapmak hatları tam senkronize tutar
    HAL_SPI_TransmitReceive(hspi, tx_buf, rx_buf, 15, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_SET);

    // Veriler rx_buf[1]'den itibaren başlar, çünkü rx_buf[0] adresi gönderirken dolan boş byte'tır.
    int16_t accel_x_raw = (rx_buf[1] << 8) | rx_buf[2];
    int16_t accel_y_raw = (rx_buf[3] << 8) | rx_buf[4];
    int16_t accel_z_raw = (rx_buf[5] << 8) | rx_buf[6];
    int16_t temp_raw    = (rx_buf[7] << 8) | rx_buf[8];
    int16_t gyro_x_raw  = (rx_buf[9] << 8) | rx_buf[10];
    int16_t gyro_y_raw  = (rx_buf[11] << 8) | rx_buf[12];
    int16_t gyro_z_raw  = (rx_buf[13] << 8) | rx_buf[14];

    // Fiziksel birimlere dönüştürme
    sensor_data->accel_x = (float)accel_x_raw / 8192.0f;
    sensor_data->accel_y = (float)accel_y_raw / 8192.0f;
    sensor_data->accel_z = (float)accel_z_raw / 8192.0f;
    sensor_data->temperature = ((float)temp_raw / 326.8f) + 25.0f;
    sensor_data->gyro_x = (float)gyro_x_raw / 65.5f;
    sensor_data->gyro_y = (float)gyro_y_raw / 65.5f;
    sensor_data->gyro_z = (float)gyro_z_raw / 65.5f;
}
