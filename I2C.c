#include "I2C.h"
#include <avr/interrupt.h>

static uint8_t TWI_buf[TWI_BUFFER_SIZE];      // Transceiver buffer
static uint8_t TWI_msgSize;                   // Number of bytes to be transmitted.
static uint8_t TWI_state = TW_NO_INFO;        // State byte. Default set to TWI_NO_STATE.

void TWI_Init() {
  #if (F_CPU < 36*F_SCK)
    #error Wrong TWBR or TWPS value
  #else
    TWSR = 0;
    TWBR = (F_CPU/F_SCK-16)/2;
  #endif
  TWDR = 0xFF;                                      // Default content = SDA released
  TWCR = 1<<TWEN;                                   // Enable TWI-interface and release TWI pins.
}

uint8_t TWI_Busy() {
  return (TWCR & (1<<TWIE));                        // If TWI Interrupt is enabled then the Transceiver is busy
}

uint8_t TWI_Get_State() {
  while (TWI_Busy());                               // Wait until TWI has completed the transmission.
  return (TWI_state);                               // Return error state.
}

void TWI_Start_Transmition (uint8_t *msg, uint8_t msgSize) {
  while (TWI_Busy());                               // Wait until TWI is ready for next transmission.
  TWI_msgSize = msgSize;                            // Number of data to transmit.
  TWI_buf[0]  = msg[0];                             // Store slave address with R/W setting.
  if (!(msg[0] & (1<<TWI_RW_BIT))) {                // If it is a write operation, then also copy data.
    for (uint8_t i=1; i<msgSize; i++)
      TWI_buf[i] = msg[i];
  }
  TWI_state = TW_NO_INFO;
  TWCR = 1<<TWEN | 1<<TWIE | 1<<TWINT | 1<<TWSTA;   // Start condition
}

void TWI_Restart_Last_Transmition() {
  while (TWI_Busy());                               // Wait until TWI is ready for next transmission.
  TWI_state = TW_NO_INFO;
  TWCR = 1<<TWEN | 1<<TWIE | 1<<TWINT | 1<<TWSTA;   // Start condition
}

uint8_t TWI_Get_Data (uint8_t *msg, uint8_t msgSize) {
  while (TWI_Busy());                                // Wait until TWI is ready for next transmission.
  if (TWI_state == TWI_LAST_TRANS_OK) {              // Last transmission competed successfully.
    for(uint8_t i=0; i<msgSize; i++) {               // Copy data from Transceiver buffer.
      msg[i] = TWI_buf[i];
    }
  }
  return TWI_state;
}

ISR (TWI_vect) {
  static uint8_t TWI_bufPtr;
  switch (TW_STATUS) {
    case TW_START:                                  // 0x08 START has been transmitted
    case TW_REP_START:                              // 0x10 Repeated START has been transmitted
      TWI_bufPtr = 0;                               //      Set buffer pointer to the TWI Address location
    case TW_MT_SLA_ACK:                             // 0x18 SLA+W has been tramsmitted and ACK received
    case TW_MT_DATA_ACK:                            // 0x28 Data byte has been tramsmitted and ACK received
      if (TWI_bufPtr < TWI_msgSize) {
        TWDR = TWI_buf[TWI_bufPtr++];
        TWCR = 1<<TWEN | 1<<TWIE | 1<<TWINT;
      }
      else {                                        //      Send STOP after last byte
        TWI_state = TWI_LAST_TRANS_OK;              //      Set status bits to completed successfully.
        TWCR = 1<<TWEN | 1<<TWINT | 1<<TWSTO;       //      STOP condition
      }
      break;
    case TW_MR_DATA_ACK:                            // 0x50 Data byte has been received and ACK tramsmitted
      TWI_buf[TWI_bufPtr++] = TWDR;
    case TW_MR_SLA_ACK:                             // 0x40 SLA+R has been tramsmitted and ACK received
      if (TWI_bufPtr < (TWI_msgSize-1)) {           //      Detect the last byte to NACK it.
        TWCR = 1<<TWEN | 1<<TWIE | 1<<TWINT | 1<<TWEA;
      }
      else {                                        //      Send NACK after next reception
        TWCR = 1<<TWEN | 1<<TWIE | 1<<TWINT;
      }
      break;
    case TW_MR_DATA_NACK:                           // 0x58 Data byte has been received and NACK tramsmitted
      TWI_buf[TWI_bufPtr] = TWDR;
      TWI_state = TWI_LAST_TRANS_OK;                //      Set status bits to completed successfully.
      TWCR = 1<<TWEN | 1<<TWINT | 1<<TWSTO;
      break;
    case TW_MT_ARB_LOST:                            // 0x38 Arbitration lost
      TWCR = 1<<TWEN | 1<<TWIE | 1<<TWINT | 1<<TWSTA;
      break;
    case TW_MT_SLA_NACK:                            // 0x20 SLA+W has been tramsmitted and NACK received
    case TW_MR_SLA_NACK:                            // 0x48 SLA+R has been tramsmitted and NACK received
    case TW_MT_DATA_NACK:                           // 0x30 Data byte has been tramsmitted and NACK received
    case TW_BUS_ERROR:                              // 0x00 Bus error due to an illegal START or STOP condition
    default:
      TWI_state = TW_STATUS;                        //      Store TWSR and automatically sets clears noErrors bit.
      TWCR = 1<<TWEN;
  }
}