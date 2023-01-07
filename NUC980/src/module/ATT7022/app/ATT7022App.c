#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h> 


/* 定义设备类型 */
#define IOC_MAGIC  'c'

/* 初始化设备 */
#define IOCINIT    _IO(IOC_MAGIC, 0)

/* 写寄存器 */
#define IOCWREG(reg)    _IOW(IOC_MAGIC, reg, int)  /* 1 位置填写  寄存器地址 */

/* 读寄存器 */
#define IOCRREG(reg)    _IOR(IOC_MAGIC, reg, int)

#define r_URmsa 0x0D /* A Phase Voltage */  /* 寄存器地址去查手册 */
#define r_URmsb 0x0E /* B Phase Voltage */
#define r_URmsc 0x0F /* C Phase Voltage */
#define r_ID 0x5D   /* 7022 ID */
/*
 * @description		: main主程序
 * @param - argc 	: argv数组元素个数
 * @param - argv 	: 具体参数
 * @return 			: 0 成功;其他 失败
 */
int main(int argc, char *argv[])
{
	int fd;
	char *filename;
	unsigned char databuf[10];


	int ret = 0;
	unsigned int value;

	if (argc != 2) {
		printf("Error Usage!\r\n");
		return -1;
	}

	filename = argv[1];
	fd = open(filename, O_RDWR);
	if(fd < 0) {
		printf("can't open file %s\r\n", filename);
		return -1;
	}
	/* read 和ioctl 二选一  不建议用read */
	ret = read(fd, databuf, sizeof(databuf));
	if(ret == 0) { 			/* 数据读取成功 */
		//printf("%d %d %d %d %d %d %d %d %d \n",databuf[0],databuf[1],databuf[2],databuf[3],databuf[4],databuf[5],databuf[6],databuf[7],databuf[8]);
		printf("\r\nread A value:0x%x volt %d:\r\n",((databuf[0]<<0)|(databuf[1]<<8)|(databuf[2]<<16)),(((databuf[0]<<0)|(databuf[1]<<8)|(databuf[2]<<16))/8192));
		printf("\r\nread B value:0x%x volt %d:\r\n",((databuf[3]<<0)|(databuf[4]<<8)|(databuf[5]<<16)),(((databuf[3]<<0)|(databuf[4]<<8)|(databuf[5]<<16))/8192));
		printf("\r\nread C value:0x%x volt %d:\r\n",((databuf[6]<<0)|(databuf[7]<<8)|(databuf[8]<<16)),(((databuf[6]<<0)|(databuf[7]<<8)|(databuf[8]<<16))/8192));
	}
	/* 获取芯片ID */
	ret = ioctl(fd, IOCRREG( r_ID ) , &value);/* 读寄存器地址 */
    if (ret) {
        perror("ioctl read:");
        exit(-4);
    }else
    {
    	printf("\r\nioctl read r_ID value:0x%x \r\n",value);
    }
	/* 读取电压 */
	ret = ioctl(fd, IOCRREG(r_URmsa) , &value);/* 读寄存器地址 */
    if (ret) {
        perror("ioctl read:");
        exit(-4);
    }else
    {
    	printf("\r\nioctl read A value:0x%x volt %f:\r\n",value,(float)(value/8192));
    }
	ret = ioctl(fd, IOCRREG(r_URmsb) , &value);/* 读寄存器地址 */
    if (ret) {
        perror("ioctl read:");
        exit(-4);
    }else
    {
    	printf("\r\nioctl read B value:0x%x volt %f:\r\n",value,(float)(value/8192));
    }
	ret = ioctl(fd, IOCRREG(r_URmsc) , &value);/* 读寄存器地址 */
    if (ret) {
        perror("ioctl read:");
        exit(-4);
    }else
    {
    	printf("\r\nioctl read C value:0x%x volt %f:\r\n",value,(float)(value/8192));
    }
    
	/* 写寄存器 基本不用一般配置芯片使用*/ 
	value = 0x0000;
	ret = ioctl(fd, IOCWREG(0xC9) , &value);/* 读寄存器地址 */
	if (ret) {
	   perror("ioctl read:");
	    exit(-4);
	}else
	{
		printf("\r\nioctl write 0xC9 ok:\r\n");
	}
	close(fd);	/* 关闭文件 */	
	return 0;
}

