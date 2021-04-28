#include <devicetree.h>
#include <device.h>
#include <drivers/i2c.h>
#include <drivers/sensor.h>
#include <drivers/regulator.h>

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

const struct device* dev_si7021 = NULL;
const struct device* pwr = NULL;

int sens_temp_init(void)
{
	dev_si7021 = device_get_binding("Si7021");

	if ( NULL == dev_si7021 ) 
	{
		return -1;
	}

	pwr = device_get_binding( "Si7021-pwr" );
	if (NULL == pwr )
	{
		return -1;
	}	

	reset_client();

    return 0;
}

int sens_temp_enable(void)
{
    int rc; 
	rc = regulator_enable( pwr, &cli );
    return rc;
}

int sens_temp_sample_fetch()
{
    int rc = sensor_sample_fetch(dev_si7021);
    return rc;
}

int sens_temp_get_temp(struct sensor_value* value)
{
    int rc = sensor_channel_get(dev_si7021, SENSOR_CHAN_AMBIENT_TEMP, value); 
    return rc;
}

int sens_temp_get_humid(struct sensor_value* value)
{
    int rc = sensor_channel_get(dev_si7021, SENSOR_CHAN_HUMIDITY, value); 
    return rc;
}
