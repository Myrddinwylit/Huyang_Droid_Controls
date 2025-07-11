// Global variables for managing robot state
var automatic = true;

var face_eyes_all = null;
var face_eyes_left = 0; 
var face_eyes_right = 0; 

var neck_rotate = 0;
var neck_tiltForward = 0;
var neck_tiltSideways = 0;

var body_rotate = 0;
var body_tiltForward = 0;
var body_tiltSideways = 0;

var monoclePosition = 0; // Global variable for monocle current position

var JoyNeck; 
var JoyNeckX = 0;
var JoyNeckY = 0;

var JoyBody; 
var JoyBodyX = 0;
var JoyBodyY = 0;

// Chest light mode (reflects enum in WebServer.h)
// 0: OFF, 1: STATIC_BLUE, 2: WARNING_BLINK, 3: PROCESSING_FADE, 4: DROID_MODE_1, 5: DROID_MODE_2
var chestLightMode = 1; // Default to STATIC_BLUE

// --- NEW Global variables for Settings ---
var robotName = "Huyang Robot";
var masterMovementSpeed = 100; // Percentage 50-150

// --- Global variables for LED visual animations ---
let currentSetIntervalAnimationId = null; // For setInterval-based animations
let currentRequestAnimationFrameAnimationId = null; // For requestAnimationFrame-based animations

const LED_COLORS = { // Define common colors for LEDs
    BLUE: 'rgb(0, 0, 255)',
    RED: 'rgb(255, 0, 0)',
    GREEN: 'rgb(0, 255, 0)',
    YELLOW: 'rgb(255, 255, 0)',
    ORANGE: 'rgb(255, 165, 0)',
    MAGENTA: 'rgb(255, 0, 255)',
    CYAN: 'rgb(0, 255, 255)',
    BLACK: 'rgb(0, 0, 0)',
    DARK_GRAY: 'rgb(50, 50, 50)'
};

// --- Helper function to send data to the ESP32 web server ---
// This function is now generalized to send data to different API endpoints.
async function sendData(endpoint, data) {
    console.log(`sendData: Preparing to send data to ${endpoint}:`, data);
    try {
        const response = await fetch(endpoint, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(data)
        });

        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }

        const responseData = await response.json();
        console.log(`sendData: Response from ${endpoint}:`, responseData);
        // After sending data, it's often good to refresh the UI
        // However, for immediate feedback, we might not need a full refresh.
        // getServerData(); // Potentially call this to ensure UI is in sync
    } catch (error) {
        console.error(`sendData: Error sending data to ${endpoint}:`, error);
    }
}

// --- Fetch initial data from server and update UI ---
async function getServerData() {
    console.log("getServerData: Fetching initial server data.");
    try {
        // Corrected endpoint from /api/get.json to /api/calibration
        const response = await fetch('/api/calibration'); 
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();
        console.log("getServerData: Received data:", data);
        updateUserInterface(data);
    } catch (error) {
        console.error("getServerData: Error fetching initial server data:", error);
    }
}

