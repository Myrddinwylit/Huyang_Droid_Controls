#ifndef HuyangNeck_h
#define HuyangNeck_h

#include "Arduino.h"
#include <Adafruit_PWMServoDriver.h>
#include "../EasingServo/EasingServo.h" // Corrected: Now directly in 'src' folder

// Servo Parameters for PCA9685 PWM Driver
#define HuyangNeck_SERVOMIN 150  // This is the 'minimum' pulse length count (out of 4096)
#define HuyangNeck_SERVOMAX 595  // This is the 'maximum' pulse length count (out of 4096)
#define HuyangNeck_SERVO_FREQ 60 // Analog servos typically run at ~50 Hz updates, 60 is fine for PCA9685

// PWM channel pins for neck servos on the PCA9685 board
#define pwm_pin_head_monocle (uint8_t)4 // Servo for monacle movement
#define pwm_pin_head_left (uint8_t)5    // Left neck servo for tilt
#define pwm_pin_head_right (uint8_t)6   // Right neck servo for tilt
#define pwm_pin_head_rotate (uint8_t)8  // Head rotation servo
#define pwm_pin_head_neck (uint8_t)9    // Main neck servo for forward/backward tilt

class HuyangNeck
{
public:
    // Constructor: takes a pointer to the PWM driver
    HuyangNeck(Adafruit_PWMServoDriver *pwm);

    // Setup function: performs initial servo centering or setup
    void setup();
    // Loop function: called repeatedly to update servo positions and handle automatic animations
    void loop();

    bool automatic = true; // Flag to enable/disable automatic neck movements

    // Public methods for manual control of neck movements
    void rotateHead(double degree, double duration = 1000);
    void tiltNeckForward(double degree, double duration = 1000);
    void tiltNeckSideways(double degree, double duration = 1000);

    // NEW: Public method for monocle control
    void setMonoclePosition(int16_t position, double duration = 500);

    // Public calibration values (directly accessible by WebServer)
    int16_t calibrationRotate = 0;
    int16_t calibrationTiltForward = 0;
    int16_t calibrationTiltSideways = 0;
    int16_t calibrationMonocle = 0; // NEW: Calibration for monocle

private:
    Adafruit_PWMServoDriver *_pwm; // Pointer to the PWM driver instance

    unsigned long _currentMillis = 0;  // Current time in milliseconds
    unsigned long _previousMillis = 0; // Previous time for general timing

    // Timers for automatic neck movements
    unsigned long _randomDoRotate = 0;
    unsigned long _randomDoTiltForward = 0;
    unsigned long _randomDoTiltSideways = 0;

    // EasingServo instances for smooth movements
    EasingServo *_neckRotateServo;
    EasingServo *_neckTiltForwardServo;
    EasingServo *_neckTiltSidewaysServo;
    EasingServo *_monocleServo; // NEW: EasingServo for monocle

    // Private helper methods
    // Maps a degree value to a PWM pulselength and sends it to the specified servo pin
    void rotateServo(uint8_t servo, double degree);
    // Easing function for smooth animation (quadrilateral ease-in-out)
    double easeInOutQuad(double t);
    // Generic easing function to calculate intermediate position
    double easeInAndOut(double start, double current, double target, double percentage);

    // Update functions for each type of neck movement, applying easing and calibration
    void updateNeckPosition(); // This function will now manage all neck servos
    void doRandomRotate();
    void doRandomTiltForward();
    void doRandomTiltSideways();
};

#endif
