
#include <ACAN2515.h>

const uint16_t THERM_ADDR_0 = 0x380;

const uint8_t NUM_THERMS = 4;
const uint8_t ADC_PINS[NUM_THERMS] = {14, 15, 16, 17};

static const byte MCP2515_CS  = 10 ; // CS input of MCP2515 (adapt to your design) 
static const byte MCP2515_INT =  3 ; // INT output of MCP2515 (adapt to your design)

ACAN2515 can (MCP2515_CS, SPI, MCP2515_INT) ;

static const uint32_t QUARTZ_FREQUENCY = 8UL * 1000UL * 1000UL ; // 8 MHz

uint16_t getTemperature(int adc_pin) {
  double temp;
  int raw_adc = analogRead(adc_pin);
  temp = log(100000.0*((1024.0/raw_adc-1)));
  temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * temp * temp ))* temp );
  temp = temp - 273.15;      
  return (uint16_t)(temp*-100);
}

void setup () {
  //--- Switch on builtin led
  pinMode (LED_BUILTIN, OUTPUT) ;
  digitalWrite (LED_BUILTIN, HIGH) ;
  //--- Start serial
  Serial.begin (9600) ;
  //--- Wait for serial (blink led at 10 Hz during waiting)
  //
  //while (!Serial) {
  //  delay (50) ;
  //  digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  //}
  //--- Begin SPI
  SPI.begin () ;
  //--- Configure ACAN2515
  Serial.println ("Configure ACAN2515") ;
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
}

//----------------------------------------------------------------------------------------------------------------------

static uint32_t gBlinkLedDate = 0 ;
static uint32_t gReceivedFrameCount = 0 ;
static uint32_t gSentFrameCount = 0 ;

//——————————————————————————————————————————————————————————————————————————————

void loop () {
  uint16_t temperature;
  CANMessage frame;
  frame.id = THERM_ADDR_0;
  frame.len = 8;

  uint8_t can_counter = 0;

  for (uint8_t i = 0; i < NUM_THERMS; i++) {
    temperature = getTemperature(ADC_PINS[i]);
    Serial.print("Temperature = ");
    Serial.println(temperature);    //Keep first byte by doing bitwise AND to first byte, increment can_counter
    
    frame.data[can_counter++] = temperature & 0xFF;
    //Keep second byte by shifting all bits right 
    frame.data[can_counter++] = (temperature >> 8) & 0xFF;
    
    if (can_counter > 7) break; //8 byte CAN cap
  }
  
  Serial.println();
  
  if (gBlinkLedDate < millis ()) {
    gBlinkLedDate += 100 ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    const bool ok = can.tryToSend (frame) ;
    if (ok) {
      gSentFrameCount += 1 ;
      Serial.print ("Sent: ") ;
      Serial.println (gSentFrameCount) ;
    } else {
      Serial.println ("Send failure") ;
    }
  }
  if (can.receive (frame)) {
    //gReceivedCount += 1 ;
    Serial.print ("Received: ") ;
    for (uint8_t i = 0; i < frame.len; i++) {
      Serial.print(frame.data[i]);
      Serial.print(" ");
    }
    Serial.println();
    //Serial.println (gReceivedCount) ;
  }
  /*
  if (can.available ()) {
    can.receive (frame) ;
    gReceivedFrameCount ++ ;
    Serial.print ("Received: ") ;
    Serial.println (gReceivedFrameCount) ;
  }
  */
}

//——————————————————————————————————————————————————————————————————————————————