// --- Update UI based on server data ---
function updateUserInterface(data) {
    // Update automatic mode button
    automatic = data.automatic;
    const automaticButton = document.getElementById('button_automatic');
    if (automaticButton) {
        automaticButton.innerText = automatic ? 'AUTOMATIC' : 'MANUAL';
        automaticButton.classList.toggle('selected', automatic);
    }

    // Update eye states (if applicable)
    face_eyes_left = data.face.leftEye;
    face_eyes_right = data.face.rightEye;
    // No direct UI update for individual eye states on index.html, handled by robot itself.

    // Update neck and body joystick values (if applicable)
    neck_rotate = data.neck.rotate;
    neck_tiltForward = data.neck.tiltForward;
    neck_tiltSideways = data.neck.tiltSideways;

    body_rotate = data.body.rotate;
    body_tiltForward = data.body.tiltForward;
    body_tiltSideways = data.body.tiltSideways;

    // Update monocle position
    monoclePosition = data.monoclePosition;
    const calMonocleSlider = document.getElementById('cal_monocle_position');
    const calMonocleValueSpan = document.getElementById('cal_monocle_position_value');
    if (calMonocleSlider && calMonocleValueSpan) {
        calMonocleSlider.value = monoclePosition;
        calMonocleValueSpan.innerText = monoclePosition;
    }

    // Update calibration sliders on calibration.html
    const calNeckRotationSlider = document.getElementById('cal_neck_rotation');
    if (calNeckRotationSlider) {
        calNeckRotationSlider.value = data.calibration.neckRotation;
        document.getElementById('cal_neck_rotation_value').innerText = data.calibration.neckRotation;
    }
    const calNeckTiltForwardSlider = document.getElementById('cal_neck_tilt_forward');
    if (calNeckTiltForwardSlider) {
        calNeckTiltForwardSlider.value = data.calibration.neckTiltForward;
        document.getElementById('cal_neck_tilt_forward_value').innerText = data.calibration.neckTiltForward;
    }
    const calNeckTiltSidewaysSlider = document.getElementById('cal_neck_tilt_sideways');
    if (calNeckTiltSidewaysSlider) {
        calNeckTiltSidewaysSlider.value = data.calibration.neckTiltSideways;
        document.getElementById('cal_neck_tilt_sideways_value').innerText = data.calibration.neckTiltSideways;
    }
    const calBodyRotationSlider = document.getElementById('cal_body_rotation');
    if (calBodyRotationSlider) {
        calBodyRotationSlider.value = data.calibration.bodyRotation;
        document.getElementById('cal_body_rotation_value').innerText = data.calibration.bodyRotation;
    }
    const calBodyTiltForwardSlider = document.getElementById('cal_body_tilt_forward');
    if (calBodyTiltForwardSlider) {
        calBodyTiltForwardSlider.value = data.calibration.bodyTiltForward;
        document.getElementById('cal_body_tilt_forward_value').innerText = data.calibration.bodyTiltForward;
    }
    const calBodyTiltSidewaysSlider = document.getElementById('cal_body_tilt_sideways');
    if (calBodyTiltSidewaysSlider) {
        calBodyTiltSidewaysSlider.value = data.calibration.bodyTiltSideways;
        document.getElementById('cal_body_tilt_sideways_value').innerText = data.calibration.bodyTiltSideways;
    }
    
    // Update chest light mode buttons
    chestLightMode = data.chestLightMode;
    updateChestLightButtons(chestLightMode);
    updateLedVisuals(chestLightMode); // Update LED visualizer on chestlights.html

    // Update settings on settings.html
    robotName = data.robotName || "Huyang Robot";
    masterMovementSpeed = data.masterMovementSpeed || 100;
    const robotNameInput = document.getElementById('robot_name_input');
    if (robotNameInput) {
        robotNameInput.value = robotName;
    }
    const masterSpeedSlider = document.getElementById('master_speed_slider');
    const masterSpeedValueSpan = document.getElementById('master_speed_value');
    if (masterSpeedSlider && masterSpeedValueSpan) {
        masterSpeedSlider.value = masterMovementSpeed;
        masterSpeedValueSpan.innerText = masterMovementSpeed + '%';
    }
    const firmwareVersionDisplay = document.getElementById('firmware_version_display');
    if (firmwareVersionDisplay) {
        firmwareVersionDisplay.innerText = data.firmwareVersion || "N/A";
    }
}

// --- Specific Send Functions for different API calls ---

function sendEyeUpdate(target, state) {
    console.log(`sendEyeUpdate: target=${target}, state=${state}`);
    // Corrected endpoint from /api/post.json to /api/action
    sendData('/api/action', { type: 'eye', target: target, state: state });
}

