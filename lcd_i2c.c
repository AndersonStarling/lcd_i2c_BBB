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
#include <linux/string.h>

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


#define LCD_ADDRESS (client_data_ptr->addr)
#define LCD_CMD_CLEAR_DISPLAY 0x01U
#define LCD_CMD_RETURN_HOME   0x02U


/**************************************************************************************** 
 * Variable
 ****************************************************************************************/
/* Register device match tabble */
static const struct of_device_id lcd_i2c_of_match[] = {
    {.compatible = "lcd_16x2"},
    {},
};


static struct i2c_client *client_data_ptr = NULL;

/**************************************************************************************** 
 * Function implementation
 ****************************************************************************************/
static ssize_t lcd_i2c_write(struct file *file, const char __user *buf, size_t len, loff_t *pos)
{
    u8 *buffer_device = NULL;
    int ret = -1;

    buffer_device = kmalloc(sizeof(u8), len + 1);

    /* clear lcd display and return home */
    lcd_i2c_send_cmd(LCD_CMD_RETURN_HOME, LCD_ADDRESS);
    mdelay(2);
    lcd_i2c_send_cmd(LCD_CMD_CLEAR_DISPLAY, LCD_ADDRESS);

    ret = copy_from_user(buffer_device, buf, len + 1);
    if(0 == ret)
    {
        pr_info("buffer_device = %s\n", buffer_device);
        lcd_send_string(buffer_device, client_data_ptr->addr);
    }
    else
    {
        pr_info("%s %d failed\n", __func__, __LINE__);
    }

    kfree(buffer_device);

    return 1;
}

static int lcd_i2c_open(struct inode *inode, struct file *file)
{

    pr_info("%s %d\n", __func__, __LINE__);

    return 0;
}

static int lcd_i2c_release(struct inode *inodep, struct file *filp)
{
    pr_info("%s %d\n", __func__, __LINE__);

    return 0;
}


static ssize_t lcd_i2c_ping(struct file *file, char __user *buf, size_t len, loff_t *pos)
{
    int ret = -1;

    pr_info("%s %d\n", __func__, __LINE__);

    ret = lcd_i2c_send_cmd(0x02, client_data_ptr->addr);
    if(ret < 0)
    {
        pr_info("Ping device failed\n");
    }

    return 0;
}


u8 lcd_i2c_send_cmd(u8 command, u32 lcd_address)
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

    messages.addr = lcd_address;
    messages.flags = client_data_ptr->flags;
    messages.len = 4;
    messages.buf = &data_send[0];

    ret = i2c_transfer(client_data_ptr->adapter, &messages, 1);
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

u8 lcd_i2c_send_data(u8 data, u32 lcd_address)
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

    messages.addr = lcd_address;
    messages.flags = client_data_ptr->flags;
    messages.len = 4;
    messages.buf = &data_send[0];

    ret = i2c_transfer(client_data_ptr->adapter, &messages, 1);
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

static void lcd_send_string(u8 *string, u32 lcd_address)
{
    u32 index = 0;

    for(index = 0; index < strlen(string); index ++)
    {
        lcd_i2c_send_data(string[index], lcd_address);
    }
}

u8 lcd_init(u32 lcd_address)
{
    u8 ret = 0;

    pr_info("lcd_address = %d\n", lcd_address);

    /* delay 50ms */
    mdelay(50);
    ret |= lcd_i2c_send_cmd(0x30, lcd_address);
    mdelay(5);
    ret |= lcd_i2c_send_cmd(0x30, lcd_address);
    mdelay(1);
    ret |= lcd_i2c_send_cmd(0x30, lcd_address);
    mdelay(10);
    ret |= lcd_i2c_send_cmd(0x20, lcd_address);
    mdelay(10);
    ret |= lcd_i2c_send_cmd(0x28, lcd_address);
    mdelay(1);
    ret |= lcd_i2c_send_cmd(0x08, lcd_address);
    mdelay(1);
    ret |= lcd_i2c_send_cmd(0x01, lcd_address);
    mdelay(2);
    ret |= lcd_i2c_send_cmd(0x06, lcd_address);
    mdelay(1);
    ret |= lcd_i2c_send_cmd(0x0C, lcd_address);

    lcd_send_string("Hi", lcd_address);

    return ret;
}

int lcd_i2c_driver_probe(struct i2c_client *client)
{
    int ret = -1;

    pr_info("Initialize lcd ...\n");

    client_data_ptr = client;
    pr_info("client_data_ptr->flag = %d\n", client_data_ptr->flags);

    pr_info("Initialize lcd ...\n");
    ret = lcd_init(client->addr);
    if(ret != 0)
    {
        pr_info("%s %d lcd_init failed\n");
    }
    else
    {
        pr_info("%s %d lcd_init successfully\n", __func__, __LINE__);
    }

    return 0;
}

int lcd_i2c_driver_remove(struct i2c_client *client)
{
    pr_info("%s %d\n", __func__, __LINE__);

<<<<<<< HEAD
    if(client_data_ptr != NULL)
    {
        kfree(client_data_ptr);
    }
=======
    lcd_i2c_send_cmd(LCD_CMD_CLEAR_DISPLAY, LCD_ADDRESS);
    misc_deregister(&lcd_i2c_device);
>>>>>>> bec7cd8... Update app and lcd_i2c kernel

    return 0;
}

static struct i2c_driver lcd_i2c_driver = {
    .probe_new = lcd_i2c_driver_probe,
    .remove = lcd_i2c_driver_remove,
    .driver = {
        .name = "lcd_i2c",
        .of_match_table = lcd_i2c_of_match,
    }
};

module_i2c_driver(lcd_i2c_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Blink Led kernel module");








































