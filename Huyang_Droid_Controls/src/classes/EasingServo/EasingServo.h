#ifndef EasingServo_h
#define EasingServo_h

#include "Arduino.h"
#include <Adafruit_PWMServoDriver.h>

class EasingServo
{
public:
    EasingServo(Adafruit_PWMServoDriver *pwm, uint8_t servo, double min, double max, double start);
    void moveServoTo(double degree, double duration = 1000);
    void loop(); // This loop function is meant to be called externally to update the servo position

    double targetDegree = 0;  // The target degree the servo should move to
    double currentDegree = 0; // The current interpolated degree of the servo

    // These methods are now public so that HuyangNeck can call them to manage the servo
    void rotateServo(double degree);    // Directly sets the servo PWM to a degree
    void updatePosition();              // Calculates and updates the servo's position based on easing

private:
    unsigned long _currentMillis = 0;   // Current time for internal timing
    unsigned long _previousMillis = 0;  // Previous time for internal timing

    Adafruit_PWMServoDriver *_pwm;      // Pointer to the PWM driver instance

    uint8_t _servoPin = 0;              // The PWM channel pin for this servo

    double _min = -100;                 // Minimum allowed degree for this servo
    double _max = 100;                  // Maximum allowed degree for this servo
    double _start = 0;                  // Starting degree for the current easing movement
    double _duration = 0;               // Duration of the current easing movement in milliseconds
    unsigned long _startMillis = 0;     // Timestamp when the current easing movement started

    // Private helper for easing calculation
    double easeInOutQuad(double t);
    double easeInAndOut(double start, double current, double target, double percentage);
};

#endif
