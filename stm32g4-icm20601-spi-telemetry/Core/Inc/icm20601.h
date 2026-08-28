#ifndef ICM20601_H
#define ICM20601_H

#include "stm32g4xx_hal.h"

// Register Adresleri
#define ICM20601_REG_WHO_AM_I   0x75
#define ICM20601_REG_USER_CTRL  0x6A
#define ICM20601_REG_PWR_MGMT_1 0x6B
#define ICM20601_REG_ACCEL_XOUT 0x3B

// CS pin tanımlamaları (Donanımsal PA4 pinimiz)
#define IMU_CS_PORT             GPIOA
#define IMU_CS_PIN              GPIO_PIN_4

typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
    float temperature;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} ICM20601_Data_t;

// Yeni SPI Fonksiyon Prototipleri
uint8_t ICM20601_Init(SPI_HandleTypeDef *hspi);
void ICM20601_Read_All(SPI_HandleTypeDef *hspi, ICM20601_Data_t *sensor_data);

#endif /* ICM20601_H */
