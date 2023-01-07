#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "att7022_reg.h"


#define ATT7022_CNT	1
#define ATT7022_NAME	"att7022"

struct att7022_dev {
	dev_t devid;				/* 设备号 	 */
	struct cdev cdev;			/* cdev 	*/
	struct class *class;		/* 类 		*/
	struct device *device;		/* 设备 	 */
	struct device_node	*nd; 	/* 设备节点 */
	int major;					/* 主设备号 */
	void *private_data;			/* 私有数据 		*/
	//int cs_gpio;				/* 片选所使用的GPIO编号		*/
	unsigned int Volt_A;
	unsigned int Volt_B;
	unsigned int Volt_C;
	struct mutex lock; /* 互斥体 */
};

static struct att7022_dev att7022dev;

/*
 * @description	: 从att7022读取多个寄存器数据
 * @param - dev:  att7022设备
 * @param - reg:  要读取的寄存器首地址
 * @param - val:  读取到的数据

 * @return 		: 操作结果
 */
static int att7022_read_regs(struct att7022_dev *dev, u8 reg, unsigned int *pbuf)
{
	int ret;
	unsigned char txdata[4]={0xff};
	unsigned char rxdata[4]={0x00};
	struct spi_message m;
	struct spi_transfer *t;
	struct spi_device *spi = (struct spi_device *)dev->private_data;

	//gpio_set_value(dev->cs_gpio, 0);				/* 片选拉低，选中att7022 */
	t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);	/* 申请内存 */
	txdata[0]= 	reg;			/* 要发送的数据 */	
	t->tx_buf = txdata;			/* 要发送的数据 */	
	t->rx_buf = rxdata;			/* 读取到的数据 */
	t->len = 0x04;				/* 要读取的数据长度 */
	spi_message_init(&m);		/* 初始化spi_message */
	spi_message_add_tail(t, &m);/* 将spi_transfer添加到spi_message队列 */
	ret = spi_sync(spi, &m);	/* 同步发送 */

	kfree(t);									/* 释放内存 */
	//gpio_set_value(dev->cs_gpio, 1);			/* 片选拉高，释放att7022 */
	*pbuf = 0x00;
	*pbuf =(unsigned int)((rxdata[1]<<16)|(rxdata[2]<<8)|(rxdata[3]));
	return ret;
}

/*
 * @description	: 向att7022多个寄存器写入数据
 * @param - dev:  att7022设备
 * @param - reg:  要写入的寄存器首地址
 * @param - val:  要写入的数据缓冲区
 * @return 	  :   操作结果
 */
static s32 att7022_write_regs(struct att7022_dev *dev, u8 reg, unsigned int buf)
{
	int ret;

	unsigned char txdata[4];
	struct spi_message m;
	struct spi_transfer *t;
	struct spi_device *spi = (struct spi_device *)dev->private_data;

	t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);	/* 申请内存 */
	//gpio_set_value(dev->cs_gpio, 0);			/* 片选拉低 */
	
	txdata[0]= reg;
	txdata[1]=((unsigned char )((buf>>16)&0xff));
	txdata[2]=((unsigned char )((buf>>8)&0xff));	
	txdata[3]=((unsigned char )(buf&0xff));
	
	t->tx_buf = txdata;			/* 要写入的数据 */
	t->len = 4;				    /* 写入的字节数 */
	spi_message_init(&m);		/* 初始化spi_message */
	spi_message_add_tail(t, &m);/* 将spi_transfer添加到spi_message队列 */
	ret = spi_sync(spi, &m);	/* 同步发送 */

	kfree(t);					/* 释放内存 */
	//gpio_set_value(dev->cs_gpio, 1);/* 片选拉高，释放att7022 */

	return ret;
}

/*
 * @description	: 读取att7022指定寄存器值，读取一个寄存器
 * @param - dev:  att7022设备
 * @param - reg:  要读取的寄存器
 * @return 	  :   读取到的寄存器值
 */
static unsigned int att7022_read_onereg(struct att7022_dev *dev, u8 reg)
{
	unsigned int  data = 0;
	att7022_read_regs(dev, reg, &data);
	return data;
}

/*
 * @description	: 向att7022指定寄存器写入指定的值，写一个寄存器
 * @param - dev:  att7022设备
 * @param - reg:  要写的寄存器
 * @param - data: 要写入的值
 * @return   :    无
 */	

