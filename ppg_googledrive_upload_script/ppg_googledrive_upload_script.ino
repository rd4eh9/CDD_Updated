#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>

// No other Address options.
#define DEF_ADDR 0x55

// Reset pin, MFIO pin
const int resPin = 2;
const int mfioPin = 1;

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 
bioData body;  

// Update below !!!!!
const char* ssid = "Marisa (2)";
const char* password = "Marisa...";
String googleSheetScriptURL = "https://script.google.com/macros/s/AKfycbx5hPLuO4OIjndLh1G3K6b-kFdF2nSsDdDVd2iTA-dTy7LNVKiMUKsPi2eCKYBY9RuY/exec";
// script:
// function doGet(e) {
//   var sheet = SpreadsheetApp.getActiveSheet();
//   sheet.appendRow([
//     new Date(),
//     e.parameter.heartrate,
//     e.parameter.confidence,
//     e.parameter.oxygen,
//     e.parameter.status,
//     e.parameter.esp32temperature
//   ]);
//   return ContentService.createTextOutput("OK");
// }

void setup(){
  Serial.begin(115200);
  WiFi.setSleep(false); 
  delay(500);


  // Wifi configuration
  Serial.println("connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(250);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Sensor configuration
  Wire.begin();

  int result = bioHub.begin();
  if (!result) Serial.println("BioHub started!");
  else Serial.println("BioHub FAILED!");

  int error = bioHub.configBpm(MODE_TWO);
  if (!error) Serial.println("BioHub configured (BPM mode).");
  else Serial.println("BioHub config error!");


  // Serial.println("Configuring Sensor...."); 
  // int error = bioHub.configBpm(MODE_TWO); // Configuring just the BPM settings. 
  // if(!error){
  //   Serial.println("Sensor configured.");
  // }
  // else {
  //   Serial.println("Error configuring sensor.");
  //   Serial.print("Error: "); 
  //   Serial.println(error); 
  // }

  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up. 
  delay(4000); 

}

void loop(){
  body = bioHub.readBpm();
  float temp = temperatureRead();

  // Build URL
  String url = googleSheetScriptURL +
              "?heartrate=" + String(body.heartRate) +
              "&confidence=" + String(body.confidence) +
              "&oxygen=" + String(body.oxygen) +
              "&status=" + String(body.status) +
              "&esp32temperature=" + String(temp);
  
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.print("Sheets Response: ");
    Serial.println(http.getString());
  } else {
    Serial.print("HTTP error: ");
    Serial.println(httpCode);
  }
   
  http.end();

  delay(2000);

}
