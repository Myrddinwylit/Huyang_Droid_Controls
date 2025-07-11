#include "WebServer.h" // Include the corresponding header file for WebServer class declaration
#include "../../config.h"    // For WebServerPort and other config defines (relative path)
#include <ESPAsyncWebServer.h> // For AsyncWebServerRequest
#include <ArduinoJson.h> // For JSON parsing
#include "FS.h" // For File System
#include "LittleFS.h" // For LittleFS
#include <Arduino.h> // For uint8_t, String, and other Arduino types
#include <cstddef>   // For size_t
#include <functional> // For std::bind and std::placeholders (still included for other potential uses, though not for these specific handlers now)

// Include full class definitions for global pointers using CORRECT RELATIVE PATHS
// These are crucial for accessing members of HuyangFace, HuyangBody, HuyangNeck
#include "../../classes/HuyangFace/HuyangFace.h"   // Corrected relative path: go up two levels, then into classes/HuyangFace
#include "../../classes/HuyangBody/HuyangBody.h"   // Corrected relative path
#include "../../classes/HuyangNeck/HuyangNeck.h"   // Corrected relative path
#include "../../classes/HuyangAudio/HuyangAudio.h" // Corrected relative path (uncomment if used)

// Define the file path for calibration data on LittleFS
#define CALIBRATION_FILE "/calibrations.json" 

// --- GLOBAL VARIABLES DEFINITIONS (MATCHING EXTERN DECLARATIONS IN definitions.h) ---
// These are the actual definitions of the global state variables.
// They are used by WebServer to update robot state and by other classes (e.g., HuyangFace, HuyangBody)
// to control the robot's hardware.

// Control flags
bool automaticAnimations = true; // Default to automatic
uint16_t allEyes = 0; // No specific "all eyes" command active by default
uint16_t faceLeftEyeState = 3;  // Default to blink (state 3)
uint16_t faceRightEyeState = 3; // Default to blink (state 3)

// Neck movement values (now hold -90 to +90 degrees)
double neckRotate = 0;
double neckTiltForward = 0;
double neckTiltSideways = 0;

// Body movement values (now hold -90 to +90 degrees)
int16_t bodyRotate = 0;
int16_t bodyTiltForward = 0;
int16_t bodyTiltSideways = 0;

int16_t monoclePosition = 0; // Monocle servo position (still 0-180 range, or mapped based on physical needs)

// Calibration values (defaults, loaded from file if present)
int16_t calNeckRotation = 0;
int16_t calNeckTiltForward = 0;
int16_t calNeckTiltSideways = 0;
int16_t calBodyRotation = 0;
int16_t calBodyTiltForward = 0;
int16_t calBodyTiltSideways = 0;
int16_t calMonoclePosition = 0; // Corrected definition to calMonoclePosition

// Chest light mode (default to LIGHT_STATIC_BLUE) - Defined here as it's a global state
LightMode chestLightMode = LIGHT_STATIC_BLUE; // Changed type to LightMode and initialized with enum value

// Settings values (defaults, loaded from file if present)
String robotName = "Huyang Robot";
int16_t masterMovementSpeed = 100; // Type now matches header (int16_t)
String firmwareVersion = "1.9"; // Firmware version string

// Pointers to robot subsystem instances (defined in Huyang_Remote_Control.ino)
// These are extern declarations, actual objects are created in the .ino file
extern HuyangFace *huyangFace;
extern HuyangBody *huyangBody;
extern HuyangNeck *huyangNeck;
extern HuyangAudio *huyangAudio; // Assuming HuyangAudio exists and is extern

// --- WebServer Class Implementation ---

// Constructor (Default) - Delegates to the constructor with port
WebServer::WebServer() : WebServer(80)
{
    // No additional logic needed here as it delegates
}

// Constructor (with port) - This is the one that should handle the actual server creation
WebServer::WebServer(uint32_t port)
{
    _server = new AsyncWebServer(port);
}


