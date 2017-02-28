
 
#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>
#include "hexdump.h"
#include "i2c.h"

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


__u8 i2c_command(__u8 i2c_cmd,__u8 i2c_addr, int numofsend, int numofreceive,__u8 *send_buf,__u8 *receive_buf)
{

		int file;
		char device[] = "/dev/i2c-0";


		if(i2c_cmd==I2C_CMD_W)  // I2c Write Cmd
		{
							
			if(i2c_addr < 0x03 || i2c_addr > 0x77)
			{
				fprintf(stdout, "Slave address out of range! [0x03..0x77]\n");
				return ERR_SLAVEADDR;
			}
			
//			fprintf(stdout, "SLAVE ADDRESS: 0x%02X\n", (unsigned int) i2c_addr);
//			fflush(stdout);
			
			if(numofsend > 255 || numofsend < 1)
			{
				fprintf(stdout, "Invalid number of bytes to write\n");
				return ERR_BYTECOUNT;
			}
			
//			fprintf(stdout, "NUMBER OF BYTES TO WRITE: %d\n", (unsigned int) numofsend);
//			fflush(stdout);
			
		
			if(send_buf == NULL)
			{
				fprintf(stdout, "Out of memory\n");
				return ERR_OUTOFMEM;
			}
			
//			fprintf(stdout, "MEMORY ALLOCATED AT ADDRESS: 0x%X\n", uint(send_buf));
//			fflush(stdout);
									
			
			if ((file = open(device, O_RDWR)) < 0)
			{
				fprintf(stdout, "%s --> ", device);
				perror("Failed to open control file");
				return ERR_OPENDEV;
			}
			
			//fprintf(stdout, "%s OPENED!\n", device);
			fflush(stdout);
			
			if(ioctl(file, I2C_SLAVE, i2c_addr) < 0)
			{
				perror("Failed to access the bus");
				close(file);
				return  ERR_BUSERR;
			}
			
			int iRslt = write(file, send_buf, numofsend);
			if(iRslt != numofsend)
			{
				perror("Failed writing to the I2C-bus");
				close(file);
				printf("Write response:%d\n", iRslt);
				return ERR_BUSWR;
			}
			else
			{
				//printf("WRITE:SUCCESS\n");
				//printf("Write response:%d\n", iRslt);
			}

			close(file);
			return 0;	
		}
		else if(i2c_cmd==I2C_CMD_R) 
		{
				
			if(i2c_addr < 0x03 || i2c_addr > 0x77)
			{
				fprintf(stdout, "Slave address out of range! [0x03..0x77]\n");
				return ERR_SLAVEADDR;
			}

			fprintf(stdout, "SLAVE ADDRESS: 0x%02X\n", (unsigned int) i2c_addr);
			fflush(stdout);

			if(numofreceive > 255 || numofreceive < 1)
			{
				fprintf(stdout, "Invalid number of bytes to read\n");
				return ERR_BYTECOUNT;
			}

			fprintf(stdout, "NUMBER OF BYTES TO READ: %d\n", (unsigned int) numofreceive);
			fflush(stdout);


			if(receive_buf == NULL)
			{
				fprintf(stdout, "Out of memory\n");
				return ERR_OUTOFMEM;
			}


			//fprintf(stdout, "MEMORY ALLOCATED AT ADDRESS: 0x%X\n", receive_buf);
			//fflush(stdout);

			if ((file = open(device, O_RDWR)) < 0)
			{
				fprintf(stdout, "%s --> ", device);
				perror("Failed to open control file");
				return ERR_OPENDEV;
			}

//			fprintf(stdout, "%s OPENED!\n", device);
//			fflush(stdout);


			if(ioctl(file, I2C_SLAVE, i2c_addr) < 0)
			{
				perror("Failed to access the bus");
				close(file);

				return  ERR_BUSERR;
			}

			if(read(file, receive_buf, numofreceive) != numofreceive)
			{
				perror("Failed reading from the I2C-bus");
				close(file);
				return ERR_BUSREAD;
			}
			else
			{
				printf("Received Data:\n");
				hexdump(receive_buf,numofreceive);
			}
					
			close(file);	
			return 0;
		}
		else if(i2c_cmd==I2C_CMD_WR)
		{

			fprintf(stdout, "Num Of Send / Receive 0x%02X/0x%02X\n", (unsigned int) numofsend,(unsigned int) numofreceive);
			fflush(stdout);

			
			if(i2c_addr < 0x03 || i2c_addr > 0x77)
			{
				fprintf(stdout, "Slave address out of range! [0x03..0x77]\n");
				return ERR_SLAVEADDR;
			}

			fprintf(stdout, "SLAVE ADDRESS: 0x%02X\n", (unsigned int) i2c_addr);
			fflush(stdout);
			
			
			if(!numofsend)
			{
				struct i2c_rdwr_ioctl_data packets;
				struct i2c_msg messages[1];
				int user;
				unsigned long funcs;
				
				if(numofsend > 255 || numofsend < 1)
				{
					fprintf(stdout, "Invalid number of bytes to write\n");
					return ERR_BYTECOUNT;
				}
				
				fprintf(stdout, "NUMBER OF BYTES TO READ: %d\n", (unsigned int) numofreceive);
				fflush(stdout);
				fprintf(stdout, "NUMBER OF BYTES TO WRITE: %d\n", (unsigned int) numofsend);
				fflush(stdout);
				
				if ((file = open(device, O_RDWR)) < 0)
				{
					fprintf(stdout, "%s --> ", device);
					perror("Failed to open control file");
					return ERR_OPENDEV;
				}
				
				fprintf(stdout, "%s OPENED!\n", device);
				fflush(stdout);	

				user = ioctl(file,I2C_FUNCS, &funcs);
				fprintf(stdout, "FUNCTIONALITY: 0x%X\n", (unsigned int)funcs);
		
				if(send_buf == NULL)
				{
					fprintf(stdout, "Out of memory\n");
					close(file);
					return ERR_OUTOFMEM;
				}
				//fprintf(stdout, "MEMORY ALLOCATED AT ADDRESS: 0x%X\n", send_buf);
				//fflush(stdout);
			

				messages[0].addr  = i2c_addr;
				messages[0].flags = 0;
				messages[0].len   = numofsend;
				messages[0].buf   = send_buf;	

				packets.msgs  = messages;
				packets.nmsgs = 1;
				
				fprintf(stdout, "\nPACKET DATA: \
								\n-------------------- \
								\nADDRESS: 0x%02X \
								\nFLAG: 0x%02X \
								\nLENGHT: 0x%02X \
								\nMESSAGES: %d\n",
								(unsigned int)i2c_addr, 
								(unsigned int)messages[0].flags,
								(unsigned int)numofsend,
								packets.nmsgs);
				fflush(stdout);
				
				if(ioctl(file, I2C_RDWR, &packets) < 0)
				{
					perror("Error sending data");
					close(file);
					return ERR_BUSWR;
				}			
				
				printf("SENDING:DONE\n");
				
				close(file);
				return 0;
				
			}
			else
			{
				struct i2c_rdwr_ioctl_data packets;
				struct i2c_msg messages[2];
				
				if(numofsend > 255 || numofsend < 1)
				{
					fprintf(stdout, "Invalid number of bytes to write\n");
					return ERR_BYTECOUNT;
				}
				
				
				if(numofreceive > 255)
				{
					fprintf(stdout, "Invalid number of bytes to read\n");
					return ERR_BYTECOUNT;
				}
				
				fprintf(stdout, "NUMBER OF BYTES TO READ: %d\n", (unsigned int) numofreceive);
				fflush(stdout);
				fprintf(stdout, "NUMBER OF BYTES TO WRITE: %d\n", (unsigned int) numofsend);
				fflush(stdout);
				
				if ((file = open(device, O_RDWR)) < 0)
				{
					fprintf(stdout, "%s --> ", device);
					perror("Failed to open control file");
					return ERR_OPENDEV;
				}
				fprintf(stdout, "%s OPENED!\n", device);
				fflush(stdout);	

							
				if(receive_buf == NULL || send_buf == NULL)
				{
					fprintf(stdout, "Out of memory\n");
					close(file);
					return ERR_OUTOFMEM;
				}
				
				bzero(receive_buf,numofreceive);
				
				/*fprintf(stdout, "MEMORY ALLOCATED AT ADDRESS: 0x%X , 0x%X \n", receive_buf,numofsend);
				fflush(stdout);	
				fprintf(stdout, "MEMORY ALLOCATED AT ADDRESS: 0x%X , 0x%X \n", send_buf,numofreceive);
				fflush(stdout);	*/
			

				messages[0].addr  = i2c_addr;
				messages[0].flags = 0;
				messages[0].len   = numofsend;
				messages[0].buf   = send_buf; 
				
				
				messages[1].addr  = i2c_addr;
				messages[1].flags = I2C_M_RD;
				messages[1].len   = numofreceive;
				messages[1].buf   = receive_buf;
				
				
				packets.msgs  = messages;
				packets.nmsgs = 2;
				
				fprintf(stdout, "\nPACKET DATA: \
								\n-------------------- \
								\nADDRESS: 0x%02X \
								\nFLAG: 0x%02X \
								\nLENGHT: 0x%02X \
								\n\nADDRESS: 0x%02X \
								\nFLAG: 0x%02X \
								\nLENGHT: 0x%02X \
								\nMESSAGES: %d\n",
								(unsigned int)messages[0].addr,
								(unsigned int)messages[0].flags,
								(unsigned int)sizeof(receive_buf),
								(unsigned int)messages[1].addr,
								(unsigned int)messages[1].flags,
								(unsigned int)sizeof(send_buf),
								packets.nmsgs);
				fflush(stdout);
				
				if(ioctl(file, I2C_RDWR, &packets) < 0)
				{
					perror("Error sending data");
					close(file);
					return ERR_BUSWR;
				}	
				printf("SENDING:DONE\n");
				
				printf("Receive Data:\n");
				hexdump(receive_buf,numofreceive);

				close(file);
				return 0;
				
			}
			
		}

		return 0;
}
