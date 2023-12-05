import serial

# Establish serial communication with the Arduino
arduino = serial.Serial('/dev/ttyACM1', 9600)  # Replace 'COM3' with the appropriate port

# Read the value from the file
with open('optimo-DAC.txt', 'r') as file:
    value = int(file.read().strip())

# Send the value to the Arduino
arduino.write(str(value).encode())

# Close the serial connection
arduino.close()
