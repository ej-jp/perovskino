#include <Wire.h>
#include <math.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_INA219.h>
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


//Forward fast voltage scan
const int dac_steps1 = 121;
const PROGMEM int DACLookup_list1[dac_steps1] =
{
  1875, 1870, 1865, 1860, 1855, 1850, 1845, 1840, 1835, 1830, 1825, 1820, 1815, 1810, 1805, 1800, 1795, 1790, 1785, 1780, 1775, 1770, 1765, 1760, 1755, 1750, 1745, 1740, 1735, 1730, 1725, 1720, 1715, 1710, 1705, 1700, 1695, 1690, 1685, 1680, 1675, 1670, 1665, 1660, 1655, 1650, 1645, 1640, 1635, 1630, 1625, 1620, 1615, 1610, 1605, 1600, 1595, 1590, 1585, 1580, 1575, 1570, 1565, 1560, 1555, 1550, 1545, 1540, 1535, 1530, 1525, 1520, 1515, 1510, 1505, 1500, 1495, 1490, 1485, 1480, 1475, 1470, 1465, 1460, 1455, 1450, 1445, 1440, 1435, 1430, 1425, 1420, 1415, 1410, 1405, 1400, 1395, 1390, 1385, 1380, 1375, 1370, 1365, 1360, 1355, 1350, 1345, 1340, 1335, 1330, 1325, 1320, 1315, 1310, 1305, 1300, 1295, 1290, 1285, 1280, 1275
};
//Backward fast voltage scan
const int dac_steps2 = 121;
const PROGMEM int DACLookup_list2[dac_steps2] =
{
  1275, 1280, 1285, 1290, 1295, 1300, 1305, 1310, 1315, 1320, 1325, 1330, 1335, 1340, 1345, 1350, 1355, 1360, 1365, 1370, 1375, 1380, 1385, 1390, 1395, 1400, 1405, 1410, 1415, 1420, 1425, 1430, 1435, 1440, 1445, 1450, 1455, 1460, 1465, 1470, 1475, 1480, 1485, 1490, 1495, 1500, 1505, 1510, 1515, 1520, 1525, 1530, 1535, 1540, 1545, 1550, 1555, 1560, 1565, 1570, 1575, 1580, 1585, 1590, 1595, 1600, 1605, 1610, 1615, 1620, 1625, 1630, 1635, 1640, 1645, 1650, 1655, 1660, 1665, 1670, 1675, 1680, 1685, 1690, 1695, 1700, 1705, 1710, 1715, 1720, 1725, 1730, 1735, 1740, 1745, 1750, 1755, 1760, 1765, 1770, 1775, 1780, 1785, 1790, 1795, 1800, 1805, 1810, 1815, 1820, 1825, 1830, 1835, 1840, 1845, 1850, 1855, 1860, 1865, 1870, 1875
};

//ina219 calibration parameters
const float correcI = 0.84253;
const float correcV = 27.38;


//settings for PO-MPP algorithm search
const int n_average = 60; //30; number of averaged measurements per data point; beta parameter
const int steps = 10;     //5; number of averaged data points constituting a k-step; delta parameter; 10 steps of MPP run ~2 seconds
const int stepcounter = 2;//2;  steps increment; epsilon parameter

//Definition of the optimal DAC integer (VGATE)
int countermpp;

//Timing triggers
unsigned long startMillis;
unsigned long currentMillis;

const unsigned long longperiod = 10800000;  //180 min. Portion of time running at MPP condition between MPP search runs.
const unsigned long shortperiod = 3000; // below this time the system is just starting


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

  startMillis = millis();
  
    // sending arduino ID
  Serial.println(arduinoID);
}

void loop() {
  currentMillis = millis();

  //search of the MPP voltage
  if  ((currentMillis - startMillis <= shortperiod) || (currentMillis - startMillis >= longperiod)) {
    //if the device has just started up or
    //if the MPP point holding time has passed the long period
    //then launch the MPP search function again.
    countermpp = mppcalculation();//JV and countermpp determination
    
    //Alternative to JV
    //countermpp = 1685;//skip JV

    startMillis = currentMillis;
  }

  // P&O MPPT algorithm
  countermpp = POmpp(countermpp);


}

// ################FUNCTIONS####################


