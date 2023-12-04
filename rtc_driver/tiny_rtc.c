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
#define BQ32000_TCH2_MASK 0x20
#define BQ32000_TCFE_MASK 0x40
#define BQ32000_TCHE_MASK 0x0F
#define BQ32000_FTF_MASK 0x01

#define I2C_M_WR 0

static struct i2c_driver rtc_driver;

static int bq32000_read(struct device *dev, uint8_t *buf, uint8_t subaddress, int count){
    //suggested
    
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
static int bq32000_write(struct device *dev, uint8_t *buf, uint8_t subaddress, int count){
    //suggested
    struct i2c_client *client = to_i2c_client(dev);
    struct i2c_msg msg;
    int ret;

    msg.addr = client->addr;
    msg.flags = I2C_M_WR;
    msg.buf = buf;
    msg.len = count + 1;

    ret = i2c_transfer(client->adapter, &msg, 1);
    if (ret != 1)
        return -1;
    return 0;
}
static int bq32000_rtc_read_time(struct device *dev, struct rtc_time *tm){
    // to do implement
    return 0;
}
static int bq32000_rtc_set_time(struct device *dev, struct rtc_time *tm){
    // to do implement
    return 0;
}
static const struct rtc_class_ops bq32000_rtc_ops = {
    .read_time = bq32000_rtc_read_time,
    .set_time = bq32000_rtc_set_time
};
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
    { "tiny_rtc", 0 },
    { }
};

MODULE_DEVICE_TABLE(i2c, rtc_ids);

static struct i2c_driver rtc_driver = {
    .driver = {
        .name = "tiny_rtc",
        .owner = THIS_MODULE,
    },
    .id_table = rtc_ids,
    .probe = rtc_probe,
    .remove = rtc_remove,
};

module_i2c_driver(rtc_driver);

MODULE_AUTHOR("Hyochan Gim & Hanbin Lee");
MODULE_DESCRIPTION("RTC driver for tinysystem, TI BQ32000");
MODULE_LICENSE("GPL");