static void att7022_write_onereg(struct att7022_dev *dev, u8 reg, unsigned int value)
{
	unsigned int buf = value;
	att7022_write_regs(dev, reg, buf); /* 命令数据固定三个字节 */
}

/*
 * @description	: 读取att7022的数据，读取原始数据，包括三轴陀螺仪、
 * 				: 三轴加速度计和内部温度。
 * @param - dev	: att7022设备
 * @return 		: 无。
 */
void att7022_readdata_volt(struct att7022_dev *dev)
{	
	unsigned int value;
	value = att7022_read_onereg(dev, r_URmsa);
	dev-> Volt_A = value;
	value = att7022_read_onereg(dev, r_URmsb);
	dev-> Volt_B = value;
	value = att7022_read_onereg(dev, r_URmsc);
	dev-> Volt_C = value;	
}

/*
 * @description		: 打开设备
 * @param - inode 	: 传递给驱动的inode
 * @param - filp 	: 设备文件，file结构体有个叫做pr似有ate_data的成员变量
 * 					  一般在open的时候将private_data似有向设备结构体。
 * @return 			: 0 成功;其他 失败
 */
static int att7022_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &att7022dev; /* 设置私有数据 */
	return 0;
}

/*
 * @description		: 从设备读取数据 
 * @param - filp 	: 要打开的设备文件(文件描述符)
 * @param - buf 	: 返回给用户空间的数据缓冲区
 * @param - cnt 	: 要读取的数据长度
 * @param - offt 	: 相对于文件首地址的偏移
 * @return 			: 读取的字节数，如果为负值，表示读取失败
 */
static ssize_t att7022_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
	unsigned char data[9];
	long err = 0;
	struct att7022_dev *dev = (struct att7022_dev *)filp->private_data;
	mutex_lock(&att7022dev.lock);
	att7022_readdata_volt(dev);
	mutex_unlock(&att7022dev.lock);    
	data[0] = (unsigned char)(dev->Volt_A>>0);
	data[1] = (unsigned char)(dev->Volt_A>>8);
	data[2] = (unsigned char)(dev->Volt_A>>16);
	data[3] = (unsigned char)(dev->Volt_B>>0);
	data[4] = (unsigned char)(dev->Volt_B>>8);
	data[5] = (unsigned char)(dev->Volt_B>>16);
	data[6] = (unsigned char)(dev->Volt_C>>0);
	data[7] = (unsigned char)(dev->Volt_C>>8);
	data[8] = (unsigned char)(dev->Volt_C>>16);
	//printk("0x%x 0x%x 0x%x \n",dev->Volt_A,dev->Volt_B,dev->Volt_C);
	err = copy_to_user(buf, data, sizeof(data));
	return 0;
}
/*
 * @description		: 从设备读取数据 
 * @param - filp 	: 要打开的设备文件(文件描述符)
 * @param - buf 	: 返回给用户空间的数据缓冲区
 * @param - cnt 	: 要读取的数据长度
 * @param - offt 	: 相对于文件首地址的偏移
 * @return 			: 读取的字节数，如果为负值，表示读取失败
 */
