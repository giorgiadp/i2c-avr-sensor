#ifndef funzioni
#define funzioni

#include <stdio.h>
#include <avr_common/uart.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h> //la uso solo per gli status code

void init(void);
uint8_t start(uint8_t address,uint8_t mode);
uint8_t rp_start(uint8_t slave_add, uint8_t mode);
uint8_t write(uint8_t data);
void write_data(uint8_t slave_add, uint8_t reg_add, uint8_t data);
uint8_t read_ACK(void);
uint8_t read_NACK(void);
void stop(void);
uint8_t read_data(uint8_t slave_add, uint8_t reg_add);


#endif
