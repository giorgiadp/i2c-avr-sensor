#include "funzioni.h"

#define F_CPU 16000000L
#define F_SCL 100000L // SCL frequency

#define TW_READ    		1
#define TW_WRITE      	0

//Equivalente della beging() della libreria Wire di Arduino
//Setta i pin per la TWI e il bitrate
void init(void){
	
	//NON DEVO SETTARLE IO!!!
	//DDRD &= ~(1<<PD1); //input sda
	//DDRD &= ~(1<<PD0); //input scl
	
	//digitalWrite(SDA,1) & digitalWrite(SCL,1)
	PORTD |= (1<<PD1); //SDA: se input attivo i resistori di pullup, se output leggo
	PORTD |= (1<<PD0); //SCL: se input attivo i resistori di pullup, se output leggo
	
	//inizializzzo il prescaler e il bit rate
	TWSR &= ~(1<<TWPS0);
	TWSR &= ~(1<<TWPS1);
	
    TWBR = ((F_CPU / F_SCL) - 16 ) / 2;
    
    //attivo il modulo TWI e gli interrupt
    TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
 
}

//segnale di START per iniziare la comunicazione
uint8_t start(uint8_t slave_add, uint8_t mode){
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT))); //aspetto che TWINT sia resettato a 0 (pulizia)

	/* Dopo la start il TWINT flag è settato dall'hardware
	 * lo status code in TWSR è 0x08 */
	if((TWSR & 0xF8) != TW_START){
		printf("Errore nella start\n"); //check TWI Status Register
		return TWSR & 0xF8;
	}
	
	TWDR = (slave_add << 1) | mode; //slave_address(7bit) + R/W(1 bit)
	TWCR = (1<<TWINT) | (1<<TWEN); //pulizia TWINT
		
	if(((TWSR & 0xF8) == TW_MT_SLA_NACK) || ((TWSR & 0xF8) == TW_MT_DATA_NACK)){
		printf("Errore invio SLA+W\n");
		return TWSR & 0xF8;
	}
	
	while (!(TWCR & (1<<TWINT))); //aspetto che TWINT sia resettato a 0 (pulizia)

	return (TWSR & 0xF8);
}

//segnale di REPEATED START per no cambiare modalità operativa
//senza perdere il controllo del bus
uint8_t rp_start(uint8_t slave_add, uint8_t mode){
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT))); //aspetto che TWINT sia resettato a 0 (pulizia)

	/* Dopo la start il TWINT flag è settato dall'hardware
	 * status code in TWSR è 0x10 */
	if((TWSR & 0xF8) != TW_REP_START){
		printf("Errore nella start\n"); //check TWI STatus Register
		return TWSR & 0xF8;
	}
	
	TWDR = (slave_add << 1) | mode; //slave_address(7bit) + R/W(1 bit)
	TWCR = (1<<TWINT) | (1<<TWEN);

	if(((TWSR & 0xF8) == TW_MT_SLA_NACK) || ((TWSR & 0xF8) == TW_MT_DATA_NACK)){
		printf("Errore invio SLA+R\n");
		return TWSR & 0xF8;
	}
	
	while (!(TWCR & (1<<TWINT)));

	return TWSR & 0xF8;
}

//carico il data packet nel TWDR
uint8_t write(uint8_t data){
	TWDR = data;
	
	TWCR = (1 << TWINT) | (1 << TWEN); //segnalo l'invio
	while(!(TWCR & (1 << TWINT))); //aspetto che TWINT sia resettato a 0
	
	if((TWSR & 0XF8) != TW_MT_DATA_ACK) printf("errore invio data\n");
	
	return TWSR & 0XF8;
}

//macro per l'invio di un pacchetto
void write_data(uint8_t slave_add, uint8_t reg_add, uint8_t data){
	start(slave_add, TW_WRITE);
	
	write(reg_add);
	
	write(data);
	
	stop();
}

//l'informazione ricevuta è nel TWDR, posso continuare a inviare/ricevere
uint8_t readACK(){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	
    while (!(TWCR & (1<<TWINT)));

    return TWDR;
}

//sono a fine trasmissione, l'informazione inviata/ricevuta è nel TWDR
uint8_t readNACK(void){
	TWCR = (1<<TWINT)|(1<<TWEN);
	
    while (!(TWCR & (1<<TWINT)));
    
    return TWDR;
}

//segnale di STOP
void stop(){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	while (TWCR & (1<<TWSTO));
}

//macro per la lettura di un pacchetto
uint8_t read_data(uint8_t slave_add, uint8_t reg_add){
	/* I dati ricevuti possono essere letti dal TWDR
	 * quando il TWINT flag è settato alto dall'hardware.
	 * Quando ho finito di leggere devo informare lo slave
	 * inviando un NACK dopo l'ultimo byte ricevuto,
	 * poi posso mandare la stop */
	 
	//printf(">start\n");
	start(slave_add, TW_WRITE);
	
	//printf(">>write reg_add\n");
	write(reg_add);
	
	//printf(">>>rp_start\n");
	rp_start(slave_add, TW_READ);
	
	//printf(">>>>read\n");
	uint8_t res = readNACK();
	
	//printf(">>>>>stop\n");
	stop();
	
	return res;
}
