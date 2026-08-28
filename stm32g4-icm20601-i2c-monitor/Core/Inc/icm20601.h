#ifndef ICM20601_H
#define ICM20601_H

#include "stm32g4xx_hal.h"

#define ICM20601_ADDR           (0x68 << 1)

#define ICM20601_REG_WHO_AM_I   0x75
#define ICM20601_REG_PWR_MGMT_1 0x6B
#define ICM20601_REG_ACCEL_XOUT 0x3B

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    float temperature; // <--- Sıcaklık için eklendi
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} ICM20601_Data_t;

uint8_t ICM20601_Init(I2C_HandleTypeDef *hi2c);
void ICM20601_Read_All(I2C_HandleTypeDef *hi2c, ICM20601_Data_t *sensor_data);

#endif /* ICM20601_H */



