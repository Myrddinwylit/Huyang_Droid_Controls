#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// This file contains global variable and object *declarations* for the Huyang Droid's subsystems.
// Their *definitions* (memory allocations) are handled in Huyang_Remote_Control.ino or WebServer.cpp.

#include <Arduino.h>
#include <Arduino_GFX_Library.h>      // For TFT displays (eyes)
#include <Adafruit_PWMServoDriver.h>  // For servo motor control

// Corrected include paths based on your provided file structure
#include "submodules/JxWifiManager/JxWifiManager.h" // For Wi-Fi management
#include "classes/HuyangFace/HuyangFace.h"        // For controlling the robot's face/eyes
#include "classes/HuyangBody/HuyangBody.h"        // For controlling the robot's body movements and lights
#include "classes/HuyangNeck/HuyangNeck.h"        // For controlling the robot's neck movements
#include "classes/HuyangAudio/HuyangAudio.h"      // For audio playback
#include "submodules/WebServer/WebServer.h"       // Corrected path for the web interface (from src/submodules/WebServer/)
#include "classes/EasingServo/EasingServo.h"      // For easing servo (from src/classes/EasingServo/)


// Global variables for time tracking (extern declarations)
extern unsigned long currentMillis;
extern unsigned long previousMillisIPAdress; // Assuming this is used elsewhere

// Wi-Fi Manager instance (extern declaration)
extern JxWifiManager *wifi;

// --- Eye Display (GC9A01 TFT) Instances (extern declarations) ---
// Corrected to Arduino_GFX* as expected by HuyangFace and defined in .ino
extern Arduino_GFX *leftEye;
extern Arduino_GFX *rightEye;
// Arduino_DataBus objects are internal to how Arduino_GFX is initialized,
// not typically exposed as global externs for the eye objects themselves.
// If you had separate Arduino_DataBus objects that needed to be global, they'd be declared here.


// PWM Servo Driver (PCA9685) instance (extern declaration)
extern Adafruit_PWMServoDriver *pwm;

// Huyang Robot Subsystem Instances (extern declarations)
extern HuyangFace *huyangFace;
extern HuyangBody *huyangBody;
extern HuyangNeck *huyangNeck;
extern HuyangAudio *huyangAudio; // Uncomment if you are using audio features

// WebServer instance (extern declaration)
extern WebServer *webserver;

// --- Global variables for robot state (extern declarations) ---
// These are the same variables defined and updated in WebServer.cpp and used in system.h
extern bool automaticAnimations;
extern uint16_t allEyes;
extern uint16_t faceLeftEyeState;
extern uint16_t faceRightEyeState;

extern double neckRotate;
extern double neckTiltForward;
extern double neckTiltSideways;

extern int16_t bodyRotate;
extern int16_t bodyTiltForward;
extern int16_t bodyTiltSideways;

extern int16_t monoclePosition;

// Calibration values
extern int16_t calNeckRotation;
extern int16_t calNeckTiltForward;
extern int16_t calNeckTiltSideways;
extern int16_t calBodyRotation;
extern int16_t calBodyTiltForward;
extern int16_t calBodyTiltSideways;
extern int16_t calMonoclePosition; // Corrected to calMonoclePosition

// Settings variables
extern String robotName;
extern int16_t masterMovementSpeed; // Consistent type with WebServer.h
extern String firmwareVersion;

// Chest light mode (matches HuyangBody::LightMode enum)
extern LightMode chestLightMode; // Use the enum type directly

#endif
