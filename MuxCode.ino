#include <Wire.h>

#define MUX_ADDR (Enter address here)
#define MUX_S0 0


// (find value from data sheet)
#define pressureTab1 (find value from data sheet)
#define pressureTab2 (find value from data sheet)


void setup() {
  
  Wire.begin();

  // Set up the select pins
  pinMode(MUX_S0, OUTPUT);



  // Set all select pins to low
  digitalWrite(MUX_S0, LOW);

}

void loop() {
  // Select the first load sensor
  selectSensor(0);
  
  // Read the data from first load sensor
  int data1 = readSensor(loadCell1);

  // Select the second load sensor
  selectSensor(1);

  // Read the data from second load sensor
  int data2 = readSensor(loadCell2);

  
  // Delay to avoid conflicts
  delay(10);
}


void selectSensor(int channel) {
  if(channel == 0){
    digitalWrite(MUX_S0, LOW);

  } else if(channel == 1){
    digitalWrite(MUX_S0, HIGH);
  } 
}

int readSensor(int address) {
  Wire.beginTransmission(address); //initiate 
  Wire.requestFrom(address, 2); //get 2 bytes from load cell 
  
  int data = (Wire.read() << 8) | Wire.read(); //combines the 2 bytes into long number but have to see if our load cell does 2 bytes at a time or just 1
   
  Wire.endTransmission();

  return data;
}
