#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_MPRLS.h"
#include <SPI.h>
#include <mcp2515.h>

#define TCA_DEF_ADDR 0x70
#define MPRLS_ADDR 0x18
struct can_frame canMsg;

// This is the number of pressure sensors you want per MUX
// CHANGE THIS NUMBER BASED ON HOW MANY PRESSURE SENSORS YOU ARE USING (AND FUCKING STANLEY)
#define NUM_MPRLS 3

// This is the number of MUX's you have
// CHANGE THIS NUMBER BASED ON HOW MANY MULTIPLEXERS YOU ARE USING
#define NUM_MUX 1

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin

Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);
MCP2515 mcp2515(10);

float pressure_values[2];

void tca_select(uint8_t i, uint8_t mux_offset) {
  if (i > 7) return;
  Wire.beginTransmission(TCA_DEF_ADDR+mux_offset);
  Wire.write(1 << i);
  Wire.endTransmission();
}

float Velocity(float pressure_values[2], int size) {
  return sqrt(2 * ((pressure_values[0] - pressure_values[1])) / 0.605);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Multi-MPRLS Simple Test");

  // Setup MPRLS Sensor
  tca_select(0, 0);
  mpr.begin();
  delay(100);
  delay(100);
  Serial.println(F("S0.0,S0.1,S0.2,S0.3,S0.4,S0.5,S1.0,S1.1,S1.2,S1.3,S1.4,S1.5"));

  // Setup CAN Bus
  while (!Serial);
  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
}
void loop() {
  for (uint8_t mux_no = 0; mux_no < NUM_MUX; mux_no++) {
    for (uint8_t board_no = 0; board_no < NUM_MPRLS; board_no++) {
      tca_select(board_no, mux_no);
      pressure_values[board_no] = mpr.readPressure();
    }
  }
  
  double sensorValue1 = analogRead(0);
  double voltageOut1 = sensorValue1 * (5.0 / 1023.0);

  double sensorValue2 = analogRead(1);
  double voltageOut2 = sensorValue2 * (5.0 / 1023.0);

  double sensorValue3 = analogRead(2);
  double voltageOut3 = sensorValue3 * (5.0 / 1023.0);

  if(Velocity(pressure_values, 2) < 0) {
    Serial.print("Velocity = 0");
  } else {
    Serial.print("Velocity = ");
    Serial.print(Velocity(pressure_values, 2));
    Serial.print(" ");
    Serial.print("Pressure 1 = ");
    Serial.print(pressure_values[0]);
    Serial.print(" ");
    Serial.print("Pressure 2 = ");
    Serial.print(pressure_values[1]);
    Serial.print(" Voltage: ");
    Serial.print(voltageOut1);
    Serial.print(" Voltage: ");
    Serial.print(voltageOut2);
    Serial.print(" Voltage: ");
    Serial.print(voltageOut3);
    Serial.println();
  }
  


  canMsg.can_id  = 0x710;
  canMsg.can_dlc = 8;


  canMsg.data[0] = Velocity(pressure_values, 2)*10;
  canMsg.data[1] = (int(Velocity(pressure_values, 2)) >> 8) *10;

  canMsg.data[2] = voltageOut1*10;
  canMsg.data[3] = (int(voltageOut1) >> 8)*10;

  canMsg.data[4] = voltageOut2*10;
  canMsg.data[5] = (int(voltageOut2) >> 8)*10;

  canMsg.data[6] = voltageOut3*10;
  canMsg.data[7] = (int(voltageOut3) >> 8)*10;
  mcp2515.sendMessage(&canMsg);
}
