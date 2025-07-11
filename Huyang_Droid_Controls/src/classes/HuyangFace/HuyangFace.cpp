#include "HuyangFace.h"
#include <Arduino.h> // For Serial.println

// Constructor
HuyangFace::HuyangFace(Arduino_GFX *left, Arduino_GFX *right)
{
    _leftEye = left;
    _rightEye = right;
    // Initialize display dimensions if displays are valid
    if (_leftEye) {
        _tftDisplayWidth = _leftEye->width();
        _tftDisplayHeight = _leftEye->height();
    } else if (_rightEye) {
        _tftDisplayWidth = _rightEye->width();
        _tftDisplayHeight = _rightEye->height();
    }
}

// Helper to convert uint16_t (from WebServer) to EyeState enum
EyeState HuyangFace::getStateFrom(uint16_t stateValue)
{
    switch (stateValue)
    {
    case 1: return EYE_STATE_OPEN;
    case 2: return EYE_STATE_CLOSED;
    case 3: return EYE_STATE_BLINK;
    case 4: return EYE_STATE_FOCUS;
    case 5: return EYE_STATE_SAD;
    case 6: return EYE_STATE_ANGRY;
    default: return EYE_STATE_NONE; // Default or unknown state
    }
}

// Public method to set both eyes to a new state
void HuyangFace::setEyesTo(EyeState newState)
{
    Serial.printf("HuyangFace::setEyesTo called with state: %d\n", newState);
    if (_leftEyeTargetState != newState || _rightEyeTargetState != newState)
    {
        _leftEyeTargetState = newState;
        _rightEyeTargetState = newState;
        _leftEyeLastSelectedState = newState; // Store the last manually selected state
        _rightEyeLastSelectedState = newState;
        _randomDuration = 0; // Reset random duration to avoid immediate automatic override
        _previousRandomMillis = _currentMillis; // Reset timer for next random animation
        Serial.println("HuyangFace: Both eyes target state updated.");
    }
}

// Public method to set the left eye to a new state
void HuyangFace::setLeftEyeTo(EyeState newState)
{
    Serial.printf("HuyangFace::setLeftEyeTo called with state: %d\n", newState);
    if (_leftEyeTargetState != newState)
    {
        _leftEyeTargetState = newState;
        _leftEyeLastSelectedState = newState;
        _randomDuration = 0; // Reset random duration
        _previousRandomMillis = _currentMillis; // Reset timer for next random animation
        Serial.println("HuyangFace: Left eye target state updated.");
    }
}

// Public method to set the right eye to a new state
void HuyangFace::setRightEyeTo(EyeState newState)
{
    Serial.printf("HuyangFace::setRightEyeTo called with state: %d\n", newState);
    if (_rightEyeTargetState != newState)
    {
        _rightEyeTargetState = newState;
        _rightEyeLastSelectedState = newState;
        _randomDuration = 0; // Reset random duration
        _previousRandomMillis = _currentMillis; // Reset timer for next random animation
        Serial.println("HuyangFace: Right eye target state updated.");
    }
}

// Method to enable/disable automatic animations
void HuyangFace::setAutomatic(bool state) {
    automatic = state;
    Serial.printf("HuyangFace: Automatic animations set to: %s\n", automatic ? "true" : "false");
    if (!automatic) {
        // If switching to manual, restore last selected states
        _leftEyeTargetState = _leftEyeLastSelectedState;
        _rightEyeTargetState = _rightEyeLastSelectedState;
        Serial.println("HuyangFace: Restoring last selected manual eye states.");
    }
}

// Setup function
void HuyangFace::setup()
{
    Serial.println("HuyangFace: Initializing eye displays...");
    if (_leftEye)
    {
        _leftEye->begin();
        _leftEye->setRotation(3); // Adjust rotation as needed for your display orientation
        _leftEye->fillScreen(_huyangEyeColor); // Fill with initial color
        Serial.println("HuyangFace: Left eye display initialized.");
    }
    else
    {
        Serial.println("HuyangFace: Left eye display not initialized (nullptr).");
    }

    if (_rightEye)
    {
        _rightEye->begin();
        _rightEye->setRotation(1); // Adjust rotation as needed
        _rightEye->fillScreen(_huyangEyeColor); // Fill with initial color
        Serial.println("HuyangFace: Right eye display initialized.");
    }
    else
    {
        Serial.println("HuyangFace: Right eye display not initialized (nullptr).");
    }

    // Set initial states
    setEyesTo(EYE_STATE_OPEN); // Start with eyes open
    Serial.println("HuyangFace: Setup complete. Eyes set to OPEN.");
}

// Main loop function
void HuyangFace::loop()
{
    _currentMillis = millis();

    // Handle millis() rollover
    if (_previousMillis > _currentMillis)
    {
        _previousMillis = _currentMillis;
    }

    // Automatic animation logic
    if (automatic == true)
    {
        // Trigger random animation after a duration
        if (_currentMillis > _previousRandomMillis + _randomDuration)
        {
            _previousRandomMillis = _currentMillis;
            _randomDuration = random(5, 10 + 1) * 1000; // New random duration (5-10 seconds)
            
            // Choose a random eye state for automatic animation (excluding NONE)
            int randomState = random(1, 7); // 1 to 6 (Open, Closed, Blink, Focus, Sad, Angry)
            setEyesTo(getStateFrom(randomState));
            Serial.printf("HuyangFace: Automatic mode triggered random state: %d\n", randomState);
        }
    }

    // Update eye states based on target states
    if (_leftEye) {
        drawEye(_leftEye, _leftEyeTargetState);
    }
    if (_rightEye) {
        drawEye(_rightEye, _rightEyeTargetState);
    }

    // Handle specific loop-based animations (e.g., blinking)
    // The doRandomBlink function will manage the actual blink animation
    if (_leftEyeTargetState == EYE_STATE_BLINK || _rightEyeTargetState == EYE_STATE_BLINK) {
        doRandomBlink();
    }

    // Call mood-specific loop functions (defined in HuyangFace_moods.cpp)
    openEyesLoop();
    closeEyesLoop();
    focusEyesLoop();
    sadEyesLoop();
    angryEyesLoop();

    _previousMillis = _currentMillis;
}

