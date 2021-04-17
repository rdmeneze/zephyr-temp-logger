#include <zephyr.h>
#include <devicetree.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/i2c.h>
#include <drivers/sensor.h>
#include <drivers/regulator.h>
#include <stdio.h>

#define SLEEP_TIME_MS 1000

static struct onoff_client cli;
static struct onoff_manager *callback_srv;
static struct onoff_client *callback_cli;
static uint32_t callback_state;
static int callback_res;
static onoff_client_callback callback_fn;

static void callback(	struct onoff_manager *srv,
						struct onoff_client *cli,
						uint32_t state,
						int res)
{
	onoff_client_callback cb = callback_fn;

	callback_srv = srv;
	callback_cli = cli;
	callback_state = state;
	callback_res = res;
	callback_fn = NULL;

	if (cb != NULL) {
		cb(srv, cli, state, res);
	}
}

static void reset_callback(void)
{
	callback_srv = NULL;
	callback_cli = NULL;
	callback_state = INT_MIN;
	callback_res = 0;
	callback_fn = NULL;
}

static void reset_client(void)
{
	cli = (struct onoff_client){};
	reset_callback();
	sys_notify_init_callback(&cli.notify, callback);
}

void main(void)
{
	const struct device* dev_si7021 = device_get_binding("Si7021");
	int rc; 

	if ( NULL == dev_si7021 ) 
	{
		printf( "Could not get Si7021 device\n" );
		return;
	}

	const struct device* pwr = device_get_binding( "Si7021-pwr" );
	if (NULL == pwr )
	{
		printf( "Could not get pwr port\n" );
		return;
	}	

	reset_client();

	rc = regulator_enable( pwr, &cli );
	if ( rc < 0 )
	{
		printk("error enabling regulator\n");
		return;
	}

	k_sleep(K_MSEC(100));

	while( true )
	{
		struct sensor_value temp, hum; 

		rc = sensor_sample_fetch(dev_si7021);
		if (rc == 0)
		{
			rc = sensor_channel_get(dev_si7021, SENSOR_CHAN_AMBIENT_TEMP, &temp); 
		}

		if (rc == 0) 
		{
			rc = sensor_channel_get(dev_si7021, SENSOR_CHAN_HUMIDITY, &hum);
		}

		if ( rc != 0)
		{
			printf( "Si7021: failed %d\n", rc);
			break;
		}

		printf("Si7021: %.f - hum %.f RH\n", 
				sensor_value_to_double( &temp ), 
				sensor_value_to_double( &hum ));

		k_sleep( K_MSEC(SLEEP_TIME_MS) );
	}
	return;
}
