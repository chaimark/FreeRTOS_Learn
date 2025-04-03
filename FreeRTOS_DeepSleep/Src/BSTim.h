#ifndef __BSTIM32_DEMO_H__
#define __BSTIM32_DEMO_H__

#define SecTo5Ms(sec) (uint64_t)(SecTo10Ms(sec) * 2)
extern void BSTIM32_Setup(void);
extern void BSTIM32_Stop(void);

#endif
