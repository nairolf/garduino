// Libraries
// Display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Temperature/Humidity
#include <DHT.h>
// Watchdog
#include <avr/wdt.h>

// Constants
// Temp/Hum sensor
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
// Display
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
// Relay
#define RELAY_PIN 7 // pin to operate relay

// values for soil moisture sensor calibration 
// completly dry and wet
const int AirValue = 765; 
const int WaterValue = 364;

const long intervalSensor = 1000; // interval at which to check sensors (milliseconds)
const long intervalDisplay = 4000; // interval at which to update display (milliseconds)

// Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value
int soilMoistureValue = 0;  // analog soild moisture value
int soilMoisturePercent = 0; // calculated percentage value
unsigned long previousMillisSensor = 0; // will store last time sensors was updated
unsigned long previousMillisDisplay = 0; // will store last time display was updated

int displayCounter = 0;

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps

  // initalise temp/hum sensor
  dht.begin();

  // initalise display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);

  // some startup text just for fun....
  display.clearDisplay();
  display.setCursor(5, 10);
  display.setTextSize(2);
  display.println("X-303.net");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setCursor(5, 10);  
  display.setTextSize(1);
  display.println("Cucumber One V0.1");
  display.display();
  delay(1000);
  display.clearDisplay();
}

void loop() {
  // check to see if it's time to check the sensors; that is, if the difference
  // between the current time and last time you checked is bigger than
  // the interval at which you want to check the sensors.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisSensor >= intervalSensor) {
    // save the last time you checked the sensors
    previousMillisSensor = currentMillis;
    readSensors();
  }

  if (currentMillis - previousMillisDisplay >= intervalDisplay) {
    // save the last time you updates the display
    previousMillisDisplay = currentMillis;
    outputOnDisplay();
  }
}

void readSensors() 
{
  soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
  // Serial.println(soilMoistureValue);
  soilMoisturePercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  // Serial.println(soilmoisturepercent);

  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");

  handleRelay();
}

void handleRelay()
{
  // relay logic  
  if (soilMoisturePercent >= 100) {
    Serial.println("100 %");
  } else if(soilMoisturePercent <= 0) {
    Serial.println("0 %");
  } else if(soilMoisturePercent > 0 && soilMoisturePercent < 100)  {
    if (soilMoisturePercent < 50) {
      // enable relay (water needed)
      digitalWrite(RELAY_PIN, LOW);
    } else {
      // disable relay (dirt is moist enough)
      digitalWrite(RELAY_PIN, HIGH);
    }
    Serial.print(soilMoisturePercent);
    Serial.println("%");    
  }
}

void outputOnDisplay() 
{
  display.clearDisplay();
  
  if (displayCounter > 2) {
    displayCounter = 0;
  }

  if (displayCounter == 0) {
    display.setCursor(15, 0);
    display.setTextSize(1);
    display.println("Soil moisture:");
    display.setCursor(15, 15);
    display.setTextSize(2);
    display.print(soilMoisturePercent);
    display.println(" % ");
    display.display();
  }

  if (displayCounter == 1) {
    display.setCursor(15, 0);
    display.setTextSize(1);
    display.println("Humidity");
    display.setCursor(15, 15);
    display.setTextSize(2);
    display.print(hum);
    display.println(" %");
    display.display();
  }

  if (displayCounter == 2) {
    display.setCursor(15, 0);
    display.setTextSize(1);
    display.println("Temperature");
    display.setCursor(15, 15);
    display.setTextSize(2);
    display.print(temp);
    display.println(" C");
    display.display();
  }
  
  displayCounter++;
}
