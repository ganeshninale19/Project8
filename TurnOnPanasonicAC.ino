#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRPanasonicAc ac(kIrLed);  // Set the GPIO used for sending messages.
bool currentStatus=1;
unsigned char temp = 16;

void printState() {
  // Display the settings.
  Serial.println("Panasonic A/C remote is in the following state:");
  Serial.printf("  %s\n", ac.toString().c_str());
  // Display the encoded IR sequence.
  unsigned char* ir_code = ac.getRaw();
  Serial.print("IR Code: 0x");
  for (uint8_t i = 0; i < kPanasonicAcStateLength; i++)
    Serial.printf("%02X", ir_code[i]);
  Serial.println();
}

void setup() {
  ac.begin();
  Serial.begin(115200);
  delay(200);
  pinMode(5,INPUT_PULLUP);
  pinMode(18,INPUT_PULLUP);
  pinMode(19,INPUT_PULLUP);

  // Set up what we want to send. See ir_Panasonic.cpp for all the options.
  Serial.println("Default state of the remote.");
  printState();
  Serial.println("Setting desired state for A/C.");
  ac.setModel(kPanasonicUnknown);
  ac.on();
  ac.setFan(kPanasonicAcFanAuto);
  ac.setMode(kPanasonicAcCool);
  ac.setTemp(16);

  ac.setSwingVertical(kPanasonicAcSwingVAuto);
  ac.setSwingHorizontal(kPanasonicAcSwingHAuto);
}

void loop() {
  // Now send the IR signal.
if(digitalRead(18)==0){
  if(temp!= 30)
    temp++;
  ac.setTemp(temp);
  ac.send();
  printState();
}

if(digitalRead(19)==0){
  if(temp!= 16)
    temp--;
  ac.setTemp(temp);
  ac.send();
  printState();
}

if(digitalRead(5)==0)
{
  currentStatus = !currentStatus;
  if(currentStatus)
    ac.off();
  else
    ac.on();
  #if SEND_PANASONIC_AC
  Serial.println("Sending IR command to A/C ...");
  ac.send();
#endif  // SEND_PANASONIC_AC
  printState();

}

}
