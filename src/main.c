#include <zephyr.h>
#include <stdio.h>
#include "temp_sens.h"

#define SLEEP_TIME_MS 1000

void main(void)
{
	int rc = sens_temp_init();
	if ( NULL != 0 ) 
	{
		printf( "Could not get sensor device\n" );
		return;
	}

	rc = sens_temp_enable();
	if ( rc < 0 )
	{
		printk("error enabling regulator\n");
		return;
	}

	k_sleep(K_MSEC(100));

	while( true )
	{
		struct sensor_value temp, hum; 

		rc = sens_temp_sample_fetch();
		if (rc == 0)
		{
			rc = sens_temp_get_temp( &temp );
		}

		if (rc == 0) 
		{
			rc = sens_temp_get_humid( &hum );
		}

		if ( rc != 0)
		{
			printf( "sens temp hum: failed %d\n", rc);
			break;
		}

		printf("temp: %.f - hum %.f RH\n", 
				sensor_value_to_double( &temp ), 
				sensor_value_to_double( &hum ));

		k_sleep( K_MSEC(SLEEP_TIME_MS) );
	}

	return;
}
