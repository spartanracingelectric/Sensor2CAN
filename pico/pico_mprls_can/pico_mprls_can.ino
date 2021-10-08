#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_MPRLS.h"

#define TCA_DEF_ADDR 0x70
#define MPRLS_ADDR 0x18

// This is the number of pressure sensors you want per MUX
// CHANGE THIS NUMBER BASED ON HOW MANY PRESSURE SENSORS YOU ARE USING
#define NUM_MPRLS 2

// This is the number of MUX's you have
// CHANGE THIS NUMBER BASED ON HOW MANY MULTIPLEXERS YOU ARE USING
#define NUM_MUX 1

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

void tca_select(uint8_t i, uint8_t mux_offset) {
  if (i > 7) return;

  Wire.beginTransmission(TCA_DEF_ADDR+mux_offset);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup() {
  Serial.begin(9600);
  Serial.println("Multi-MPRLS Simple Test");

  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();

  tca_select(0, 0);

  /*
  //mpr.begin tends to provide a false value, so omit this code and call 
  if (!mpr.begin()) {
    Serial.println(mpr.readStatus());
    Serial.println("Failed to communicate with MPRLS sensor, check wiring?");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Found MPRLS sensor");
  */
  
  mpr.begin();
  delay(100);
}


void loop() {

  for (uint8_t mux_no = 0; mux_no < NUM_MUX; mux_no++) {
    Serial.print("******** BOARD ");
    Serial.print(mux_no);
    Serial.println(" ********");
    for (uint8_t board_no = 0; board_no < NUM_MPRLS; board_no++) {
    
      tca_select(board_no, mux_no);
      float pressure_hpa = mpr.readPressure();
      Serial.print("Pressure ");
      Serial.print(board_no);
      Serial.print(" (hPa): ");
      Serial.println(pressure_hpa);
      Serial.print("Pressure ");
      Serial.print(board_no);
      Serial.print(" (PSI): ");
      Serial.println(pressure_hpa / 68.947572932);
    }
  }
  Serial.println("-----------------");
  delay(1500); //For reading purposes, remove this statement if you want to read at full speed
}
