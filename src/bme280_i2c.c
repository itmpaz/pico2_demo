
#include <stdio.h>
#include <stdint.h>

#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "bme280_i2c.h"


#define PICO_BME280_I2C PICO_DEFAULT_I2C
#define PICO_BME280_I2C_SDA_PIN PICO_DEFAULT_I2C_SDA_PIN
#define PICO_BME280_I2C_SCL_PIN PICO_DEFAULT_I2C_SCL_PIN




 // device has default bus address of 0x76
#define ADDR _u(0x76)

// hardware registers
#define REG_CONFIG _u(0xF5)
#define REG_CTRL_MEAS _u(0xF4)
#define REG_RESET _u(0xE0)

#define REG_TEMP_XLSB _u(0xFC)
#define REG_TEMP_LSB _u(0xFB)
#define REG_TEMP_MSB _u(0xFA)

#define REG_PRESSURE_XLSB _u(0xF9)
#define REG_PRESSURE_LSB _u(0xF8)
#define REG_PRESSURE_MSB _u(0xF7)

// calibration registers
#define REG_DIG_T1_LSB _u(0x88)
#define REG_DIG_T1_MSB _u(0x89)
#define REG_DIG_T2_LSB _u(0x8A)
#define REG_DIG_T2_MSB _u(0x8B)
#define REG_DIG_T3_LSB _u(0x8C)
#define REG_DIG_T3_MSB _u(0x8D)
#define REG_DIG_P1_LSB _u(0x8E)
#define REG_DIG_P1_MSB _u(0x8F)
#define REG_DIG_P2_LSB _u(0x90)
#define REG_DIG_P2_MSB _u(0x91)
#define REG_DIG_P3_LSB _u(0x92)
#define REG_DIG_P3_MSB _u(0x93)
#define REG_DIG_P4_LSB _u(0x94)
#define REG_DIG_P4_MSB _u(0x95)
#define REG_DIG_P5_LSB _u(0x96)
#define REG_DIG_P5_MSB _u(0x97)
#define REG_DIG_P6_LSB _u(0x98)
#define REG_DIG_P6_MSB _u(0x99)
#define REG_DIG_P7_LSB _u(0x9A)
#define REG_DIG_P7_MSB _u(0x9B)
#define REG_DIG_P8_LSB _u(0x9C)
#define REG_DIG_P8_MSB _u(0x9D)
#define REG_DIG_P9_LSB _u(0x9E)
#define REG_DIG_P9_MSB _u(0x9F)

#define REG_DIG_H1 _u(0xA1)
#define REG_DIG_H2 _u(0xE1)



#define REG_HUM_CTRL _u(0xF2)
#define REG_HUM_LSB _u(0xFE)
#define REG_HUM_MSB _u(0xFD)


#define REG_CHIP_ID _u(0xD0)





// number of calibration registers to be read
#define NUM_CALIB_PARAMS (24+8)

struct bme280_calib_param {

   // temperature params
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;

    // pressure params
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;


  uint8_t  dig_H1;
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  int8_t   dig_H6;



};

struct bme280_calib_param bme280_params={0};

int bme280_init_sensor() {

    // use the "handheld device dynamic" optimal setting (see datasheet)
    uint8_t buf[2];



    buf[0] = REG_HUM_CTRL;
    buf[1] = 0b101; //x16
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);




    // 500ms sampling time, x16 filter
    const uint8_t reg_config_val = ((0x04 << 5) | (0x05 << 2)) & 0xFC;

    // send register number followed by its corresponding value
    buf[0] = REG_CONFIG;
    buf[1] = reg_config_val;
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);

    // osrs_t x1, osrs_p x4, normal mode operation
    const uint8_t reg_ctrl_meas_val = (0x01 << 5) | (0x03 << 2) | (0x03);
    buf[0] = REG_CTRL_MEAS;
    buf[1] = reg_ctrl_meas_val;
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);



    uint8_t chipid;
    uint8_t reg = REG_CHIP_ID;
    i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, &chipid, 1, false);  // false - finished with bus    

   
    return chipid;


}

