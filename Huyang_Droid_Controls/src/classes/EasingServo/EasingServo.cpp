#include "EasingServo.h" // In the same folder
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include "../HuyangNeck/HuyangNeck.h" // NEW: Include HuyangNeck.h to get SERVOMIN/MAX definitions

EasingServo::EasingServo(Adafruit_PWMServoDriver *pwm, uint8_t servo, double min, double max, double start)
{
    _pwm = pwm;
    _servoPin = servo;
    _min = min;
    _max = max;
    currentDegree = start; // Initialize current degree to start position
    targetDegree = start;  // Initialize target degree to start position
    _start = start;        // Set initial start for easing to current position
    _duration = 0;         // No active easing movement initially
    _startMillis = 0;      // No active easing movement initially
}

// Sets the servo to a specific degree immediately (without easing)
void EasingServo::rotateServo(double degree)
{
    // Clamp degree within min/max range
    if (degree < _min) degree = _min;
    if (degree > _max) degree = _max;

    // Map the degree to the PCA9685 pulse length range
    uint16_t pulselength = map(degree, 0, 180, HuyangNeck_SERVOMIN, HuyangNeck_SERVOMAX);
    _pwm->setPWM(_servoPin, 0, pulselength);
    currentDegree = degree; // Update current degree immediately
    targetDegree = degree;  // Target is also reached immediately
    _duration = 0;          // No active easing
    _startMillis = 0;       // No active easing
}

// Initiates a smooth movement to a target degree over a specified duration
void EasingServo::moveServoTo(double degree, double duration)
{
    // Clamp target degree within min/max range
    if (degree < _min) degree = _min;
    if (degree > _max) degree = _max;

    targetDegree = degree;
    _start = currentDegree; // Start easing from the current position
    _duration = duration;
    _startMillis = millis(); // Record start time
}

// Updates the servo's position during an easing movement
void EasingServo::updatePosition()
{
    if (_duration > 0) // If an easing movement is active
    {
        unsigned long currentMillis = millis();
        unsigned long elapsedMillis = currentMillis - _startMillis;
        double percentage = (double)elapsedMillis / _duration;

        if (percentage >= 1.0)
        {
            currentDegree = targetDegree; // Reached target
            _duration = 0; // End easing movement
        }
        else
        {
            // Calculate eased position
            currentDegree = easeInAndOut(_start, currentDegree, targetDegree, percentage);
        }
        // Apply the calculated degree to the servo
        // Map the degree to the PCA9685 pulse length range
        uint16_t pulselength = map(currentDegree, 0, 180, HuyangNeck_SERVOMIN, HuyangNeck_SERVOMAX);
        _pwm->setPWM(_servoPin, 0, pulselength);
    }
}

// Easing function: Ease-in-out quadratic
double EasingServo::easeInOutQuad(double t)
{
    // Quadratic easing in/out function
    return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
}

// Generic easing function to calculate intermediate position
double EasingServo::easeInAndOut(double start, double current, double target, double percentage)
{
    double result = target; // Default to target if percentage is >= 1.0

    if (percentage > 1.0)
    {
        percentage = 1.0; // Cap percentage at 1.0 (100%)
    }

    // Only apply easing if current is not already at target
    if (current != target)
    {
        double easeInOut = easeInOutQuad(percentage);

        if (current < target)
        {
            double subTarget = (target - start);
            result = start + (subTarget * easeInOut);
            if (result > target)
            {
                result = target;
            }
        }
        else if (current > target)
        {
            double subTarget = (start - target);
            result = start - (subTarget * easeInOut);
            if (result < target)
            {
                result = target;
            }
        }
    }
    return result;
}
