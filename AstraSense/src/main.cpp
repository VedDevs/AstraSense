// Inclusions
#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// definations

// DHT11 - 1
#define DHTPIN 2
#define DHTTYPE DHT11

// DHT11 - 2
#define DHTPIN2 3
#define DHTTYPE2 DHT11

// DHT11 - 3
#define DHTPIN3 4
#define DHTTYPE3 DHT11

// GP2Y dust sensor
#define DUSTPIN A0
#define DUSTLED 8

// Oled
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
bool oledStatus = true;


DHT dht1(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE2);
DHT dht3(DHTPIN3, DHTTYPE3);
Adafruit_BMP085 bmp;
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// Initialliation
void setup(){
  Serial.begin(9600);
  
  dht1.begin();
  dht2.begin();
  dht3.begin();

  if (bmp.begin() == false)
  {
    Serial.println("Error in initializing of BMP180, Check sensor and Wiring");
  }
  
  pinMode(DUSTLED, OUTPUT);
  digitalWrite(DUSTLED, HIGH);

  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
     Serial.println("OLED coudn't be initialized");
     oledStatus = false;
     Serial.println("Oled Status set to false");
  }
  else{
   oled.clearDisplay();
   oled.setTextColor(SSD1306_WHITE);
   oled.setTextSize(1);
   oled.display();
  }

}

// Loop
void loop(){
  float temp1;
  float hum1;
  float temp2;
  float hum2;
  float temp3;
  float hum3;
  float avgtemp;
  float avghum;

  float press;
  float Alt;
  float pressh;

  int dustRAW;
  float dustVoltage;
  float DustDensity;
  String aqi;

  // DHT11 - 1
  temp1 = dht1.readTemperature();
  hum1 = dht1.readHumidity();

  // DHT11 - 2
  temp2 = dht2.readTemperature();
  hum2 = dht2.readHumidity();

  // DHT11 - 3
  temp3 = dht3.readTemperature();
  hum3 = dht3.readHumidity();

  // BMP180
  press = bmp.readPressure();
  Alt = bmp.readAltitude();


   
  // Validation
  if(isnan(temp1) || isnan(hum1)){
    Serial.println("Reading too fast from DHT11 - 1, Increase Delay time or Check Wiring, Power and sensor");
    delay(2000);
    return;
  }  
  if(isnan(temp2) || isnan(hum2)){
    Serial.println("Reading too fast from DHT11 - 2, Increase Delay time or Check Wiring, Power and sensor");
    delay(2000);
    return;
  }
    if(isnan(temp3) || isnan(hum3)){
    Serial.println("Reading too fast from DHT11 - 3, Increase Delay time or Check Wiring, Power and sensor");
    delay(2000);
    return;
  }
  // Calculating Average
  avgtemp = (temp1 + temp2 + temp3)/3.0;
  avghum = (hum1 + hum2 + hum3)/3.0;
  //Conversion of Pa to hPa
  pressh = press/100.0;
  // Measuring Dust levels
  digitalWrite(DUSTLED,LOW);
  delayMicroseconds(280);
  dustRAW = analogRead(DUSTPIN);
  delayMicroseconds(40);
  digitalWrite(DUSTLED, HIGH);
  delayMicroseconds(9680);

  // converting to volltage
  dustVoltage = (dustRAW * 5.0)/1023.0;

  // Converting to DustDensity
  DustDensity = (0.17 * dustVoltage - 0.1);

  // classifying AQI
  if (DustDensity < 0)
  {
    DustDensity = 0;
  }

  if (DustDensity <= 0.05)
  {
    aqi = "Good";
  }
  else if (DustDensity <= 0.1)
  {
    aqi = "Moderate";
  }
  else if (DustDensity <= 0.2)
  {
    aqi = "Poor";
  }
  else{
    aqi = "Hazardous";
  }
  
  

  // Printing temp and hum
  Serial.print("Temprature: ");
  Serial.print(temp1);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(hum1);
  Serial.println(" %");  

  Serial.println(" ");

  Serial.print("Temprature 2 : ");
  Serial.print(temp2);
  Serial.println(" °C");

  Serial.print("Humidity 2: ");
  Serial.print(hum2);
  Serial.println(" %");  

  Serial.println(" ");

  Serial.print("Temprature 3: ");
  Serial.print(temp3);
  Serial.println(" °C");

  Serial.print("Humidity 3: ");
  Serial.print(hum3);
  Serial.println(" %");

  Serial.println(" ");
 
  // Printing AVG TEMP and HUM
  Serial.print("AVG Temp: ");
  Serial.print(avgtemp);
  Serial.println(" °C");

  Serial.print("AVG Hum: ");
  Serial.print(avghum);
  Serial.println(" %");
  
  // Printing Pressure and Altitude
  Serial.print("pressure: ");
  Serial.print(pressh);
  Serial.println(" hPa");

  Serial.print("Altitude: ");
  Serial.print(Alt);
  Serial.println(" m");

  // printing RAW values for GP2Y dust sensor
  Serial.print("Dust Raw: ");
  Serial.println(dustRAW);

  Serial.print("Dust Voltage: ");
  Serial.print(dustVoltage);
  Serial.println(" V");
  
  Serial.print("Dust Density: ");
  Serial.print(DustDensity);
  Serial.println(" mg/m3");

  //printing AQI
  Serial.print("Air Quality: ");
  Serial.println(aqi);

  //Oled
  if (oledStatus == true)
  {
    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.print("T:");
    oled.print(avgtemp);
    oled.print("C");

    oled.setCursor(0,10);
    oled.print("H:");
    oled.print(avghum);
    oled.print("%");

    oled.setCursor(0,20);
    oled.print("P:");
    oled.print(pressh);
    oled.print("hPa");

    oled.setCursor(0,30);
    oled.print("Alt:");
    oled.print(Alt);
    oled.print("m");

    oled.setCursor(0,40);
    oled.print("AQI:");
    oled.print(aqi);

    oled.display();
  }
  

  delay(2000);
}
