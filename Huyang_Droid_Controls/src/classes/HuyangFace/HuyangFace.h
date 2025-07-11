#ifndef HuyangFace_h
#define HuyangFace_h

#include "Arduino.h"
#include <Arduino_GFX_Library.h> // For TFT displays (eyes)

// Define eye states (enum)
enum EyeState {
    EYE_STATE_NONE = 0,
    EYE_STATE_OPEN = 1,
    EYE_STATE_CLOSED = 2,
    EYE_STATE_BLINK = 3,
    EYE_STATE_FOCUS = 4,
    EYE_STATE_SAD = 5,
    EYE_STATE_ANGRY = 6
};

class HuyangFace
{
public:
    // Constructor: Takes pointers to the left and right eye display instances
    HuyangFace(Arduino_GFX *left, Arduino_GFX *right);

    // Setup function: Initializes eye displays
    void setup();

    // Main loop function: Called repeatedly to update eye animations
    void loop();

    // Flag to enable/disable automatic eye animations (publicly accessible)
    bool automatic = true;

    // Public methods to control individual eyes or both
    void setLeftEyeTo(EyeState state);
    void setRightEyeTo(EyeState state);
    void setEyesTo(EyeState state); // Sets both eyes to the same state
    void setAutomatic(bool state); // Declaration for setting automatic mode

    // Helper to convert uint16_t (from WebServer) to EyeState enum
    EyeState getStateFrom(uint16_t stateValue);

private:
    Arduino_GFX *_leftEye;  // Pointer to the left eye display instance
    Arduino_GFX *_rightEye; // Pointer to the right eye display instance

    unsigned long _currentMillis = 0;  // Current time in milliseconds
    unsigned long _previousMillis = 0; // Previous time for general timing

    // Eye animation state variables
    EyeState _currentLeftEyeState = EYE_STATE_OPEN;  // Current actual state of the left eye
    EyeState _currentRightEyeState = EYE_STATE_OPEN; // Current actual state of the right eye

    // Target states for easing/transitions (what the eye *should* be doing)
    EyeState _leftEyeTargetState = EYE_STATE_OPEN;
    EyeState _rightEyeTargetState = EYE_STATE_OPEN;

    // Last manually selected states (for returning from automatic mode)
    EyeState _leftEyeLastSelectedState = EYE_STATE_OPEN;
    EyeState _rightEyeLastSelectedState = EYE_STATE_OPEN;

    // Timers for blinking and other animations
    unsigned long _lastBlinkMillis = 0;
    uint16_t _blinkInterval = 5000; // Default blink every 5 seconds

    // Variables for random animation timing
    unsigned long _previousRandomMillis = 0;
    unsigned long _randomDuration = 0;

    // Display dimensions (assuming square displays for simplicity, or get from _leftEye->width() etc.)
    // These should ideally be set dynamically in setup() from the actual display objects.
    uint16_t _tftDisplayWidth = 240;  // Example default, adjust if your displays are different
    uint16_t _tftDisplayHeight = 240; // Example default

    // Default eye color (used for open eyes, etc.)
    uint16_t _huyangEyeColor = 0x07E0; // A green color (RGB565 format)

    // Private helper functions for drawing eye states (declared here, defined in HuyangFace.cpp or HuyangFace_moods.cpp)
    void drawEye(Arduino_GFX *eye, EyeState state); // Generic drawing function
    
    // Specific drawing functions (defined in HuyangFace.cpp)
    void drawOpenEye(Arduino_GFX *eye);
    void drawClosedEye(Arduino_GFX *eye);
    void drawFocusEye(Arduino_GFX *eye);
    void drawSadEye(Arduino_GFX *eye);
    void drawAngryEye(Arduino_GFX *eye);
    void drawBlinkAnimation(Arduino_GFX *eye); // Handles the animation steps for blinking

    // Loop functions for specific moods (defined in HuyangFace_moods.cpp)
    void openEyesLoop();
    void closeEyesLoop();
    void focusEyesLoop();
    void sadEyesLoop();
    void angryEyesLoop();

    // Direct drawing functions (defined in HuyangFace_moods.cpp)
    void openEyes(uint16_t color);
    void openEye(Arduino_GFX *eye, uint16_t color);
    void closeEyes(uint16_t color);
    void closeEye(Arduino_GFX *eye, uint16_t color);
    void focusEyes(uint16_t color);
    void focusEye(Arduino_GFX *eye, uint16_t color);
    void sadEyes(uint16_t color);
    void sadEye(Arduino_GFX *eye, bool inner, uint16_t color); // Added 'inner' parameter
    void angryEyes(uint16_t color);
    void angryEye(Arduino_GFX *eye, bool inner, uint16_t color); // Added 'inner' parameter
    
    // Private functions for random eye movements/animations
    void doRandomBlink();
};

#endif
