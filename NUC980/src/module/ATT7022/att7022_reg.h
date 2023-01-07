#ifndef ATT7022_H
#define ATT7022_H

#include <linux/ioctl.h>    // 内核空间

/* 定义设备类型 */
#define IOC_MAGIC  'c'

/* 初始化设备 */
#define IOCINIT    _IO(IOC_MAGIC, 0)

/* 写寄存器 */
#define IOCWREG(reg)    _IOW(IOC_MAGIC, reg, int)  /* 1 位置填写  寄存器地址 */

/* 读寄存器 */
#define IOCRREG(reg)    _IOR(IOC_MAGIC, reg, int)

#define IOC_MAXNR  0xFF

#define  ATT7022_ID                0x7122A0
#define  ATT7022E_ID               0x7022E0

#define r_Pa 0x01 /* A Phase Power */
#define r_Pb 0x02 /* B Phase Power */
#define r_Pc 0x03 /* C Phase Power */
#define r_Pt 0x04 /* 3 Phase Power */
#define r_Qa 0x05 /* A Phase Q Power */
#define r_Qb 0x06 /* B Phase Q Power */
#define r_Qc 0x07 /* C Phase Q Power */
#define r_Qt 0x08 /* 3 Phase Q Power */
#define r_Sa 0x09 /* A Phase S Power */
#define r_Sb 0x0A /* B Phase S Power */
#define r_Sc 0x0B /* C Phase S Power */
#define r_St 0x0C /* 3 Phase S Power */

#define r_URmsa 0x0D /* A Phase Voltage */
#define r_URmsb 0x0E /* B Phase Voltage */
#define r_URmsc 0x0F /* C Phase Voltage */
#define r_IRmsa 0x10 /* A Phase Current */
#define r_IRmsb 0x11 /* B Phase Current */
#define r_IRmsc 0x12 /* C Phase Current */
#define r_Pfa   0x14 /* A Phase gong lv yin shu */
#define r_Pfb   0x15 /* B Phase gong lv yin shu */
#define r_Pfc   0x16 /* C Phase gong lv yin shu */
#define r_Pft   0x17 /* ZONG   gong lv yin shu */
#define r_Pga   0x18 /* A dianyadianliu jiajiao */
#define r_Pgb   0x19 /* B dianyadianliu jiajiao */
#define r_Pgc   0x1a /* C dianyadianliu jiajiao */
#define r_INTFlag   0x1b /* zhongduanbiaozhi */

#define r_Freq 0x1C /* Frequency */
#define r_EFlag 0x1D /* Energy Register Status */
#define r_Epa  0x1E  /* A Phase Energy */
#define r_Epb  0x1F  /* B Phase Energy */
#define r_Epc  0x20  /* C Phase Energy */
#define r_Ept  0x21  /* 3 Phase Energy */
#define r_Eqa  0x22  /* A Phase qEnergy */
#define r_Eqb  0x23  /* B Phase qEnergy */
#define r_Eqc  0x24  /* C Phase qEnergy */
#define r_Eqt  0x25  /* 3 Phase qEnergy */

#define r_YUaUb  0x26
#define r_YUbUc  0x27
#define r_YUcUa  0x28
#define r_I0Rms  0x29
#define r_TPSD  0x2a
#define r_UtRms  0x2b
#define r_SFlag 0x2C /* Error Flags */
#define r_WSPIData1 0x2D  /* Last Time SPI Write Data*/
#define r_WSPIData2 0x2D  /* Last Time SPI Write Data*/
#define r_ChkSum1 0x2E /* Calibration Data Chksum */
//#define r_RSPIData 0x28  /* Last Time SPI Readout Data */
//#define r_SFlag 0x2C /* Error Flags */
//#define r_WSPIData1 0x2D  /* Last Time SPI Write Data*/
//#define r_WSPIData2 0x2E  /* Last Time SPI Write Data*/
#define r_Eqa2 0x35 /* Phase A S energy, same to Eqa, can be clear to 0 after read. */
#define r_Eqb2 0x36 /* Phase B S energy, same to Eqb, can be clear to 0 after read. */
#define r_Eqc2 0x37 /* Phase C S energy, same to Eqc, can be clear to 0 after read. */
#define r_Eqt2 0x38 /* 3-phase combined S energy, same to Eqt, can be clear to 0 after read. */
#define r_PFlag  0x3D  /* Direction of Power */
#define r_Epa2 0xff /* Same as Epa, Clear After Read */
#define r_Epb2 0xff /* Same as Epb, Clear After Read */
#define r_Epc2 0xff /* Same as Epc, Clear After Read */
#define r_Ept2 0xff /* Same as Ept, Clear After Read */
	



