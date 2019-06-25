#include <stdio.h>
#include <avr_common/uart.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#include "funzioni.h"

const int MPU=0x68;  // I2C address of the MPU-6050
uint16_t AcX, AcX, AcY, AcY, AcZ, AcZ, Tmp, Tmp, GyX, GyX, GyY, GyY, GyZ, GyZ;

int gx , gy, gz;
#define FREQ 30.0 // sample freq in Hz

double gyrXoffs = -281.00, gyrYoffs = 18.00, gyrZoffs = -83.00;
double gSensitivity = 65.5; // for 500 deg/s, check data sheet

void calibrate(){

  int i, x, y, z;
  int xSum = 0, ySum = 0, zSum = 0; 
  int num = 500;

  for (i = 0; i < num; i++){

	x = read_data(MPU, 0x43)<<8|read_data(MPU, 0x44); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
	y = read_data(MPU, 0x45)<<8|read_data(MPU, 0x46); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
	z = read_data(MPU, 0x47)<<8|read_data(MPU, 0x48); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
	
	xSum += x;
	ySum += y;
	zSum += z;
		
  }
  
  gyrXoffs = xSum / num;
  gyrYoffs = ySum / num;
  gyrZoffs = zSum / num;

  //printf("Calibration: %d , %d, %d\n", gyrXoffs, gyrYoffs, gyrZoffs);
  
} 

int main(void){
	uint8_t sample_div;
	 
	printf_init(); //inizializzo printf
	
	printf("inizio\n");
	init();
	
	//all'accensione il sensore si trova nella sleep-mode
	//per svegliarlo azzero il bit 6 del registro PWR_MGMT_1 di indirizzo 0x6B
	printf("Sveglia\n");
	write_data(MPU, 0x6B, 0x00); //azzero tutto il registro
	
	// CONFIG:
	// Low pass filter samples, 1khz sample rate
	printf("Config\n");
	write_data(MPU, 0x1A, 0x01);

	// GYRO_CONFIG:
	// 500 deg/s, FS_SEL=1
	// This means 65.5 LSBs/deg/s
	printf("Gyro config\n");
	write_data(MPU, 0x1B, 0x08);

	// CONFIG:
	// set sample rate
	// sample rate FREQ = Gyro sample rate / (sample_div + 1)
	// 1kHz / (div + 1) = FREQ  
	// reg_value = 1khz/FREQ - 1
	printf("config sample rate\n");
	sample_div = 1000 / FREQ - 1;
	write_data(MPU, 0x19, sample_div);
	
	printf("Calibrating...\n");
	calibrate();
	printf("done\n");
	
	int i;
	for(i=0; i<5000; i++){
		
		int ax, ay, az;
		
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
		
		GyX = (GyX - gyrXoffs) / gSensitivity;
		GyY = (GyY - gyrYoffs) / gSensitivity;
		GyZ = (GyZ - gyrZoffs) / gSensitivity;
		
		// angles based on accelerometer
		ay = atan2(AcX, sqrt( pow(AcY, 2) + pow(AcZ, 2))) * 180 / M_PI; //pitch
		ax = atan2(AcY, sqrt( pow(AcX, 2) + pow(AcZ, 2))) * 180 / M_PI; //roll
		
		// angles based on gyro (deg/s)
		gx = gx + GyX / FREQ;
		gy = gy - GyY / FREQ;
		gz = gz + GyZ / FREQ;

		// complementary filter
		// tau = DT*(A)/(1-A)
		// = 0.48sec
		gx = gx * 0.96 + ax * 0.04;
		gy = gy * 0.96 + ay * 0.04;

		//printf("fine letture\n");
		//int temp = ((int)Tmp/340.00+36.53);
		//printf("ACCELEROMETRO %d | Y: %d | Z: %d\nTEMPERATURA: %d°C\nGIROSCOPIO X:%d | Y:%d | Z:%d\n", AcX, AcY, AcZ, temp, GyX, GyY, GyZ);
		//printf("> %d %d %d\n", ax, ay, az);
		
		printf("> %d %d %d\n", gx, gy, gz);
		//printf("\n");
	
		gz = 0;
		//_delay_ms(100); //leggo ogni 100ms
	}
	
}
