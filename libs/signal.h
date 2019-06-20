#ifndef signal
#define signal

#include <avr/io.h>
#include <avr/interrupt.h>

void init(void);

void start(void);
void repeated_start(void);
void stop(void);
int status(void);

void write(uint16_t);
void writeSLA_R(uint8_t);
void writeSLA_W(uint8_t);

uint8_t readACK(void);
uint8_t readNACK(void);



int read_data();




#endif
