#include <AHTxx.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int x_coord = 0;
int y_coord = 10;

AHTxx aht;


// Battery check
#define VOLTAGE 3.1  // must be 3.3 it fucks brains
// пин для считывания напряжения
#define ANALOG_PIN A0
// максимальный заряд аккумулятора
float max_v = 4.2;   // with two resistors must be 2.1
// минимальный заряд аккумулятора
float min_v = 3.6; 


void setup() {

  // analogReference(EXTERNAL);

  Serial.begin(9600);

  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  
  oled.clearDisplay(); // clear display
  
  oled.setTextSize(1);          // text size
  oled.setTextColor(WHITE);     // text color

}

 

void loop() {

  delay(1000);

  auto temp = aht.readTemperature();
  auto humid = aht.readHumidity();
  // get current voltage
  float sensorValue = analogRead(ANALOG_PIN); //read the A0 pin value
  float voltage = sensorValue * (VOLTAGE / 1024.00); //convert the value to a true voltage.
  voltage = voltage / (9.99 / (9.99 + 9.99));  // r1 / (r1 + r2)
  // get battery charge
  // int battery = map(voltage, min_v, max_v, 0, 100);
  int battery = (voltage - min_v) / (max_v - min_v) * 100;
  battery = battery >= 100 ? 100 : battery;


  Serial.println("//Arduino & AHT10//");
  Serial.println("Quick Test – Serial Monitor");

  Serial.println(String("") + "Humidity(%RH):\t\t" + String(humid) + "%");

  Serial.println(String("") + "Temperature(℃):\t" + String(temp) + "℃");
  Serial.println(String("") + "V:\t" + String(voltage) + "\tbat:\t " + battery + "\tSensor:\t" + sensorValue);


  oled.clearDisplay(); // clear display
  oled.setCursor(x_coord, y_coord);        // position to display
  oled.println(String("") + "Temperature(C) " + String(temp) + "%"); // text to display
  oled.println("");
  oled.println(String("") + "Humidity(%RH)  " + String(humid) + "%"); // text to display
  oled.println("");
  oled.println(String("") + "Battery " + String(battery) + "%" + " V: " + String(voltage));


  oled.display();               // show on OLED

}

