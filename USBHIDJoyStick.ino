// #include <usbhid.h>
// this file needs the most changing other files should be fine

#include <hiduniversal.h>
#include <usbhub.h>
#include "hidjoystickrptparser.h"

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);
int inputPin4 = 4; //The RSL
const int inputPin5 = 5; //Right motor pin
const int inputPin7 = 7; //Left motor pin
const float gain = 1; // Changable gain to lower speed of robot (doesn't work you might need to map)
bool delayRunning = false;
int forwardSpeed;
int turnValue;

//constants used for simplicity
const int joystickCenter = 128;
const int forwardMin = 100;
const int forwardMax = 179;
const int backwardsMin = 201;
const int backwardsMax = 240;
const int stopMin = 180;
const int stopMax = 200;

// back 130 stop 180-200 forward 250
// you might need ur own testing but more backwards is faster forward (100 faster than 150) and vice versa

void setup() {
    //average setup things you prolly won't need to change this

    Serial.begin(115200);
    #if !defined(__MIPSEL__)
    while (!Serial); 
    #endif
    Serial.println("Start");

    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");

    delay(200);

    if (!Hid.SetReportParser(0, &Joy))
        ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);

    pinMode(inputPin4, OUTPUT);
    pinMode(inputPin5, OUTPUT);
    pinMode(inputPin7, OUTPUT);
}

void loop() {
    
    Usb.Task();


    //check if the deadswitch is being pressed
    if (JoystickEvents::mostRecentEvent.Z2 == 1) {

        digitalWrite(inputPin4, HIGH); //this just turns on the light you can try doing flashing using a timer

        //get joystick output
        int joyX = JoystickEvents::mostRecentEvent.X;
        int joyY = JoystickEvents::mostRecentEvent.Y;

        // past testing with constraining values
        // int speedRight = (speedY + speedX) - 65;
        // int speedLeft = (speedY - speedX) - 65;

        // speedLeft = constrain(speedLeft, 70, 255);
        // speedRight = constrain(speedRight, 70, 255);


        //printing results
        Serial.print("X: ");
        Serial.println(joyX);
        Serial.print("Y: ");
        Serial.println(joyY);


        //experimental mapping (you don't have to do this if ur just going front and back)
        // if (joyY > 120) {
        //   forwardSpeed = map(joyY, 120, 255, 169, 254);
        // } else if (joyY < 135) {
        //   forwardSpeed = map(joyY, 135, 0, 179, 0);
        // }
        // else {
        //   forwardSpeed = 180;
        // }
        

        //very experimental formula (gain doesn't work by applying this directly i would say maybe times the input by the gain)
        if (joyY < joystickCenter) {
        // Forward: joystickY (127–0) set motor (179–0)
        forwardSpeed = forwardMax - ((joystickCenter - joyY) * forwardMax / joystickCenter);
        } else if (joyY > joystickCenter) {
        // Backward: joystickY (129–255) set motor (201–254)
        forwardSpeed = forwardMin + ((joyY - joystickCenter) * (backwardsMax - backwardsMin) / (255 - joystickCenter));
        } else {
        forwardSpeed = (stopMin + stopMin) / 2; // sets the motors to a stopping point
    }

    // turning experiments

    // if (joyX < 100) {
    //     // Left turn: joystickX (127–0) set (0 to -25)
    //     turnValue = -((joystickCenter - joyX) * 25 / joystickCenter);
    // } else if (joyX > 150) {
    //     // Right turn: joystickX (129–255) set (0 to 25)
    //     turnValue = ((joyX - joystickCenter) * 25 / (255 - joystickCenter));
    // } else {
    //     // no turn
    //     turnValue = 0;
    // }

    // more turning stuff if it actually worked
    // int leftMotorSpeed = forwardSpeed - turnValue;
    // int rightMotorSpeed = forwardSpeed + turnValue;

    //keeping the speed of each motor to the limits only
    // if (leftMotorSpeed < forwardMin) leftMotorSpeed = forwardMin;
    // if (leftMotorSpeed > backwardsMax) leftMotorSpeed = backwardsMax;

    // if (rightMotorSpeed < forwardMin) rightMotorSpeed = forwardMin;
    // if (rightMotorSpeed > backwardsMax) rightMotorSpeed = backwardsMax;


    // checking the speeds outputted for trial/error
    // Serial.print("leftSpeed: ");
    // Serial.println(leftMotorSpeed);
    // Serial.print("rightSpeed: ");
    // Serial.println(rightMotorSpeed);


      //apply the speeds to motors
      analogWrite(inputPin7, forwardSpeed);
      analogWrite(inputPin5, forwardSpeed);
        
      
    } else {
        system("CLS");


        // turn off light and motors if the deadswitch not pressed
        analogWrite(inputPin4, LOW);
        analogWrite(inputPin5, 0);
        analogWrite(inputPin7, 0);
    }
}
