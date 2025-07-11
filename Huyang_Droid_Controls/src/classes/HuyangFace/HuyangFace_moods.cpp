#include "HuyangFace.h"
#include <Arduino.h> // For Serial.println and millis()

// --- openEyesLoop ---
void HuyangFace::openEyesLoop()
{
    // Check if either eye's target state is OPEN or BLINK
    if ((_leftEyeTargetState == EyeState::EYE_STATE_OPEN || _leftEyeTargetState == EyeState::EYE_STATE_BLINK) ||
        (_rightEyeTargetState == EyeState::EYE_STATE_OPEN || _rightEyeTargetState == EyeState::EYE_STATE_BLINK))
    {
        // Determine if each eye needs to transition to OPEN
        bool shouldDoLeftEye = (_leftEyeTargetState == EyeState::EYE_STATE_OPEN || _leftEyeTargetState == EyeState::EYE_STATE_BLINK) && _currentLeftEyeState != EyeState::EYE_STATE_OPEN;
        bool shouldDoRightEye = (_rightEyeTargetState == EyeState::EYE_STATE_OPEN || _rightEyeTargetState == EyeState::EYE_STATE_BLINK) && _currentRightEyeState != EyeState::EYE_STATE_OPEN;

        // If both eyes need to open, perform a synchronized open animation
        if (shouldDoLeftEye && shouldDoRightEye)
        {
            openEyes(_huyangEyeColor); // Use the main eye color
            _currentLeftEyeState = EyeState::EYE_STATE_OPEN;
            _currentRightEyeState = EyeState::EYE_STATE_OPEN;
        }
        else // Otherwise, handle individual eye opening
        {
            if (shouldDoLeftEye)
            {
                openEye(_leftEye, _huyangEyeColor);
                _currentLeftEyeState = EyeState::EYE_STATE_OPEN;
            }
            if (shouldDoRightEye)
            {
                openEye(_rightEye, _huyangEyeColor);
                _currentRightEyeState = EyeState::EYE_STATE_OPEN;
            }
        }
    }
}

// --- openEyes (both eyes) ---
void HuyangFace::openEyes(uint16_t color)
{
    if (!_leftEye || !_rightEye) return;

    Serial.println("HuyangFace: Performing openEyes animation.");
    for (uint16_t step = 0; step <= _tftDisplayHeight / 2; step++)
    {
        uint16_t position = (_tftDisplayHeight / 2) - step; // From center outwards
        _leftEye->drawFastHLine(0, position, _tftDisplayWidth, color);
        _leftEye->drawFastHLine(0, _tftDisplayHeight - 1 - position, _tftDisplayWidth, color);

        _rightEye->drawFastHLine(0, position, _tftDisplayWidth, color);
        _rightEye->drawFastHLine(0, _tftDisplayHeight - 1 - position, _tftDisplayWidth, color);

        _previousMillis = _currentMillis;
        while (_currentMillis - _previousMillis < _blinkInterval) // Use _blinkInterval for speed control
        {
            _currentMillis = millis();
        }
    }
}

// --- openEye (single eye) ---
void HuyangFace::openEye(Arduino_GFX *eye, uint16_t color)
{
    if (!eye) return;

    Serial.printf("HuyangFace: Performing openEye animation for single eye.\n");
    for (uint16_t step = 0; step <= _tftDisplayHeight / 2; step++)
    {
        uint16_t position = (_tftDisplayHeight / 2) - step; // From center outwards
        eye->drawFastHLine(0, position, _tftDisplayWidth, color);
        eye->drawFastHLine(0, _tftDisplayHeight - 1 - position, _tftDisplayWidth, color);

        _previousMillis = _currentMillis;
        while (_currentMillis - _previousMillis < _blinkInterval) // Use _blinkInterval for speed control
        {
            _currentMillis = millis();
        }
    }
}

