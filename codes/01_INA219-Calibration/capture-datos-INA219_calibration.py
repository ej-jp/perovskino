import serial, time
#import pandas as pd
import csv
from datetime import datetime


#portpc = '/dev/ttyACM0'
portpc = '/dev/ttyACM1'
#portpc = '/dev/ttyUSB0'

folderdata = 'dataraw/'

baudios = 9600

arduino = serial.Serial(portpc, baudios, timeout=5, write_timeout=1)
arduino.flushInput()



############info recolectada para limpiar el flush
#abrir y cerrar despues de instanciar
arduino.dtr=True
arduino.close()
arduino.open() 
time.sleep(2)
# arduino.flush()



#print("Connected to Arduino port: " + portpc)

while True:
    t = time.localtime()
    decoded_time_for_filename = time.strftime('%Y-%m-%d_%H', t)
    fileName =folderdata + decoded_time_for_filename + ".csv"
    file = open(fileName, "a")
    
    filadatos = arduino.readline().decode().strip()


    tt = datetime.now()
    decoded_time = tt.strftime('%Y-%m-%d %H:%M:%S.%f')
    
    
    print(decoded_time,"\t",filadatos)
    
    with open(fileName, "a", newline='') as f:
        writer = csv.writer(f, delimiter = "\t",escapechar="\t", quoting=csv.QUOTE_NONE)
        #writerow with seperate sensorValue
        writer.writerow([decoded_time, filadatos])
        f.close()
        