// Setup function: configures server routes and initializes file system
void WebServer::setup(bool enableEyes,
                      bool enableMonacle,
                      bool enableNeckMovement,
                      bool enableHeadRotation,
                      bool enableBodyMovement,
                      bool enableBodyRotation,
                      bool enableTorsoLights)
{
    // Store feature flags
    _enableEyes = enableEyes;
    _enableMonacle = enableMonacle;
    _enableNeckMovement = enableNeckMovement;
    _enableHeadRotation = enableHeadRotation;
    _enableBodyMovement = enableBodyMovement;
    _enableBodyRotation = enableBodyRotation;
    _enableTorsoLights = enableTorsoLights;

    Serial.println("WebServer setup started.");

    // Initialize LittleFS
    if (!LittleFS.begin())
    {
        Serial.println("LittleFS failed to mount. Formatting...");
        LittleFS.format(); // Attempt to format if mount fails
        if (!LittleFS.begin())
        {
            Serial.println("LittleFS still failed after format. Cannot proceed.");
            return;
        }
    }
    Serial.println("LittleFS mounted successfully.");

    // Load calibration and settings data on startup
    loadCalibration();
    // loadSettings(); // Settings are now loaded as part of loadCalibration

    // --- Serve static files ---
    // This line serves all files from the root of LittleFS.
    // Ensure your HTML, CSS, JS files are uploaded to the LittleFS root.
    _server->serveStatic("/", LittleFS, "/"); //.setDefaultAuthentication("user", "password"); // Optional authentication
    Serial.println("Static file server configured.");

    // --- API Endpoints ---

    // GET /api/calibration - Returns current robot state and calibration data
    _server->on("/api/calibration", HTTP_GET, [&](AsyncWebServerRequest *request) {
        this->apiGetCalibration(request); // Use 'this->' for clarity, but not strictly needed inside lambda
    });
    Serial.println("GET /api/calibration route configured.");

    // POST /api/action - For general robot control commands (eyes, neck, body, monocle, automatic)
    _server->on("/api/action", HTTP_POST, [&](AsyncWebServerRequest *request){}, NULL,
                [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        this->apiPostAction(request, data, len, index, total);
    });
    Serial.println("POST /api/action route configured.");

    // POST /api/calibrate - For calibration specific actions (update, save, reset)
    _server->on("/api/calibrate", HTTP_POST, [&](AsyncWebServerRequest *request){}, NULL,
                [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        this->apiCalibratePostAction(request, data, len, index, total);
    });
    Serial.println("POST /api/calibrate route configured.");

    // POST /api/lights - For chest light control
    _server->on("/api/lights", HTTP_POST, [&](AsyncWebServerRequest *request){}, NULL,
                [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        this->apiLightsPostAction(request, data, len, index, total);
    });
    Serial.println("POST /api/lights route configured.");

    // POST /api/settings - For general robot settings (name, speed)
    _server->on("/api/settings", HTTP_POST, [&](AsyncWebServerRequest *request){}, NULL,
                [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        this->apiSettingsPostAction(request, data, len, index, total);
    });
    Serial.println("POST /api/settings route configured.");

    // POST /api/system - For system commands (reboot, factory reset)
    _server->on("/api/system", HTTP_POST, [&](AsyncWebServerRequest *request){}, NULL,
                [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        this->apiSystemPostAction(request, data, len, index, total);
    });
    Serial.println("POST /api/system route configured.");

    // Not Found Handler
    _server->onNotFound([&](AsyncWebServerRequest *request) {
        this->notFound(request);
    });
    Serial.println("Not Found handler configured.");

    _server->begin();
    Serial.println("Web server started.");
}

void WebServer::start()
{
    // The server is started in setup(), this function might be redundant
    // or used for a different purpose in the original design.
    // For now, it doesn't need explicit code here.
}

