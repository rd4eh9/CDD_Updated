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