function sendNeckUpdate() {
    console.log("sendNeckUpdate called.");
    // JoyNeckX and JoyNeckY are updated by the joystick callback
    neck_rotate = JoyNeckX;
    neck_tiltForward = JoyNeckY; // Assuming Y controls forward/backward tilt
    // If you have a separate control for sideways tilt, update neck_tiltSideways here too.

    // Corrected endpoint from /api/post.json to /api/action
    sendData('/api/action', {
        type: 'neck',
        rotate: neck_rotate,
        tiltForward: neck_tiltForward,
        tiltSideways: neck_tiltSideways // Ensure this is controlled if applicable
    });
}

function sendBodyUpdate() {
    console.log("sendBodyUpdate called.");
    // JoyBodyX and JoyBodyY are updated by the joystick callback
    body_rotate = JoyBodyX;
    body_tiltForward = JoyBodyY; // Assuming Y controls body forward/backward tilt
    // If you have a separate control for sideways tilt, update body_tiltSideways here too.

    // Corrected endpoint from /api/post.json to /api/action
    sendData('/api/action', {
        type: 'body',
        rotate: body_rotate,
        tiltForward: body_tiltForward,
        tiltSideways: body_tiltSideways // Ensure this is controlled if applicable
    });
}

function sendMonocleUpdate() {
    const monocleSlider = document.getElementById('monocle_position');
    if (monocleSlider) {
        monoclePosition = parseInt(monocleSlider.value);
        console.log("sendMonocleUpdate: Monocle position:", monoclePosition);
        // Corrected endpoint from /api/post.json to /api/action
        sendData('/api/action', { type: 'monocle', position: monoclePosition });
    }
}

function changeAutomatic(state) {
    console.log("changeAutomatic called with state:", state);
    // Corrected endpoint from /api/post.json to /api/action
    sendData('/api/action', { type: 'automatic', state: state });
    // Update UI immediately for responsiveness
    automatic = state;
    const automaticButton = document.getElementById('button_automatic');
    if (automaticButton) {
        automaticButton.innerText = automatic ? 'AUTOMATIC' : 'MANUAL';
        automaticButton.classList.toggle('selected', automatic);
    }
}

// --- Calibration Page Functions ---
function sendCalibrationUpdate(type, param, value) {
    console.log(`sendCalibrationUpdate: type=${type}, param=${param}, value=${value}`);
    let data = { type: type, action: 'update' };
    data[param] = parseInt(value); // Ensure value is integer
    sendData('/api/calibrate', data); // Use the new /api/calibrate endpoint
}

function sendCalibrationSave() {
    console.log("sendCalibrationSave called.");
    sendData('/api/calibrate', { action: 'save' }); // Use the new /api/calibrate endpoint
}

function sendCalibrationReset() {
    console.log("sendCalibrationReset called.");
    if (confirm("Are you sure you want to reset all calibration values to default (0)? This cannot be undone!")) {
        sendData('/api/calibrate', { action: 'reset' }); // Use the new /api/calibrate endpoint
    }
}

// NEW: Functions for "Set Middle and Lock" and "Unlock Servos"
function sendSetMiddleAndLock() {
    console.log("sendSetMiddleAndLock called.");
    // This will send a command to the ESP32 to set servos to middle and potentially lock them
    sendData('/api/calibrate', { action: 'set_middle_and_lock' });
    alert("Command sent: Set servos to middle position and lock. Please re-attach servo horns.");
}

function sendUnlockServos() {
    console.log("sendUnlockServos called.");
    // This will send a command to the ESP32 to unlock servos
    sendData('/api/calibrate', { action: 'unlock_servos' });
    alert("Command sent: Unlock servos.");
}


// --- Chest Lights Page Functions ---
function sendChestLightUpdate(mode) {
    console.log("sendChestLightUpdate called with mode:", mode);
    // Corrected endpoint from /api/post.json to /api/lights
    sendData('/api/lights', { mode: mode });
    updateChestLightButtons(mode); // Update UI immediately
    updateLedVisuals(mode); // Update LED visualizer immediately
}