// Helper function to read file from LittleFS
String WebServer::readFile(const char *path)
{
    Serial.printf("Reading file: %s\n", path);
    if (!LittleFS.exists(path))
    {
        Serial.printf("File not found: %s\n", path);
        return String(); // Return empty string if file doesn't exist
    }
    File file = LittleFS.open(path, "r");
    if (!file)
    {
        Serial.printf("Failed to open file for reading: %s\n", path);
        return String();
    }
    String content = file.readString();
    file.close();
    Serial.printf("Read %d bytes from %s\n", content.length(), path);
    return content;
}

// Helper function to write file to LittleFS
void WebServer::writeFile(const char *path, const char *message)
{
    Serial.printf("Writing file: %s\n", path);
    File file = LittleFS.open(path, "w");
    if (!file)
    {
        Serial.printf("Failed to open file for writing: %s\n", path);
        return;
    }
    if (file.print(message))
    {
        Serial.println("File written successfully.");
    }
    else
    {
        Serial.println("File write failed.");
    }
    file.close();
}

// Load calibration data from file
void WebServer::loadCalibration()
{
    Serial.println("Loading calibration data...");
    String jsonString = readFile(CALIBRATION_FILE);
    if (jsonString.length() > 0)
    {
        DynamicJsonDocument doc(512); // Adjust size as needed
        DeserializationError error = deserializeJson(doc, jsonString);
        if (error)
        {
            Serial.print(F("deserializeJson for calibration failed: "));
            Serial.println(error.f_str());
            // Do not return here, proceed to reset defaults if parsing fails
        } else {
            calNeckRotation = doc["neck"]["rotation"] | 0;
            calNeckTiltForward = doc["neck"]["tiltForward"] | 0;
            calNeckTiltSideways = doc["neck"]["tiltSideways"] | 0;
            calBodyRotation = doc["body"]["rotation"] | 0;
            calBodyTiltForward = doc["body"]["tiltForward"] | 0;
            calBodyTiltSideways = doc["body"]["tiltSideways"] | 0;
            calMonoclePosition = doc["monocle"]["position"] | 0; // Load monocle calibration

            // Load settings from calibration file as well (if they are stored there)
            robotName = doc["settings"]["robotName"] | "Huyang Robot";
            masterMovementSpeed = doc["settings"]["masterMovementSpeed"] | 100;
            Serial.println("Calibration and settings data loaded.");
            return; // Return if successful
        }
    }
    Serial.println("No valid calibration file found or empty. Using default values and resetting.");
    resetCalibrationToDefaults(); // Ensure defaults are applied and saved if file is missing/empty/corrupt
}

// Save calibration data to file
void WebServer::saveCalibration()
{
    Serial.println("Saving calibration data...");
    DynamicJsonDocument doc(512); // Adjust size as needed

    doc["neck"]["rotation"] = calNeckRotation;
    doc["neck"]["tiltForward"] = calNeckTiltForward;
    doc["neck"]["tiltSideways"] = calNeckTiltSideways;
    doc["body"]["rotation"] = calBodyRotation;
    doc["body"]["tiltForward"] = calBodyTiltForward;
    doc["body"]["tiltSideways"] = calBodyTiltSideways;
    doc["monocle"]["position"] = calMonoclePosition; // Save monocle calibration

    // Save settings along with calibration
    doc["settings"]["robotName"] = robotName;
    doc["settings"]["masterMovementSpeed"] = masterMovementSpeed;

    String jsonString;
    serializeJson(doc, jsonString);
    writeFile(CALIBRATION_FILE, jsonString.c_str());
    Serial.println("Calibration and settings data saved.");
}

// Reset calibration to defaults (0) and save
void WebServer::resetCalibrationToDefaults()
{
    Serial.println("Resetting calibration to defaults...");
    calNeckRotation = 0;
    calNeckTiltForward = 0;
    calNeckTiltSideways = 0;
    calBodyRotation = 0;
    calBodyTiltForward = 0;
    calBodyTiltSideways = 0;
    calMonoclePosition = 0; // Reset monocle calibration

    // Reset settings to defaults
    robotName = "Huyang Robot";
    masterMovementSpeed = 100;

    saveCalibration(); // Save the reset values
    Serial.println("Calibration reset to defaults and saved.");
}