/* below is for calibration */
#define w_ModeCfg 0x81 /*mode control*/
#define w_EMUCfg 0x83 /*EMU mode control*/
#define w_PoffsetA 0x93 /*A POWER offset*/
#define w_PoffsetB 0x94 /*B POWER offset*/
#define w_PoffsetC 0x95 /*C POWER offset*/
#define w_QPhscal 0x96 /*Q POWER Phase Calibration*/
#define w_FailValtage 0x9f /* Fail Vailtage set up*/
#define w_GainADC7 0xA0 /* ADC7 gain*/
#define w_QoffsetA 0xA1 /* Q power offset A */
#define w_QoffsetB 0xA2 /* Q power offset B */
#define w_QoffsetC 0xA3 /* Q power offset C */
#define w_UaRmsoffset 0xA4 
#define w_UbRmsoffset 0xA5
#define w_UcRmsoffset 0xA6
#define w_IaRmsoffset 0xA7
#define w_IbRmsoffset 0xA8
#define w_IcRmsoffset 0xA9
#define w_EMUIE 0xB0 
#define w_ModulCFG 0xB1


#define w_Iregion1 0xB7 /* Phase Calibration Region1 */
#define w_Iregion2 0xff /* Phase Calibration Region2 */
#define w_Iregion3 0xff /* Phase Calibration Region3 */
#define w_Iregion4 0xff /* Phase Calibration Region4 */
#define w_PgainA0 0x84 /* A Active Power Gain 0*/
#define w_PgainB0 0x85 /* B Active Power Gain 0*/
#define w_PgainC0 0x86 /* C Active Power Gain 0*/
#define w_PgainA1 0xff /* A Active Power Gain 1*/
#define w_PgainB1 0xff /* B Active Power Gain 1*/
#define w_PgainC1 0xff /* C Active Power Gain 1*/
#define w_QgainA0 0x87 /* A Q Power Gain 0*/
#define w_QgainB0 0x88 /* B Q Power Gain 0*/
#define w_QgainC0 0x89 /* C Q Power Gain 0*/
#define w_SgainA0 0x8A /* A S Power Gain 0*/
#define w_SgainB0 0x8B /* B SPower Gain 0*/
#define w_SgainC0 0x8C /* C S Power Gain 0*/

#define w_PhsregA0 0x8D /* A Region 0 Phase Calibration */
#define w_PhsregA1 0x90 /* A Region 1 Phase Calibration */
#define w_PhsregA2 0xff /* A Region 2 Phase Calibration */
#define w_PhsregA3 0xff /* A Region 3 Phase Calibration */
#define w_PhsregA4 0xff /* A Region 4 Phase Calibration */
#define w_PhsregB0 0x8E /* B Region 0 Phase Calibration */
#define w_PhsregB1 0x91 /* B Region 1 Phase Calibration */
#define w_PhsregB2 0xff /* B Region 2 Phase Calibration */
#define w_PhsregB3 0xff /* B Region 3 Phase Calibration */
#define w_PhsregB4 0xff /* B Region 4 Phase Calibration */
#define w_PhsregC0 0x8F /* C Region 0 Phase Calibration */
#define w_PhsregC1 0x92 /* C Region 1 Phase Calibration */
#define w_PhsregC2 0xff /* C Region 2 Phase Calibration */
#define w_PhsregC3 0xff /* C Region 3 Phase Calibration */
#define w_PhsregC4 0xff /* C Region 4 Phase Calibration */
#define w_UgainA 0x97 /*Phase A voltage gain     */                     
#define w_UgainB 0x98 /*Phase B voltage gain   */                       
#define w_UgainC 0x99 /*Phase C voltage gain */      
#define w_Irechg 0xFF /*Active power calibration region setup */        
#define w_Istartup 0x9D /* Starting current threshold setup */
#define w_HFConst 0x9E  /*High-frequency calibration pulse output setup */
#define w_LFConst 0xFF  /*Low-frequency pulse output setup */
#define w_IgainA 0x9A /* Phase A current gain */
#define w_IgainB 0x9B /* Phase B current gain */
#define w_IgainC 0x9C /* Phase C current gain */
#define w_EAddMode 0xAA /* Calculating mode of 3 phase energy (sum in algebra/ absolute) */
#define w_EnDtIorder 0xFF /* Current phase sequence detecting enable control */
#define w_HFDouble 0xB3 /* Pulse constant reduplication select */
#define w_UADCPga 0x82  /* Voltage channel ADC gain select */

#define EnergyRgMaxValue 0x34c; /* While 2 Times Read 7022, the max possible changed value of energy register */
#define EnergyRgMinValue 0x140; /* 1/10 * 320 */


#endif

