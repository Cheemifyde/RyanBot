// #include <usbhid.h>

#include <hiduniversal.h>
#include <usbhub.h>
#include "hidjoystickrptparser.h"

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);
const int inputPin4 = 4; //The RSL (Robot Signal Light)
const int inputPin5 = 5; //Left motor pin
const int inputPin9 = 9; //Right motor pin
const float gain = 1; //Changable gain to lower speed of robot

void setup() {
    Serial.begin(115200);
    #if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
    #endif
    Serial.println("Start");

    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");

    delay(200);

    if (!Hid.SetReportParser(0, &Joy))
        ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);

    pinMode(inputPin4, OUTPUT);
    pinMode(inputPin5, OUTPUT);
    pinMode(inputPin9, OUTPUT);
}

void loop() {
    
    Usb.Task();

    if (JoystickEvents::mostRecentEvent.Z2 == 1) {
        
        delay(200);
        digitalWrite(inputPin4, HIGH);
        delay(200);
        digitalWrite(inputPin4, LOW);
      

        int joyX = JoystickEvents::mostRecentEvent.X;
        int joyY = JoystickEvents::mostRecentEvent.Y;

        int x = joyX - 128;
        int y = joyY - 128; 

        float speedLeft = -((y+x) * gain);
        float speedRight = -((y-x) * gain);

        // Limit speeds
        speedLeft = constrain(speedLeft, -255, 255);
        speedRight = constrain(speedRight, -255, 255);

        // Set motor speeds and directions

        int leftSpeed = speedLeft;
        int rightSpeed = speedRight;

        // Serial.println(x); 
        // Serial.println(y);

      // Left motor control

      if (leftSpeed >= 0) {
         analogWrite(inputPin9, leftSpeed);   // Left Forward
         Serial.println("Left Forward");
      } else {
         analogWrite(inputPin9, -(abs(leftSpeed)));  // Left Reverse
         Serial.println("Left Reverse");
      }

      // Right motor control
      if (rightSpeed >= 0) {
         analogWrite(inputPin5, rightSpeed);   // Right Forward
         Serial.println("Right Forward");
      } else {
         analogWrite(inputPin5, -(abs(rightSpeed)));  // Right Reverse
         Serial.println("Right Reverse");
      }
      
    } else {
        analogWrite(inputPin4, LOW);
        analogWrite(inputPin5, 0);
        analogWrite(inputPin9, 0);
    }
}
