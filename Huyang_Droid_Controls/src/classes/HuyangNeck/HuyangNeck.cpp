#include "HuyangNeck.h" // In the same folder
#include <Adafruit_PWMServoDriver.h>
#include "../EasingServo/EasingServo.h" // Corrected: Path to EasingServo.h from HuyangNeck.cpp
#include <Arduino.h> // For Serial.println

HuyangNeck::HuyangNeck(Adafruit_PWMServoDriver *pwm)
{
	_pwm = pwm;

    // Initialize EasingServo instances for neck movements
    // Parameters: pwm driver, servo pin, min degree, max degree, start degree
    // EasingServo is designed to work with a 0-180 range internally for standard servos.
    // The mapping from -90 to 90 degrees will be handled *before* passing to moveServoTo.
    _neckRotateServo = new EasingServo(_pwm, pwm_pin_head_rotate, 0, 180, 90); // 0-180 degrees, start at 90 (center)
    _neckTiltForwardServo = new EasingServo(_pwm, pwm_pin_head_neck, 0, 180, 90); // 0-180 degrees, start at 90 (center)
    _neckTiltSidewaysServo = new EasingServo(_pwm, pwm_pin_head_left, 0, 180, 90); // 0-180 degrees, start at 90 (center)

    // NEW: Initialize EasingServo for monocle
    _monocleServo = new EasingServo(_pwm, pwm_pin_head_monocle, 0, 180, 0); // Example: 0-180 degrees, start at 0 (retracted)
}

void HuyangNeck::setup()
{
	// Initial centering or setup for servos if needed
	// EasingServos are initialized with a start position, so they should be at that position.
    Serial.println("HuyangNeck: Initializing PWM driver and setting frequency.");
    _pwm->begin(); // Initialize the PWM driver
    _pwm->setPWMFreq(HuyangNeck_SERVO_FREQ); // Set the PWM frequency
    
    // Set initial positions for all servos using their public rotateServo method
    // These will be relative to their 0-180 range, with 90 being mechanical center for neck.
    _neckRotateServo->rotateServo(_neckRotateServo->currentDegree);
    _neckTiltForwardServo->rotateServo(_neckTiltForwardServo->currentDegree);
    _neckTiltSidewaysServo->rotateServo(_neckTiltSidewaysServo->currentDegree);
    _monocleServo->rotateServo(_monocleServo->currentDegree); // Set initial monocle position
    Serial.println("HuyangNeck: Servos initialized to default positions.");
}

// Easing function: Ease-in-out quadratic
double HuyangNeck::easeInOutQuad(double t)
{
    // Quadratic easing in/out function
	return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
}

// Generic easing function to calculate intermediate position
double HuyangNeck::easeInAndOut(double start, double current, double target, double percentage)
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

// Public method to set target rotation for the head
void HuyangNeck::rotateHead(double degree, double duration)
{
    Serial.printf("HuyangNeck::rotateHead: Input degree (User -90 to 90): %.2f\n", degree);
    // Convert -90 to 90 degree range to 0 to 180 range for EasingServo, then apply calibration
    double mappedDegree = degree + 90.0 + calibrationRotate;
    // Clamp to 0-180 to prevent issues with servo limits
    if (mappedDegree < 0) mappedDegree = 0;
    if (mappedDegree > 180) mappedDegree = 180;
    
    Serial.printf("HuyangNeck::rotateHead: Mapped to 0-180 range (with cal): %.2f, Duration: %.0f\n", mappedDegree, duration);
    _neckRotateServo->moveServoTo(mappedDegree, duration);
}

// Public method to set target forward tilt for the neck
void HuyangNeck::tiltNeckForward(double degree, double duration)
{
    Serial.printf("HuyangNeck::tiltNeckForward: Input degree (User -90 to 90): %.2f\n", degree);
    // Convert -90 to 90 degree range to 0 to 180 range for EasingServo, then apply calibration
    double mappedDegree = degree + 90.0 + calibrationTiltForward;
    // Clamp to 0-180
    if (mappedDegree < 0) mappedDegree = 0;
    if (mappedDegree > 180) mappedDegree = 180;

    Serial.printf("HuyangNeck::tiltNeckForward: Mapped to 0-180 range (with cal): %.2f, Duration: %.0f\n", mappedDegree, duration);
    _neckTiltForwardServo->moveServoTo(mappedDegree, duration);
}

