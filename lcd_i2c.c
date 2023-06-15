/**************************************************************************************** 
 * Include Library
 ****************************************************************************************/
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/time.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/delay.h>

/**************************************************************************************** 
 * Definition
 ****************************************************************************************/
#define P0 (1u << 0)
#define P1 (1u << 1)
#define P2 (1u << 2)
#define P3 (1u << 3)
#define P4 (1u << 4)
#define P5 (1u << 5)
#define P6 (1u << 6)
#define P7 (1u << 7)

#define LCD_I2C_RS  P0
#define LCD_I2C_RW  P1
#define LCD_I2C_E   P2
#define LCD_I2C_BLK P3
#define LCD_I2C_D4  P4
#define LCD_I2C_D5  P5
#define LCD_I2C_D6  P6
#define LCD_I2C_D7  P7

// typedef struct
// {
//     struct i2c_client *client;
//     u8 command;
// } lcd_i2c_struct_t;




/**************************************************************************************** 
 * Variable
 ****************************************************************************************/




/* Register device match tabble */
static const struct of_device_id lcd_i2c_of_match[] = {
    {.compatible = "lcd_16x2"},
    {},
};

/**************************************************************************************** 
 * Function implementation
 ****************************************************************************************/

u8 lcd_i2c_send_cmd(u8 command, struct i2c_client *client)
{
    u8 data_upper = 0;
    u8 data_lower = 0;
    u8 data_send[4] = {0};
    u8 ret = -1;
    struct i2c_msg messages;

    /* extract 4 upper bit from command */
    data_upper = ((command & 0xF0) & (~LCD_I2C_RW));
    /*extract 4 lower bit from command */
    data_lower = (((command << 4) & 0xF0) & (~LCD_I2C_RW));

    /* Send 4 upper bit command to lcd */
    data_send[0] = data_upper | (LCD_I2C_BLK | LCD_I2C_E);
    /* Create dummy frame because LCD need delay between two intructions */
    data_send[1] = data_upper | (LCD_I2C_BLK);

    /* Send 4 upper bit command to lcd */
    data_send[2] = data_lower | (LCD_I2C_BLK | LCD_I2C_E);
    /* Create dummy frame because LCD need delay between two intructions */
    data_send[3] = data_lower | (LCD_I2C_BLK);

    pr_info("data[0] = %d\n", data_send[0]);
    pr_info("data[1] = %d\n", data_send[1]);
    pr_info("data[2] = %d\n", data_send[2]);
    pr_info("data[3] = %d\n", data_send[3]);

    messages.addr = client->addr;
    messages.flags = client->flags;
    messages.len = 4;
    messages.buf = &data_send[0];

    ret = i2c_transfer(client->adapter, &messages, 1);
    if(ret < 0)
    {
        pr_info("%s %d tranfer failed\n", __func__, __LINE__);
        return -1;
    }
    else if(ret != 1)
    {
        pr_info("%s %d ack failed\n", __func__, __LINE__);
        return -1;
    }
    else
    {
        /**/
    }

    return 0;
}

u8 lcd_i2c_send_test(u8 data, struct i2c_client *client)
{
    struct i2c_msg messages;
    u8 ret = -1;

    messages.addr = client->addr;
    messages.flags = client->flags;
    messages.len = 1;
    messages.buf = &data;

    pr_info("%s %d client->flags = %d\n", __func__, __LINE__, client->flags);
    pr_info("%s %d messages.buf = %d\n", __func__, __LINE__, *messages.buf);

    ret = i2c_transfer(client->adapter, &messages, 1);
    if(ret < 0)
    {
        pr_info("%s %d tranfer failed\n", __func__, __LINE__);
        return -1;
    }
    else if(ret != 1)
    {
        pr_info("%s %d ack failed\n", __func__, __LINE__);
        return -1;
    }
    else
    {
        /**/
    }

    return 0;
}