static long att7022_ioctl(struct file *file, unsigned int cmd, \
                        unsigned long arg)
{
    //printk("[%s]\n", __func__);

    int ret;
    unsigned char nr;
    unsigned int value;
   	struct att7022_dev *dev = (struct att7022_dev *)file->private_data;

    /* 检查设备类型 */
    if (_IOC_TYPE(cmd) != IOC_MAGIC) {
       printk("[%s] command type [%c] error!\n", \
            __func__, _IOC_TYPE(cmd));
        return -ENOTTY; 
    }

    /* 检查序数 */
    if (_IOC_NR(cmd) > IOC_MAXNR) { 
        printk("[%s] command numer [%d] exceeded!\n", 
            __func__, _IOC_NR(cmd));
        return -ENOTTY;
    }    

    /* 检查访问模式 */
    if (_IOC_DIR(cmd) & _IOC_READ)
        ret= !access_ok(VERIFY_WRITE, (void __user *)arg, \
                _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        ret= !access_ok(VERIFY_READ, (void __user *)arg, \
                _IOC_SIZE(cmd));
    if (ret)
        return -EFAULT;
     /* 读寄存器 */
    if (_IOC_DIR(cmd) & _IOC_READ) 
    {
    	nr =  _IOC_NR(cmd);
    	mutex_lock(&att7022dev.lock);
    	value = att7022_read_onereg(dev,nr);
    	mutex_unlock(&att7022dev.lock);
    	//printk("read reg: 0x%x value:0x%x \n",nr,value);
    	ret = copy_to_user((unsigned int *)arg, &value, sizeof(value)); /* 数据发送到用户层 */
        if (ret) 
            return -EFAULT;
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)  /* 写寄存器 */
    {
    	nr =  _IOC_NR(cmd);
        ret = copy_from_user(&value,(unsigned int  *)arg, sizeof(value));
        //printk("write reg: 0x%x value:0x%x \n",nr,value);
        if (ret) 
            return -EFAULT;
    	mutex_lock(&att7022dev.lock);
    	att7022_write_onereg(dev,nr,value);   		/* 写寄存器 */
    	mutex_unlock(&att7022dev.lock);    
    	
    }
    return 0;
}
/*
 * @description		: 关闭/释放设备
 * @param - filp 	: 要关闭的设备文件(文件描述符)
 * @return 			: 0 成功;其他 失败
 */
static int att7022_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/* att7022操作函数 */
static const struct file_operations att7022_ops = {
	.owner = THIS_MODULE,
	.open = att7022_open,
	.read = att7022_read,
	.release = att7022_release,
	.unlocked_ioctl = att7022_ioctl,
};

/*
 * att7022内部寄存器初始化函数 
 * @param  	: 无
 * @return 	: 无
 */
 
void att7022_reginit(void)
{
	unsigned int value = 0;
	//int i=0;
	printk("ATT7022 reg att7022_reginit\r\n");	
	att7022_write_onereg(&att7022dev,0xD3,0x000000);   		/* 复位IC */
	//delay_ms(100);//延时1m,等待AT7022正常工作
	msleep(1000); 
		//printk("ATT7022 reg URmsa = %#X\r\n", value);	
	//value = att7022_read_onereg(&att7022dev, r_URmsb);
	//printk("ATT7022 reg URmsb = %#X\r\n", value);	
	//dev-> Volt_A = value;
	//value = att7022_read_onereg(&att7022dev, r_URmsc);
	//printk("ATT7022 reg URmsc = %#X\r\n", value);	
	
	value = att7022_read_onereg(&att7022dev, 0x5D);
	printk("ATT7022 reg 0x5D = %#X\r\n", value);
	value = att7022_read_onereg(&att7022dev, 0x00);
	
	if(value < 0x702200)
	{
		printk("ATT7022 ID = %#X error\r\n", value);	
	}else
	{
		printk("ATT7022 ID = %#X\r\n", value);	
	}
	att7022_write_onereg(&att7022dev,0xc3,0x0000);//清校表数据
	msleep(100); 
//	value = att7022_read_onereg(&att7022dev, 0x00);
//	printk("ATT7022 ID = %#X\r\n", value);	
	att7022_write_onereg(&att7022dev,0xc9,0x005A);//校表数据写使能
	att7022_write_onereg(&att7022dev,0x81,0xBFFE);//模式选择
	att7022_write_onereg(&att7022dev,0x83,0xFD04);//EMU配置
	att7022_write_onereg(&att7022dev,0x9E,0x0D00);
	//att7022_write_onereg(&att7022dev,0xB1,0x3427);//模拟模块使能
	att7022_write_onereg(&att7022dev,0xB1,0x3c27);//模拟模块使能 spi 低速模式
	//delay_us(2);
	msleep(1); 
	//att7022_write_onereg(&att7022dev,0x82,0x0155);//ADC增益选择  2倍增益 
	att7022_write_onereg(&att7022dev,0x82,0x00);//ADC增益选择  1倍增益 
//	readatt7022(0x82);
	att7022_write_onereg(&att7022dev,0xA4,0x0037);//A相电压Offset校正
	att7022_write_onereg(&att7022dev,0xA5,0x002C);//B相电压Offset校正
	att7022_write_onereg(&att7022dev,0xA6,0x0040);//C相电压Offset校正
	att7022_write_onereg(&att7022dev,0xA7,0x0007);//A相电压Offset校正
	att7022_write_onereg(&att7022dev,0xA8,0x0007);//B相电流Offset校正
	att7022_write_onereg(&att7022dev,0xA9,0x0007);//C相电流Offset校正
   
	att7022_write_onereg(&att7022dev,0x9A,0x85B4);//A相电流增益校正
	att7022_write_onereg(&att7022dev,0x9B,0x85B4);//B相电流增益校正
	att7022_write_onereg(&att7022dev,0x9C,0x85B5);//C相电流增益校正
   
//	att7022_write_onereg(&att7022dev,0x97,0xC278);//A相电压增益校正
//	att7022_write_onereg(&att7022dev,0x98,0xC2A4);//B相电压增益校正
//	att7022_write_onereg(&att7022dev,0x99,0xC369);//C相电压增益校正


/* V1.03 */
//	att7022_write_onereg(&att7022dev,0x97,0xB9B6);//A相电压增益校正
//	att7022_write_onereg(&att7022dev,0x98,0xB9B6);//B相电压增益校正
//	att7022_write_onereg(&att7022dev,0x99,0xB9B6);//C相电压增益校正
/* V1.04 */

	att7022_write_onereg(&att7022dev,0x97,0xDE0D);//A相电压增益校正
	att7022_write_onereg(&att7022dev,0x98,0xDE0D);//B相电压增益校正
	att7022_write_onereg(&att7022dev,0x99,0xDE0D);//C相电压增益校正
	
	att7022_write_onereg(&att7022dev,0x84,0x8C47);//A相有功功率增益校正
	att7022_write_onereg(&att7022dev,0x85,0x8C31);//B相有功功率增益校正
	att7022_write_onereg(&att7022dev,0x86,0x8C5F);//C相有功功率增益校正
   
	att7022_write_onereg(&att7022dev,0x87,0x8BF4);//A相无功功率增益校正
	att7022_write_onereg(&att7022dev,0x88,0x8C03);//B相无功功率增益校正
	att7022_write_onereg(&att7022dev,0x89,0x8C29);//C相无功功率增益校正
   
	att7022_write_onereg(&att7022dev,0xB4,0x7D68);//C相无功功率增益校正
   
	att7022_write_onereg(&att7022dev,0xC9,0x0000);//校表数据禁使能
	
	msleep(1); 
	
	//printk("ATT7022 0xB4  = %#X\r\n", att7022_read_onereg(&att7022dev, 0xB4));	
	//printk("ATT7022 0x97  = %#X\r\n", att7022_read_onereg(&att7022dev, 0x97));	
	//printk("ATT7022 0x98  = %#X\r\n", att7022_read_onereg(&att7022dev, 0x98));	
	//printk("ATT7022 0x99  = %#X\r\n", att7022_read_onereg(&att7022dev, 0x99));	
	/*
	att7022_write_onereg(&att7022dev,0xC6,0x005A);//校表数据读使能
	msleep(1); 
	for(i=0;i<100;i++) //读出校准寄存器
	{
		value = att7022_read_onereg(&att7022dev, i);
		printk("ATT7022 0x%x reg value = %#X\r\n",i, value);	
	}
	att7022_write_onereg(&att7022dev,0xC6,0x0000);//校表数据读使能
		msleep(1); 
	for(i=0;i<100;i++) //读出测量寄存器
	{
		value = att7022_read_onereg(&att7022dev, i);
		printk("ATT7022 0x%x reg value = %#X\r\n",i, value);	
	}
	*/
	/*
	value = att7022_read_onereg(&att7022dev, r_URmsa);
	printk("ATT7022 reg URmsa = %#X\r\n", value);	
	value = att7022_read_onereg(&att7022dev, r_URmsb);
	printk("ATT7022 reg URmsb = %#X\r\n", value);	
	value = att7022_read_onereg(&att7022dev, r_URmsc);
	printk("ATT7022 reg URmsc = %#X\r\n", value);	
	*/
}

 /*
  * @description     : spi驱动的probe函数，当驱动与
  *                    设备匹配以后此函数就会执行
  * @param - client  : spi设备
  * @param - id      : spi设备ID
  * 
  */	
static int att7022_probe(struct spi_device *spi)
{
	//int ret = 0;

     printk("att7022_probe \n");
	/* 1、构建设备号 */
	if (att7022dev.major) {/* 定义了主设备号 */
		att7022dev.devid = MKDEV(att7022dev.major, 0);
		register_chrdev_region(att7022dev.devid, ATT7022_CNT, ATT7022_NAME);
	} else {
		alloc_chrdev_region(&att7022dev.devid, 0, ATT7022_CNT, ATT7022_NAME);/* 申请设备号 */
		att7022dev.major = MAJOR(att7022dev.devid);
	}
	printk("att7022_probe att7022dev.major=%d\n",att7022dev.major);

	/* 2、注册设备 */
	cdev_init(&att7022dev.cdev, &att7022_ops);/* 初始化 cdev 结构体变量 */
	cdev_add(&att7022dev.cdev, att7022dev.devid, ATT7022_CNT);/* 添加字符设备 */

	/* 3、创建类 */
	att7022dev.class = class_create(THIS_MODULE, ATT7022_NAME);
	if (IS_ERR(att7022dev.class)) {
		return PTR_ERR(att7022dev.class);
	}

	/* 4、创建设备 */
	att7022dev.device = device_create(att7022dev.class, NULL, att7022dev.devid, NULL, ATT7022_NAME);
	if (IS_ERR(att7022dev.device)) {
		return PTR_ERR(att7022dev.device);
	}

	/* 获取设备树中cs片选信号 */
	//att7022dev.nd = of_find_node_by_path("/soc/aips-bus@02000000/spba-bus@02000000/ecspi@02010000");
	//if(att7022dev.nd == NULL) {
	//	printk("ecspi3 node not find!\r\n");
	//	return -EINVAL;
	//} 

	/* 2、 获取设备树中的gpio属性，得到BEEP所使用的BEEP编号 */
	//att7022dev.cs_gpio = of_get_named_gpio(att7022dev.nd, "cs-gpio", 0);
	//if(att7022dev.cs_gpio < 0) {
	//	printk("can't get cs-gpio");
	//	return -EINVAL;
	//}

	/* 3、设置GPIO1_IO20为输出，并且输出高电平 */
	//ret = gpio_direction_output(att7022dev.cs_gpio, 1);
	//if(ret < 0) {
	//	printk("can't set gpio!\r\n");
	//}

	/*初始化spi_device */
	spi->mode = SPI_MODE_1;	/*MODE1，CPOL=1，CPHA=0*/
	// printk("spi_txbitlen 0x%x",spi->txbitlen);
	spi_setup(spi);
	att7022dev.private_data = spi; /* 设置私有数据 */

	mutex_init(&att7022dev.lock); /* 初始化互斥体 */
	/* 初始化ATT7022内部寄存器 */
	att7022_reginit();		
	return 0;
}

/*
 * @description     : spi驱动的remove函数，移除spi驱动的时候此函数会执行
 * @param - client 	: spi设备
 * @return          : 0，成功;其他负值,失败
 */
static int att7022_remove(struct spi_device *spi)
{
	/* 删除设备 */
	cdev_del(&att7022dev.cdev);
	unregister_chrdev_region(att7022dev.devid, ATT7022_CNT);

	/* 注销掉类和设备 */
	device_destroy(att7022dev.class, att7022dev.devid);
	class_destroy(att7022dev.class);
	return 0;
}

/* 传统匹配方式ID列表 */
static const struct spi_device_id att7022_id[] = {
	{"HiTrendtech,att7022", 0},  
	{}
};

/* 设备树匹配列表 */
static const struct of_device_id att7022_of_match[] = {
	{ .compatible = "HiTrendtech,att7022" },
	{ /* Sentinel */ }
};

/* SPI驱动结构体 */	
static struct spi_driver att7022_driver = {
	.probe = att7022_probe,
	.remove = att7022_remove,
	.driver = {
		   	.name = "att7022",
		   	.of_match_table = att7022_of_match, 
		   	//.of_match_table = of_match_ptr(att7022_of_match),
		   	.owner = THIS_MODULE,
		   },
	.id_table = att7022_id,
};
		   
/*
 * @description	: 驱动入口函数
 * @param 		: 无
 * @return 		: 无
 */
static int __init att7022_init(void)
{
	printk("att7022_init \n");	
	return spi_register_driver(&att7022_driver);
}

/*
 * @description	: 驱动出口函数
 * @param 		: 无
 * @return 		: 无
 */
static void __exit att7022_exit(void)
{
	printk("att7022_exit \n");	
	spi_unregister_driver(&att7022_driver);
}

module_init(att7022_init);
module_exit(att7022_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xcharger");



