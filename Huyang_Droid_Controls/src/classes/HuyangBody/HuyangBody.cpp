#include "HuyangBody.h" // In the same folder
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h> // Include Wire.h for I2C communication
#include <Arduino.h> // For Serial.println

// Corrected typo: Adafruit_PWMServoDriver
HuyangBody::HuyangBody(Adafruit_PWMServoDriver *pwm)
{
	_pwm = pwm;
	// Initialize NeoPixel object for 2 pixels on NEO_PIXEL_PIN
	// This pin MUST be defined in config.h or similar if it's not a fixed value.
	_neoPixelLights = new Adafruit_NeoPixel(NEO_PIXEL_COUNT, NEO_PIXEL_PIN, pixelFormat);
	_neoPixelLights->setBrightness(20); // Set initial brightness (0-255)
	// Note: _neoPixelLights->begin() is called here, but _pwm->begin() is still needed below.
}

void HuyangBody::setup()
{
    // --- IMPORTANT: Initialize the PWM driver before using it ---
    // This begins I2C communication with the PCA9685 board.
    // It is crucial this happens BEFORE any setPWM calls.
    Serial.println("HuyangBody: Initializing PCA9685 PWM Driver...");
    if (!_pwm->begin()) {
        Serial.println("HuyangBody: Failed to initialize PCA9685! Check wiring or I2C address.");
        // Consider adding a more robust error handling, e.g., halting or retrying.
        while (1); // Halt if initialization fails to prevent further crashes
    }
    Serial.println("HuyangBody: PCA9685 PWM Driver initialized.");

    // Set the PWM frequency for the servos (e.g., 50Hz for standard servos)
    _pwm->setPWMFreq(HuyangBody_SERVO_FREQ); // Use the define from HuyangBody.h

	// Initialize NeoPixel library (already in constructor, but safe to call again or move here)
	_neoPixelLights->begin();

	// Initial centering of all body servos
	centerAll();
	_neoPixelLights->show(); // Ensure NeoPixels are off or at their default state
	updateChestLights(); // Set initial light mode
    Serial.println("HuyangBody: Setup complete.");
}

// Maps a degree value to a PWM pulselength and sends it to the specified servo pin
void HuyangBody::rotateServo(uint8_t servo, uint16_t degree)
{
    Serial.printf("HuyangBody::rotateServo - Servo: %d, Degree (0-180 for PWM): %d\n", servo, degree);
	// Map the input degree (0-180 for standard servos) to the PCA9685 pulse length range
	uint16_t pulselength = map(degree, 0, 180, HuyangBody_SERVOMIN, HuyangBody_SERVOMAX);

	// Set the PWM output for the specified servo pin
	_pwm->setPWM(servo, 0, pulselength);
    Serial.printf("HuyangBody::rotateServo - Set PWM for servo %d to pulselength %d\n", servo, pulselength);
}

// Main loop for HuyangBody, called repeatedly from system.h
void HuyangBody::loop()
{
	_currentMillis = millis(); // Get current time

	// Reset previousMillis if system time rolls over
	if (_previousMillis > _currentMillis)
	{
		_previousMillis = _currentMillis;
	}

	// If in automatic mode, trigger random movements
	if (automatic == true)
	{
		doRandomRotate();
		doRandomTiltForward();
		doRandomTiltSideways();
	}

	updateChestLights(); // Continuously update chest lights based on current mode
}

// --- Body Movement Control Functions ---