void bme280_read_raw(int32_t* temp, int32_t* pressure, int32_t* hum) {
    // BMP280 data registers are auto-incrementing and we have 3 temperature and
    // pressure registers each, so we start at 0xF7 and read 6 bytes to 0xFC
    // note: normal mode does not require further ctrl_meas and config register writes

    uint8_t buf[8];
    uint8_t reg = REG_PRESSURE_MSB;
    i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, buf, 8, false);  // false - finished with bus

    // store the 20 bit read in a 32 bit signed integer for conversion
    *pressure = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);
    *temp = (buf[3] << 12) | (buf[4] << 4) | (buf[5] >> 4);
    *hum = ((int32_t)buf[6]) << 8 | buf[7];


}

void bme280_reset() {
    // reset the device with the power-on-reset procedure
    uint8_t buf[2] = { REG_RESET, 0xB6 };
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
}

// intermediate function that calculates the fine resolution temperature
// used for both pressure and temperature conversions
int32_t bme280_convert(int32_t temp, struct bme280_calib_param* params) {
    // use the 32-bit fixed point compensation implementation given in the
    // datasheet
    
    int32_t var1, var2;
    var1 = ((((temp >> 3) - ((int32_t)params->dig_t1 << 1))) * ((int32_t)params->dig_t2)) >> 11;
    var2 = (((((temp >> 4) - ((int32_t)params->dig_t1)) * ((temp >> 4) - ((int32_t)params->dig_t1))) >> 12) * ((int32_t)params->dig_t3)) >> 14;
    return var1 + var2;
}

int32_t bme280_convert_temp(int32_t temp, struct bme280_calib_param* params) {
    // uses the BMP280 calibration parameters to compensate the temperature value read from its registers
    int32_t t_fine = bme280_convert(temp, params);
    return (t_fine * 5 + 128) >> 8;
}