function updateChestLightButtons(currentMode) {
    // This function highlights the active chest light button
    const buttons = document.querySelectorAll('.menu_light_buttons .button');
    buttons.forEach(button => {
        button.classList.remove('selected');
    });

    // For chestlights.html
    const lightButtons = document.querySelectorAll('.light-control-buttons .button');
    lightButtons.forEach(button => {
        button.classList.remove('selected');
        // Check the onclick attribute to find the matching button
        if (button.onclick && button.onclick.toString().includes(`sendChestLightUpdate(${currentMode})`)) {
            button.classList.add('selected');
        }
    });

    // For index.html quick access buttons
    const quickAccessButtons = document.querySelectorAll('.menu_light_buttons .button');
    quickAccessButtons.forEach(button => {
        button.classList.remove('selected');
        if (button.onclick && button.onclick.toString().includes(`sendChestLightUpdate(${currentMode})`)) {
            button.classList.add('selected');
        }
    });
}

// --- LED Visualizer Functions (for chestlights.html) ---
function updateLedVisuals(mode) {
    // Clear any existing animations
    if (currentSetIntervalAnimationId) {
        clearInterval(currentSetIntervalAnimationId);
        currentSetIntervalAnimationId = null;
    }
    if (currentRequestAnimationFrameAnimationId) {
        cancelAnimationFrame(currentRequestAnimationFrameAnimationId);
        currentRequestAnimationFrameAnimationId = null;
    }

    const led1 = document.getElementById('led1');
    const led2 = document.getElementById('led2');

    if (!led1 || !led2) return; // Exit if LEDs not found (e.g., on other pages)

    // Reset LEDs to default off state
    led1.style.backgroundColor = LED_COLORS.DARK_GRAY;
    led1.style.boxShadow = `0 0 5px ${LED_COLORS.DARK_GRAY}`;
    led2.style.backgroundColor = LED_COLORS.DARK_GRAY;
    led2.style.boxShadow = `0 0 5px ${LED_COLORS.DARK_GRAY}`;

    switch (mode) {
        case 0: // LIGHT_OFF
            // Already set to dark gray above
            break;
        case 1: // LIGHT_STATIC_BLUE
            setLedColor(led1, LED_COLORS.BLUE);
            setLedColor(led2, LED_COLORS.BLUE);
            break;
        case 2: // LIGHT_WARNING_BLINK (Alternating Red/Blue)
            let warningState = 0;
            currentSetIntervalAnimationId = setInterval(() => {
                if (warningState === 0) {
                    setLedColor(led1, LED_COLORS.RED);
                    setLedColor(led2, LED_COLORS.BLUE);
                } else {
                    setLedColor(led1, LED_COLORS.BLUE);
                    setLedColor(led2, LED_COLORS.RED);
                }
                warningState = 1 - warningState; // Toggle 0 and 1
            }, 500); // Blink every 500ms
            break;
        case 3: // LIGHT_PROCESSING_FADE (Random fade, then slow blinks)
            // This is a more complex animation, let's use requestAnimationFrame for smoothness
            let fadeStartTime = null;
            let fadeDuration = 2000; // Total duration for fade cycle
            let blinkInterval = 1000; // For the final slow blinks
            let fadePhase = 0; // 0: fading, 1: slow blinking
            let lastBlinkToggle = 0;

            function animateProcessing(currentTime) {
                if (!fadeStartTime) fadeStartTime = currentTime;
                const elapsed = currentTime - fadeStartTime;

                if (fadePhase === 0) { // Fading phase
                    const progress = (elapsed % fadeDuration) / fadeDuration;
                    const brightness = Math.abs(Math.sin(progress * Math.PI)); // Sine wave for smooth fade
                    
                    // Randomly pick a color for fading, or cycle through a few
                    const colors = [LED_COLORS.CYAN, LED_COLORS.MAGENTA, LED_COLORS.YELLOW];
                    const colorIndex = Math.floor(progress * colors.length);
                    const currentColor = colors[colorIndex % colors.length];

                    const r = Math.floor(parseInt(currentColor.substring(currentColor.indexOf('(') + 1, currentColor.indexOf(','))) * brightness);
                    const g = Math.floor(parseInt(currentColor.substring(currentColor.indexOf(',') + 1, currentColor.lastIndexOf(','))) * brightness);
                    const b = Math.floor(parseInt(currentColor.substring(currentColor.lastIndexOf(',') + 1, currentColor.indexOf(')'))) * brightness);
                    
                    const animatedColor = `rgb(${r}, ${g}, ${b})`;

                    setLedColor(led1, animatedColor);
                    setLedColor(led2, animatedColor);

                    if (elapsed > fadeDuration * 2) { // Transition to slow blinking after a few cycles
                        fadePhase = 1;
                        lastBlinkToggle = currentTime;
                        setLedColor(led1, LED_COLORS.BLUE); // Start with blue for blinking
                        setLedColor(led2, LED_COLORS.BLUE);
                    }
                } else { // Slow blinking phase
                    if (currentTime - lastBlinkToggle > blinkInterval) {
                        lastBlinkToggle = currentTime;
                        if (led1.style.backgroundColor === LED_COLORS.BLUE) {
                            setLedColor(led1, LED_COLORS.BLACK);
                            setLedColor(led2, LED_COLORS.BLACK);
                        } else {
                            setLedColor(led1, LED_COLORS.BLUE);
                            setLedColor(led2, LED_COLORS.BLUE);
                        }
                    }
                }
                currentRequestAnimationFrameAnimationId = requestAnimationFrame(animateProcessing);
            }
            currentRequestAnimationFrameAnimationId = requestAnimationFrame(animateProcessing);
            break;
        case 4: // LIGHT_DROID_MODE_1 (Star Wars Droid Indicator)
            let droid1State = 0;
            currentSetIntervalAnimationId = setInterval(() => {
                if (droid1State === 0) {
                    setLedColor(led1, LED_COLORS.ORANGE);
                    setLedColor(led2, LED_COLORS.BLACK);
                } else if (droid1State === 1) {
                    setLedColor(led1, LED_COLORS.BLACK);
                    setLedColor(led2, LED_COLORS.ORANGE);
                } else { // Brief off state
                    setLedColor(led1, LED_COLORS.BLACK);
                    setLedColor(led2, LED_COLORS.BLACK);
                }
                droid1State = (droid1State + 1) % 3; // Cycle 0, 1, 2
            }, 300);
            break;
        case 5: // LIGHT_DROID_MODE_2 (Star Wars Droid Indicator)
            let droid2State = 0;
            currentSetIntervalAnimationId = setInterval(() => {
                if (droid2State === 0) {
                    setLedColor(led1, LED_COLORS.MAGENTA);
                    setLedColor(led2, LED_COLORS.CYAN);
                } else if (droid2State === 1) {
                    setLedColor(led1, LED_COLORS.CYAN);
                    setLedColor(led2, LED_COLORS.MAGENTA);
                } else {
                    setLedColor(led1, LED_COLORS.BLACK);
                    setLedColor(led2, LED_COLORS.BLACK);
                }
                droid2State = (droid2State + 1) % 3;
            }, 200); // Faster cycle
            break;
    }
}

