#ifndef __I2C_h__
#define __I2C_h__

#ifndef F_CPU
#define F_CPU               16000000UL      // Change if use use another frequency
#endif

#include <util/twi.h>

#define F_SCK               100000UL        // 1000000 Hz or 4000000 Hz
#define TWI_BUFFER_SIZE     7
#define TWI_RW_BIT          0
#define TWI_LAST_TRANS_OK   0xFF
#define TWI_READ            1
#define TWI_WRITE           0

void    TWI_Init();
uint8_t TWI_Busy();
uint8_t TWI_Get_State();
void    TWI_Start_Transmition (uint8_t *msg, uint8_t msgSize);
void    TWI_Restart_Last_Transmition();
uint8_t TWI_Get_Data (uint8_t *msg, uint8_t msgSize);

#endif // __I2C_h__