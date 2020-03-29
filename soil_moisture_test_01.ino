// Libraries
// Display
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// Watchdog
// #include <avr/wdt.h>

// Constants
#define DEBUG true
// Display
// #define OLED_RESET 4
// Adafruit_SSD1306 display(OLED_RESET);
// Relay
#define RELAY_PIN 7 // pin to operate relay

// values for soil moisture sensor calibration 
// completly dry and wet
const int AirValue = 950; 
const int WaterValue = 364;

/**
 * Time to activate relay for pouring (1000 * x = x seconds)
 */
const int timeToPour = 1000 * 10;

/**
 * Check sensor every 1 second (value in milliseconds)
 */
const long intervalSensor = 2000;

/**
 * Update display every 4 second (value in milliseconds)
 */
const long intervalDisplay = 4000;

// Variables
/**
 * analog soil moisture value
 */
int soilMoistureValue = 0; 
/**
 * calculated percentage value
 */
int soilMoisturePercent = 0; 
/**
 * store last time sensors was updated
 */
unsigned long previousMillisSensor = 0;
/**
 * store last time display was updated
 */
unsigned long previousMillisDisplay = 0; 
/**
 * Used to loop through the different sensor values 
 */
int displayCounter = 0;

void setup() {
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  // initalise display
  // initialiseDisplay();

  // some startup text just for fun....
  /*display.setCursor(5, 10);
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
  display.clearDisplay();*/
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

  /*if (currentMillis - previousMillisDisplay >= intervalDisplay) {
    // save the last time you updates the display
    previousMillisDisplay = currentMillis;
    outputOnDisplay();
  }*/
}

/**
 * Read values from all connected sensors.
 */
void readSensors() 
{
  soilMoistureValue = analogRead(A0);
  debugOutput("soilMoistureValue: " + (String) soilMoistureValue);
  soilMoisturePercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

  handleRelay();
}

/**
 * Activate relay based on soilmoisture value
 */
void handleRelay()
{
  // relay logic  
  if (soilMoisturePercent >= 100) {
    debugOutput("Soil moisture: 100 %");
  } else if(soilMoisturePercent <= 0) {
    debugOutput("Soil moisture: 0 %");
  } else if(soilMoisturePercent > 0 && soilMoisturePercent < 100)  {
    debugOutput("Soil moisture: " + (String) soilMoisturePercent + "%");
    if (soilMoisturePercent < 50) {
      // enable relay (water needed)
      digitalWrite(RELAY_PIN, LOW);      
      debugOutput("Pouring water");
      // pour for an adjustable time, than set stop pouring
      delay(timeToPour);
      digitalWrite(RELAY_PIN, HIGH);      
    } else {
      // disable relay (dirt is moist enough)
      digitalWrite(RELAY_PIN, HIGH);
      debugOutput("No pouring needed");
    }
  }
}

/**
 * Output sensor values on display
 */
void outputOnDisplay() 
{
  /*display.clearDisplay();
  display.setCursor(15, 0);
  display.setTextSize(1);
  display.println("Soil moisture:");
  display.setCursor(15, 15);
  display.setTextSize(2);
  display.print(soilMoisturePercent);
  display.println(" % ");
  display.display();*/
}

/**
 * Initialise display. 
 * Potentially usefull if display is made toggable
 */
void initialiseDisplay()
{
  /*display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();*/
}

/**
 * Output text on serial monitor if debug is on.
 */
void debugOutput(String text)
{
  if (DEBUG) {
    Serial.println(text);
  }
}
