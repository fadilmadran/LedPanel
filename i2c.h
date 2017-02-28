

#ifndef _I2C_H
#define _I2C_H

#include <linux/types.h>
 
#define I2C_CMD_W		0x01
#define I2C_CMD_R		0x02
#define I2C_CMD_WR		0x03

#define ERR_TIMEOUT		0x01
#define ERR_SLAVEADDR	0x02
#define ERR_BYTECOUNT	0x03
#define ERR_OUTOFMEM	0x04
#define ERR_OPENDEV		0x05
#define ERR_BUSERR		0x06
#define ERR_BUSWR		0x07
#define ERR_BUSREAD		0x08
 
__u8 i2c_command(__u8 i2c_cmd,__u8 i2c_addr,int numofsend, int numofreceive,__u8 *send_buf,__u8 *receive_buf);
 
#endif


