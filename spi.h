#include <stdint.h>

/*
inputs: 
  fd: an open file descriptor
  tx: pointer to byte array to be transmitted
outputs:
  rx: pointer to byte array AT LEAST AS LARGE AS tx used for received bytes
  
returns:
  status code.  
    0 -> okay; request fulfilled.
    1 -> fail.  
*/
int spiRW(int fd, unsigned int len, uint8_t* tx, uint8_t* rx);

/*
inputs:
  fd: open file descriptor
    -1 for defaults

returns:
  0-> ok
  1-> can't set spi mode
  2-> can't set clk speed
*/
int initSPI(int fd, int8_t spi_mode, int32_t clk_speed);
