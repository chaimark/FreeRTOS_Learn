#include "Define.h"

void CMT_SPI_Init(void) {
    CMT_SPI_CSB_Out();
    CMT_SPI_CSB_H();   /* CSB has an internal pull-up resistor */
    CMT_SPI_SCLK_Out();
    CMT_SPI_SCLK_L();   /* SCLK has an internal pull-down resistor */
    CMT_SPI_SDIO_Set_Out();
    CMT_SPI_SDIO_H();
    CMT_SPI_FCSB_Out();
    CMT_SPI_FCSB_H();  /* FCSB has an internal pull-up resistor */
    FL_DelayUs(22);
}

void CMT_SPI_Send(unsigned char data8) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        CMT_SPI_SCLK_L();
        /* Send byte on the rising edge of SCLK */
        if (data8 & 0x80)CMT_SPI_SDIO_H();
        else CMT_SPI_SDIO_L();
        FL_DelayUs(22);
        data8 <<= 1;
        CMT_SPI_SCLK_H();
        FL_DelayUs(22);
    }
}

unsigned char CMT_SPI_Receive(void) {
    unsigned char i;
    unsigned char data8 = 0xFF;
    for (i = 0; i < 8; i++) {
        CMT_SPI_SCLK_L();
        FL_DelayUs(22);
        data8 <<= 1;
        CMT_SPI_SCLK_H();
        /* Read byte on the rising edge of SCLK */
        if (CMT_SPI_SDIO_Read()) data8 |= 0x01;
        else data8 &= ~0x01;
        FL_DelayUs(22);
    }
    return data8;
}

void CMT_SPI_Write(unsigned char addr, unsigned char dat) {
    CMT_SPI_SDIO_Set_Out();
    CMT_SPI_SDIO_H();
    CMT_SPI_SCLK_Out();
    CMT_SPI_SCLK_L();
    CMT_SPI_FCSB_Out();
    CMT_SPI_FCSB_H();
    CMT_SPI_CSB_Out();
    CMT_SPI_CSB_L();
    /* > 0.5 SCLK cycle */
    FL_DelayUs(22);
    FL_DelayUs(22);
    /* r/w = 0 */
    CMT_SPI_Send(addr & 0x7F);
    CMT_SPI_Send(dat);
    CMT_SPI_SCLK_L();
    /* > 0.5 SCLK cycle */
    FL_DelayUs(22);
    FL_DelayUs(22);
    CMT_SPI_CSB_H();
    CMT_SPI_SDIO_H();
    CMT_SPI_SDIO_Set_In();
    CMT_SPI_FCSB_H();
}

void CMT_SPI_Read(unsigned char addr, unsigned char * p_dat) {
    CMT_SPI_SDIO_Set_Out();
    CMT_SPI_SDIO_H();
    CMT_SPI_SCLK_Out();
    CMT_SPI_SCLK_L();
    CMT_SPI_FCSB_Out();
    CMT_SPI_FCSB_H();
    CMT_SPI_CSB_Out();
    CMT_SPI_CSB_L();
    /* > 0.5 SCLK cycle */
    FL_DelayUs(22);
    FL_DelayUs(22);
    /* r/w = 1 */
    CMT_SPI_Send(addr | 0x80);
    /* Must set SDIO to input before the falling edge of SCLK */
    CMT_SPI_SDIO_Set_In();
    *p_dat = CMT_SPI_Receive();
    CMT_SPI_SCLK_L();
    /* > 0.5 SCLK cycle */
    FL_DelayUs(22);
    FL_DelayUs(22);
    CMT_SPI_CSB_H();
    CMT_SPI_SDIO_H();
    CMT_SPI_SDIO_Set_In();
    CMT_SPI_FCSB_H();
}

void CMT_SPI_Write_fifo(const unsigned char * p_buf, unsigned int len) {
    unsigned int i;
    CMT_SPI_FCSB_Out();
    CMT_SPI_FCSB_H();
    CMT_SPI_CSB_Out();
    CMT_SPI_CSB_H();
    CMT_SPI_SCLK_Out();
    CMT_SPI_SCLK_L();
    CMT_SPI_SDIO_Set_Out();
    for (i = 0; i < len; i++) {
        CMT_SPI_FCSB_L();
        /* > 1 SCLK cycle */
        FL_DelayUs(22);
        FL_DelayUs(22);
        CMT_SPI_Send(p_buf[i]);
        CMT_SPI_SCLK_L();
        /* > 2 us */
        FL_DelayUs(95);
        CMT_SPI_FCSB_H();
        /* > 4 us */
        FL_DelayUs(190);
    }
    CMT_SPI_SDIO_Set_In();
    CMT_SPI_FCSB_H();
}

void CMT_SPI_Read_fifo(unsigned char * p_buf, unsigned int len) {
    unsigned int i;
    CMT_SPI_FCSB_Out();
    CMT_SPI_FCSB_H();
    CMT_SPI_CSB_Out();
    CMT_SPI_CSB_H();
    CMT_SPI_SCLK_Out();
    CMT_SPI_SCLK_L();
    CMT_SPI_SDIO_Set_In();
    for (i = 0; i < len; i++) {
        CMT_SPI_FCSB_L();
        /* > 1 SCLK cycle */
        FL_DelayUs(22);
        FL_DelayUs(22);
        p_buf[i] = CMT_SPI_Receive();
        CMT_SPI_SCLK_L();
        /* > 2 us */
        FL_DelayUs(95);
        CMT_SPI_FCSB_H();
        /* > 4 us */
        FL_DelayUs(190);
    }
    CMT_SPI_SDIO_Set_In();
    CMT_SPI_FCSB_H();
}

/*! ********************************************************
* @name    CMT2300A_InitGpio
* @desc    Initializes the CMT2300A interface GPIOs.
* *********************************************************/
void CMT2300A_InitGpio(void) {
    CMT_SPI_GPIO1_Set_In();
    CMT_SPI_Init();
}

/*! ********************************************************
* @name    CMT2300A_ReadReg
* @desc    Read the CMT2300A register at the specified address.
* @param   addr: register address
* @return  Register value
* *********************************************************/
unsigned char CMT2300A_ReadReg(unsigned char addr) {
    unsigned char dat = 0xFF;
    CMT_SPI_Read(addr, &dat);
    return dat;
}

/*! ********************************************************
* @name    CMT2300A_WriteReg
* @desc    Write the CMT2300A register at the specified address.
* @param   addr: register address
*          dat: register value
* *********************************************************/
void CMT2300A_WriteReg(unsigned char addr, unsigned char dat) {
    CMT_SPI_Write(addr, dat);
}

/*! ********************************************************
* @name    CMT2300A_ReadFifo
* @desc    Reads the contents of the CMT2300A FIFO.
* @param   buf: buffer where to copy the FIFO read data
*          len: number of bytes to be read from the FIFO
* *********************************************************/
void CMT2300A_ReadFifo(unsigned char buf[], unsigned int len) {
    CMT_SPI_Read_fifo(buf, len);
}

/*! ********************************************************
* @name    CMT2300A_WriteFifo
* @desc    Writes the buffer contents to the CMT2300A FIFO.
* @param   buf: buffer containing data to be put on the FIFO
*          len: number of bytes to be written to the FIFO
* *********************************************************/
void CMT2300A_WriteFifo(const unsigned char buf[], unsigned int len) {
    CMT_SPI_Write_fifo(buf, len);
}