// Controls body sideways tilt
void HuyangBody::tiltBodySideways(int16_t degree)
{
    Serial.printf("HuyangBody::tiltBodySideways: Input degree (User -90 to 90): %d\n", degree);
	// Convert -90 to 90 degree range to 0 to 180 range, then apply calibration
    // Also, apply specific physical mapping if required for sideways servos
	int16_t mappedDegree = degree + 90 + calibrationTiltSideways;

    // Apply specific offsets/inversions for physical setup if necessary, e.g.,
    // if left and right servos need opposite degree inputs from the same mappedDegree.
    // Example from original code had specific shifts and different mapping for left/right:
    // int16_t calibrationShift = -22; // max 0 to -60
    // int16_t calibratedDegree = map(degree, -100, 100, -60 + calibrationShift, 60 + calibrationShift);
    // uint16_t rotateDegree = map(calibratedDegree, -100, 100, 0, 170);

    // Assuming a symmetrical setup where +ve degree maps to one side, -ve to other.
    // The incoming `degree` is already -90 to 90.
    // Let's use `mappedDegree` (0-180) for rotateServo directly.
    
    // Clamp to 0-180
    if (mappedDegree < 0) mappedDegree = 0;
    if (mappedDegree > 180) mappedDegree = 180;

	rotateServo(pwm_pin_sideway_left, mappedDegree);
	rotateServo(pwm_pin_sideway_right, 180 - mappedDegree); // Invert for opposing motion
    Serial.printf("HuyangBody::tiltBodySideways: Mapped to 0-180 range (with cal): %d, Sending to servos.\n", mappedDegree);
}

// Controls body forward/backward tilt
void HuyangBody::tiltBodyForward(int16_t degree)
{
    Serial.printf("HuyangBody::tiltBodyForward: Input degree (User -90 to 90): %d\n", degree);
	// Convert -90 to 90 degree range to 0 to 180 range, then apply calibration
	int16_t mappedDegree = degree + 90 + calibrationTiltForward;
    
    // Clamp to 0-180
    if (mappedDegree < 0) mappedDegree = 0;
    if (mappedDegree > 180) mappedDegree = 180;

	rotateServo(pwm_pin_forward_left, mappedDegree);
	rotateServo(pwm_pin_forward_right, 180 - mappedDegree); // Invert for opposing motion
    Serial.printf("HuyangBody::tiltBodyForward: Mapped to 0-180 range (with cal): %d, Sending to servos.\n", mappedDegree);
}

// Controls body rotation (hip/torso rotation)
void HuyangBody::rotateBody(int16_t degree)
{
    Serial.printf("HuyangBody::rotateBody: Input degree (User -90 to 90): %d\n", degree);
	// Convert -90 to 90 degree range to 0 to 180 range, then apply calibration
	int16_t mappedDegree = degree + 90 + calibrationRotate;
    
    // Clamp to 0-180
    if (mappedDegree < 0) mappedDegree = 0;
    if (mappedDegree > 180) mappedDegree = 180;

	rotateServo(pwm_pin_body_rotate, mappedDegree);
    Serial.printf("HuyangBody::rotateBody: Mapped to 0-180 range (with cal): %d, Sending to servos.\n", mappedDegree);
}

// Sets all body servos to their predefined center positions
void HuyangBody::centerAll()
{
    Serial.println("HuyangBody::centerAll called.");
	// These values (0) now correspond to the center of the -90 to 90 range.
	tiltBodySideways(0);
	delay(500); // Small delay to allow servos to reach position
	tiltBodyForward(0);
	delay(500);
	rotateBody(0);
    Serial.println("HuyangBody: All body servos commanded to center.");
}

// --- Random Movement Functions (for automatic mode) ---

void HuyangBody::doRandomRotate()
{
	if (_randomDoRotate == 0)
	{
		_randomDoRotate = _currentMillis + 2000 + (random(6, 12 + 1) * 1000);
	}

	if (_currentMillis > _randomDoRotate)
	{
		_randomDoRotate = 0;

		int16_t randomDegree;
		if (random(0, 2) == 0) // Randomly choose direction
		{
			randomDegree = -(random(10, 80 + 1)); // Rotate left within -90 range
		}
		else
		{
			randomDegree = random(10, 80 + 1); // Rotate right within 90 range
		}
		rotateBody(randomDegree);
        Serial.printf("HuyangBody::doRandomRotate - New random rotation triggered to %d\n", randomDegree);
	}
}

