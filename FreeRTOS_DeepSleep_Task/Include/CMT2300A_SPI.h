#ifndef CMT2300A_SPI_H
#define CMT2300A_SPI_H

// CMT2300A_SPI
extern void CMT2300A_ReadFifo(unsigned char buf[], unsigned int len);
extern void CMT2300A_InitGpio(void);
extern unsigned char CMT2300A_ReadReg(unsigned char addr);
extern void CMT2300A_WriteReg(unsigned char addr, unsigned char dat);
extern void CMT2300A_WriteFifo(const unsigned char buf[], unsigned int len);
// CMT2300A_SPI
extern unsigned char CMT2300A_ReadReg(unsigned char addr);
extern void CMT2300A_WriteReg(unsigned char addr, unsigned char dat);

#endif // CMT2300A_SPI_H
