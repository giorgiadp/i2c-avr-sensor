#include <stdio.h>
#include <avr_common/uart.h>
#include <avr/io.h>
#include <util/delay.h>

#include "funzioni.h"

const int MPU=0x68;  // I2C address of the MPU-6050
uint16_t AcX, AcX, AcY, AcY, AcZ, AcZ, Tmp, Tmp, GyX, GyX, GyY, GyY, GyZ, GyZ;


int main(void){
	printf_init(); //inizializzo printf
	
	printf("inizio\n");
	init();
	
	int i;
	for(i=0; i<20; i++){
		
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
		
		printf("fine letture\n");
		printf("ACCELEROMETRO: AcX -> %2X\n", AcX);
		printf("AcY -> %2X\n", AcY);
		printf("AcZ -> %2X\n", AcZ);
		printf("\n");
		printf("TEMPERATURA: Tmp -> %2X\n", Tmp); //gradi centigradi
		int temp = ((int)Tmp/340.00+36.53);
		printf("CI SONO %d C\n", temp);
		printf("\n");
		printf("GIROSCOPIO: GyX_H -> %d\n", GyX);
		printf("GyY -> %d\n", GyY);
		printf("GyZ -> %d\n", GyZ);
		printf("\n");
		printf("\n");
		printf("\n");
		_delay_ms(100); //leggo ogni 100ms
	}
	
}