// --- closeEyesLoop ---
void HuyangFace::closeEyesLoop()
{
    // Check if either eye's target state is CLOSED or BLINK
    if ((_leftEyeTargetState == EyeState::EYE_STATE_CLOSED || _leftEyeTargetState == EyeState::EYE_STATE_BLINK) ||
        (_rightEyeTargetState == EyeState::EYE_STATE_CLOSED || _rightEyeTargetState == EyeState::EYE_STATE_BLINK))
    {
        // If both eyes need to close and are not already closed
        if ((_leftEyeTargetState == EyeState::EYE_STATE_CLOSED || _leftEyeTargetState == EyeState::EYE_STATE_BLINK) && _currentLeftEyeState != EyeState::EYE_STATE_CLOSED &&
            (_rightEyeTargetState == EyeState::EYE_STATE_CLOSED || _rightEyeTargetState == EyeState::EYE_STATE_BLINK) && _currentRightEyeState != EyeState::EYE_STATE_CLOSED)
        {
            closeEyes(0x0000); // Black for closed eyes
            _currentLeftEyeState = EyeState::EYE_STATE_CLOSED;
            _currentRightEyeState = EyeState::EYE_STATE_CLOSED;
        }
        else // Handle individual eye closing
        {
            if ((_leftEyeTargetState == EyeState::EYE_STATE_CLOSED || _leftEyeTargetState == EyeState::EYE_STATE_BLINK) && _currentLeftEyeState != EyeState::EYE_STATE_CLOSED)
            {
                closeEye(_leftEye, 0x0000);
                _currentLeftEyeState = EyeState::EYE_STATE_CLOSED;
            }
            if ((_rightEyeTargetState == EyeState::EYE_STATE_CLOSED || _rightEyeTargetState == EyeState::EYE_STATE_BLINK) && _currentRightEyeState != EyeState::EYE_STATE_CLOSED)
            {
                closeEye(_rightEye, 0x0000);
                _currentRightEyeState = EyeState::EYE_STATE_CLOSED;
            }
        }
    }
}

// --- closeEyes (both eyes) ---
void HuyangFace::closeEyes(uint16_t color)
{
    if (!_leftEye || !_rightEye) return;

    Serial.println("HuyangFace: Performing closeEyes animation.");
    for (uint16_t step = 0; step <= _tftDisplayHeight / 2; step++)
    {
        _leftEye->drawFastHLine(0, step, _tftDisplayWidth, color);
        _leftEye->drawFastHLine(0, _tftDisplayHeight - 1 - step, _tftDisplayWidth, color);

        _rightEye->drawFastHLine(0, step, _tftDisplayWidth, color);
        _rightEye->drawFastHLine(0, _tftDisplayHeight - 1 - step, _tftDisplayWidth, color);

        _previousMillis = _currentMillis;
        while (_currentMillis - _previousMillis < _blinkInterval)
        {
            _currentMillis = millis();
        }
    }
}

// --- closeEye (single eye) ---
void HuyangFace::closeEye(Arduino_GFX *eye, uint16_t color)
{
    if (!eye) return;

    Serial.printf("HuyangFace: Performing closeEye animation for single eye.\n");
    for (uint16_t step = 0; step <= _tftDisplayHeight / 2; step++)
    {
        eye->drawFastHLine(0, step, _tftDisplayWidth, color);
        eye->drawFastHLine(0, _tftDisplayHeight - 1 - step, _tftDisplayWidth, color);

        _previousMillis = _currentMillis;
        while (_currentMillis - _previousMillis < _blinkInterval)
        {
            _currentMillis = millis();
        }
    }
}

// --- focusEyesLoop ---
void HuyangFace::focusEyesLoop()
{
    if (_leftEyeTargetState == EyeState::EYE_STATE_FOCUS || _rightEyeTargetState == EyeState::EYE_STATE_FOCUS)
    {
        bool shouldDoLeftEye = (_leftEyeTargetState == EyeState::EYE_STATE_FOCUS && _currentLeftEyeState != EyeState::EYE_STATE_FOCUS);
        bool shouldDoRightEye = (_rightEyeTargetState == EyeState::EYE_STATE_FOCUS && _currentRightEyeState != EyeState::EYE_STATE_FOCUS);

        if (shouldDoLeftEye && shouldDoRightEye)
        {
            focusEyes(_huyangEyeColor);
            _currentLeftEyeState = EyeState::EYE_STATE_FOCUS;
            _currentRightEyeState = EyeState::EYE_STATE_FOCUS;
        }
        else
        {
            if (shouldDoLeftEye)
            {
                focusEye(_leftEye, _huyangEyeColor);
                _currentLeftEyeState = EyeState::EYE_STATE_FOCUS;
            }
            if (shouldDoRightEye)
            {
                focusEye(_rightEye, _huyangEyeColor);
                _currentRightEyeState = EyeState::EYE_STATE_FOCUS;
            }
        }
    }
}

