#include "funzioni.h"
#include "queue.h"

int main(void){
	printf_init();
	
	Queue queue;
	Queue* q = &queue;
	
	init();
	printf("ok\n");
	
	uint8_t Acc_X_H = read_data(0x68, 0x3B);
	uint8_t ACC_X_L = read_data(0x68, 0x3C);
	uint8_t ACC_Y_H = read_data(0x68, 0x3D);
	uint8_t ACC_Y_L = read_data(0x68, 0x3E);
	uint8_t ACC_Z_H = read_data(0x68, 0x3F);
	uint8_t ACC_Z_L = read_data(0x68, 0x40);
	uint8_t TmpH = read_data(0x68, 0x41);
	uint8_t TmpL = read_data(0x68, 0x42);
	uint8_t Gyro_X_H = read_data(0x68, 0x43);
	uint8_t Gyro_X_L = read_data(0x68, 0x44);
	uint8_t Gyro_Y_H = read_data(0x68, 0x45);
	uint8_t Gyro_Y_L = read_data(0x68, 0x46);
	uint8_t Gyro_Z_H = read_data(0x68, 0x47);
	uint8_t Gyro_Z_L = read_data(0x68, 0x48);
	
	
	printf("%2X\n", Acc_X_H);
	printf("%2X\n", ACC_X_L);
	printf("%2X\n", ACC_Y_H);
	printf("%2X\n", ACC_Y_L);
	printf("%2X\n", ACC_Z_H);
	printf("%2X\n", ACC_Z_L);
	printf("---->%2X\n", TmpH);
	printf("---->%2X\n", TmpL);
	printf("%2X\n", Gyro_X_H);
	printf("%2X\n", Gyro_X_L);
	printf("%2X\n", Gyro_Y_H);
	printf("%2X\n", Gyro_Y_L);
	printf("%2X\n", Gyro_Z_H);
	printf("%2X\n", Gyro_Z_L);
	
}