void HuyangBody::doRandomTiltForward()
{
	if (_randomDoTiltForward == 0)
	{
		_randomDoTiltForward = _currentMillis + 2500 + (random(6, 12 + 1) * 1050);
	}

	if (_currentMillis > _randomDoTiltForward)
	{
		_randomDoTiltForward = 0;

		int16_t randomDegree;
		if (random(0, 2) == 0) // Randomly choose direction
		{
			randomDegree = -(random(10, 80 + 1)); // Tilt forward within -90 range
		}
		else
		{
			randomDegree = random(10, 80 + 1); // Tilt backward within 90 range
		}
		tiltBodyForward(randomDegree);
        Serial.printf("HuyangBody::doRandomTiltForward - New random tilt forward triggered to %d\n", randomDegree);
	}
}

void HuyangBody::doRandomTiltSideways()
{
	if (_randomDoTiltSideways == 0)
	{
		_randomDoTiltSideways = _currentMillis + 3000 + (random(5, 10 + 1) * 1100);
	}

	if (_currentMillis > _randomDoTiltSideways)
	{
		_randomDoTiltSideways = 0;

		int16_t randomDegree;
		if (random(0, 2) == 0) // Randomly choose direction
		{
			randomDegree = -(random(10, 80 + 1)); // Tilt left within -90 range
		}
		else
		{
			randomDegree = random(10, 80 + 1); // Tilt right within 90 range
		}
		tiltBodySideways(randomDegree);
        Serial.printf("HuyangBody::doRandomTiltSideways - New random tilt sideways triggered to %d\n", randomDegree);
	}
}

// --- NEW: Chest Light Control Functions ---

// Main function to update chest light behavior based on currentLightMode
void HuyangBody::updateChestLights()
{
	switch (currentLightMode)
	{
	case LIGHT_OFF:
		setAllLights(0); // Turn all pixels off (black color)
		break;
	case LIGHT_STATIC_BLUE: // New case for static blue
		setAllLights(_neoPixelLights->Color(0, 0, 255)); // Blue color
		break;
	case LIGHT_WARNING_BLINK: // New case for warning blink (Red/Blue alternating)
		if (_currentMillis - _lastLightToggleMillis > _blinkInterval)
		{
			_lastLightToggleMillis = _currentMillis;
			if (_neoPixelLights->getPixelColor(0) == _neoPixelLights->Color(255, 0, 0)) // If red
			{
				setAllLights(_neoPixelLights->Color(0, 0, 255)); // Set to blue
			}
			else
			{
				setAllLights(_neoPixelLights->Color(255, 0, 0)); // Set to red
			}
		}
		break;
	case LIGHT_PROCESSING_FADE: // Placeholder for more complex fade animation
		// Implement fading logic here
		// For now, a simple pulse or static color
		setAllLights(_neoPixelLights->Color(0, 255, 255)); // Cyan for processing
		break;
	case LIGHT_DROID_MODE_1: // Placeholder for Droid Mode 1
		setAllLights(_neoPixelLights->Color(255, 128, 0)); // Orange
		break;
	case LIGHT_DROID_MODE_2: // Placeholder for Droid Mode 2
		setAllLights(_neoPixelLights->Color(128, 0, 255)); // Purple
		break;
	default:
		setAllLights(0); // Default to off
		break;
	}
	_neoPixelLights->show(); // Update the NeoPixels to reflect the changes
}

// Sets a single NeoPixel to a specified color
void HuyangBody::setLight(uint8_t pixelNum, uint32_t color)
{
	if (pixelNum < NEO_PIXEL_COUNT)
	{
		_neoPixelLights->setPixelColor(pixelNum, color);
	}
}

// Sets all NeoPixels to a specified color
void HuyangBody::setAllLights(uint32_t color)
{
	for (uint8_t i = 0; i < NEO_PIXEL_COUNT; i++)
	{
		_neoPixelLights->setPixelColor(i, color);
	}
}
