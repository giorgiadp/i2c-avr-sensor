#ifndef funzioni
#define funzioni

#include <stdio.h>
#include <avr_common/uart.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h> //la uso solo per gli status code
/* START       	   	0x08
 * REPEATED_START  	0x10
 * MT_SLA_ACK		0x18
 * MT_SLA_NACK		0x20
 * MT_DATA_ACK     	0x28
 * MT_DATA_NACK    	0x30
 * MR_SLA_ACK		0x40
 * MR_SLA_NACK		0x48
 * MR_DATA_ACK     	0x50
 * MR_DATA_NACK    	0x58
 * LOST_ARBTRTN    	0x38
 * TW_STATUS      	(TWSR & 0xF8) */

void init(void);
uint8_t start(uint8_t address,uint8_t mode);
uint8_t rp_start(uint8_t slave_add, uint8_t mode);
uint8_t write(uint8_t data);
void write_data(uint8_t slave_add, uint8_t reg_add, uint8_t data);
uint8_t readACK(void);
uint8_t readNACK(void);
uint8_t read_data(uint8_t slave_add, uint8_t reg_add);
void stop(void);


#endif
