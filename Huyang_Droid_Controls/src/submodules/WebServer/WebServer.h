#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "FS.h" // For File System
#include "LittleFS.h" // For LittleFS

// Forward declarations of classes used by WebServer
// These are needed so WebServer.h knows about these types before their full definitions.
class HuyangFace;
class HuyangBody;
class HuyangNeck;
class HuyangAudio;
class JxWifiManager;

// Define light modes (updated with new modes)
enum LightMode {
    LIGHT_OFF = 0,
    LIGHT_STATIC_BLUE = 1,          // Both LEDs static blue (initial mode)
    LIGHT_WARNING_BLINK = 2,        // Both blink alternatively Red/Blue
    LIGHT_PROCESSING_FADE = 3,      // Two LEDs fade randomly on/off, slow to fast fade, then slow blinks
    LIGHT_DROID_MODE_1 = 4,         // Star Wars Droid indicator lights - Mode 1
    LIGHT_DROID_MODE_2 = 5          // Star Wars Droid indicator lights - Mode 2
};

// --- GLOBAL VARIABLES DECLARATIONS (Accessible throughout your project) ---
// These variables hold the current state of the robot.
// They are updated by the WebServer and read by the HuyangRobot class (or similar).
// These declarations must match those in definitions.h exactly.

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

extern int16_t calNeckRotation;
extern int16_t calNeckTiltForward;
extern int16_t calNeckTiltSideways;
extern int16_t calBodyRotation;
extern int16_t calBodyTiltForward;
extern int16_t calBodyTiltSideways;
extern int16_t calMonoclePosition; // Consistent with definitions.h

extern String robotName;
extern int16_t masterMovementSpeed; // Consistent with definitions.h
extern String firmwareVersion;

extern LightMode chestLightMode; // Consistent with definitions.h

// --- WebServer Class Declaration ---
class WebServer
{
public:
    // Declare the default constructor
    WebServer(); 
    // Existing constructor with port
    WebServer(uint32_t port);

    // Enum for different HTML pages to serve
    enum class Page // Changed to enum class for stronger typing
    {
        indexPage,
        settingsPage,
        calibrationPage,
        lightsPage
    };

    void setup(bool enableEyes,
               bool enableMonacle,
               bool enableNeckMovement,
               bool enableHeadRotation,
               bool enableBodyMovement,
               bool enableBodyRotation,
               bool enableTorsoLights);
    void start();

private:
    AsyncWebServer *_server;

    // Feature enable flags (from config.h)
    bool _enableEyes;
    bool _enableMonacle;
    bool _enableNeckMovement;
    bool _enableHeadRotation;
    bool _enableBodyMovement;
    bool _enableBodyRotation;
    bool _enableTorsoLights;

    // Helper functions for file operations
    String readFile(const char *path);
    void writeFile(const char *path, const char *message);

    // Calibration management functions
    void loadCalibration();
    void saveCalibration();
    void resetCalibrationToDefaults();

    // API action handlers
    void apiPostAction(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    void apiCalibratePostAction(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    void apiLightsPostAction(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    void apiSettingsPostAction(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    void apiSystemPostAction(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    void apiGetCalibration(AsyncWebServerRequest *request);

    // HTML page serving function
    String getPage(Page page, AsyncWebServerRequest *request);
    void notFound(AsyncWebServerRequest *request);
};

#endif
