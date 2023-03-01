#include <Wire.h>
#include "Adafruit_MPRLS.h"
#include <ACAN2515.h>


// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS sensor1 = Adafruit_MPRLS(RESET_PIN, EOC_PIN);
Adafruit_MPRLS sensor2 = Adafruit_MPRLS(RESET_PIN, EOC_PIN);



int velocity = 0 ;
static const byte MCP2515_SCK  = 2 ; // SCK input of MCP2515
static const byte MCP2515_MOSI = 3 ; // SDI input of MCP2515
static const byte MCP2515_MISO = 4 ; // SDO output of MCP2515

static const byte MCP2515_CS  = 5 ;  // CS input of MCP2515 (adapt to your design)
static const byte MCP2515_INT = 1 ;  // INT output of MCP2515 (adapt to your design)

static uint32_t gBlinkLedDate = 0 ;
static uint32_t gReceivedFrameCount = 0 ;
static uint32_t gSentFrameCount = 0 ;

ACAN2515 can (MCP2515_CS, SPI, MCP2515_INT) ;
static const uint32_t QUARTZ_FREQUENCY = 20UL * 1000UL * 1000UL ; // 20 MHz


void setup() {
    //--- Switch on builtin led
  pinMode (LED_BUILTIN, OUTPUT) ;
  digitalWrite (LED_BUILTIN, HIGH) ;
  //--- Start serial
  Serial.begin (115200) ;
  //--- Wait for serial (blink led at 10 Hz during waiting)
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
  //--- There are no default SPI pins so they must be explicitly assigned
  SPI.setSCK(MCP2515_SCK);
  SPI.setTX(MCP2515_MOSI);
  SPI.setRX(MCP2515_MISO);
  SPI.setCS(MCP2515_CS);
  //--- Begin SPI
  SPI.begin () ;

  //--- Configure ACAN2515
  Serial.println ("Configure ACAN2515") ;
  ACAN2515Settings settings (QUARTZ_FREQUENCY, 125UL * 1000UL) ; // CAN bit rate 125 kb/s
  //settings.mTransmitBuffer0Size = 30 ; // if you want to change driver transmit buffer
  settings.mRequestedMode = ACAN2515Settings::LoopBackMode ; // Select loopback mode
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


//----------------------------------------------------------------------------------------------------------------------

//——————————————————————————————————————————————————————————————————————————————

   Wire.begin();
   Serial.begin(115200);
   // Initialize the first sensor with address 0x18
   sensor1.begin(0x18);

  // Initialize the second sensor with address 0x19
   sensor2.begin(0x19);

//test sensor
//=============================================================================

  Serial.println("MPRLS Simple Test");
  if (! sensor1.begin()) {
    Serial.println("Failed to communicate with MPRLS sensor, check wiring?");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Found 1st MPRLS sensor");

  if (! sensor2.begin()) {
    Serial.println("Failed to communicate with MPRLS sensor, check wiring?");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Found 2nd MPRLS sensor");

    

  }
  


float Pressure_to_Velocity( float Pt , float Ps){
    
    velocity = (int)(2.24 * sqrt(((Pt-Ps)*6894.75)/0.605));

    return velocity;
}


void loop() {

  float Pt = sensor1.readPressure();
  float Ps = sensor2.readPressure();

  CANMessage message;
  message.id = 0x123; // id of the can bus
  message.len = 8; // 8 bytes = 32 bits
  message.data[0] = velocity & 0xFF;
  message.data[1] = (velocity >> 8) & 0xFF;
  message.data[2] = (velocity >> 16) & 0xFF;
  message.data[3] = (velocity >> 24) & 0xFF;


  if (gBlinkLedDate < millis ()) {
    gBlinkLedDate += 2000 ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    const bool ok = can.tryToSend (message) ;
    if (ok) {
      gSentFrameCount += 1 ;
      Serial.print ("Sent: ") ;
      Serial.println (gSentFrameCount) ;
    } else {
      Serial.println ("Send failure") ;
    }
  }
  if (can.available ()) {
    can.receive (message) ;
    gReceivedFrameCount ++ ;
    Serial.print ("Received: ") ;
    Serial.println (gReceivedFrameCount) ;
  }


  Serial.print("Pressure (hPa1): "); Serial.println(Pt);
  Serial.print("Pressure (PSI1): "); Serial.println(Pt / 68.947572932);
  Serial.print("Pressure (hPa2): "); Serial.println(Ps);
  Serial.print("Pressure (PSI2): "); Serial.println(Ps / 68.947572932);
  Serial.print("Velociy: "); Serial.println(Pressure_to_Velocity(Pt,Ps));

  
}



