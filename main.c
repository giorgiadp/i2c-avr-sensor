#include <stdio.h>
#include <avr_common/uart.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#include "funzioni.h"

/* SKETCH ARDUINO
 
	#include<Wire.h>
	const int MPU=0x68;  // I2C address of the MPU-6050

	int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

	void setup(){
	  Wire.begin(); //inizializzo la libreria Wire

	  //devo svegliare il sensore (disabilitare sleep-mode)
	  //per farlo azzero il bit 6 del registro PWR_MGMT_1 di indirizzo 0x6B
	  Wire.beginTransmission(MPU);
	  Wire.write(0x6B);  // registro PWR_MGMT_1
	  Wire.write(0);     // setto a 0
	  Wire.endTransmission(true);
	 
	  Serial.begin(9600); //inizializzo la seriale
	}
	
	void loop(){
	
	  Wire.beginTransmission(MPU); 
	  Wire.write(0x3B);  // la lettura inizia dal registro 0x3B (AccX_H)
	  Wire.endTransmission(false);
	  Wire.requestFrom(MPU,14,true);  // leggo 14 registri (6 accelerometro,
									  // 2 temperatura, 6 giroscopio)
	  
	  AcX = Wire.read()<<8|Wire.read(); // 0x3B (ACC_X_H) & 0x3C (ACC_X_L)     
	  AcY = Wire.read()<<8|Wire.read(); // 0x3D (ACC_Y_H) & 0x3E (ACC_Y_L)
	  AcZ = Wire.read()<<8|Wire.read(); // 0x3F (ACC_Z_H) & 0x40 (ACC_Z_L)
	  Tmp = Wire.read()<<8|Wire.read(); // 0x41 (TEMP_H) & 0x42 (TEMP_L)
	  GyX = Wire.read()<<8|Wire.read(); // 0x43 (GYR_X_H) & 0x44 (GYR_X_L)
	  GyY = Wire.read()<<8|Wire.read(); // 0x45 (GYR_Y_H) & 0x46 (GYR_Y_L)
	  GyZ = Wire.read()<<8|Wire.read(); // 0x47 (GYR_Z_H) & 0x48 (GYR_Z_L)
	  
	  Serial.print("ACCELEROMETRO: ");
	  Serial.print("X = "); Serial.print(AcX); Serial.print("   ");
	  // con HEX stampo il valore in esadecimale
	  //Serial.print("X = "); Serial.print(AcX, HEX);
	  
	  Serial.print("Y = "); Serial.print(AcY); Serial.print("   ");
	  //Serial.print("Y = "); Serial.print(AcY, HEX);
	  
	  Serial.print("Z = "); Serial.print(AcZ); Serial.print("   ");
	  //Serial.print("Z = "); Serial.print(AcZ, HEX);
	  Serial.println();
	  
	  // conversione temperatura in gradi centigradi (vedi datasheet)
	  //Serial.print("TEMPERATURA: H -> "); Serial.print(Tmp,HEX); Serial.print("  "); 
	  Serial.print("Temperature: "); Serial.print(Tmp/340.00+36.53); Serial.print(" C "); 
	  Serial.println();
	  
	  Serial.print("GIROSCOPIO: ");
	  Serial.print("X = "); Serial.print(GyX); Serial.print("   ");
	  //Serial.print("X = "); Serial.print(GyX, HEX);
	   
	  Serial.print("Y = "); Serial.print(GyY); Serial.print("   ");
	  //Serial.print("Y = "); Serial.print(GyY, HEX);
	  
	  Serial.print("Z = "); Serial.print(GyZ); Serial.print("   ");
	  //Serial.print("Z = "); Serial.print(GyZ, HEX);
	  Serial.println();Serial.println();
	  
	  delay(333);
	} */

const int MPU=0x68;  // I2C address of the MPU-6050
uint16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

int main(void){
	 
	printf_init(); //inizializzo printf
	
	printf("inizio\n");
	init();
	
	//all'accensione il sensore si trova nella sleep-mode
	//per svegliarlo azzero il bit 6 del registro PWR_MGMT_1 di indirizzo 0x6B
	printf("Sveglia\n");
	write_data(MPU, 0x6B, 0x00); //azzero tutto il registro
	
	int i;
	for(i=0; i<5000; i++){
		
		/* Ogni valore è costruito sulla coppia HIGH e LOW (8 bit ciascuno)
		 * Faccio letture in blocco perchè i registri sono contigui (16 bit)
		 * Il registro di partenza è 0x3B
		 */
		
		AcX = read_data(MPU, 0x3B)<<8|read_data(MPU, 0x3C); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L) 
		AcY = read_data(MPU, 0x3D)<<8|read_data(MPU, 0x3E); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
		AcZ = read_data(MPU, 0x3F)<<8|read_data(MPU, 0x40); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
		
		Tmp = read_data(MPU, 0x41)<<8|read_data(MPU, 0x42); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
		
		GyX = read_data(MPU, 0x43)<<8|read_data(MPU, 0x44); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
		GyY = read_data(MPU, 0x45)<<8|read_data(MPU, 0x46); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
		GyZ = read_data(MPU, 0x47)<<8|read_data(MPU, 0x48); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

		/* ***** STAMPA ARDUINO ***** */
		int temp = ((int)Tmp/340.00+36.53);
		printf("ACCELEROMETRO %d | Y: %d | Z: %d\nTEMPERATURA: %d C\nGIROSCOPIO X:%d | Y:%d | Z:%d\n", AcX, AcY, AcZ, temp, GyX, GyY, GyZ);
		
		/* ***** valori che legge GRAFICO.PY ***** */
		printf("> %d %d %d %d %d %d\n", AcX, AcY, AcZ, GyX, GyY, GyZ);
		printf("\n");
		_delay_ms(500); //leggo ogni mezzo secondo sennò il grafico non
						//riesce a disegnare i valori aggiornati in tempo

	}
	
}
