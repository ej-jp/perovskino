#include <Wire.h>
#include <math.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_INA219.h>
#include "dac_lookup_lists4SOP.h"
//#include <LTR390.h>
//#include <DHT.h>

const char *arduinoID = "ARDUINO123";


//Solar cell testing
Adafruit_MCP4725 dac;
Adafruit_INA219 ina219(0x40);

//Thermistors settings
//Sourcingmap – a14092200ux0209 10 K Ohm 0,05 W
//NTC Thermistor;Model : MF52-103;Insulation Material : Ceramic
//Color : Black;Rated Power : 0.05W;Resistance Value : 10K Ohm
//B Value : 3435(+/-1%);Pin Pitch : 1.5mm / 0.059""
const int Rc = 10000; //resistance value of the voltage divider
const float Vcc = 5.0;//input voltage of the voltage divider (arduino)
const int SensorPIN = A0;//voltage readings for thermistor in solar cell testing
//Thermistor parameters
#define A 1.098E-03
#define  B 2.446E-04
#define  C -3.693E-09


//ina219 calibration parameters
const float correcI = 0.84253;
const float correcV = 27.38;


//settings for JV-SOP MPP algorithm search
const int n_average = 60; //30; number of averaged measurements per data point; beta parameter
const int steps = 60;     //5; number of averaged data points constituting a k-step; delta parameter; 10 steps of MPP run ~2 seconds

//Definition of the optimal DAC integer (VGATE)
int countermpp;

//Timing triggers
unsigned long startMillis;
unsigned long currentMillis;

//const unsigned long longperiod = 360000;  //6 min. Portion of time running at MPP condition between MPP search runs.
const unsigned long longperiod = 60000;  //1 min. Portion of time running at MPP condition between MPP search runs.
const unsigned long shortperiod = 3000; // below this time the system is just starting

int loopCount; //global definition of this counter

void setup() {
  Serial.begin(9600);

  //Adafruit MCP4725A dac checkings
  if (! dac.begin(0x60)) {
    Serial.println("Failed to find MCP4725");
    while (1) {
      delay(10);
    }
  }
  dac.begin(0x60);

  //Adafruit INA219 checkings
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219");
    while (1) {
      delay(10);
    }
  }
  ina219.setCalibration_16V_400mA();

  startMillis = millis();  // setting starting time before void loop()
  loopCount = 1;       // setting initial loop id before void loop()


  // sending arduino ID
  //Serial.println(arduinoID);
}

void loop() {
  currentMillis = millis();

  //search of the MPP voltage
  if  ((currentMillis - startMillis <= shortperiod) || (currentMillis - startMillis >= longperiod)) {
    //if the device has just started up or
    //if the MPP point holding time has passed the long period
    //then launch the MPP search function again.

    // Execute different functions based on the loop count
    if (loopCount >= 1 && loopCount <= 1) {
      mppcalculation(6);
    }
    else if (loopCount >= 2 && loopCount <= 2) {
      mppcalculation(6);
    }
    else if (loopCount >= 3 && loopCount <= 3) {
      mppcalculation(6);
    }

    // Increment loop count
    loopCount++;

    // Reset loop count after reaching 4 loop counts
    if (loopCount >= 4) {
      loopCount = 1;
    }

    startMillis = currentMillis;
  }


  //reading manual input for the DAC
  //int optvalue = 0;
  int optvalue; //var for store +- DAC integers
  if (Serial.available()) {
    optvalue = Serial.parseInt();
    Serial.print("Received value: ");
    Serial.println(optvalue);
    }


  //run SOP stage during longperiod, manual input optvalue accepted
  SOPmpp(countermpp, optvalue);

}

// ################FUNCTIONS####################

void SOPmpp(int countermpp, int optvalue) {
  countermpp = countermpp + optvalue;

  int var = 0;

  while (var <= steps) {
    dac.setVoltage(countermpp, false);
    delay(10);

    float voltageapplied = countermpp * (5000.0 / 4095); //this maps the applied voltage in volts from 0 t 5000 mV
    
    //init variables storing data measured by INA219
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
    float power = voltagesc * current_mAsc / 1000.0; // mW
    delay(10);



    //solar cell temperature measurement
    float temperature = termistorT();


    //serial output
    Serial.print(arduinoID); Serial.print("\t");
    Serial.print("MPP"); Serial.print("\t");
    Serial.print(countermpp); Serial.print("\t");
    Serial.print(voltageapplied); Serial.print("\t");
    Serial.print(voltagesc); Serial.print("\t");
    Serial.print(current_mAsc); Serial.print("\t");
    Serial.print(power); Serial.print("\t");
    Serial.print(countermpp); Serial.print("\t");
    Serial.print(power); Serial.print("\t");
    Serial.println(temperature);

    var++;
  }


}

int mppcalculation(int listNumber) {

  float powermpp = -1.0;// initial value to ensure that it picks up any positive value
  countermpp = 0;// initial value to ensure that it picks up any positive value

  const int* dacLookupList;
  int dacSteps;

  if (listNumber == 1)
  {
    dacLookupList = DACLookup_list1;
    dacSteps = dac_steps1;
    //serial.println("using list1");
  }
  if (listNumber == 2)
  {
    dacLookupList = DACLookup_list2;
    dacSteps = dac_steps2;
    //serial.println("using list2");
  }

  if (listNumber == 6)
  {
    dacLookupList = DACLookup_list6;
    dacSteps = dac_steps6;
    //serial.println("using list6");
  }


  for (int j = 0; j < dacSteps  ; j = j + 1) {

    int counter = pgm_read_word(&(dacLookupList[j]));
    dac.setVoltage(counter, false);// the dac sets the voltage on the mosfet
    delay(10);


    float voltageapplied = counter * (5000.0 / 4095); //this maps the applied voltage in volts from 0 t 5000 mV

    //init variables storing data measured by INA219
    float busvoltage = 0;
    float current_mA = 0;
    float shuntvoltage = 0;


    //n_average: number of measurements to be averaged
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
    float power = voltagesc * current_mAsc / 1000.0; // mW



    //Classic MPP Search. Fetch dac step recording the MPP
    if ((power >= powermpp)) {
      powermpp = power;
      countermpp = counter;
    }

    //solar cell temperature measurement
    float temperature = termistorT();


    //serial output
    Serial.print(arduinoID); Serial.print("\t");
    Serial.print("JV"); Serial.print("\t");
    Serial.print(counter); Serial.print("\t");
    Serial.print(voltageapplied); Serial.print("\t");
    Serial.print(voltagesc); Serial.print("\t");
    Serial.print(current_mAsc); Serial.print("\t");
    Serial.print(power); Serial.print("\t");
    Serial.print(countermpp); Serial.print("\t");
    Serial.print(powermpp); Serial.print("\t");
    Serial.println(temperature);
  }
  return countermpp;
}


float termistorT() {
  float raw = 0;
  for (int i = 0; i < 30; i++) {
    raw = raw + analogRead(SensorPIN);
    delay(1);
  }
  raw = raw / 30.0;
  float V =  raw / 1024 * Vcc;
  float R = (Rc * V ) / (Vcc - V);
  float logRR  = log(R);
  float kelvin = 1.0 / (A + B * logRR + C * logRR * logRR * logRR );
  float celsius = kelvin - 273.15;
  return celsius;
}