// --- API Action Handlers ---

// Handles POST requests to /api/action for robot control
void WebServer::apiPostAction(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    Serial.println("apiPostAction received.");
    if (len == 0) {
        Serial.println("apiPostAction: Empty request body.");
        request->send(400, "text/plain", "Bad Request: Empty body.");
        return;
    }

    DynamicJsonDocument doc(1024); // Adjust size as needed
    DeserializationError error = deserializeJson(doc, data, len);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid JSON\"}");
        return;
    }

    String type = doc["type"];
    Serial.printf("apiPostAction: Type received: %s\n", type.c_str());

    // --- Handle EYE commands ---
    if (type == "eye" && _enableEyes)
    {
        String target = doc["target"];
        uint16_t state = doc["state"];
        Serial.printf("apiPostAction: Eye command - Target: %s, State: %d\n", target.c_str(), state);

        if (target == "all")
        {
            // Ensure huyangFace is initialized before calling its methods
            if (huyangFace) huyangFace->setEyesTo(huyangFace->getStateFrom(state));
        }
        else if (target == "left")
        {
            if (huyangFace) huyangFace->setLeftEyeTo(huyangFace->getStateFrom(state));
        }
        else if (target == "right")
        {
            if (huyangFace) huyangFace->setRightEyeTo(huyangFace->getStateFrom(state));
        }
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Eye command received\"}");
    }
    // --- Handle NECK commands ---
    else if (type == "neck" && (_enableNeckMovement || _enableHeadRotation))
    {
        // Assume UI sends values from -100 to 100, map to -90 to 90 degrees
        double inputRotate = doc["rotate"] | 0.0;
        double inputTiltForward = doc["tiltForward"] | 0.0;
        double inputTiltSideways = doc["tiltSideways"] | 0.0;

        // Map incoming values (e.g., -100 to 100) to actual -90 to 90 degrees
        neckRotate = map((long)inputRotate, -100, 100, -90, 90);
        neckTiltForward = map((long)inputTiltForward, -100, 100, -90, 90);
        neckTiltSideways = map((long)inputTiltSideways, -100, 100, -90, 90);

        Serial.printf("apiPostAction: Neck command - Raw Input R:%.2f, TF:%.2f, TS:%.2f\n", inputRotate, inputTiltForward, inputTiltSideways);
        Serial.printf("apiPostAction: Neck command - Mapped Degrees R:%.2f, TF:%.2f, TS:%.2f\n", neckRotate, neckTiltForward, neckTiltSideways);

        // Call HuyangNeck methods (assuming they are public and handle the movement)
        if (huyangNeck) {
            huyangNeck->rotateHead(neckRotate);
            huyangNeck->tiltNeckForward(neckTiltForward);
            huyangNeck->tiltNeckSideways(neckTiltSideways);
        } else {
            Serial.println("HuyangNeck instance is null, cannot set neck movements.");
        }

        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Neck command received\"}");
    }
    // --- Handle BODY commands ---
    else if (type == "body" && (_enableBodyMovement || _enableBodyRotation))
    {
        // Assume UI sends values from -100 to 100, map to -90 to 90 degrees
        int16_t inputRotate = doc["rotate"] | 0;
        int16_t inputTiltForward = doc["tiltForward"] | 0;
        int16_t inputTiltSideways = doc["tiltSideways"] | 0;

        // Map incoming values (e.g., -100 to 100) to actual -90 to 90 degrees
        bodyRotate = map(inputRotate, -100, 100, -90, 90);
        bodyTiltForward = map(inputTiltForward, -100, 100, -90, 90);
        bodyTiltSideways = map(inputTiltSideways, -100, 100, -90, 90);

        Serial.printf("apiPostAction: Body command - Raw Input R:%d, TF:%d, TS:%d\n", inputRotate, inputTiltForward, inputTiltSideways);
        Serial.printf("apiPostAction: Body command - Mapped Degrees R:%d, TF:%d, TS:%d\n", bodyRotate, bodyTiltForward, bodyTiltSideways);

        // Call HuyangBody methods (assuming they are public and handle the movement)
        if (huyangBody) {
            huyangBody->rotateBody(bodyRotate);
            huyangBody->tiltBodyForward(bodyTiltForward);
            huyangBody->tiltBodySideways(bodyTiltSideways);
        } else {
            Serial.println("HuyangBody instance is null, cannot set body movements.");
        }

        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Body command received\"}");
    }
    // --- Handle MONOCLE commands ---
    else if (type == "monocle" && _enableMonacle)
    {
        // Monocle assumed to receive 0-180 directly or a specific mapped range
        int16_t position = doc["position"] | monoclePosition;
        monoclePosition = position; // No mapping applied here, assume UI sends correct range for monocle

        if (huyangNeck) {
            Serial.printf("HuyangNeck: Calling setMonoclePosition(%d)\n", monoclePosition);
            huyangNeck->setMonoclePosition(monoclePosition); // Call the specific monocle method
        } else {
            Serial.println("HuyangNeck instance is null, cannot set monocle position.");
        }
        Serial.printf("apiPostAction: Monocle command - Position: %d\n", monoclePosition);
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Monocle command received\"}");
    }
    // --- Handle AUTOMATIC command ---
    else if (type == "automatic")
    {
        bool state = doc["state"];
        automaticAnimations = state;
        if (huyangFace) huyangFace->setAutomatic(automaticAnimations); // Update the HuyangFace instance
        if (huyangBody) huyangBody->automatic = automaticAnimations; // Update HuyangBody instance
        if (huyangNeck) huyangNeck->automatic = automaticAnimations; // Update HuyangNeck instance
        Serial.printf("apiPostAction: Automatic mode set to: %s\n", automaticAnimations ? "true" : "false");
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Automatic mode updated\"}");
    }
    else
    {
        Serial.printf("apiPostAction: Unknown or disabled command type: %s\n", type.c_str());
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Unknown or disabled command type\"}");
    }
}

