import serial
import matplotlib.pyplot as plt 
from drawnow import *

arduino = serial.Serial('/dev/ttyACM0', 19200, timeout=1)
plt.ion()

#Accelerometro
AccXarray = []
AccYarray = []
AccZarray = []

#Giroscopio
GyrXarray = []
GyrYarray = []
GyrZarray = []

def grafico():
	

	plt.subplot(2,1,1)
	plt.title('Accelerometro')
	plt.grid(True)
	plt.plot(AccXarray)
	plt.plot(AccYarray)
	plt.plot(AccZarray)
	plt.plot(AccXarray, 'r' , label = 'AcX')
	plt.plot(AccYarray, 'b' , label = 'AcY')
	plt.plot(AccZarray, 'g' , label = 'AcZ')
	plt.legend(loc='upper left')
	
	plt.subplot(2,1,2)
	plt.title('Giroscopio')
	plt.grid(True)
	plt.plot(GyrXarray)
	plt.plot(GyrYarray)
	plt.plot(GyrZarray)
	plt.plot(GyrXarray, 'r' , label = 'GyX')
	plt.plot(GyrYarray, 'b' , label = 'GyY')
	plt.plot(GyrZarray, 'g' , label = 'GyZ')
	plt.legend(loc='upper left')
			
def main():
    while True:
		count = 0
		array = arduino.readline ().split()
		# in seriale mi faccio arrivare i valori AcX, AcY, AcZ, GyX, GyY, GyZ
		# in sequenza preceduti dal marcatore ">"
		# in questo modo all'inizio se ho la seriale sporca non mi blocco
		for i in array:
			if i == '>':
				x = array[1:]
				if(len(x) == 6):
					print(x)
				
					AccX = float(x[0])
					AccXarray.append(AccX)   
					AccY = float(x[1])
					AccYarray.append(AccY)
					AccZ = float(x[2])
					AccZarray.append(AccZ)
				
					GyrX = float(x[3])
					GyrXarray.append(GyrX)   
					GyrY = float(x[4])
					GyrYarray.append(GyrY)
					GyrZ = float(x[5])
					GyrZarray.append(GyrZ)
				
					#ogni 100 valori pulisco gli array
					count = count+1
					if(count > 100):
						AccXarray.pop(0)
						AccYarray.pop(0)
						AccZarray.pop(0)
						GyrXarray.pop(0)
						GyrYarray.pop(0)
						GyrZarray.pop(0)					
				
					drawnow(grafico)

if __name__ == '__main__': main()
