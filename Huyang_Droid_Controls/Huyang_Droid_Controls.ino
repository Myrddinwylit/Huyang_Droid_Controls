// Huyang_Remote_Control.ino - Main sketch file for the Huyang Droid Remote Control
// This file sets up the global objects and contains the main setup() and loop() functions.

// Include configuration and definitions
#include "config.h"      // Robot configuration (e.g., Wifi credentials, WebServerPort)
#include "calibration.h" // Calibration values (defaults, loaded from LittleFS)
#include "src/includes.h" // Includes common libraries and class headers
#include "src/definitions.h" // Declares global variables and object pointers as 'extern'
#include <Wire.h> // Required for I2C communication (used by PCA9685)
#include <Arduino_GFX_Library.h> // Ensure this is included for Arduino_GFX type

// --- GLOBAL OBJECT DEFINITIONS ---
// These lines *define* the global objects that were *declared* as 'extern' in definitions.h.
// They must be instantiated here so they exist before setup() runs.

// Wi-Fi Manager instance
JxWifiManager *wifi = new JxWifiManager();

// Web Server instance (using the port defined in config.h)
WebServer *webserver = new WebServer(WebServerPort);

// Eye Display (GC9A01 TFT) Instances
// IMPORTANT: You MUST uncomment and adjust these lines based on your actual display hardware and pins.
// These are placeholders. Replace `nullptr` with actual new `Arduino_GFX` objects.
// Example for a GC9A01 display with hardware SPI on ESP8266 (adjust pins as per your config.h)
// Ensure LEFT_EYE_DC_PIN, LEFT_EYE_CS_PIN, LEFT_EYE_RST_PIN are defined in config.h
// Arduino_DataBus *leftBus = new Arduino_ESP8266SPI(LEFT_EYE_DC_PIN, LEFT_EYE_CS_PIN, -1, -1, -1); // SCK, MOSI, MOSI are often default for HW SPI
// Arduino_GFX *leftEye = new Arduino_GC9A01(leftBus, LEFT_EYE_RST_PIN, -1, true); // -1 for no backlight pin, true for rotation
// Arduino_DataBus *rightBus = new Arduino_ESP8266SPI(RIGHT_EYE_DC_PIN, RIGHT_EYE_CS_PIN, -1, -1, -1);
// Arduino_GFX *rightEye = new Arduino_GC9A01(rightBus, RIGHT_EYE_RST_PIN, -1, true);

// For now, keeping them as nullptr to allow compilation, but functionality will be missing.
Arduino_GFX *leftEye = nullptr;
Arduino_GFX *rightEye = nullptr;

// PWM Servo Driver (PCA9685) instance
Adafruit_PWMServoDriver *pwm = new Adafruit_PWMServoDriver(0x40); // Default I2C address for PCA9685

// Huyang Robot Subsystem Instances
// HuyangFace only expects two Arduino_GFX pointers.
HuyangFace *huyangFace = new HuyangFace(leftEye, rightEye);
HuyangBody *huyangBody = new HuyangBody(pwm);
HuyangNeck *huyangNeck = new HuyangNeck(pwm);
HuyangAudio *huyangAudio = new HuyangAudio(); // Assuming HuyangAudio exists and is extern

// --- GLOBAL FEATURE ENABLE FLAGS (DEFINED HERE) ---
// These flags control which robot features are enabled.
// Set to 'true' to enable, 'false' to disable.
bool enableEyes = true;            // the two TFT Displays as eyes
bool enableMonacle = true;         // the servo inside of the head shell to move the monocle
bool enableNeckMovement = true;    // the two servos inside of the head
bool enableHeadRotation = true;    // the rotation servo on the bottom of the neck
bool enableBodyMovement = true;    // the four 60kg servos from the c-3po build plan
bool enableBodyRotation = true;    // the 80kg servo inside of the hip
bool enableTorsoLights = true;     // the ws2812b led inside of the torso


// Global currentMillis variable (from system.h, now defined here)
unsigned long currentMillis = 0;


