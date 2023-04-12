#define forceMax 200
#define voltIn 4
#define constantVal 0.002

int sensorPin1 = A0;
int sensorPin2 = A1;
int sensorPin3 = A2;
int sensorPin4 = A3;

float voltageOut1;
float voltageOut2;
float voltageOut3;
float voltageOut4;

float force1;
float force2;
float force3;
float force4;

int sensorValue1 = 0, sensorValue2 = 0, sensorValue3 = 0, sensorValue4 = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Read sensor 1
  sensorValue1 = analogRead(sensorPin1);
  voltageOut1 = sensorValue1 * (5.0 / 1023.0);
  force1 = voltageOut1 * 50;

  // Read sensor 2
  sensorValue2 = analogRead(sensorPin2);
  voltageOut2 = sensorValue2 * (5.0 / 1023.0);
  force2 = voltageOut2 * 50;

  // Read sensor 3
  sensorValue3 = analogRead(sensorPin3);
  voltageOut3 = sensorValue3 * (5.0 / 1023.0);
  force3 = voltageOut3 * 50;

  // Read sensor 4
  sensorValue4 = analogRead(sensorPin4);
  voltageOut4 = sensorValue4 * (5.0 / 1023.0);
  force4 = voltageOut4 * 50;

  Serial.print("Force 1: ");
  Serial.print(force1);
  Serial.print(" lbs, Force 2: ");
  Serial.print(force2);
  Serial.print(" lbs, Force 3: ");
  Serial.print(force3);
  Serial.print(" lbs, Force 4: ");
  Serial.print(force4);
  Serial.println(" lbs");

  delay(100);
}
