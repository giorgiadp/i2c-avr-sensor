#include "funzioni.h"

#define TWI_FREQ 400000L // SCL frequency

#define START           0x08
#define REPEATED_START  0x10
#define MT_SLA_ACK		0x18
#define MT_SLA_NACK		0x20
#define MT_DATA_ACK     0x28
#define MT_DATA_NACK    0x30
#define MR_SLA_ACK		0x40
#define MR_SLA_NACK		0x48
#define MR_DATA_ACK     0x50
#define MR_DATA_NACK    0x58
#define LOST_ARBTRTN    0x38
#define TW_STATUS      (TWSR & 0xF8)

#define TW_READ    		1
#define TW_WRITE      	0

void init(void){
	
	DDRD &= ~(1<<PD1); //input sda
	DDRD &= ~(1<<PD0); //input scl
	
	PORTD |= (1<<PD1); //pullup sda
	PORTD |= (1<<PD0); //pullup scl
	
	TWSR &= ~(1<<TWPS0);
	TWSR &= ~(1<<TWPS1);
    
    TWBR = ((F_CPU / TWI_FREQ) - 16 ) / 2;
    
    TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
 
}

uint8_t start(uint8_t slave_add, uint8_t mode){
	
	uint8_t status = 0;
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	/* Dopo la start il TWINT flag è settato dall'hardware
	 * status code in TWSR è 0x08 */
	
	while (!(TWCR & (1<<TWINT))); //aspetto che TWINT sia resettato a 0

	if(TW_STATUS != START) printf("Errore nella start\n"); //check TWI STatus Register
	
	TWDR = (slave_add << 1) | mode; //slave_address(7bit) + R/W
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	if(TW_STATUS == MT_SLA_NACK) printf("Errore invio SLA+W\n");  
	
	while (!(TWCR & (1<<TWINT)));
	status = TW_STATUS;
	return status;
}

uint8_t rp_start(uint8_t slave_add, uint8_t mode){
	
	uint8_t status = 0;
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	/* Dopo la start il TWINT flag è settato dall'hardware
	 * status code in TWSR è 0x08 */
	
	while (!(TWCR & (1<<TWINT))); //aspetto che TWINT sia resettato a 0

	if(TW_STATUS != REPEATED_START) printf("Errore nella rp_start\n"); //check TWI STatus Register
	
	TWDR = (slave_add << 1) | mode; //slave_address(7bit) + R/W
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	if(TW_STATUS == MR_SLA_NACK) printf("Errore nell'invio SLA+R\n"); 
	
	while (!(TWCR & (1<<TWINT)));
	status = TW_STATUS;
	return status;
}

uint8_t write(uint8_t data){
	TWDR = data;
	
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
	
	if(TW_STATUS != MT_DATA_ACK) printf("errore invio data\n");
	
	return TW_STATUS;
}

void write_data(uint8_t slave_add, uint8_t reg_add, uint8_t data){
	start(slave_add, TW_WRITE);
	
	write(reg_add);
	
	write(data);
	
	stop();
}

uint8_t read_ACK(){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	
    while (!(TWCR & (1<<TWINT)));

    return TWDR;
}

uint8_t read_NACK(void){
	TWCR = (1<<TWINT)|(1<<TWEN);
	
    while (!(TWCR & (1<<TWINT)));
    
    return TWDR;
}


void stop(){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	while (TWCR & (1<<TWSTO));
}

uint8_t read_data(uint8_t slave_add, uint8_t reg_add){
	/* I dati ricevuti possono essere letti dal TWDR
	 * quando il TWINT flag è settato alto dall'hardware.
	 * Quando ho finito di leggere devo informare lo slave
	 * inviando un NACK dopo l'ultimo byte ricevuto,
	 * poi posso mandare la stop */
	printf(">start1\n");
	start(slave_add, TW_WRITE);
	
	printf(">>write reg_add\n");
	write(reg_add);
	
	printf(">>>rp_start\n");
	rp_start(slave_add, TW_READ);
	
	printf(">>>>read\n");
	uint8_t res = read_NACK();
	
	printf(">stop\n");
	stop();
	
	return res;
}