// Public method to set target sideways tilt for the neck
void HuyangNeck::tiltNeckSideways(double degree, double duration)
{
    Serial.printf("HuyangNeck::tiltNeckSideways: Input degree (User -90 to 90): %.2f\n", degree);
    // Convert -90 to 90 degree range to 0 to 180 range for EasingServo, then apply calibration
    double mappedDegree = degree + 90.0 + calibrationTiltSideways;
    // Clamp to 0-180
    if (mappedDegree < 0) mappedDegree = 0;
    if (mappedDegree > 180) mappedDegree = 180;

    Serial.printf("HuyangNeck::tiltNeckSideways: Mapped to 0-180 range (with cal): %.2f, Duration: %.0f\n", mappedDegree, duration);
    _neckTiltSidewaysServo->moveServoTo(mappedDegree, duration);
}

// NEW: Public method to set target position for the monocle
void HuyangNeck::setMonoclePosition(int16_t position, double duration)
{
    Serial.printf("HuyangNeck::setMonoclePosition: Input position (raw, e.g., 0-180): %d\n", position);
    // Monocle position is assumed to be in the 0-180 range already from the UI or fixed values
    double finalPosition = position + calibrationMonocle;
    // Clamp to 0-180
    if (finalPosition < 0) finalPosition = 0;
    if (finalPosition > 180) finalPosition = 180;

    Serial.printf("HuyangNeck::setMonoclePosition: Final position (with cal): %.2f, Duration: %.0f\n", finalPosition, duration);
    _monocleServo->moveServoTo(finalPosition, duration);
}


void HuyangNeck::updateNeckPosition()
{
    // Update all easing servos using their public updatePosition method
    _neckRotateServo->updatePosition();
    _neckTiltForwardServo->updatePosition();
    _neckTiltSidewaysServo->updatePosition();
    _monocleServo->updatePosition(); // NEW: Update monocle servo
}


void HuyangNeck::loop()
{
	_currentMillis = millis();

	// Handle millis() rollover
	if (_previousMillis > _currentMillis)
	{
		_previousMillis = _currentMillis;
	}

	// Update all servo positions based on their easing calculations
	updateNeckPosition();

	// Automatic movement logic (only if 'automatic' flag is true)
	if (automatic == true)
	{
		doRandomRotate();
		doRandomTiltForward();
		doRandomTiltSideways();
	}
}

// Function for random head rotation (for automatic mode)
void HuyangNeck::doRandomRotate()
{
	if (_randomDoRotate == 0) // If no random rotation is scheduled
	{
		// Schedule next random rotation after an initial delay + random interval
		_randomDoRotate = _currentMillis + 2000 + (random(5, 10 + 1) * 1000);
	}

	if (_currentMillis > _randomDoRotate) // If scheduled time has passed
	{
		_randomDoRotate = 0; // Reset schedule

		// Generate random degree in -90 to 90 range
		double randomDegree = random(-90, 90 + 1); // +1 to include max
		rotateHead(randomDegree, random(2, 5 + 1) * 1000);
        Serial.printf("HuyangNeck::doRandomRotate - New random rotation triggered to %.2f\n", randomDegree);
	}
}

void HuyangNeck::doRandomTiltForward()
{
	if (_randomDoTiltForward == 0) // If no random forward tilt is scheduled
	{
		// Schedule next random tilt after an initial delay + random interval
		_randomDoTiltForward = _currentMillis + 2500 + (random(6, 12 + 1) * 1050);
	}

	if (_currentMillis > _randomDoTiltForward) // If scheduled time has passed
	{
		_randomDoTiltForward = 0; // Reset schedule

		// Generate random degree in -90 to 90 range
		double randomDegree = random(-90, 90 + 1);
		tiltNeckForward(randomDegree, random(3, 6 + 1) * 1000);
        Serial.printf("HuyangNeck::doRandomTiltForward - New random tilt forward triggered to %.2f\n", randomDegree);
	}
}

// Function for random sideways tilt (for automatic mode)
void HuyangNeck::doRandomTiltSideways()
{
	if (_randomDoTiltSideways == 0) // If no random sideways tilt is scheduled
	{
		// Schedule next random tilt after an initial delay + random interval
		_randomDoTiltSideways = _currentMillis + 3000 + (random(5, 10 + 1) * 1100);
	}

	if (_currentMillis > _randomDoTiltSideways) // If scheduled time has passed
	{
		_randomDoTiltSideways = 0; // Reset schedule

		// Generate random degree in -90 to 90 range
		double randomDegree = random(-90, 90 + 1);
		tiltNeckSideways(randomDegree, random(2, 5 + 1) * 1000);
        Serial.printf("HuyangNeck::doRandomTiltSideways - New random tilt sideways triggered to %.2f\n", randomDegree);
	}
}
