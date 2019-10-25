import json
import time
from websocket import create_connection
# Needed modules will be imported.
import RPi.GPIO as GPIO
import datetime
import threading
import Adafruit_DHT
# Set sensor type : Options are DHT11,DHT22 or AM2302
sensorTempHum=Adafruit_DHT.DHT11
 
# Set GPIO sensor is connected to
GPIO_TEMP_HUM=17


GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
# The input pin of the Sensor will be declared. 
# Additional to that the pullup resistor will be activated
LED=21 
estado=0
GPIO.setup(LED,GPIO.OUT)



ws = create_connection("ws://192.168.1.106:3333/adonis-ws")
#ws = create_connection("ws://192.168.0.20:3333/ws")
ws.send(json.dumps({
					"t": 1,
					"d": { "topic": "IoT" }
					}))

def horafecha():
	now=datetime.datetime.now()
	return now.strftime("%Y-%m-%d %H:%M:%S")


def send_data_sensor():
	humidity, temperature = Adafruit_DHT.read_retry(sensorTempHum, GPIO_TEMP_HUM)
	GPIO.setup(LED,GPIO.IN)
	estado=GPIO.input(LED)
	GPIO.setup(LED,GPIO.OUT)
	datatemp='{0:0.1f}'.format(temperature)
	datahum='{0:0.1f}'.format(humidity)
	'''
	print(datatemp)
	print(datahum)
	print('Temp={0:0.1f}*C  Humidity={1:0.1f}%'.format(temperature, humidity))
	'''
	data={"data":estado,"time":horafecha(),"temp":datatemp,"hum":datahum}
	data2={"t": 7,"d": { "topic": "IoT","event":"message","data":data}}
	return (estado,temperature,humidity)

class SensoresThread(threading.Thread):

     def __init__(self,ws,tiempo):
         threading.Thread.__init__(self)
         self.ws=ws 
         self.tiempo=tiempo
         self.terminar=False


     def run(self):
     	while not (self.terminar):
     		print ("ENVIADO DATOS RUN .....................")
     		estado,temperature,humidity=send_data_sensor();
     		datos=json.dumps({
										"t": 7,
										"d": { "topic": "IoT","event":"message","data":{"data":estado,"time":horafecha(),"temp":temperature,"hum":humidity}
											 }
										})
     		ws.send(datos)
     	


class SummingThread(threading.Thread):

     def __init__(self,ws,tiempo):
         threading.Thread.__init__(self)
         self.ws=ws 
         self.tiempo=tiempo
         self.terminar=False

     def run(self):
     		print (self.terminar)     		
     		while not (self.terminar):
     			print("Entra")
     			self.ws.send(json.dumps({"t": 8,"d": { "topic": "IoT"}}))
     			print("t:8\n")
     			time.sleep(self.tiempo)
     			

thread1 = SummingThread(ws,18)
sensores = SensoresThread(ws,18)

def getData(data):
	print ("Received '%s'" % data)
	dataResult=None
	t=data["t"]
	print(data)
	if t==7: 
		dataD=result["d"]
		dataData=dataD["data"]
		event=dataD["event"]
		if event==u'message':
			print("message",dataData)
			dataResult=dataData["data"]
			estado=dataResult
			print("dataResult",dataResult)
			GPIO.output(LED,dataResult) 

		else: 
			if(event==u'conecction'):
				
				thread1.start() # This actually causes the thread to run
				sensores.start()
				dataResult=dataData
				estado,temperature,humidity=send_data_sensor();
				datos=json.dumps({
										"t": 7,
										"d": { "topic": "IoT","event":"message","data":{"data":estado,"time":horafecha(),"temp":temperature,"hum":humidity}
											 }
										})
				ws.send(datos)
			else:
				if(event==u'bienvenida'):
					estado,temperature,humidity=send_data_sensor();
					datos=json.dumps({
										"t": 7,
										"d": { "topic": "IoT","event":"message","data":{"data":estado,"time":horafecha(),"temp":temperature,"hum":humidity}
											 }
										})
					ws.send(datos)
	elif t==3:
		dataD=result["d"]
		topic=dataD["topic"]
		print(topic)
		thread1.start() # This actually causes the thread to run
		sensores.start()
		estado,temperature,humidity=send_data_sensor();
		datos=json.dumps({
										"t": 7,
										"d": { "topic": "IoT","event":"message","data":{"data":estado,"time":horafecha(),"temp":temperature,"hum":humidity}
											 }
										})
		ws.send(datos)


	return dataResult
c=0 
while True:
	try:
		result = ws.recv()
		result = json.loads(str(result))
		print("getData",getData(result))
	except KeyboardInterrupt:
		thread1.terminar=True
		sensores.terminar=True
		thread1.join()  # This waits until the thread has completed
		sensores.join()
		GPIO.cleanup()
		ws.close()
