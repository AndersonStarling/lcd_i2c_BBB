/**************************************************************************************** 
 * Include Library
 ****************************************************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

/**************************************************************************************** 
 * Definition
 ****************************************************************************************/
#define LCD_FILE "/dev/lcd_i2c_1602"


/**************************************************************************************** 
 * Function prototype
 ****************************************************************************************/

/**************************************************************************************** 
 * Variable
 ****************************************************************************************/

/**************************************************************************************** 
 * Function implementation
 ****************************************************************************************/
int main(void)
{
    int fd = -1;

    fd = open(LCD_FILE, O_RDWR);
    if(fd < 0)
    {
        printf("Open file failed\n");
        return -1;
    }

    write(fd, "Hello", strlen("Hello"));
    sleep(1);
    write(fd, "How are you", strlen("How are you"));
    sleep(1);
    write(fd, "I am fine", strlen("I am fine"));

    return 0;
}