function setLedColor(ledElement, color) {
    if (ledElement) {
        ledElement.style.backgroundColor = color;
        ledElement.style.boxShadow = `0 0 15px ${color}`; // Add glow effect
    }
}

// --- Settings Page Functions ---
function initSettings() {
    // This function will be called when settings.html DOM is loaded
    getServerData(); // Fetch and populate settings
}

function sendSettingsUpdate() {
    console.log("sendSettingsUpdate called.");
    const robotNameInput = document.getElementById('robot_name_input');
    const masterSpeedSlider = document.getElementById('master_speed_slider');

    if (robotNameInput) {
        robotName = robotNameInput.value;
    }
    if (masterSpeedSlider) {
        masterMovementSpeed = parseInt(masterSpeedSlider.value);
    }

    sendData('/api/settings', { // Use the new /api/settings endpoint
        robotName: robotName,
        masterMovementSpeed: masterMovementSpeed
    });
}

function sendRebootCommand() {
    console.log("sendRebootCommand called.");
    if (confirm("Are you sure you want to reboot the robot?")) {
        sendData('/api/system', { command: 'reboot' }); // Use the new /api/system endpoint
        alert("Reboot command sent. The robot will restart.");
    }
}

function confirmFactoryReset() {
    console.log("confirmFactoryReset called.");
    if (confirm("WARNING: Are you absolutely sure you want to perform a FACTORY RESET? This will erase ALL settings and calibration and cannot be undone!")) {
        sendData('/api/system', { command: 'factory_reset' }); // Use the new /api/system endpoint
        alert("Factory reset command sent. The robot will reset and reboot.");
    }
}