// Handles POST requests to /api/calibrate for calibration actions
void WebServer::apiCalibratePostAction(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    Serial.println("apiCalibratePostAction received.");
    if (len == 0) {
        Serial.println("apiCalibratePostAction: Empty request body.");
        request->send(400, "text/plain", "Bad Request: Empty body.");
        return;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, data, len);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid JSON\"}");
        return;
    }

    String action = doc["action"];
    Serial.printf("apiCalibratePostAction: Action received: %s\n", action.c_str());

    if (action == "update")
    {
        String type = doc["type"];
        if (type == "neck")
        {
            if (doc.containsKey("rotation")) calNeckRotation = doc["rotation"];
            if (doc.containsKey("tiltForward")) calNeckTiltForward = doc["tiltForward"];
            if (doc.containsKey("tiltSideways")) calNeckTiltSideways = doc["tiltSideways"];
            Serial.printf("Calibration Update: Neck - Rot:%d, TF:%d, TS:%d\n", calNeckRotation, calNeckTiltForward, calNeckTiltSideways);
        }
        else if (type == "body")
        {
            if (doc.containsKey("rotation")) calBodyRotation = doc["rotation"];
            if (doc.containsKey("tiltForward")) calBodyTiltForward = doc["tiltForward"];
            if (doc.containsKey("tiltSideways")) calBodyTiltSideways = doc["tiltSideways"];
            Serial.printf("Calibration Update: Body - Rot:%d, TF:%d, TS:%d\n", calBodyRotation, calBodyTiltForward, calBodyTiltSideways);
        }
        else if (type == "monocle")
        {
            if (doc.containsKey("position")) calMonoclePosition = doc["position"]; // Use calMonoclePosition
            Serial.printf("Calibration Update: Monocle - Pos:%d\n", calMonoclePosition);
        }
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Calibration update received\"}");
    }
    else if (action == "save")
    {
        saveCalibration();
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Calibration saved\"}");
    }
    else if (action == "reset")
    {
        resetCalibrationToDefaults();
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Calibration reset\"}");
    }
    else if (action == "set_middle_and_lock")
    {
        // This action would typically involve setting all servos to a middle position
        // and then disabling PWM output to "lock" them for horn attachment.
        // This logic needs to be implemented within HuyangNeck and HuyangBody classes.
        // For now, we'll just acknowledge the command.
        Serial.println("Command: Set middle and lock servos (implementation needed in HuyangNeck/Body).");
        // Example: if (huyangNeck) huyangNeck->setAllServosToMiddleAndLock();
        // Example: if (huyangBody) huyangBody->setAllServosToMiddleAndLock();
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Set middle and lock command received\"}");
    }
    else if (action == "unlock_servos")
    {
        // This action would typically re-enable PWM output to servos.
        Serial.println("Command: Unlock servos (implementation needed in HuyangNeck/Body).");
        // Example: if (huyangNeck) huyangNeck->unlockServos();
        // Example: if (huyangBody) huyangBody->unlockServos();
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Unlock servos command received\"}");
    }
    else
    {
        Serial.printf("apiCalibratePostAction: Unknown action: %s\n", action.c_str());
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Unknown calibration action\"}");
    }
}

