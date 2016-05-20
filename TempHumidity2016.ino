// 20/05/2016 - Written by Matteo Pasqualin

#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 6        // digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11 sensor

// lcd display object instance
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// dht sensor object instance
DHT dht(DHTPIN, DHTTYPE);

// delay time settings
int stepDelay       = 100;
int shortDelay      = 200;
int mediumDelay     = 500;
int oneSecDelay     = 1000;
int longDelay       = 3000;
int nCounter        = 3;

// variables used in the sketch
float humidity      = 0;
float tempFahr      = 0;
float tempCels      = 0;  
float tempHeatC     = 0;
float tempHeatF     = 0;
float tempMIN       = 0;
float tempMAX       = 0;
bool  isFirstExec   = true;

//String txtTemp      = "";   // Temperature text
//String txtUmidita   = "";   // Humidity text
//String txtHeatTemp  = "";   // Heat Temperature text

//
// ******************************************************* [LOGIC] **************************************************
//

// debug
void debugDataCelsius(float h, float t, float hit) {
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");

  Serial.print("Heat Temp.: ");
  Serial.print(hit, 1);
  Serial.print(" *C ");

  Serial.println("\t");
}

String getTextTemparature() {
  String txt = "TEMP: ";
  txt.concat(String(tempCels, 0));
  txt.concat((char)223);          // ASCII degree ° char http://www.rapidtables.com/code/text/ascii-table.htm
  txt.concat("C");
  return txt;
}

String getTextHumidity() {
  return "UMIDITA': " + String(humidity, 0) + "%";
}

String getTextHeatTemp() {
  String txt  = "H.TEMP: ";
  txt.concat(String(tempHeatC, 1));
  txt.concat((char)223);          
  txt.concat("C");
  return txt;
}

String getTextMinMaxTemp() {
  String txt = "MAX:";
  txt.concat(String(tempMAX, 0));
  txt.concat((char)223);          
  txt.concat(" ");
  txt.concat("MIN:");
  txt.concat(String(tempMIN, 0));
  txt.concat((char)223);
  return txt;
}

void readSensorData() {
  //Serial.println("DHT11 sensor data request...");

  humidity = dht.readHumidity();            // Read humidity data

  tempCels = dht.readTemperature();         // Read temperature as Celsius

  //tempFahr = dht.readTemperature(true);   // Read temperature as Fahrenheit - not used

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(tempCels) || isnan(tempFahr)) {
    //Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  //tempHeatF = dht.computeHeatIndex(tempFahr, humidity);

  // Compute heat index in Celsius (isFahreheit = false)
  tempHeatC = dht.computeHeatIndex(tempCels, humidity, false);

  if(isFirstExec){
    tempMAX = tempMIN = tempCels;
    isFirstExec = false;
  }

  // set max temp.
  if (tempCels != tempMAX && tempCels > tempMAX) {
      tempMAX = tempCels;
  }
  
  // set min temp.
  if (tempCels != tempMIN && tempCels < tempMIN) {
      tempMIN = tempCels;
  }
}

void printLcdTemperatureRow() {
  
  lcd.setCursor(0, 0);                // moving to first row
  
  lcd.print(getTextTemparature());    // it always prints temp. in first row
}

void printLcdDataRow() {
  
  lcd.setCursor(0, 1);                // moving to second row
  
  // it checks what to print
  if (nCounter == 0) {
    lcd.print(getTextHumidity());     // if loop #1 it prints humidity in second row
  } else if (nCounter == 1) {
    lcd.print(getTextHeatTemp());     // if loop #2 it prints heat temp.
  } else if (nCounter == 2) {
    lcd.print(getTextMinMaxTemp());   // if loop #3 it prints min/max temp.
  }
}

void scrollText(){
  // scroll text to the right
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    lcd.scrollDisplayRight();
    delay(stepDelay);
  }
}


//
//
//

//
// ******************************************************* [SETUP] ****************************************************
//

void setup() {
  
  // Serial.begin(9600);                // debug
  
  lcd.begin(16, 2);                     // set up the LCD's number of columns and rows:

  dht.begin();

  //Serial.println("DHT11 Started..."); // debug
}



//
// ******************************************************** [LOOP] *****************************************************
//

void loop() {

  lcd.clear();

  if (nCounter > 2) {
    nCounter = 0;       // reset counter
    readSensorData();   // every 3 loops it reads data from sensor
  }

  //debugDataCelsius(humidity, tempCels, tempHeatC);

  printLcdTemperatureRow();
  printLcdDataRow();
  delay(longDelay);

  lcd.clear();

  delay(mediumDelay);

  printLcdTemperatureRow();
  printLcdDataRow();
  delay(longDelay);

  scrollText();

  nCounter++;

  delay(mediumDelay);
}
