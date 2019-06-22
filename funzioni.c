#include "funzioni.h"

#define F_CPU 16000000L
#define F_SCL 100000L // SCL frequency

//#define START           0x08
//#define REPEATED_START  0x10
//#define MT_SLA_ACK		0x18
//#define MT_SLA_NACK		0x20
//#define MT_DATA_ACK     0x28
//#define MT_DATA_NACK    0x30
//#define MR_SLA_ACK		0x40
//#define MR_SLA_NACK		0x48
//#define MR_DATA_ACK     0x50
//#define MR_DATA_NACK    0x58
//#define LOST_ARBTRTN    0x38
//#define TW_STATUS      (TWSR & 0xF8)

#define TW_READ    		1
#define TW_WRITE      	0

void init(void){
	
	//DDRD &= ~(1<<PD1); //input sda
	//DDRD &= ~(1<<PD0); //input scl
	
	PORTD |= (1<<PD1); //SDA: se input pullup, se output leggo
	PORTD |= (1<<PD0); //SCL: se input pullup, se output leggo
	
	TWSR &= ~(1<<TWPS0);
	TWSR &= ~(1<<TWPS1);
    
    TWBR = ((F_CPU / F_SCL) - 16 ) / 2;
    
    TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
 
}


uint8_t start(uint8_t slave_add, uint8_t mode){
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	/* Dopo la start il TWINT flag è settato dall'hardware
	 * status code in TWSR è 0x08 */
	
	while (!(TWCR & (1<<TWINT))); //aspetto che TWINT sia resettato a 0

	//uint8_t status = TW_STATUS & 0xF8;
	if((TWSR & 0xF8) != TW_START){
		printf("Errore nella start\n"); //check TWI STatus Register
		return TW_STATUS;
	}
	
	TWDR = (slave_add << 1) | mode; //slave_address(7bit) + R/W
	TWCR = (1<<TWINT) | (1<<TWEN);
		
	if(((TWSR & 0xF8) == TW_MT_SLA_NACK) || ((TWSR & 0xF8) == TW_MT_DATA_NACK)){
		printf("Errore invio SLA+W\n");
		return TWSR & 0xF8;
	}
	
	while (!(TWCR & (1<<TWINT)));
	//status = TW_STATUS & 0xF8;

	return (TWSR & 0xF8);
}

uint8_t rp_start(uint8_t slave_add, uint8_t mode){
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	/* Dopo la start il TWINT flag è settato dall'hardware
	 * status code in TWSR è 0x08 */
	
	while (!(TWCR & (1<<TWINT))); //aspetto che TWINT sia resettato a 0

	//uint8_t status = TWSR & 0XF8;
	if((TWSR & 0xF8) != TW_REP_START){
		printf("Errore nella start\n"); //check TWI STatus Register
		return TWSR & 0xF8;
	}
	
	TWDR = (slave_add << 1) | mode; //slave_address(7bit) + R/W
	TWCR = (1<<TWINT) | (1<<TWEN);

	//status = TWSR & 0XF8;
	if(((TWSR & 0xF8) == TW_MT_SLA_NACK) || ((TWSR & 0xF8) == TW_MT_DATA_NACK)){
		printf("Errore invio SLA+R\n");
		return TWSR & 0xF8;
	}
	
	while (!(TWCR & (1<<TWINT)));
	//status = TWSR & 0XF8;

	return TWSR & 0xF8;
}

uint8_t write(uint8_t data){
	TWDR = data;
	
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
	
	if((TWSR & 0XF8) != TW_MT_DATA_ACK) printf("errore invio data\n");
	
	return TWSR & 0XF8;
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
	printf(">start\n");
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