// --- focusEyes (both eyes) ---
void HuyangFace::focusEyes(uint16_t color)
{
    if (!_leftEye || !_rightEye) return;

    Serial.println("HuyangFace: Performing focusEyes animation.");
    // This is a simplified animation; you might want to draw concentric circles or a shrinking pupil.
    // For now, it's a simple fill and then drawing a smaller pupil.
    for (uint16_t step = 0; step <= ((_tftDisplayHeight / 2) / 6 * 4); step++)
    {
        _leftEye->drawFastHLine(0, step, _tftDisplayWidth, color);
        _leftEye->drawFastHLine(0, _tftDisplayHeight - 1 - step, _tftDisplayWidth, color);

        _rightEye->drawFastHLine(0, step, _tftDisplayWidth, color);
        _rightEye->drawFastHLine(0, _tftDisplayHeight - 1 - step, _tftDisplayWidth, color);

        _previousMillis = _currentMillis;
        while (_currentMillis - _previousMillis < _blinkInterval)
        {
            _currentMillis = millis();
        }
    }
    // After animation, ensure they are in the final focused state (e.g., smaller pupil)
    drawFocusEye(_leftEye);
    drawFocusEye(_rightEye);
}

// --- focusEye (single eye) ---
void HuyangFace::focusEye(Arduino_GFX *eye, uint16_t color)
{
    if (!eye) return;

    Serial.printf("HuyangFace: Performing focusEye animation for single eye.\n");
    for (uint16_t step = 0; step <= ((_tftDisplayHeight / 2) / 6 * 4); step++)
    {
        eye->drawFastHLine(0, step, _tftDisplayWidth, color);
        eye->drawFastHLine(0, _tftDisplayHeight - 1 - step, _tftDisplayWidth, color);

        _previousMillis = _currentMillis;
        while (_currentMillis - _previousMillis < _blinkInterval)
        {
            _currentMillis = millis();
        }
    }
    drawFocusEye(eye);
}

// --- sadEyesLoop ---
void HuyangFace::sadEyesLoop()
{
    if (_leftEyeTargetState == EyeState::EYE_STATE_SAD || _rightEyeTargetState == EyeState::EYE_STATE_SAD)
    {
        bool shouldDoLeftEye = (_leftEyeTargetState == EyeState::EYE_STATE_SAD && _currentLeftEyeState != EyeState::EYE_STATE_SAD);
        bool shouldDoRightEye = (_rightEyeTargetState == EyeState::EYE_STATE_SAD && _currentRightEyeState != EyeState::EYE_STATE_SAD);

        if (shouldDoLeftEye && shouldDoRightEye)
        {
            sadEyes(_huyangEyeColor);
            _currentLeftEyeState = EyeState::EYE_STATE_SAD;
            _currentRightEyeState = EyeState::EYE_STATE_SAD;
        }
        else
        {
            if (shouldDoLeftEye)
            {
                sadEye(_leftEye, true, _huyangEyeColor); // Assuming 'true' for inner eyebrow for left eye
                _currentLeftEyeState = EyeState::EYE_STATE_SAD;
            }
            if (shouldDoRightEye)
            {
                sadEye(_rightEye, false, _huyangEyeColor); // Assuming 'false' for outer eyebrow for right eye
                _currentRightEyeState = EyeState::EYE_STATE_SAD;
            }
        }
    }
}

