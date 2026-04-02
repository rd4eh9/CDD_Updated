#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>

// No other Address options.
#define DEF_ADDR 0x55

// Reset pin, MFIO pin
const int resPin = 2;
const int mfioPin = 1;

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
  int error = bioHub.configBpm(MODE_TWO); // Configuring just the BPM settings. 
  if(!error){
    Serial.println("Sensor configured.");

    int swError = bioHub.setPulseWidth(118); //changes pwm to 118 and checks that it happened, 0 means success
    int srError = bioHub.setSampleRate(100); //changes sampling rate to 100Hz and checks that it happened, 0 means success

    if(swError == 0 && srError == 0) //checks for errors
        Serial.println("Pulse Width (118microseconds) and Sample Rate (100Hz) set successfully!");
    else
        Serial.println("Warning: Sensor rejected custom pulse/rate settings.");
  }
      
  else {
    Serial.println("Error configuring sensor.");
    Serial.print("Error: "); 
    Serial.println(error); 
  }
    
    
  //Adjust LED Intensity
  //setLedIntensity();

  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up. 
  
  delay(4000); 

}

void loop(){
    float temp = temperatureRead();
    // Information from the readBpm function will be saved to our "body"
    // variable.  
    body = bioHub.readBpm();
    //Serial.print("Heartrate: ");
    Serial.print(body.heartRate); 
    Serial.print(",");
    //Serial.print("Confidence: ");
    Serial.print(body.confidence); 
    Serial.print(",");
    //Serial.print("Oxygen: ");
    Serial.print(body.oxygen); 
    Serial.print(",");
    //Serial.print("Status: ");
    Serial.print(body.status); 
    Serial.print(",");
    Serial.print(String(temp));
    //Serial.println(body.heartRate,body.confidence,body.oxygen,body.status,String(temp));
    Serial.println();
    delay(250); // Slowing it down, we don't need to break our necks here.
}
