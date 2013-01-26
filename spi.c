#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

static uint8_t mode = 3;
static uint32_t speed = 1000000;
static uint16_t delay = 0;

/*
inputs: 
  fd: an open file descriptor
  len: lenth of message
  tx: pointer to byte array to be transmitted
outputs:
  rx: pointer to byte array AT LEAST AS LARGE AS tx used for received bytes
  
RETURNS:
  status code.  
    0 -> okay; request fulfilled.
    1 -> fail.  
*/
int spiRW(int fd, unsigned int len,uint8_t *tx, uint8_t *rx)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (uintptr_t)tx,
		.rx_buf = (uintptr_t)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = 0,
		.bits_per_word = 0,
		.cs_change = 0,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 0) //CONTRARY to kernel doc example ret is num bytes sent
		return -1; //can't send spi message

	return 0;
}

/*
inputs:
  fd: open file descriptor
    -1 for defaults

Returns:
  0-> ok
  1-> can't set spi mode
  2-> can't set clk speed
*/
int initSPI(int fd, int8_t spi_mode, int32_t clk_speed){
	int ret;
	if (spi_mode>=0) {
		mode = (uint8_t)spi_mode;
	}
	if (clk_speed > 0) {
		speed = (uint32_t)clk_speed;
	} 

	//set SPI mode
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
        return 1;
	//set SPI maximum speed in Hz
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
        return 2;

    return 0;
}

