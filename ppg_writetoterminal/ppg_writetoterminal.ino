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

/*
void setLedIntensity(uint8_t red, uint8_t ir) {
    bioHub.writeRegister(0x0C, red); // 0x0C: LED1 (Red) Amplitude Register
    bioHub.writeRegister(0x0D, ir);  // 0x0D: LED2 (IR) Amplitude Register
}
*/

void setup(){

  Serial.begin(115200);

  Wire.begin();
  int result = bioHub.begin();
  if (!result)
    Serial.println("Sensor started!");
  else
    Serial.println("Could not communicate with the sensor!!!");

  Serial.println("Configuring Sensor...."); 
  int error0 = bioHub.configBpm(MODE_TWO); // Configuring just the BPM settings. 
  //bioHub.configSensorBpm(100, 215); // Sample rate and pulse width
  if(!error0){
    Serial.println("Sensor configured.");
  }
  else {
    Serial.println("Error configuring sensor.");
    Serial.print("Error: "); 
    Serial.println(error0); 
  }
  uint8_t error1 = bioHub.setPulseWidth(215);
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
    body = bioHub.readBpm();
    Serial.print("Heartrate: ");
    Serial.println(body.heartRate); 
    Serial.print("Confidence: ");
    Serial.println(body.confidence); 
    Serial.print("Oxygen: ");
    Serial.println(body.oxygen); 
    Serial.print("Status: ");
    Serial.println(body.status); 
    Serial.print("Red Data: ");
    Serial.println(body.redLed);
    Serial.print("IR Data: ");
    Serial.println(body.irLed);

    delay(250); // Slowing it down, we don't need to break our necks here.
}