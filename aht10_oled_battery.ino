#include <AHTxx.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SSD1306Wire.h>


#define SCL_PIN D1
#define SDA_PIN D2

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// declare an SSD1306 display object connected to I2C
SSD1306Wire oled(0x3c, SDA_PIN, SCL_PIN); // SDA , SCL 


int x_coord = 0;
int y_coord = 10;

AHTxx aht;


// (3.75 - 3/4, 3.65 - 1/2, 3.5 - 1/4, 3.3 - 0)
// 4.2V - 100%; 4V - 80%; 3.8V - 60%; 3.6V - 40%; 3.4V - 20%

// Battery check 
#define VOLTAGE 3.3  // must be 3.3 it fucks brains
// Pin for reading voltage 
//(it must be connected to plus and minus of battery with two equal resistors 1-10kOm)
#define ANALOG_PIN A0
// Max value of l-ion battery
float max_v = 4.0;   // with two resistors must be 2.1
// Min value of l-ion battery  
float min_v = 3.4;
// Mean value of charge
float mean_charge[10] = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50};


float get_mean_value(float battery) {
  float dummy = 0;
  for (int i = 0; i < 9; ++i) {
    mean_charge[9 - i] = mean_charge[8 - i]; 
    dummy += mean_charge[9 - i];
  }
  mean_charge[0] = battery;
  dummy += battery;
  return dummy / 10;
}


void setup() {

  // analogReference(EXTERNAL);

  Serial.begin(9600);

  if (!oled.init()) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  
  oled.flipScreenVertically();

}


void loop() {
  
  delay(1000);

  auto temp = aht.readTemperature();
  auto humid = aht.readHumidity();

  // get current voltage
  float sensorValue = analogRead(ANALOG_PIN); //read the A0 pin value
  float voltage = sensorValue * (VOLTAGE / 1023.00); //convert the value to a true voltage.
  voltage = voltage / (9.99 / (9.99 + 9.99));  // r1 / (r1 + r2)
  voltage = voltage * 0.96;                    // reduce value because of bugs
  
  // get battery charge
  int battery = (voltage - min_v) / (max_v - min_v) * 100;
  battery = battery >= 100 ? 100 : battery;
  battery = battery <= 0 ? 0 : battery;

  battery = get_mean_value(battery);

  /* ----- SERIAL ----- */
  Serial.println("//Arduino & AHT10//");
  Serial.println("Quick Test – Serial Monitor");

  Serial.println(String("") + "Humidity(%RH):\t\t" + String(humid) + "%");

  Serial.println(String("") + "Temperature(℃):\t" + String(temp) + "℃");
  Serial.println(String("") + "V:\t" + String(voltage) + "\tbat:\t " + battery + "\tSensor:\t" + sensorValue);
  for (int i = 0; i < 10; ++i) {
    Serial.print(String(mean_charge[i]) + " ");
  }
  Serial.println();


  /* ----- DISPLAY ----- */
  oled.clear(); // clear display

  oled.setFont(ArialMT_Plain_10);

  // (String("") + "V: " + String(voltage) + "  Battery " + String(battery) + "%");
  String text_temperature = (String("") + "Temperature(C) " + String(temp) + "%"); // text to display
  String text_humidity = (String("") + "Humidity(%RH)  " + String(humid) + "%"); // text to display

  // drawString(x, y, text)
  oled.drawString(5, 20, text_temperature);  
  oled.drawString(5, 35, text_humidity);

  oled.drawString(10, 2, "V: " + String(voltage));
  int bat_pos = 78;
  if (battery >= 100) bat_pos -= 5;
  if (battery < 10) bat_pos += 5;
  oled.drawString(bat_pos, 2, String(battery) + "%");
  oled.drawRect(102, 4, 20, 8); 
  oled.drawRect(101, 6, 2, 4); 
  oled.fillRect(104, 6, int(16 * battery / 100), 4); 

  oled.display();               // show on OLED

}

