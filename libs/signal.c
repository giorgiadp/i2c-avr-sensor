#include "signal.h"

#include <util/delay.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_SCL 100000UL // SCL frequency
#define Prescaler 1
#define TWBR_val ((((F_CPU / F_SCL) / Prescaler) - 16 ) / 2)

#define I2C_READ 0x01
#define I2C_WRITE 0x00

void init(void){ //recheck questo è per avr328(?)
    
    TWBR = (uint8_t)TWBR_val;
    
    /*//set SCL to 400kHz
    TWSR = 0x00;
    TWBR = 0x0C;
    //enable TWI
    TWCR = (1<<TWEN);*/
    
   /* TWBR = TWI_TWBR;                                  // Set bit rate register. Defined in header file.
  TWDR = 0xFF;                                      // Default content = SDA released.
  TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins.
         (0<<TWIE)|(0<<TWINT)|                      // Disable Interupt.
         (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // No Signal requests.
         (0<<TWWC); */
}

void start(){
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	/* Dopo la start il TWINT flag è settato dall'hardware
	 * status code in TWSR è 0x08 */
	
	while (!(TWCR & (1<<TWINT))); //aspetto che TWINT sia resettato a 0
	if((TWSR & 0xF8) != 0x08) printf("errore start\n");
}

void repeated_start(){
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT))); //aspetto che TWINT sia resettato a 0
	
	/* Dopo la start il TWINT flag è settato dall'hardware
	 * status code in TWSR è 0x08 */
	
	if((TWSR & 0xF8) != 0x10) printf("errore repeated_start\n");
}

void stop(){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

//non lo uso
int status(void){ //per leggere lo status code di TWSR
	int status = TWSR & 0xF8; //leggo i primi 5bit, maschero gli ultimi 3
    return status;
}

void write(uint16_t data){
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT))); //aspetto che la trasmissione sia completata

	// ??? if ((status != 0x18) && (status != 0x20) && (status != 0x38)) printf("errore write\n");
}

//è una semplice write in realtà
void writeSLA_R(uint8_t SLA_R){
	TWDR = SLA_R; //va settato a SLA+R o SLA+W
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
		
	/* stati possibli nel TWSR
	 * 0x38: ho perso SLA+R o il NACK
	 * 0x40: SLA+R trasmesso & ACK ricevuto
	 * 0x48: SLA+R trasmesso & NACK ricevuto
	 */
	
	uint8_t status = TWSR & 0xF8;
	if ((status != 0x38) && (status != 0x40) && (status != 0x48)) printf("errore writeSLA_R\n"); 
}

void writeSLA_W(uint8_t SLA_W){
	TWDR = SLA_W; //va settato a SLA+R o SLA+W
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
		
	/* stati possibli nel TWSR
	 * 0x38: ho perso SLA+R o il NACK
	 * 0x40: SLA+R trasmesso & ACK ricevuto
	 * 0x48: SLA+R trasmesso & NACK ricevuto
	 */
	
	uint8_t status = TWSR & 0xF8;
	if ((status != 0x18) && (status != 0x20) && (status != 0x38)) printf("errore writeSLA_W\n"); 
}

//va bene uint8_t??
uint8_t readACK(void){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}

uint8_t readNACK(void){
	TWCR = (1<<TWINT)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}

//va bene int?
int read_data(){ //i dati ricevuti possono essere letti
	//dal TWDR quando il TWINT flag è settato alto dall'hardware
	//quando ho finito di leggere devo informare lo slave
	//inviando un NACK dopo l'ultimo byte ricevuto
	//poi posso mandare la stop
	int data;
	data = TWDR; //va settato a SLA+R o SLA+W
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
}
