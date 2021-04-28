#pragma once

#include <drivers/sensor.h>

int sens_temp_init(void);

int sens_temp_enable(void);

int sens_temp_sample_fetch( void );

int sens_temp_get_temp(struct sensor_value* value);

int sens_temp_get_humid(struct sensor_value* value);
