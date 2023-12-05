#include <Wire.h>
#include <math.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_INA219.h>
Adafruit_MCP4725 dac;
Adafruit_INA219 ina219(0x40);

//const float correcV = 33.53;
//const float correcI = 0.8117;
const float correcV = 0.0;
const float correcI = 1.0;

const int n_average = 60; // number of averaged measurements per data point; beta parameter
const int steps = 10;     // number of averaged data points constituting a k-step; delta parameter; 10 steps of MPP run ~2 seconds

void setup() {
  Serial.begin(9600);

  //powering the breakouts if not using custom shield
  //pinMode(2, OUTPUT);//Set 2 as VCC
  //pinMode(3, OUTPUT);//Set 3 as GND
  //pinMode(A3, OUTPUT);//Set A3 as VCC
  //pinMode(A2, OUTPUT);//Set A2 as GND
  //pinMode(12, OUTPUT);//Set A2 as GND
  //digitalWrite(2, HIGH);
  //digitalWrite(3, LOW);
  //digitalWrite(A3, HIGH);
  //digitalWrite(A2, LOW);
  //digitalWrite(12, LOW);

  // For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
  // For Adafruit MCP4725A0 the address is 0x60 or 0x61
  // For Adafruit MCP4725A2 the address is 0x64 or 0x65


  if (! dac.begin(0x60)) {
    Serial.println("Failed to find MCP4725 chip");
    while (1) {
      delay(10);
    }
  }
  dac.begin(0x60);


  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) {
      delay(10);
    }
  }
  ina219.setCalibration_16V_400mA();


  //Serial.println("start");
}

void loop() {

//INA219 calibration
//open MOSFET: Jsc
int countermpp = 4095;

//closed MOSFET: Voc
//int countermpp = 1;

setmpp(countermpp);

}

void setmpp(int countermpp) {

  int var = 0;

  while (var <= steps) {
    dac.setVoltage(countermpp, false);
    delay(10);
    
    float voltageapplied = countermpp * (5000.0 / 4095); //this maps the applied voltage in volts from 0 t 5000 mV

    float busvoltage = 0;
    float current_mA = 0;
    float shuntvoltage = 0;    

    for (int i = 0; i < n_average; i++) {
      busvoltage = busvoltage + ina219.getBusVoltage_V() * 1000;
      current_mA = current_mA + ina219.getCurrent_mA();
      shuntvoltage = shuntvoltage + ina219.getShuntVoltage_mV();
      delay(1);
    }
    float busvoltagesc = busvoltage / n_average;
    float current_mAsc = ( correcI * current_mA ) / n_average;
    float shuntvoltagesc = shuntvoltage / n_average;
    float voltagesc = busvoltagesc + shuntvoltagesc + correcV;
    float power = voltagesc * current_mAsc / 1000.0;
    delay(10);



    Serial.print("MPP"); Serial.print("\t");
    Serial.print(countermpp); Serial.print("\t");
    Serial.print(voltageapplied); Serial.print("\t");
    Serial.print(voltagesc); Serial.print("\t");
    Serial.print(current_mAsc); Serial.print("\t");
    Serial.print(power); Serial.print("\t");
    Serial.print(countermpp); Serial.print("\t");
    Serial.println(power); Serial.print("\t");


    var++;

  }
}