// --- Calibration Page Functions ---
function initCalibrationSliders() {
    // This function will be called when calibration.html DOM is loaded
    getServerData(); // Fetch and populate calibration data
}


// --- Joystick Initialization ---
function initJoystick() {
    console.log("initJoystick: Starting joystick initialization.");

    // Initialize JoyNeck if the canvas element exists and JoyStick is available
    const joyNeckElement = document.getElementById('joyNeck');
    if (joyNeckElement && typeof JoyStick !== 'undefined' && !JoyNeck) {
        console.log("initJoystick: Attempting to initialize joyNeck.");
        try {
            JoyNeck = new JoyStick('joyNeck', {}, function(stickData) {
                if (JoyNeckX !== stickData.x || JoyNeckY !== stickData.y) {
                    JoyNeckX = stickData.x;
                    JoyNeckY = stickData.y;
                    sendNeckUpdate(); // Send update when joystick moves
                }
            });
            console.log("initJoystick: JoyNeck initialized successfully:", JoyNeck);
        } catch (e) {
            console.error("initJoystick: Error initializing JoyNeck:", e);
        }
    } else {
        if (joyNeckElement == null) {
            console.log("initJoystick: joyNeck canvas element NOT found in DOM. (This is expected if not on index.html, but unexpected if on index.html).");
        } else if (typeof JoyStick === 'undefined') {
            console.error("initJoystick: JoyStick constructor is UNDEFINED for joyNeck. Check if joystick.js loaded correctly and without errors.");
        } else {
            console.log("initJoystick: JoyNeck already initialized or other condition not met for joyNeck.");
        }
    }

    // Initialize JoyBody if the canvas element exists and JoyStick is available
    const joyBodyElement = document.getElementById('joyBody');
    if (joyBodyElement && typeof JoyStick !== 'undefined' && !JoyBody) {
        console.log("initJoystick: Attempting to initialize joyBody.");
        try {
            JoyBody = new JoyStick('joyBody', {}, function(stickData) {
                if (JoyBodyX !== stickData.x || JoyBodyY !== stickData.y) {
                    JoyBodyX = stickData.x;
                    JoyBodyY = stickData.y;
                    sendBodyUpdate(); 
                }
            });
            console.log("initJoystick: JoyBody initialized successfully:", JoyBody);
        } catch (e) {
            console.error("initJoystick: Error initializing JoyBody:", e);
        }
    } else {
        // NEW: More specific logs for why initialization might be skipped
        if (joyBodyElement == null) {
            console.log("initJoystick: joyBody canvas element NOT found in DOM. (This is expected if not on index.html, but unexpected if on index.html).");
        } else if (typeof JoyStick === 'undefined') {
            console.error("initJoystick: JoyStick constructor is UNDEFINED for joyBody. Check if joystick.js loaded correctly and without errors.");
        } else {
            console.log("initJoystick: JoyBody already initialized or other condition not met for joyBody.");
        }
    }
}


// --- System Initialization (called on DOMContentLoaded) ---
function systemInit() {
    getServerData(); // Fetch initial data from server
    initJoystick(); // Initialize joysticks
    // Other initializations can go here
}

// Ensure systemInit() is only called when DOM is fully loaded
document.addEventListener('DOMContentLoaded', function() {
    console.log("DOM loaded, calling systemInit().");
    systemInit();
});