int POmpp(int countermpp) {

  //variable storing the average power and voltage of each while run.
  float power_avg1 = 0.0;
  float power_avg2 = 0.0;
  float power_avg3 = 0.0;
  float voltg_avg1 = 0.0;
  float voltg_avg2 = 0.0;
  float voltg_avg3 = 0.0;

  // First while run, parameters definition
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
    float power = voltagesc * current_mAsc / 1000.0; // mW
    delay(10);

    // Averaged power and voltage in the while loop
    if (var <= steps && var > 0) {// discards first measurement. 
      power_avg1 = power_avg1 + power;
      voltg_avg1 = voltg_avg1 + voltagesc;
    }
    if (var == steps) {
      power_avg1 = power_avg1 / steps;
      voltg_avg1 = voltg_avg1 / steps;
    }

    //solar cell temperature measurement each step in var while
    float temperature = termistorT();


    //serial output
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

  //Serial.print("power_avg1: "); Serial.println(power_avg1);

  // Second while run, parameters definition
  var = 0;//restart variable
  countermpp = countermpp + stepcounter;// countermpp + epsilon parameter

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
    float power = voltagesc * current_mAsc / 1000.0; // mW
    delay(10);

    // Averaged power and voltage in the while loop
    if (var <= steps && var > 0) {
      power_avg2 = power_avg2 + power;
      voltg_avg2 = voltg_avg2 + voltagesc;
    }                       
    if (var == steps) {     
      power_avg2 = power_avg2 / steps;
      voltg_avg2 = voltg_avg2 / steps;
    }

    //solar cell temperature measurement each step in var while
    float temperature = termistorT();


    //serial output
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

  // Third while run, parameters definition
  var = 0;
  countermpp = countermpp - stepcounter;//comeback to original countermpp from while no. 1
  
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

    // Averaged power and voltage in the while loop
    if (var <= steps && var > 0) {
      power_avg3 = power_avg3 + power;
      voltg_avg3 = voltg_avg3 + voltagesc;
    }
    if (var == steps) {
      power_avg3 = power_avg3 / steps;
      voltg_avg3 = voltg_avg3 / steps;
    }

    //solar cell temperature measurement
    float temperature = termistorT();


    //serial output
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


  //  P&O: choice of next countermpp
  float diffpower_avg =  power_avg2 - power_avg1;//difference detected in power between n and n + 1 step
  float diffvoltg_avg =  voltg_avg2 - voltg_avg1;//difference detected in voltage between n and n + 1 step
  float diffpower_avg_rcheck = power_avg3 - power_avg1;//difference detected in power  between n and n 
  
  countermpp = countermpp + stepcounter;//counter for the second while


  if (diffpower_avg >= 0.0 && diffvoltg_avg >= 0.0 )   {
    countermpp = countermpp;// if n+1 counter produces large power, set as the new countermpp for the next.
    if (countermpp >= 1800)   {
        return countermpp = 1800;//keeps limits
    }
    if (diffpower_avg_rcheck < 0.0) {
        return countermpp = countermpp - stepcounter - stepcounter; // if rechecking detects decrease of power, go back.
    }
    return countermpp;
  }
  
  if (diffpower_avg >= 0.0 && diffvoltg_avg < 0.0 )   {
    countermpp = countermpp;// if n+1 counter produces large power, set as the new countermpp for the next.
    if (countermpp >= 1800)   {
        return countermpp = 1800;//keeps limits
    }
    if (diffpower_avg_rcheck < 0.0) {
        return countermpp = countermpp - stepcounter - stepcounter; // if rechecking detects decrease of power, go back.
    }
    return countermpp;
  }
 
  if (diffpower_avg < 0.0 && diffvoltg_avg >= 0.0 )   {
    countermpp = countermpp - stepcounter - stepcounter;// if n+1 counter produces lower power, countermpp backs to n-1.
    if (countermpp <= 1200)   {
        return countermpp = 1200;//keeps limits
    }
    if (diffpower_avg_rcheck > 0.0) {
        return countermpp = countermpp + stepcounter + stepcounter; // if rechecking detects increase of power, go ahead.
    }
    return countermpp;
  }
  
  if (diffpower_avg < 0.0 && diffvoltg_avg < 0.0 )   {
    countermpp = countermpp - stepcounter - stepcounter;// if n+1 counter produces lower power, countermpp backs to n-1.
    if (countermpp <= 1200)   {
        return countermpp = 1200;//keeps limits
    }
    if (diffpower_avg_rcheck > 0.0) {
        return countermpp = countermpp + stepcounter + stepcounter; // if rechecking detects increase of power, go ahead.
    }
    return countermpp;
  }
}


int mppcalculation() { // JV loop with initial MPP determination
  float powermpp;// definition
  int countermpp = 0;// initial value to ensure that it picks up any positive value



  //Backward MPP determination
  powermpp = 0.0;// initial value to ensure that it picks up any positive value
  int countermpp_bwd = 0;// definition


  for (int j = 0; j < dac_steps2  ; j = j + 1) {
    int counter = pgm_read_word(&(DACLookup_list2[j]));
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


    //Classic MPP Search. Fetch dac step recording the MPP  for the BWD curve
    if ((power >= powermpp)) {
      powermpp = power;
      countermpp_bwd = counter;
    }

    //solar cell temperature measurement
    float temperature = termistorT();


    //serial output
    Serial.print("JV"); Serial.print("\t");
    Serial.print(counter); Serial.print("\t");
    Serial.print(voltageapplied); Serial.print("\t");
    Serial.print(voltagesc); Serial.print("\t");
    Serial.print(current_mAsc); Serial.print("\t");
    Serial.print(power); Serial.print("\t");
    Serial.print(countermpp_bwd); Serial.print("\t");
    Serial.print(powermpp); Serial.print("\t");
    Serial.println(temperature);
  }


  //Forward MPP determination
  powermpp = 0.0;// initial value to ensure that it picks up any positive value
  int countermpp_fwd = 0;// definition


  for (int j = 0; j < dac_steps1  ; j = j + 1) {
    int counter = pgm_read_word(&(DACLookup_list1[j]));
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


    //Classic MPP Search. Fetch dac step recording the MPP for the FWD curve
    if ((power >= powermpp)) {
      powermpp = power;
      countermpp_fwd = counter;
    }

    //solar cell temperature measurement
    float temperature = termistorT();


    //serial output
    Serial.print("JV"); Serial.print("\t");
    Serial.print(counter); Serial.print("\t");
    Serial.print(voltageapplied); Serial.print("\t");
    Serial.print(voltagesc); Serial.print("\t");
    Serial.print(current_mAsc); Serial.print("\t");
    Serial.print(power); Serial.print("\t");
    Serial.print(countermpp_fwd); Serial.print("\t");
    Serial.print(powermpp); Serial.print("\t");
    Serial.println(temperature);
  }
  // Average calculation between counter during MPP of the FWD and BWD branch
  int sum = countermpp_fwd + countermpp_bwd;
  countermpp = sum / 2 - 10; // temperature or fast/slow scan effect. 
  //countermpp = sum / 2 ; // conventional
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
