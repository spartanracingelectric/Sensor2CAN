#include <SPI.h>          
#include <mcp2515.h>      

 
struct can_frame canMsg;
 
MCP2515 mcp2515(10);
 
 
 
void setup()
{
  while (!Serial);
  Serial.begin(9600);
  SPI.begin();               //Begins SPI communication
 
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); //Sets CAN at speed 500KBPS and Clock 8MHz
  mcp2515.setNormalMode();
    Serial.begin(9600);

}
 
 
void loop()
{
  double sensorValue1 = analogRead(0);
  double voltageOut1 = sensorValue1 * (5.0 / 1023.0);

  canMsg.can_id  = 0x710;           
  canMsg.can_dlc = 8;               
  canMsg.data[0] = voltageOut1*100;  
  Serial.println(voltageOut1);

  canMsg.data[1] = 255;               
  canMsg.data[2] = 255;            
  canMsg.data[3] = 255;
  canMsg.data[4] = 255;
  canMsg.data[5] = 255;
  canMsg.data[6] = 255;
  canMsg.data[7] = 255;

  mcp2515.sendMessage(&canMsg);     //Sends the CAN message

        
 
   
 
}
