import serial, time
import serial.tools.list_ports
import csv
from datetime import datetime
import re

folderdata = 'dataraw/'
baudios = 9600

def find_arduino_port(arduino_id, max_attempts=5):
    #get all ports 
    ports = serial.tools.list_ports.comports()
    # Imprimir todos los puertos detectados para depuración
    print("Puertos detectados:")
    for port in ports:
        print(port.device)
    # Filtering only to match /dev/ACMX or /dev/USBX
    pattern = re.compile(r'/dev/(ttyACM|ttyUSB)\d+')
    filtered_ports = [port.device for port in ports if pattern.match(port.device)]
    print("Puertos Filtered:")
    print(filtered_ports)
    
    
    for port in filtered_ports:
        try:
            ser = serial.Serial(port, baudios, timeout=1)
            time.sleep(2)  # Espera para que la conexión serial se establezca
            for _ in range(max_attempts):
                line = ser.readline().decode(errors='ignore').strip()
                if arduino_id in line:
                    ser.close()
                    return port
            ser.close()
        except serial.SerialException:
            pass
    return None

# Arduino ID
expected_arduino_id = "ARDUINO123"

#finding the dev
arduino_port = find_arduino_port(expected_arduino_id)

if arduino_port:
    print(f"Arduino found in {arduino_port}")
    # Open serial
    with serial.Serial(arduino_port, baudios, timeout=1) as arduino:
        arduino.readline() #ignore first line containing empty data
        arduino.readline() #ignore second line containing the id
        while True:
            t = datetime.now()
            decoded_time_for_filename = t.strftime('%Y-%m-%d_%H')
            decoded_time_for_dataline = t.strftime('%Y-%m-%d %H:%M:%S.%f')
            fileName = folderdata + decoded_time_for_filename + ".csv"

            # reading full line
            line = arduino.readline().decode().strip()

            # Print and save data into CSV file
            print(decoded_time_for_dataline, "\t", line)
            with open(fileName, "a", newline='') as f:
                writer = csv.writer(f, delimiter="\t", escapechar="\t", quoting=csv.QUOTE_NONE)
                writer.writerow([decoded_time_for_dataline, line])
else:
    print("Arduino not found in port.")

