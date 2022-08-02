#include <Adafruit_MCP3008.h>
#include <SPI.h>

//use GPIO pins to identify pin numbers
#define CS_PIN 13
#define CLOCK_PIN 10
#define MOSI_PIN 11
#define MISO_PIN 12

Adafruit_MCP3008 adc; //mcp object

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);

  //set pins to output mode
  pinMode(CS_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, OUTPUT);
  
  //initialize SPI bus
  SPI1.setSCK(CLOCK_PIN);
  SPI1.setCS(CS_PIN);
  SPI1.setTX(MOSI_PIN);
  SPI1.setRX(MISO_PIN);
  SPI1.begin();

  //initialize mcp object with spi bus
  adc.begin(CS_PIN, &SPI1);
  Serial.print("Digital Value:");
  Serial.print("\t");
  Serial.println("Analog Value:");

}

void loop() {
  //adc.begin(CS_PIN, &SPI1);
  // put your main code here, to run repeatedly:
  int val = adc.readADC(0); // read Channel 0 from MCP3008 ADC
  Serial.print(val);
  Serial.print("\t");
  Serial.print("\t");

  //convert digital value back to analog value
  float analogVolt = val * 5.0 / 1023;
  Serial.println(analogVolt);
  delay(500);

}