u8 lcd_i2c_send_data(u8 data, struct i2c_client *client)
{
    u8 data_upper = 0;
    u8 data_lower = 0;
    u8 data_send[4] = {0};
    u8 ret = -1;
    struct i2c_msg messages;

    /* extract 4 upper bit from command */
    data_upper = ((data & 0xF0) & (~LCD_I2C_RW));
    /*extract 4 lower bit from command */
    data_lower = (((data << 4) & 0xF0) & (~LCD_I2C_RW));

    /* Send 4 upper bit command to lcd */
    data_send[0] = data_upper | (LCD_I2C_BLK | LCD_I2C_E | LCD_I2C_RS);
    /* Create dummy frame because LCD need delay between two intructions */
    data_send[1] = data_upper | (LCD_I2C_BLK | LCD_I2C_RS);

    /* Send 4 upper bit command to lcd */
    data_send[2] = data_lower | (LCD_I2C_BLK | LCD_I2C_E | LCD_I2C_RS);
    /* Create dummy frame because LCD need delay between two intructions */
    data_send[3] = data_lower | (LCD_I2C_BLK | LCD_I2C_RS);

    messages.addr = client->addr;
    messages.flags = client->flags;
    messages.len = 4;
    messages.buf = &data_send[0];

    ret = i2c_transfer(client->adapter, &messages, 1);
    if(ret < 0)
    {
        pr_info("%s %d tranfer failed\n", __func__, __LINE__);
        return -1;
    }
    else if(ret != 1)
    {
        pr_info("%s %d ack failed\n", __func__, __LINE__);
        return -1;
    }
    else
    {
        /**/
    }

    return 0;
}

u8 lcd_init(struct i2c_client *client)
{
    u8 ret = 0;

    /* delay 50ms */
    mdelay(50);
    ret |= lcd_i2c_send_cmd(0x30, client);
    mdelay(5);
    ret |= lcd_i2c_send_cmd(0x30, client);
    mdelay(1);
    ret |= lcd_i2c_send_cmd(0x30, client);
    mdelay(10);
    ret |= lcd_i2c_send_cmd(0x20, client);
    mdelay(10);
    ret |= lcd_i2c_send_cmd(0x28, client);
    mdelay(1);
    ret |= lcd_i2c_send_cmd(0x08, client);
    mdelay(1);
    ret |= lcd_i2c_send_cmd(0x01, client);
    mdelay(2);
    ret |= lcd_i2c_send_cmd(0x06, client);
    mdelay(1);
    ret |= lcd_i2c_send_cmd(0x0C, client);

    ret |= lcd_i2c_send_data('H', client);
    pr_info("%s %d ret = %d\n", __func__, __LINE__, ret);
    ret |= lcd_i2c_send_data('E', client);
    pr_info("%s %d ret = %d\n", __func__, __LINE__, ret);
    ret |= lcd_i2c_send_data('L', client);
    pr_info("%s %d ret = %d\n", __func__, __LINE__, ret);
    ret |= lcd_i2c_send_data('L', client);
    pr_info("%s %d ret = %d\n", __func__, __LINE__, ret);
    ret |= lcd_i2c_send_data('O', client);
    pr_info("%s %d ret = %d\n", __func__, __LINE__, ret);

    return ret;
}

int lcd_i2c_driver_probe(struct i2c_client *client)
{
    u8 dummy_data = 0;
    int ret = -1;

    struct i2c_msg messages =
    {
        .addr = client->addr,
        .flags = 0,
        .buf = &dummy_data,
        .len = 1
    };

    pr_info("%s %d\n", __func__, __LINE__);
    pr_info("client->addr = %d\n", client->addr);

    pr_info("Pinging Lcd device ...\n");

    if(NULL == client->adapter)
    {
        pr_info("client->adapter is NULL");
        return -1;
    }

    // ret = i2c_transfer(client->adapter, &messages, 1);

    // /*ret = i2c_master_send(client, &dummy_data, 0);*/
    // if(ret < 0)
    // {
    //     pr_info("i2c tranfer failed\n");
    //     return -1;
    // }
    // else if(ret != 1)
    // {
    //     pr_info("i2c ack failed\n");
    //     return -1;
    // }
    // else
    // {
    //     pr_info("Ping device successfully\n");
    // }

    // pr_info("Initialize lcd ...\n");
    ret = lcd_init(client);
    if(ret != 0)
    {
        pr_info("%s %d lcd_init failed\n");
    }
    else
    {
        pr_info("%s %d lcd_init successfully\n", __func__, __LINE__);
    }

    // ret = lcd_i2c_send_test(0xff, client);
    // pr_info("%s %d ret = %d\n", __func__, __LINE__, ret);

    return 0;
}

int lcd_i2c_driver_remove(struct i2c_client *client)
{
    pr_info("%s %d\n", __func__, __LINE__);

    return 0;
}

#if 0
struct i2c_device_id lcd_i2c_device_id = 
{
	.name = "lcd_16x2",
	.driver_data = 1	/* Data private to the driver */
};
#endif

static struct i2c_driver lcd_i2c_driver = {
    .probe_new = lcd_i2c_driver_probe,
    .remove = lcd_i2c_driver_remove,
    /*.id_table = &lcd_i2c_device_id,*/
    .driver = {
        .name = "lcd_i2c",
        .of_match_table = lcd_i2c_of_match,
    }
};

module_i2c_driver(lcd_i2c_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Blink Led kernel module");








































