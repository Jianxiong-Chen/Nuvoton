/*-
 * Copyright 2013 Broadcom Corporation
 *
 * Copyright (c) 2009-2010 Micron Technology, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Henry Pan <hspan@micron.com>
 *
 * based on nand.h
 */
#ifndef __LINUX_MTD_SPI_NAND_H
#define __LINUX_MTD_SPI_NAND_H

#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/mtd/mtd.h>

/* cmd */
#define CMD_READ			0x13
#define CMD_READ_RDM			0x03
#define CMD_READ_QUAD			0x6B
#define CMD_PROG_PAGE_CLRCACHE		0x02
#define CMD_PROG_PAGE			0x84
#define CMD_PROG_PAGE_EXC		0x10
#define CMD_ERASE_BLK			0xd8
#define CMD_WR_ENABLE			0x06
#define CMD_WR_DISABLE			0x04
#define CMD_READ_ID			0x9f
#define CMD_RESET			0xff
#define CMD_READ_REG			0x0f
#define CMD_WRITE_REG			0x1f

/* feature/ status reg */
#define REG_BLOCK_LOCK			0xa0
#define REG_OTP				0xb0
#define REG_STATUS			0xc0/* timing */

/* status */
#define STATUS_OIP_MASK			0x01
#define STATUS_READY			(0 << 0)
#define STATUS_BUSY			(1 << 0)

#define STATUS_E_FAIL_MASK		0x04
#define STATUS_E_FAIL			(1 << 2)

#define STATUS_P_FAIL_MASK		0x08
#define STATUS_P_FAIL			(1 << 3)

#define STATUS_ECC_MASK			0x30
#define STATUS_ECC_1BIT_CORRECTED	(1 << 4)
#define STATUS_ECC_ERROR		(2 << 4)
#define STATUS_ECC_RESERVED		(3 << 4)

#define STATUS_ECC_MASK_GIGA		0x70
#define STATUS_ECC_ERROR_GIGA		0x70
#define STATUS_ECC_MASK_MACRONIX	0x30
#define STATUS_ECC_ERROR_MACRONIX	0x20
#define STATUS_ECC_MASK_MICRON		0x70
#define STATUS_ECC_ERROR_MICRON		0x20
#define SPINAND_ECC_ERROR		0x1
#define SPINAND_ECC_CORRECTED		0x2
#define STATUS_ECC_MASK_XTX_8bit	0xF0
#define STATUS_ECC_ERROR_XTX_8bit	0xF0

/*ECC enable defines*/
#define OTP_ECC_MASK			0x10
#define OTP_ECC_OFF			0
#define OTP_ECC_ON			1

#define ECC_DISABLED
#define ECC_IN_NAND
#define ECC_SOFT

/* block lock */
#define BL_ALL_LOCKED      0x38
#define BL_1_2_LOCKED      0x30
#define BL_1_4_LOCKED      0x28
#define BL_1_8_LOCKED      0x20
#define BL_1_16_LOCKED     0x18
#define BL_1_32_LOCKED     0x10
#define BL_1_64_LOCKED     0x08
#define BL_ALL_UNLOCKED    0

/* wb multi die */
#define WB_MULTI_DIESELECT      0xC2


struct spinand_cmd {
	u8		cmd;
	u32		n_addr;		/* Number of address */
	u8		addr[3];	/* Reg Offset */
	u32		n_dummy;	/* Dummy use */
	u32		n_tx;		/* Number of tx bytes */
	u8		*tx_buf;	/* Tx buf */
	u32		n_rx;		/* Number of rx bytes */
	u8		*rx_buf;	/* Rx buf */
};

struct spinand_ops {
	u8   maf_id;
	u16   dev_id;
	void (*spinand_set_defaults)(struct spi_device *spi_nand);
	void (*spinand_read_cmd)(struct spinand_cmd *cmd, u32 page_id);
	void (*spinand_read_data)(struct spinand_cmd *cmd, u16 column,
	                          u32 page_id);
	void (*spinand_write_cmd)(struct spinand_cmd *cmd, u32 page_id);
	void (*spinand_write_data)(struct spinand_cmd *cmd, u16 column,
	                           u32 page_id);
	void (*spinand_erase_blk)(struct spinand_cmd *cmd, u32 page_id);
	int (*spinand_parse_id)(struct spi_device *spi_nand, u8 *nand_id,
	                        u8 *id);
	int (*spinand_verify_ecc)(u8 status);
};

struct spinand_info {
	struct nand_ecclayout *ecclayout;
	struct spi_device *spi;
	void *priv;
	struct spinand_ops *dev_ops;
	struct mtd_partition    *parts;     // mtd partition
	int                     nr_parts;   // mtd partition number
};

struct spinand_state {
	uint32_t	col;
	uint32_t	row;
	int		buf_ptr;
	u8		*buf;
};

extern int spinand_mtd(struct mtd_info *mtd);
extern void spinand_mtd_release(struct mtd_info *mtd);

#endif /* __LINUX_MTD_SPI_NAND_H */