// Handles POST requests to /api/lights for chest light control
void WebServer::apiLightsPostAction(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    Serial.println("apiLightsPostAction received.");
    if (len == 0) {
        Serial.println("apiLightsPostAction: Empty request body.");
        request->send(400, "text/plain", "Bad Request: Empty body.");
        return;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, data, len);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid JSON\"}");
        return;
    }

    uint16_t mode = doc["mode"];
    Serial.printf("apiLightsPostAction: Light mode received: %d\n", mode);

    if (_enableTorsoLights) {
        if (huyangBody) { // Null check added
            huyangBody->currentLightMode = (LightMode)mode; // Corrected: Cast to global LightMode enum
            Serial.printf("Chest light mode set to: %d\n", huyangBody->currentLightMode);
        } else {
            Serial.println("HuyangBody instance is null, cannot set light mode.");
        }
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Chest light mode updated\"}");
    } else {
        Serial.println("Chest lights are disabled in config.");
        request->send(403, "application/json", "{\"status\":\"error\", \"message\":\"Chest lights disabled\"}");
    }
}

// Handles POST requests to /api/settings for robot settings
void WebServer::apiSettingsPostAction(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    Serial.println("apiSettingsPostAction received.");
    if (len == 0) {
        Serial.println("apiSettingsPostAction: Empty request body.");
        request->send(400, "text/plain", "Bad Request: Empty body.");
        return;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, data, len);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid JSON\"}");
        return;
    }

    if (doc.containsKey("robotName")) {
        robotName = doc["robotName"].as<String>();
        Serial.printf("Settings Update: Robot Name set to: %s\n", robotName.c_str());
    }
    if (doc.containsKey("masterMovementSpeed")) {
        masterMovementSpeed = doc["masterMovementSpeed"];
        Serial.printf("Settings Update: Master Movement Speed set to: %d\n", masterMovementSpeed);
    }

    saveCalibration(); // Save settings (assuming they are stored in the same calibration file)
    request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Settings updated\"}");
}

// Handles POST requests to /api/system for system commands
void WebServer::apiSystemPostAction(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    Serial.println("apiSystemPostAction received.");
    if (len == 0) {
        Serial.println("apiSystemPostAction: Empty request body.");
        request->send(400, "text/plain", "Bad Request: Empty body.");
        return;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, data, len);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid JSON\"}");
        return;;
    }

    String command = doc["command"];
    Serial.printf("apiSystemPostAction: Command received: %s\n", command.c_str());

    if (command == "reboot") {
        Serial.println("System command: Rebooting ESP.");
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Rebooting...\"}");
        delay(100); // Give time for response to send
        ESP.restart();
    } else if (command == "factory_reset") {
        Serial.println("System command: Performing factory reset.");
        resetCalibrationToDefaults(); // This also saves
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Factory reset and rebooting...\"}");
        delay(100); // Give time for response to send
        ESP.restart();
    } else {
        Serial.printf("apiSystemPostAction: Unknown system command: %s\n", command.c_str());
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Unknown system command\"}");
    }
}

