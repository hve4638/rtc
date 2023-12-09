/*
 * Driver for TI BQ32000 Real Time Clock
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/rtc.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/errno.h>

// I2C Address on the Beagleplay board
#define BQ32000_ADDR 0x68
// RTC Normal Registers
#define BQ32000_REG_SECONDS 0x00
#define BQ32000_REG_MINUTES 0x01
#define BQ32000_REG_CENT_HOURS  0x02
#define BQ32000_REG_DAY 0x03
#define BQ32000_REG_DATE    0x04
#define BQ32000_REG_MONTH   0x05
#define BQ32000_REG_YEARS   0x06
#define BQ32000_REG_CAL_CFG1    0x07
#define BQ32000_REG_TCH2    0x08
#define BQ32000_REG_CFG2    0x09
// RTC Special Function Registers
#define BQ32000_REG_SFKEY1  0x20
#define BQ32000_REG_SFKEY2  0x21
#define BQ32000_REG_SFR 0x22

// I2C Read After Backup Mode
//To ensure that the correct time is read after backup mode, the host should wait longer than 1 second after the main supply is greater than 2.8 V and VBACK.

// RTC MASKS
// MASK10 and MASK1 are used to mask BCD expressions
#define BQ32000_SECONDS_MASK    0x7F
#define BQ32000_STOP_MASK   0x80
#define BQ32000_SECONDS_MASK10  0x70
#define BQ32000_SECONDS_MASK1   0x0F
#define BQ32000_MINUTES_MASK    0x7F
#define BQ32000_OF_MASK  0x80
#define BQ32000_MINUTES_MASK10  0x70
#define BQ32000_MINUTES_MASK1   0x0F
#define BQ32000_CENT_EN_MASK    0x80
#define BQ32000_CENT_MASK   0x40
#define BQ32000_HOURS_MASK  0x3F
#define BQ32000_HOURS_MASK10   0x30
#define BQ32000_HOURS_MASK1    0x0F
#define BQ32000_DAY_MASK    0x07
#define BQ32000_DATE_MASK   0x3F
#define BQ32000_DATE_MASK10    0x30
#define BQ32000_DATE_MASK1 0x0F
#define BQ32000_MONTH_MASK  0x1F
#define BQ32000_MONTH_MASK10   0x10
#define BQ32000_MONTH_MASK1    0x0F
#define BQ32000_YEARS_MASK  0xFF
#define BQ32000_YEARS_MASK10   0xF0
#define BQ32000_YEARS_MASK1    0x0F

#define BQ32000_OUT_MASK  0x80
#define BQ32000_FT_MASK  0x40
#define BQ32000_S_MASK 0x20
#define BQ32000_CAL_MASK 0x10
#define bq32000_TCH2_MASK 0x20
#define BQ32000_TCFE_MASK 0x40
#define BQ32000_TCHE_MASK 0x0F
#define BQ32000_FTF_MASK 0x01

#define BQ32000_TCFE BIT(6)

#define I2C_M_WR 0

struct bq32000_regs {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t cent_hours;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t years;
};
static struct i2c_driver rtc_driver;

static int bq32000_read(struct device *dev, void *buf, uint8_t subaddress, int count){
    struct i2c_client *client = to_i2c_client(dev);
    struct i2c_msg msg[2];
    int ret;

    msg[0].addr = client->addr;
    msg[0].flags = I2C_M_WR;
    msg[0].buf = &subaddress;
    msg[0].len = 1;

    msg[1].addr = client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].buf = buf;
    msg[1].len = count;

    ret = i2c_transfer(client->adapter, msg, 2);
    if (ret != 2)
        return -1;
    return 0;
}
static int bq32000_write(struct device *dev, void *buf, uint8_t subaddress, int count){
    struct i2c_client *client = to_i2c_client(dev);
    uint8_t buffer[11];
    int ret;

    buffer[0] = subaddress;
    memcpy(&buffer[1], buf, count);

    ret = i2c_master_send(client, buffer, count + 1);
    if (ret != count + 1)
        return -1;
    return 0;
}
static int bq32000_rtc_read_time(struct device *dev, struct rtc_time *tm){
    struct bq32000_regs regs;
    int ret;

    ret = bq32000_read(dev, &regs, 0, sizeof(regs));
    if (ret)
        return ret;
    tm->tm_sec = ((regs.seconds & BQ32000_SECONDS_MASK10) >> 4) * 10 + (regs.seconds & BQ32000_SECONDS_MASK1);
    tm->tm_min = ((regs.minutes & BQ32000_MINUTES_MASK10) >> 4) * 10 + (regs.minutes & BQ32000_MINUTES_MASK1);
    tm->tm_hour = ((regs.cent_hours & BQ32000_HOURS_MASK10) >> 4) * 10 + (regs.cent_hours & BQ32000_HOURS_MASK1);
    tm->tm_mday = ((regs.date & BQ32000_DATE_MASK10) >> 4) * 10 + (regs.date & BQ32000_DATE_MASK1);
    tm->tm_wday = regs.day & BQ32000_DAY_MASK;
    tm->tm_mon = ((regs.month & BQ32000_MONTH_MASK10) >> 4) * 10 + (regs.month & BQ32000_MONTH_MASK1);
    tm->tm_year = ((regs.years & BQ32000_YEARS_MASK10) >> 4) * 10 + (regs.years & BQ32000_YEARS_MASK1);
    return ret;
}
static int bq32000_rtc_set_time(struct device *dev, struct rtc_time *tm){
    struct bq32000_regs regs;

    regs.seconds = ((tm->tm_sec / 10) << 4) | (tm->tm_sec % 10);
    regs.minutes = ((tm->tm_min / 10) << 4) | (tm->tm_min % 10);
    regs.cent_hours = ((tm->tm_hour / 10) << 4) | (tm->tm_hour % 10);
    regs.date = ((tm->tm_mday / 10) << 4) | (tm->tm_mday % 10);
    regs.day = tm->tm_wday;
    regs.month = ((tm->tm_mon / 10) << 4) | (tm->tm_mon % 10);
    regs.years = ((tm->tm_year / 10) << 4) | (tm->tm_year % 10);
    return bq32000_write(dev, &regs, 0, sizeof(regs));
}

static int trickle_charger_of_init(struct device *dev, struct device_node *node)
{
	unsigned char reg;
	int error;
	u32 ohms = 0;

	if (of_property_read_u32(node, "trickle-resistor-ohms" , &ohms))
		return 0;

	switch (ohms) {
	case 180+940:
		/*
		 * TCHE[3:0] == 0x05, TCH2 == 1, TCFE == 0 (charging
		 * over diode and 940ohm resistor)
		 */

		if (of_property_read_bool(node, "trickle-diode-disable")) {
			dev_err(dev, "diode and resistor mismatch\n");
			return -EINVAL;
		}
		reg = 0x05;
		break;

	case 180+20000:
		/* diode disabled */

		if (!of_property_read_bool(node, "trickle-diode-disable")) {
			dev_err(dev, "bq32000: diode and resistor mismatch\n");
			return -EINVAL;
		}
		reg = 0x45;
		break;

	default:
		dev_err(dev, "invalid resistor value (%d)\n", ohms);
		return -EINVAL;
	}

	error = bq32000_write(dev, &reg, BQ32000_REG_CFG2, 1);
	if (error)
		return error;

	reg = 0x20;
	error = bq32000_write(dev, &reg, BQ32000_REG_TCH2, 1);
	if (error)
		return error;

	dev_info(dev, "Enabled trickle RTC battery charge.\n");
	return 0;
}

