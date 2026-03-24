/* 
#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>

// No other Address options.
#define DEF_ADDR 0x55

// Reset pin, MFIO pin
const int resPin = 1;
const int mfioPin = 2;

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 

bioData body;  

// Required PPG acquisition settings.
const uint8_t PPG_SAMPLE_RATE_SPS = 100;
const uint16_t PPG_PULSE_WIDTH_US = 215;

//void setLedIntensity(uint8_t red, uint8_t ir) {
    //bioHub.writeRegister(0x0C, red); // 0x0C: LED1 (Red) Amplitude Register
    //bioHub.writeRegister(0x0D, ir);  // 0x0D: LED2 (IR) Amplitude Register
//}
  

void setup(){

  Serial.begin(115200);

  Wire.begin();
  int result = bioHub.begin();
  if (!result)
    Serial.println("Sensor started!");
  else
    Serial.println("Could not communicate with the sensor!!!");

  Serial.println("Configuring Sensor...."); 

  // MODE_TWO enables red + IR LEDs for SpO2 (red ~650-670nm, IR ~870-900nm).
  int error0 = bioHub.configSensorBpm(MODE_TWO);
  int errorSampleRate = bioHub.setSampleRate(PPG_SAMPLE_RATE_SPS);

  if(!error0 && !errorSampleRate){
    Serial.println("Sensor configured: MODE_TWO + sample rate 100 SPS.");
  }
  else {
    Serial.println("Error configuring sensor.");
    Serial.print("configSensorBpm error: ");
    Serial.println(error0);
    Serial.print("setSampleRate error: ");
    Serial.println(errorSampleRate);
  }

  // setting pulse width 
  uint8_t error1 = bioHub.setPulseWidth(PPG_PULSE_WIDTH_US);

  if (error1 == 0) {
    Serial.println("Pulse width set successfully to 215us.");
  } else {
    Serial.print("Error setting pulse width: ");
    Serial.println(error1);
  }

  //Adjust LED Intensity
  //setLedIntensity();

  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up. 
  delay(4000); 

}

void loop(){

    // Information from the readBpm function will be saved to our "body"
    // variable.  
    // read sensor data 
    body = bioHub.readBpm();

    Serial.print("Heartrate: ");
    Serial.println(body.heartRate); 
    Serial.print("Confidence: ");

    //confidence value 
    Serial.println(body.confidence); 
    Serial.print("Oxygen: ");
    Serial.println(body.oxygen); 
    
    //sensor status 
    Serial.print("Status: ");
    Serial.println(body.status);
    
    //raw data 
    Serial.print("Red Data: ");
    Serial.println(body.redLed);
    Serial.print("IR Data: ");
    Serial.println(body.irLed);

    delay(250); // Slowing it down, we don't need to break our necks here.
}

*/

#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>

// Sensor address
#define DEF_ADDR 0x55

// Reset pin, MFIO pin
const int resPin = 1;
const int mfioPin = 2;

// Sensor object
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin);
bioData body;

// ---- Scaling window ----
#define WINDOW 100
long irBuffer[WINDOW];
int bufferIndex = 0;
bool bufferFilled = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  int result = bioHub.begin();
  if (!result)
    Serial.println("Sensor started!");
  else {
    Serial.println("Could not communicate with the sensor!!!");
    while (1);
  }

  Serial.println("Configuring sensor for raw LED data...");
  int error0 = bioHub.configSensor();
  if (!error0) {
    Serial.println("Sensor configured.");
  } else {
    Serial.print("Error configuring sensor: ");
    Serial.println(error0);
    while (1);
  }

  uint8_t error1 = bioHub.setPulseWidth(215);
  if (error1 == 0) {
    Serial.println("Pulse width set to 215us.");
  } else {
    Serial.print("Error setting pulse width: ");
    Serial.println(error1);
  }

  // Initialize buffer
  for (int i = 0; i < WINDOW; i++) {
    irBuffer[i] = 0;
  }

  delay(2000); // let sensor settle
}

void loop() {
  // Read sensor
  body = bioHub.readSensor();
  long irRaw = body.irLed;

  // Store in buffer
  irBuffer[bufferIndex++] = irRaw;
  if (bufferIndex >= WINDOW) {
    bufferIndex = 0;
    bufferFilled = true;
  }

  // Compute min/max over window
  long minIR = irBuffer[0];
  long maxIR = irBuffer[0];

  int limit = bufferFilled ? WINDOW : bufferIndex;

  for (int i = 1; i < limit; i++) {
    if (irBuffer[i] < minIR) minIR = irBuffer[i];
    if (irBuffer[i] > maxIR) maxIR = irBuffer[i];
  }

  // Scale to 0–1023
  int irScaled = 0;
  if (maxIR - minIR > 0) {
    irScaled = (irRaw - minIR) * 1023 / (maxIR - minIR);
  }

  // Print results
  Serial.print("Raw IR: ");
  Serial.print(irRaw);

  Serial.print(" | Scaled (0-1023): ");
  Serial.println(irScaled);

  // ~100 Hz sampling
  delay(10);
}

