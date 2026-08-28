#include "icm20601.h"

uint8_t ICM20601_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t who_am_i = 0;
    uint8_t pwr_mgmt_data = 0x00;

    HAL_I2C_Mem_Read(hi2c, ICM20601_ADDR, ICM20601_REG_WHO_AM_I, I2C_MEMADD_SIZE_8BIT, &who_am_i, 1, 100);

    if (who_am_i != 0x00) {
        HAL_I2C_Mem_Write(hi2c, ICM20601_ADDR, ICM20601_REG_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &pwr_mgmt_data, 1, 100);
        HAL_Delay(10);
        return 1;
    }
    return 0;
}

void ICM20601_Read_All(I2C_HandleTypeDef *hi2c, ICM20601_Data_t *sensor_data) {
    uint8_t data_buffer[14];

    HAL_I2C_Mem_Read(hi2c, ICM20601_ADDR, ICM20601_REG_ACCEL_XOUT, I2C_MEMADD_SIZE_8BIT, data_buffer, 14, 100);

    sensor_data->accel_x = (int16_t)((data_buffer[0] << 8) | data_buffer[1]);
    sensor_data->accel_y = (int16_t)((data_buffer[2] << 8) | data_buffer[3]);
    sensor_data->accel_z = (int16_t)((data_buffer[4] << 8) | data_buffer[5]);

    // Sıcaklık verisini oku ve dönüştür
    int16_t raw_temp = (int16_t)((data_buffer[6] << 8) | data_buffer[7]);
    sensor_data->temperature = ((float)raw_temp / 326.8f) + 25.0f;

    sensor_data->gyro_x  = (int16_t)((data_buffer[8] << 8)  | data_buffer[9]);
    sensor_data->gyro_y  = (int16_t)((data_buffer[10] << 8) | data_buffer[11]);
    sensor_data->gyro_z  = (int16_t)((data_buffer[12] << 8) | data_buffer[13]);
}