static ssize_t bq32000_sysfs_show_tricklecharge_bypass(struct device *dev,
					       struct device_attribute *attr,
					       char *buf)
{
	int reg, error;

	error = bq32000_read(dev, &reg, BQ32000_REG_CFG2, 1);
	if (error)
		return error;

	return sprintf(buf, "%d\n", (reg & BQ32000_TCFE) ? 1 : 0);
}

static ssize_t bq32000_sysfs_store_tricklecharge_bypass(struct device *dev,
						struct device_attribute *attr,
						const char *buf, size_t count)
{
	int reg, enable, error;

	if (kstrtoint(buf, 0, &enable))
		return -EINVAL;

	error = bq32000_read(dev, &reg, BQ32000_REG_CFG2, 1);
	if (error)
		return error;

	if (enable) {
		reg |= BQ32000_TCFE;
		error = bq32000_write(dev, &reg, BQ32000_REG_CFG2, 1);
		if (error)
			return error;

		dev_info(dev, "Enabled trickle charge FET bypass.\n");
	} else {
		reg &= ~BQ32000_TCFE;
		error = bq32000_write(dev, &reg, BQ32000_REG_CFG2, 1);
		if (error)
			return error;

		dev_info(dev, "Disabled trickle charge FET bypass.\n");
	}

	return count;
}