// --- sadEyes (both eyes) ---
void HuyangFace::sadEyes(uint16_t color)
{
    if (!_leftEye || !_rightEye) return;

    Serial.println("HuyangFace: Performing sadEyes animation.");
    uint16_t length = _tftDisplayHeight; // Use actual height
    for (uint16_t step = 0; step <= length / 2; step++)
    {
        // This is a placeholder for actual sad eye animation.
        // You would draw the sad eye shape (e.g., eyebrows).
        // For now, it's a simple fill.
        _leftEye->fillScreen(color);
        _rightEye->fillScreen(color);

        _previousMillis = _currentMillis;
        while (_currentMillis - _previousMillis < _blinkInterval)
        {
            _currentMillis = millis();
        }
    }
    drawSadEye(_leftEye);
    drawSadEye(_rightEye);
}

// --- sadEye (single eye) ---
void HuyangFace::sadEye(Arduino_GFX *eye, bool inner, uint16_t color)
{
    if (!eye) return;

    Serial.printf("HuyangFace: Performing sadEye animation for single eye (inner: %s).\n", inner ? "true" : "false");
    uint16_t length = _tftDisplayHeight; // Use actual height
    for (uint16_t step = 0; step <= length / 2; step++)
    {
        // Placeholder for actual sad eye animation.
        eye->fillScreen(color);

        _previousMillis = _currentMillis;
        while (_currentMillis - _previousMillis < _blinkInterval)
        {
            _currentMillis = millis();
        }
    }
    drawSadEye(eye);
}

// --- angryEyesLoop ---
void HuyangFace::angryEyesLoop()
{
    if (_leftEyeTargetState == EyeState::EYE_STATE_ANGRY || _rightEyeTargetState == EyeState::EYE_STATE_ANGRY)
    {
        bool shouldDoLeftEye = (_leftEyeTargetState == EyeState::EYE_STATE_ANGRY && _currentLeftEyeState != EyeState::EYE_STATE_ANGRY);
        bool shouldDoRightEye = (_rightEyeTargetState == EyeState::EYE_STATE_ANGRY && _currentRightEyeState != EyeState::EYE_STATE_ANGRY);

        if (shouldDoLeftEye && shouldDoRightEye)
        {
            angryEyes(_huyangEyeColor);
            _currentLeftEyeState = EyeState::EYE_STATE_ANGRY;
            _currentRightEyeState = EyeState::EYE_STATE_ANGRY;
        }
        else
        {
            if (shouldDoLeftEye)
            {
                angryEye(_leftEye, false, _huyangEyeColor); // Assuming 'false' for outer eyebrow for left eye
                _currentLeftEyeState = EyeState::EYE_STATE_ANGRY;
            }
            if (shouldDoRightEye)
            {
                angryEye(_rightEye, true, _huyangEyeColor); // Assuming 'true' for inner eyebrow for right eye
                _currentRightEyeState = EyeState::EYE_STATE_ANGRY;
            }
        }
    }
}

// --- angryEyes (both eyes) ---
void HuyangFace::angryEyes(uint16_t color)
{
    if (!_leftEye || !_rightEye) return;

    Serial.println("HuyangFace: Performing angryEyes animation.");
    uint16_t length = _tftDisplayHeight; // Use actual height
    for (uint16_t step = 0; step <= length / 2; step++)
    {
        // This is a placeholder for actual angry eye animation.
        // You would draw the angry eye shape (e.g., slanted eyebrows).
        _leftEye->fillScreen(color);
        _rightEye->fillScreen(color);

        _previousMillis = _currentMillis;
        while (_currentMillis - _previousMillis < _blinkInterval)
        {
            _currentMillis = millis();
        }
    }
    drawAngryEye(_leftEye);
    drawAngryEye(_rightEye);
}

// --- angryEye (single eye) ---
void HuyangFace::angryEye(Arduino_GFX *eye, bool inner, uint16_t color)
{
    if (!eye) return;

    Serial.printf("HuyangFace: Performing angryEye animation for single eye (inner: %s).\n", inner ? "true" : "false");
    uint16_t length = _tftDisplayHeight; // Use actual height
    for (uint16_t step = 0; step <= length / 2; step++)
    {
        // Placeholder for actual angry eye animation.
        eye->fillScreen(color);

        _previousMillis = _currentMillis;
        while (_currentMillis - _previousMillis < _blinkInterval)
        {
            _currentMillis = millis();
        }
    }
    drawAngryEye(eye);
}
