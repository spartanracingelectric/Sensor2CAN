#include <Wire.h>
#include <ACAN2515.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_MPRLS.h"

const uint8_t TCA_DEF_ADDR = 0x70;
const uint8_t MPRLS_ADDR = 0x18;

// This is the number of pressure sensors you want per MUX
// CHANGE THIS NUMBER BASED ON HOW MANY PRESSURE SENSORS YOU ARE USING
const uint8_t NUM_MPRLS = 2;

// This is the number of MUX's you have
// CHANGE THIS NUMBER BASED ON HOW MANY MULTIPLEXERS YOU ARE USING
const uint8_t NUM_MUX = 1;

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
const int RESET_PIN = -1;  // set to any GPIO pin # to hard-reset on begin()
const int EOC_PIN = -1;  // set to any GPIO pin to read end-of-conversion by pin

const uint16_t CAN_CYCLE_TIME = 50;

static const byte MCP2515_SCK  = 2 ; // SCK input of MCP2515
static const byte MCP2515_MOSI = 3 ; // SDI input of MCP2515
static const byte MCP2515_MISO = 4 ; // SDO output of MCP2517

static const byte MCP2515_CS  = 5 ;  // CS input of MCP2515 (adapt to your design)
static const byte MCP2515_INT = 6 ;  // INT output of MCP2515 (adapt to your design)

Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

ACAN2515 can (MCP2515_CS, SPI, MCP2515_INT) ;

static const uint32_t QUARTZ_FREQUENCY = 8UL * 1000UL * 1000UL ; // 8 MHz

static uint32_t gBlinkLedDate = 0 ;
static uint32_t gReceivedFrameCount = 0 ;
static uint32_t gSentFrameCount = 0 ;

void tca_select(uint8_t i, uint8_t mux_offset) {
  if (i > 7) return;

  Wire1.beginTransmission(TCA_DEF_ADDR+mux_offset);
  Wire1.write(1 << i);
  Wire1.endTransmission();  
}

void setup() {
  pinMode (LED_BUILTIN, OUTPUT) ;
  digitalWrite (LED_BUILTIN, HIGH) ;

  SPI.setSCK(MCP2515_SCK);
  SPI.setTX(MCP2515_MOSI);
  SPI.setRX(MCP2515_MISO);
  SPI.setCS(MCP2515_CS);
  //--- Begin SPI
  SPI.begin () ;
  
  Serial.begin(115200);

  /*
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
  */

  //Serial.println ("Configure ACAN2515") ;
  ACAN2515Settings settings (QUARTZ_FREQUENCY, 500UL * 1000UL) ; // CAN bit rate 500 kb/s
  //settings.mRequestedMode = ACAN2515Settings::LoopBackMode ; // Select loopback mode

  const uint16_t errorCode = can.begin (settings, [] { can.isr () ; }) ;
  if (errorCode == 0) {
    
    Serial.print ("Bit Rate prescaler: ") ;
    Serial.println (settings.mBitRatePrescaler) ;
    Serial.print ("Propagation Segment: ") ;
    Serial.println (settings.mPropagationSegment) ;
    Serial.print ("Phase segment 1: ") ;
    Serial.println (settings.mPhaseSegment1) ;
    Serial.print ("Phase segment 2: ") ;
    Serial.println (settings.mPhaseSegment2) ;
    Serial.print ("SJW: ") ;
    Serial.println (settings.mSJW) ;
    Serial.print ("Triple Sampling: ") ;
    Serial.println (settings.mTripleSampling ? "yes" : "no") ;
    Serial.print ("Actual bit rate: ") ;
    Serial.print (settings.actualBitRate ()) ;
    Serial.println (" bit/s") ;
    Serial.print ("Exact bit rate ? ") ;
    Serial.println (settings.exactBitRate () ? "yes" : "no") ;
    Serial.print ("Sample point: ") ;
    Serial.print (settings.samplePointFromBitStart ()) ;
    Serial.println ("%") ;
    
  } else {
    
    Serial.print ("Configuration error 0x") ;
    Serial.println (errorCode, HEX) ;
    
  }

  Wire1.setSDA(14);
  Wire1.setSCL(15);
  Wire1.begin();

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
  mpr.begin(0x18, &Wire1);
  delay(100);
}


void loop() {

  uint32_t pressure_hpa_arr[NUM_MUX*NUM_MPRLS];

  CANMessage frame;
  frame.id = 0x77F;
  frame.len = 8;

  uint8_t can_counter = 0;
  
  for (uint8_t mux_no = 0; mux_no < NUM_MUX; mux_no++) {
    /*
    Serial.print("******** BOARD ");
    Serial.print(mux_no);
    Serial.println(" ********");
    */
    for (uint8_t board_no = 0; board_no < NUM_MPRLS; board_no++) {
    
      tca_select(board_no, mux_no);
      float pressure_hpa = mpr.readPressure();
      pressure_hpa_arr[board_no+(mux_no*NUM_MPRLS)] = (uint32_t)(pressure_hpa*100);
      
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
  //Serial.println("-----------------");
  
  for (uint8_t i = 0; i < NUM_MUX*NUM_MPRLS; i++) {
    
    //Keep first byte by doing bitwise AND to first byte, increment can_counter
    frame.data[can_counter++] = pressure_hpa_arr[i] & 0xFF;
    //Keep second byte by shifting all bits right 
    frame.data[can_counter++] = (pressure_hpa_arr[i] >> 8) & 0xFF;
    //Keep third byte by shifting all bits right 
    frame.data[can_counter++] = (pressure_hpa_arr[i] >> 16) & 0xFF;
    //Keep fourth byte by shifting all bits right 
    frame.data[can_counter++] = (pressure_hpa_arr[i] >> 24) & 0xFF;
   
    
    if (can_counter > 7) break; //8 byte CAN cap
  }
   
  
  if (gBlinkLedDate < millis ()) {
    gBlinkLedDate += CAN_CYCLE_TIME ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    const bool ok = can.tryToSend (frame) ;
    if (ok) {
      gSentFrameCount += 1 ;
      //Serial.print ("Sent: ") ;
      //Serial.println (gSentFrameCount) ;
    } else {
      //Serial.print ("Send failure") ;
      //Serial.println (ok);
    }
  }
}
