

#define BME280_CHIPID 0x60


int bme280_read(float* pres,float* temp,float* hum);
int bme280_init();