// Generic drawing function based on state
void HuyangFace::drawEye(Arduino_GFX *eye, EyeState state) {
    if (!eye) return;

    switch (state) {
        case EYE_STATE_OPEN:
            drawOpenEye(eye);
            break;
        case EYE_STATE_CLOSED:
            drawClosedEye(eye);
            break;
        case EYE_STATE_BLINK:
            // Blink is handled by doRandomBlink, which calls drawOpenEye/drawClosedEye
            // This case might not need direct drawing here, or it could draw a default "open" state
            // if not actively blinking.
            break;
        case EYE_STATE_FOCUS:
            drawFocusEye(eye);
            break;
        case EYE_STATE_SAD:
            drawSadEye(eye);
            break;
        case EYE_STATE_ANGRY:
            drawAngryEye(eye);
            break;
        case EYE_STATE_NONE:
        default:
            eye->fillScreen(0x0000); // Black screen for NONE or unknown
            break;
    }
}

// Private functions for drawing eye states (simplified examples)
void HuyangFace::drawOpenEye(Arduino_GFX *eye) {
    if (!eye) return;
    eye->fillScreen(_huyangEyeColor); // Fill with main eye color
    // Add pupil, highlights etc.
    eye->fillCircle(eye->width() / 2, eye->height() / 2, eye->width() / 4, 0x0000); // Black pupil
    // Serial.println("HuyangFace: Drawing Open Eye.");
}

void HuyangFace::drawClosedEye(Arduino_GFX *eye) {
    if (!eye) return;
    eye->fillScreen(0x0000); // Black screen for closed
    // Serial.println("HuyangFace: Drawing Closed Eye.");
}

void HuyangFace::drawBlinkAnimation(Arduino_GFX *eye) {
    // This function would contain the animation steps for a smooth blink.
    // For now, it's a placeholder. The doRandomBlink function will toggle states.
    // Serial.println("HuyangFace: Drawing Blink Animation (placeholder).");
}

void HuyangFace::drawFocusEye(Arduino_GFX *eye) {
    if (!eye) return;
    eye->fillScreen(_huyangEyeColor);
    eye->fillCircle(eye->width() / 2, eye->height() / 2, eye->width() / 5, 0x0000); // Smaller pupil
    eye->drawCircle(eye->width() / 2, eye->height() / 2, eye->width() / 4, 0xFFFF); // White ring
    // Serial.println("HuyangFace: Drawing Focus Eye.");
}

void HuyangFace::drawSadEye(Arduino_GFX *eye) {
    if (!eye) return;
    eye->fillScreen(_huyangEyeColor);
    eye->fillCircle(eye->width() / 2, eye->height() / 2, eye->width() / 4, 0x0000);
    // Draw a sad eyebrow (e.g., a line)
    eye->drawLine(eye->width() / 4, eye->height() / 4, eye->width() * 3 / 4, eye->height() / 4 + 10, 0xFFFF);
    // Serial.println("HuyangFace: Drawing Sad Eye.");
}

void HuyangFace::drawAngryEye(Arduino_GFX *eye) {
    if (!eye) return;
    eye->fillScreen(_huyangEyeColor);
    eye->fillCircle(eye->width() / 2, eye->height() / 2, eye->width() / 4, 0x0000);
    // Draw an angry eyebrow (e.g., a slanted line)
    eye->drawLine(eye->width() / 4, eye->height() / 4 + 10, eye->width() * 3 / 4, eye->height() / 4, 0xFFFF);
    // Serial.println("HuyangFace: Drawing Angry Eye.");
}

// Private functions for random eye movements/animations
void HuyangFace::doRandomBlink()
{
    // This function is called continuously if the target state is BLINK.
    // It manages the actual timing of the blink animation.
    if (_currentMillis - _lastBlinkMillis > _blinkInterval)
    {
        _lastBlinkMillis = _currentMillis;
        // Toggle between open and closed for a quick blink
        if (_currentLeftEyeState == EYE_STATE_OPEN) {
            _currentLeftEyeState = EYE_STATE_CLOSED;
            _currentRightEyeState = EYE_STATE_CLOSED;
            // Set a very short interval for the closed state during a blink
            _blinkInterval = 100; 
        } else {
            _currentLeftEyeState = EYE_STATE_OPEN;
            _currentRightEyeState = EYE_STATE_OPEN;
            // Reset to normal blink interval
            _blinkInterval = random(3000, 7000); // Randomize next blink
        }
        Serial.printf("HuyangFace: Random blink triggered. Current state: %d\n", _currentLeftEyeState);
    }
    // Update the actual drawing state based on _currentLeftEyeState/_currentRightEyeState
    // This is handled by drawEye in the main loop, which will pick up _currentLeftEyeState.
    // For a smooth blink, you would typically use an easing function here to draw intermediate states.
}