// NEW: Implementation for apiGetCalibration
void WebServer::apiGetCalibration(AsyncWebServerRequest *request)
{
    Serial.println("GET /api/calibration received.");
    DynamicJsonDocument r(1024); // Adjust size as needed

    // Include current control values (now in -90 to +90 degree range)
    r["automatic"] = automaticAnimations;
    r["face"]["leftEye"] = faceLeftEyeState;
    r["face"]["rightEye"] = faceRightEyeState;
    r["neck"]["rotate"] = neckRotate;
    r["neck"]["tiltForward"] = neckTiltForward;
    r["neck"]["tiltSideways"] = neckTiltSideways;
    r["body"]["rotate"] = bodyRotate;
    r["body"]["tiltForward"] = bodyTiltForward;
    r["body"]["tiltSideways"] = bodyTiltSideways;
    r["monoclePosition"] = monoclePosition; // Monocle position is 0-180

    // Include calibration values
    r["calibration"]["neckRotation"] = calNeckRotation;
    r["calibration"]["neckTiltForward"] = calNeckTiltForward;
    r["calibration"]["neckTiltSideways"] = calNeckTiltSideways;
    r["calibration"]["bodyRotation"] = calBodyRotation;
    r["calibration"]["bodyTiltForward"] = calBodyTiltForward;
    r["calibration"]["bodyTiltSideways"] = calBodyTiltSideways;
    r["calibration"]["monocle"] = calMonoclePosition; // Include monocle calibration

    // Include chest light mode
    // Ensure huyangBody is not null before accessing its member
    if (huyangBody) {
        r["chestLightMode"] = (uint16_t)huyangBody->currentLightMode; // Cast LightMode enum to uint16_t for JSON
    } else {
        r["chestLightMode"] = 0; // Default or error state if huyangBody not initialized
    }

    // Include settings
    r["robotName"] = robotName;
    r["masterMovementSpeed"] = masterMovementSpeed;
    r["firmwareVersion"] = firmwareVersion;

    String result;
    serializeJson(r, result);
    request->send(200, "application/json", result);
    Serial.println("GET /api/calibration response sent.");
}


// HTML page serving function (not currently used directly, but declared)
String WebServer::getPage(Page page, AsyncWebServerRequest *request)
{
    String pageContent = "";
    const char* filePath = "";

    switch (page)
    {
    case Page::indexPage: // Corrected: Removed WebServer:: prefix
        filePath = "/index.html";
        break;
    case Page::settingsPage: // Corrected: Removed WebServer:: prefix
        filePath = "/settings.html";
        break;
    case Page::calibrationPage: // Corrected: Removed WebServer:: prefix
        filePath = "/calibration.html";
        break;
    case Page::lightsPage: // Corrected: Removed WebServer:: prefix
        filePath = "/chestlights.html";
        break;
    default:
        Serial.println("WARNING: Unknown page requested in getPage!");
        request->send(404, "text/plain", "Page Not Found"); 
        return ""; 
    }

    pageContent = readFile(filePath);
    if (pageContent.length() == 0) {
        Serial.printf("ERROR: %s is empty or not found! Cannot serve page.\r\n", filePath);
        request->send(500, "text/plain", "Server Error: Requested HTML file is missing or empty.");
        return "";
    }
    
    Serial.printf("Generated page for type %d (file: %s) with %d bytes.\n", page, filePath, pageContent.length());
    return pageContent;
}

void WebServer::notFound(AsyncWebServerRequest *request)
{
    Serial.printf("404 Not Found: %s\n", request->url().c_str());
    request->send(404, "text/plain", "Not Found");
}