int32_t bme280_convert_pressure(int32_t pressure, int32_t temp, struct bme280_calib_param* params) {
    // uses the BMP280 calibration parameters to compensate the pressure value read from its registers

    int32_t t_fine = bme280_convert(temp, params);

    int32_t var1, var2;
    uint32_t converted = 0.0;
    var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)params->dig_p6);
    var2 += ((var1 * ((int32_t)params->dig_p5)) << 1);
    var2 = (var2 >> 2) + (((int32_t)params->dig_p4) << 16);
    var1 = (((params->dig_p3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)params->dig_p2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t)params->dig_p1)) >> 15);
    if (var1 == 0) {
        return 0;  // avoid exception caused by division by zero
    }
    converted = (((uint32_t)(((int32_t)1048576) - pressure) - (var2 >> 12))) * 3125;
    if (converted < 0x80000000) {
        converted = (converted << 1) / ((uint32_t)var1);
    } else {
        converted = (converted / (uint32_t)var1) * 2;
    }
    var1 = (((int32_t)params->dig_p9) * ((int32_t)(((converted >> 3) * (converted >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(converted >> 2)) * ((int32_t)params->dig_p8)) >> 13;
    converted = (uint32_t)((int32_t)converted + ((var1 + var2 + params->dig_p7) >> 4));
    return converted;
}




float compensateHumidity(int32_t adc_H, struct bme280_calib_param*  cal, int32_t t_fine) 
{

    
  int32_t v_x1_u32r;

  v_x1_u32r = (t_fine - ((int32_t)76800));

  v_x1_u32r = (((((adc_H << 14) - (((int32_t)cal->dig_H4) << 20) -
      (((int32_t)cal->dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
         (((((((v_x1_u32r * ((int32_t)cal->dig_H6)) >> 10) *
        (((v_x1_u32r * ((int32_t)cal->dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
      ((int32_t)2097152)) * ((int32_t)cal->dig_H2) + 8192) >> 14));

  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
           ((int32_t)cal->dig_H1)) >> 4));

  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  float h = (v_x1_u32r>>12);
  return  h / 1024.0;
}





void bme280_get_calib_params(struct bme280_calib_param* params) {
    // raw temp and pressure values need to be calibrated according to
    // parameters generated during the manufacturing of the sensor
    // there are 3 temperature params, and 9 pressure params, each with a LSB
    // and MSB register, so we read from 24 registers

    uint8_t buf[32] = { 0 };
    uint8_t reg = REG_DIG_T1_LSB;
    i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  // true to keep master control of bus
    // read in one go as register addresses auto-increment
    i2c_read_blocking(i2c_default, ADDR, buf, 24, false);  // false, we're done reading

    reg = REG_DIG_H1;
    i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  
    i2c_read_blocking(i2c_default, ADDR, &buf[24], 1, false); 

    reg = REG_DIG_H2;
    i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  
    i2c_read_blocking(i2c_default, ADDR, &buf[25], 7, false); 


    // store these in a struct for later use
    params->dig_t1 = (uint16_t)(buf[1] << 8) | buf[0];
    params->dig_t2 = (int16_t)(buf[3] << 8) | buf[2];
    params->dig_t3 = (int16_t)(buf[5] << 8) | buf[4];

    params->dig_p1 = (uint16_t)(buf[7] << 8) | buf[6];
    params->dig_p2 = (int16_t)(buf[9] << 8) | buf[8];
    params->dig_p3 = (int16_t)(buf[11] << 8) | buf[10];
    params->dig_p4 = (int16_t)(buf[13] << 8) | buf[12];
    params->dig_p5 = (int16_t)(buf[15] << 8) | buf[14];
    params->dig_p6 = (int16_t)(buf[17] << 8) | buf[16];
    params->dig_p7 = (int16_t)(buf[19] << 8) | buf[18];
    params->dig_p8 = (int16_t)(buf[21] << 8) | buf[20];
    params->dig_p9 = (int16_t)(buf[23] << 8) | buf[22];



    params->dig_H1 = buf[24];
    params->dig_H2 = (int16_t)((buf[26]<<8) | buf[25]);
    params->dig_H3 = buf[27];
    params->dig_H4 = (int16_t)((buf[28]<<4) | (buf[29] & 0xF));
    params->dig_H5 = (((int16_t)buf[30]) <<4 ) | (buf[29] >> 4 );
    params->dig_H6 = (int8_t)buf[31];
/*
    printf("buf: ");
    for(int i=24;i<=31;i++)
        printf("%02X ",buf[i]);
    printf("\n");

	printf("H1 %d\n",params->dig_H1);
	printf("H2 %d\n",params->dig_H2);
	printf("H3 %d\n",params->dig_H3);
	printf("H4 %d\n",params->dig_H4);
	printf("H5 %d\n",params->dig_H5);
	printf("H6 %d\n",params->dig_H6);
*/	

}



int bme280_init() {



    bi_decl(bi_2pins_with_func(PICO_BME280_I2C_SDA_PIN, PICO_BME280_I2C_SCL_PIN, GPIO_FUNC_I2C));
    bi_decl(bi_program_description("BME280 example"));

    

    
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_BME280_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_BME280_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_BME280_I2C_SDA_PIN);
    gpio_pull_up(PICO_BME280_I2C_SCL_PIN);

    
    int chipid = bme280_init_sensor();

    if (chipid==BME280_CHIPID)
        bme280_get_calib_params(&bme280_params);

    return chipid;



}


int bme280_read(float* pres_hpa,float* temp_c,float* hum_persent)
{
    int32_t raw_temperature;
    int32_t raw_pressure;
    int32_t raw_hum;
    sleep_ms(250); // sleep so that data polling and register update don't collide
    
    bme280_read_raw(&raw_temperature, &raw_pressure,&raw_hum);
    int32_t temperature = bme280_convert_temp(raw_temperature, &bme280_params);
    int32_t pressure = bme280_convert_pressure(raw_pressure, raw_temperature, &bme280_params);

    int32_t t_fine = bme280_convert(raw_temperature, &bme280_params);
    *hum_persent = compensateHumidity(raw_hum, &bme280_params,t_fine);

    *pres_hpa = pressure / 100.f;
    *temp_c = temperature / 100.f;

    return 0;
}