// --- MAIN ARDUINO SETUP FUNCTION ---
void setup()
{
    Serial.begin(115200); // Start serial communication for debugging
    Serial.println("Huyang! v1.9 by Jeanette Müller");

    // Initialize I2C communication (crucial for PCA9685)
    Wire.begin();

    // Wi-Fi setup based on configuration from config.h
    wifi->currentMode = WiFiDefaultMode;
    wifi->host = IPAddress(192, 168, 10, 1); // Default AP IP
    wifi->subnetMask = IPAddress(255, 255, 255, 0);
    wifi->hotspot_Ssid = WifiSsidOfHotspot;
    wifi->hotspot_Password = WifiPasswordHotspot;
    wifi->network_Ssid = WifiSsidConnectTo;
    wifi->network_Password = WifiPasswordConnectTo;
    wifi->setup(); // Initialize Wi-Fi connection

    // Web server setup: pass feature enable flags from config.h
    webserver->setup(enableEyes,
                     enableMonacle,
                     enableNeckMovement,
                     enableHeadRotation,
                     enableBodyMovement,
                     enableBodyRotation,
                     enableTorsoLights);
    webserver->start(); // Start the web server (routes are configured in setup)

    // Initialize robot subsystems
    // These objects are created in Huyang_Remote_Control.ino
    if (huyangFace) huyangFace->setup();
    if (huyangBody) huyangBody->setup();
    if (huyangNeck) huyangNeck->setup();
    if (huyangAudio) huyangAudio->setup(); // Setup audio if enabled
}

// --- MAIN ARDUINO LOOP FUNCTION ---
void loop()
{
    // Update global currentMillis for all time-based operations
    currentMillis = millis();

    // --- Wi-Fi Manager Loop ---
    wifi->loop(); // Keep Wi-Fi connection alive and print IP address

    // --- Control Face (Eyes) ---
    // The HuyangFace class handles its own loop and state transitions based on faceLeftEyeState/faceRightEyeState
    // and automaticAnimations.
    if (enableEyes)
    {
        // If automatic animations are off, manually set eye states from global variables
        if (automaticAnimations == false)
        {
            if (allEyes != 0) { // If an "all eyes" command is active
                huyangFace->setEyesTo(huyangFace->getStateFrom(allEyes));
                allEyes = 0; // Reset after setting to prevent continuous re-setting
            } else { // Otherwise, apply individual eye states
                huyangFace->setLeftEyeTo(huyangFace->getStateFrom(faceLeftEyeState));
                huyangFace->setRightEyeTo(huyangFace->getStateFrom(faceRightEyeState));
            }
        }
        huyangFace->loop(); // Run the eye animation loop
    }

    // --- Control Monocle ---
    if (enableMonacle && automaticAnimations == false)
    {
        // The monocle position is directly set via monoclePosition global variable
        // and handled by HuyangNeck (assuming setMonoclePosition exists and is public)
        huyangNeck->setMonoclePosition(monoclePosition + calMonoclePosition); // Apply calibration
    }

    // --- Control Neck ---
    // The HuyangNeck class handles its own loop and state transitions.
    // If automatic animations are off, manual control values are passed.
    huyangNeck->automatic = automaticAnimations; // Pass automatic flag to neck
    if (automaticAnimations == false) // If manual control
    {
        // Apply calibration to the manual control values
        double calibratedNeckRotate = neckRotate + calNeckRotation;
        double calibratedNeckTiltForward = neckTiltForward + calNeckTiltForward;
        double calibratedNeckTiltSideways = neckTiltSideways + calNeckTiltSideways;

        huyangNeck->rotateHead(calibratedNeckRotate);
        huyangNeck->tiltNeckForward(calibratedNeckTiltForward);
        huyangNeck->tiltNeckSideways(calibratedNeckTiltSideways);
    }
    huyangNeck->loop(); // Run the neck control loop

    // --- Control Body ---
    // The HuyangBody class handles its own loop and state transitions.
    // If automatic animations are off, manual control values are passed.
    huyangBody->automatic = automaticAnimations;

    if (automaticAnimations == false) // If manual control
    {
        // Apply calibration to the manual control values
        int16_t calibratedBodyRotate = bodyRotate + calBodyRotation;
        int16_t calibratedBodyTiltForward = bodyTiltForward + calBodyTiltForward;
        int16_t calibratedBodyTiltSideways = bodyTiltSideways + calBodyTiltSideways;

        huyangBody->rotateBody(calibratedBodyRotate);
        huyangBody->tiltBodyForward(calibratedBodyTiltForward);
        huyangBody->tiltBodySideways(calibratedBodyTiltSideways);
    }
    // Update chest light mode based on global variable
    huyangBody->currentLightMode = (LightMode)chestLightMode;

    huyangBody->loop(); // Run the body control loop

    // huyangAudio->loop(); // Audio loop (currently commented out in original, uncomment if needed)
}