static DEVICE_ATTR(trickle_charge_bypass, 0644,
		   bq32000_sysfs_show_tricklecharge_bypass,
		   bq32000_sysfs_store_tricklecharge_bypass);

static const struct rtc_class_ops bq32000_rtc_ops = {
    .read_time = bq32000_rtc_read_time,
    .set_time = bq32000_rtc_set_time
};

static int bq32000_sysfs_register(struct device *dev)
{
	return device_create_file(dev, &dev_attr_trickle_charge_bypass);
}

static void bq32000_sysfs_unregister(struct device *dev)
{
	device_remove_file(dev, &dev_attr_trickle_charge_bypass);
}

static int rtc_probe(struct i2c_client *client){
    struct device *dev = &client->dev;
    struct rtc_device *rtc;
    uint8_t oscillator;

    /* Check Oscillator Stop flag */
    /* 
        * Oscillator stop. 
        * The STOP bit is used to force the oscillator to stop oscillating. 
        * STOP is set to 0 on initial application of power, on all subsequent power cycles STOP remains unchanged. 
        * On initial power application STOP can be written to 1 and then written to 0 to force start the oscillator.
        * 0 Normal
        * 1 Stop
    */
    if (bq32000_read(dev, &oscillator, BQ32000_REG_SECONDS, 1)){
        return -1;
    }
    if (oscillator & BQ32000_STOP_MASK){
        /* Oscillator is stopped */
        /* Clear the STOP bit */
        oscillator = 0x00;
        if (bq32000_write(dev, &oscillator, BQ32000_REG_SECONDS, 1))
            return -1;
    }

    /* Check Oscillator Fail Flag */
    /* 
        * Oscillator fail flag. 
        * The OF bit is a latched flag indicating when the 32.768-kHz oscillator has dropped at least four consecutive pulses. 
        * The OF flag is always set on initial power-up, and it can be cleared through the serial interface. 
        * When OF is 0, no oscillator failure has been detected. 
        * When OF is 1, the oscillator fail detect circuit has detected at least four consecutive dropped pulses.
        * 0 No failure detected
        * 1 Failure detected
    */
    if (bq32000_read(dev, &oscillator, BQ32000_REG_MINUTES, 1))
        return -1;
    if (oscillator & BQ32000_OF_MASK){
        /* Oscillator has failed */
        /* Warning */
        //fprintf doesn't work in kernel space
        //fprintf(stderr, "Oscillator has failed\n");
    }

    rtc = devm_rtc_device_register(dev, rtc_driver.driver.name, &bq32000_rtc_ops, THIS_MODULE);
    i2c_set_clientdata(client, rtc);
    //fprintf doesn't work in kernel space
    //fprintf(stderr, "tiny_rtc probed\n");
    return 0;
}
static void rtc_remove(struct i2c_client *client){
    //fprintf doesn't work in kernel space
    //fprintf(stderr, "tiny_rtc removed\n");
    // temporary disable remove
    //device_remove_file(&client->dev, *);
}

static struct i2c_device_id rtc_ids[] = {
    { "bq32000", 0 },
    { }
};

MODULE_DEVICE_TABLE(i2c, rtc_ids);

static const __maybe_unused struct of_device_id bq32000_of_match[] = {
	{ .compatible = "ti,bq32000" },
	{ }
};
MODULE_DEVICE_TABLE(of, bq32000_of_match);

static struct i2c_driver rtc_driver = {
    .driver = {
        .name = "tiny_rtc",
        .of_match_table = of_match_ptr(bq32000_of_match),
    },
    .id_table = rtc_ids,
    .probe = rtc_probe,
    .remove = rtc_remove,
};

module_i2c_driver(rtc_driver);

MODULE_AUTHOR("Hyochan Gim & Hanbin Lee");
MODULE_DESCRIPTION("RTC driver for tinysystem, TI BQ32000");
MODULE_LICENSE("GPL");
