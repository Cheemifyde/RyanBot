// #include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "hidjoystickrptparser.h"

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);
const int inputPin1 = 10;
const int inputPin2 = 11;
const int inputPin3 = 9;
const int inputPin4 = 8;
int EN1 = 5;
int EN2 = 6;

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

    pinMode(EN1, OUTPUT);   // where the motor is connected to
    pinMode(EN2, OUTPUT);   // where the motor is connected to
    pinMode(inputPin1, OUTPUT);
    pinMode(inputPin2, OUTPUT);
    pinMode(inputPin3, OUTPUT);
    pinMode(inputPin4, OUTPUT);
}

void loop() {
    Usb.Task();

    if (JoystickEvents::mostRecentEvent.Z2 == 1) {
        Serial.print("X: ");
        Serial.println(JoystickEvents::mostRecentEvent.X);
        Serial.print("Y: ");
        Serial.println(JoystickEvents::mostRecentEvent.Y);
        Serial.print("Boolean: ");
        Serial.println(JoystickEvents::mostRecentEvent.Z2);

        int joyX = analogRead(JoystickEvents::mostRecentEvent.X);  // Read X-axis from joystick (between 0 and 1023)
        int joyY = analogRead(JoystickEvents::mostRecentEvent.Y);  // Read Y-axis from joystick (between 0 and 1023)

        // Convert joystick to motor speeds (0 to 255)
        int speedLeft = map(joyY, 0, 1023, -255, 255);  // Map Y-axis to motor speed
        int speedRight = map(joyY, 0, 1023, -255, 255); // Map Y-axis to motor speed

        int speedDiff = map(joyX, 0, 1023, -255, 255);   // Map X-axis to speed difference

        speedLeft += speedDiff;
        speedRight -= speedDiff;

        // Limit speeds
        speedLeft = constrain(speedLeft, -255, 255);
        speedRight = constrain(speedRight, -255, 255);

        // Set motor speeds and directions
        analogWrite(EN1, abs(speedLeft));  // Set speed for Motor 1
        analogWrite(EN2, abs(speedRight)); // Set speed for Motor 2

        if (speedLeft >= 0) {
            digitalWrite(inputPin1, HIGH);
            digitalWrite(inputPin2, LOW);
        } else {
            digitalWrite(inputPin1, LOW);
            digitalWrite(inputPin2, HIGH);
        }

        if (speedRight >= 0) {
            digitalWrite(inputPin3, HIGH);
            digitalWrite(inputPin4, LOW);
        } else {
            digitalWrite(inputPin3, LOW);
            digitalWrite(inputPin4, HIGH);
        }

        // delay(20);  // Adjust delay as needed
    }
}
