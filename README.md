<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Huyang Remote Control - Interactive Setup Guide</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <!-- Chosen Palette: Tech Indigo -->
    <!-- Application Structure Plan: The application is structured as a task-oriented, tabbed interface to guide the user through the setup process. This breaks the linear README into manageable, logical steps (Overview, Prerequisites, Configuration, Final Steps), improving navigation and reducing cognitive load. The user flow is sequential, moving from one tab to the next, which mirrors the natural progression of a technical setup. This structure was chosen over a long scrolling page to create a more organized, interactive, and less intimidating experience for the user. -->
    <!-- Visualization & Content Choices: 
        - Report Info: Full README. Goal: Guide a technical setup.
        - Sectioning (Tabs): Presentation: HTML/Tailwind tabs. Interaction: JS click events to show/hide content panels. Justification: Organizes the setup process into distinct, digestible phases.
        - Features/Hardware: Presentation: Two-column grid with Unicode icons (⚙️, 🔌). Interaction: None. Justification: Visually separates and highlights key information.
        - Code Snippets (config.h): Presentation: Toggled code blocks. Interaction: JS-powered buttons to switch between Wi-Fi and Hotspot code. Justification: Provides focused, context-specific information without cluttering the UI.
        - Instructions: Presentation: Numbered lists within styled cards. Interaction: None. Justification: Clearly presents sequential steps in a readable format.
        - Libraries: Presentation: List with styled icons. Justification: Enhances scannability.
        - CONFIRMATION: NO SVG graphics used. NO Mermaid JS used. All visuals are HTML/CSS/Tailwind and Unicode characters.
    -->
    <!-- CONFIRMATION: NO SVG graphics used. NO Mermaid JS used. -->
    <style>
        body { font-family: 'Inter', sans-serif; }
        @import url('https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap');
        .tab-button.active {
            border-color: #4f46e5;
            color: #4f46e5;
            background-color: #eef2ff;
        }
        .tab-content { display: none; }
        .tab-content.active { display: block; }
        .code-block {
            background-color: #1f2937;
            color: #d1d5db;
            padding: 1rem;
            border-radius: 0.5rem;
            font-family: 'Courier New', Courier, monospace;
            white-space: pre-wrap;
            word-break: break-all;
        }
    </style>
</head>
<body class="bg-slate-100 text-slate-800">

    <div class="container mx-auto p-4 md:p-8 max-w-5xl">
        
        <header class="text-center mb-8">
            <h1 class="text-3xl md:text-4xl font-bold text-slate-900">Huyang Remote Control</h1>
            <p class="text-lg text-slate-600 mt-2">Interactive Setup Guide</p>
        </header>

        <div class="bg-white rounded-lg shadow-lg p-4 sm:p-6">
            <!-- Navigation Tabs -->
            <nav class="flex flex-wrap border-b border-slate-200 mb-6">
                <button data-tab="overview" class="tab-button flex-grow sm:flex-initial text-center py-3 px-4 font-semibold border-b-2 transition-colors duration-300">Overview</button>
                <button data-tab="prerequisites" class="tab-button flex-grow sm:flex-initial text-center py-3 px-4 font-semibold border-b-2 transition-colors duration-300">Prerequisites</button>
                <button data-tab="configuration" class="tab-button flex-grow sm:flex-initial text-center py-3 px-4 font-semibold border-b-2 transition-colors duration-300">Configuration</button>
                <button data-tab="final_steps" class="tab-button flex-grow sm:flex-initial text-center py-3 px-4 font-semibold border-b-2 transition-colors duration-300">Final Steps</button>
            </nav>

            <!-- Tab Content -->
            <main>
                <!-- Overview Tab -->
                <div id="overview" class="tab-content">
                    <h2 class="text-2xl font-bold mb-4">Welcome!</h2>
                    <p class="mb-6 text-slate-600">This guide will walk you through setting up the software for your ESP8266 to control a DroidDivision Huyang unit. This software enables remote control from any browser or mobile device and uses two GC9A01 TFT displays for visual feedback.</p>
                    <div class="grid md:grid-cols-2 gap-6">
                        <div class="bg-slate-50 rounded-lg p-4">
                            <h3 class="text-xl font-semibold mb-3 flex items-center">✨ Key Features</h3>
                            <ul class="space-y-2 text-slate-600 list-inside">
                                <li class="flex items-start"><span class="mr-2 text-indigo-500">✔</span> Browser/Mobile Control</li>
                                <li class="flex items-start"><span class="mr-2 text-indigo-500">✔</span> Dual GC9A01 TFT Display Support</li>
                                <li class="flex items-start"><span class="mr-2 text-indigo-500">✔</span> Flexible Wi-Fi or Hotspot Mode</li>
                                <li class="flex items-start"><span class="mr-2 text-indigo-500">✔</span> Easy to Configure</li>
                            </ul>
                        </div>
                        <div class="bg-slate-50 rounded-lg p-4">
                            <h3 class="text-xl font-semibold mb-3 flex items-center">🛠️ Hardware Requirements</h3>
                            <ul class="space-y-2 text-slate-600 list-inside">
                                <li class="flex items-start"><span class="mr-2 text-indigo-500">✔</span> An ESP8266 Development Board</li>
                                <li class="flex items-start"><span class="mr-2 text-indigo-500">✔</span> Two GC9A01 TFT Displays</li>
                            </ul>
                             <div class="mt-4 bg-indigo-100 border-l-4 border-indigo-500 text-indigo-700 p-3 rounded-r-lg">
                                <p class="font-bold">Wiring Diagrams</p>
                                <p>Refer to the `/Wiring Diagrams` folder in the repository for connection instructions.</p>
                            </div>
                        </div>
                    </div>
                </div>

                <!-- Prerequisites Tab -->
                <div id="prerequisites" class="tab-content">
                    <h2 class="text-2xl font-bold mb-4">Software Prerequisites</h2>
                    <p class="mb-6 text-slate-600">Before you can compile and upload the code, you need to set up your Arduino IDE with the correct board support and libraries. Follow the steps below carefully.</p>
                    
                    <div class="space-y-6">
                        <div class="bg-slate-50 rounded-lg p-4">
                            <h3 class="text-xl font-semibold mb-3">1. Install ESP Board Library</h3>
                            <ol class="list-decimal list-inside space-y-2 text-slate-600">
                                <li>Open Arduino IDE and go to `File > Preferences`.</li>
                                <li>In `Additional boards manager URLs`, add: <br><code class="text-sm bg-slate-200 p-1 rounded">http://arduino.esp8266.com/stable/package_esp8266com_index.json</code></li>
                                <li>Restart the Arduino IDE.</li>
                                <li>Go to `Tools > Board > Boards Manager...`.</li>
                                <li>Search for "esp" and install `ESP8266 Boards`.</li>
                                <li>Select your board from `Tools > Board`.</li>
                            </ol>
                        </div>

                        <div class="bg-slate-50 rounded-lg p-4">
                            <h3 class="text-xl font-semibold mb-3">2. Install Required Libraries</h3>
                            <p class="mb-3 text-slate-600">Go to `Tools > Manage Libraries...` and install the following libraries. Be sure to install any dependencies if prompted.</p>
                             <ul class="grid sm:grid-cols-2 gap-x-6 gap-y-2 text-slate-600">
                                <li class="flex items-center">📚 ESPAsyncWebServer</li>
                                <li class="flex items-center">📚 Adafruit PWM Servo Driver Library</li>
                                <li class="flex items-center">📚 Adafruit NeoPixel</li>
                                <li class="flex items-center">📚 GFX Library for Arduino</li>
                                <li class="flex items-center">📚 DFRobotDFPlayerMini</li>
                                <li class="flex items-center">📚 ArduinoJson</li>
                            </ul>
                        </div>
                         <div class="bg-slate-50 rounded-lg p-4">
                            <h3 class="text-xl font-semibold mb-3">3. Install LittleFS Plugin for IDE 2.x</h3>
                            <p class="text-slate-600">Install the `arduino-littlefs-upload` plugin by following the instructions at <a href="https://randomnerdtutorials.com/arduino-ide-2-install-esp8266-littlefs/#installing" target="_blank" class="text-indigo-600 hover:underline">Random Nerd Tutorials</a>.</p>
                        </div>
                    </div>
                </div>

                <!-- Configuration Tab -->
                <div id="configuration" class="tab-content">
                    <h2 class="text-2xl font-bold mb-4">Configure `config.h`</h2>
                    <p class="mb-6 text-slate-600">Before uploading, you must configure how your Huyang will connect to the network. Open the `config.h` file and choose one of the two modes below. Click a button to see the required code changes.</p>
                    
                    <div class="flex space-x-4 mb-4">
                        <button id="btnWifiMode" class="config-toggle-button bg-indigo-500 text-white py-2 px-4 rounded-lg shadow hover:bg-indigo-600 transition">Wi-Fi Mode</button>
                        <button id="btnHotspotMode" class="config-toggle-button bg-slate-500 text-white py-2 px-4 rounded-lg shadow hover:bg-slate-600 transition">Hotspot Mode</button>
                    </div>

                    <div id="config-details">
                        <p class="text-slate-500">Select a mode to view its configuration.</p>
                    </div>
                </div>
                
                <!-- Final Steps Tab -->
                <div id="final_steps" class="tab-content">
                    <h2 class="text-2xl font-bold mb-4">Build, Upload, and Run</h2>
                    <p class="mb-6 text-slate-600">You're almost there! Follow these final steps to get your Huyang online and ready for commands.</p>
                    
                     <div class="space-y-4">
                        <div class="bg-slate-50 rounded-lg p-4">
                            <h3 class="text-xl font-semibold mb-3">1. Build and Upload</h3>
                             <p class="text-slate-600">With your board and port selected correctly in the Arduino IDE, click the "Upload" button to build the sketch and write it to your ESP8266.</p>
                        </div>
                        <div class="bg-slate-50 rounded-lg p-4">
                             <h3 class="text-xl font-semibold mb-3">2. Monitor Serial Output</h3>
                            <p class="text-slate-600">Open the Serial Monitor (`Tools > Serial Monitor`) and set the baud rate to **115200**. This is where you will find the IP address if you are using Wi-Fi mode.</p>
                        </div>
                        <div class="bg-slate-50 rounded-lg p-4">
                            <h3 class="text-xl font-semibold mb-3">3. Access via Browser</h3>
                            <p class="text-slate-600 mb-3">Use the toggles below to see how to connect to the web interface based on your chosen configuration.</p>
                            <div class="flex space-x-4 mb-4">
                                <button id="btnAccessWifi" class="access-toggle-button bg-indigo-500 text-white py-2 px-4 rounded-lg shadow hover:bg-indigo-600 transition">Wi-Fi Mode</button>
                                <button id="btnAccessHotspot" class="access-toggle-button bg-slate-500 text-white py-2 px-4 rounded-lg shadow hover:bg-slate-600 transition">Hotspot Mode</button>
                            </div>
                            <div id="access-details" class="bg-slate-100 p-3 rounded">
                                 <p class="text-slate-500">Select a mode to view connection instructions.</p>
                            </div>
                        </div>
                    </div>
                </div>

            </main>
        </div>
         <footer class="text-center mt-8 text-sm text-slate-500">
            <p>Interactive guide created from the <a href="https://github.com/Myrddinwylit/Huyang_Droid_Controls" target="_blank" class="text-indigo-600 hover:underline">Huyang Droid Controls</a> repository.</p>
        </footer>
    </div>

    <script>
        document.addEventListener('DOMContentLoaded', function() {
            const tabs = document.querySelectorAll('.tab-button');
            const contents = document.querySelectorAll('.tab-content');

            function switchTab(tabName) {
                tabs.forEach(tab => {
                    tab.classList.remove('active');
                    if (tab.dataset.tab === tabName) {
                        tab.classList.add('active');
                    }
                });
                contents.forEach(content => {
                    content.classList.remove('active');
                    if (content.id === tabName) {
                        content.classList.add('active');
                    }
                });
            }

            tabs.forEach(tab => {
                tab.addEventListener('click', () => switchTab(tab.dataset.tab));
            });

            // Set initial tab
            switchTab('overview');

            // --- Configuration Toggles ---
            const configDetails = document.getElementById('config-details');
            const configWifiContent = `
                <h4 class="font-semibold text-lg mb-2">Wi-Fi Mode Configuration</h4>
                <p class="mb-3 text-slate-600">Your Huyang will connect to your existing home Wi-Fi network. Make sure the SSID and password are correct.</p>
                <div class="code-block">#define WIFI_MODE
// #define HOTSPOT_MODE

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";</div>`;
            
            const configHotspotContent = `
                <h4 class="font-semibold text-lg mb-2">Hotspot Mode Configuration</h4>
                <p class="mb-3 text-slate-600">Your Huyang will create its own Wi-Fi network. You will connect your phone or computer directly to it.</p>
                <div class="code-block">// #define WIFI_MODE
#define HOTSPOT_MODE

const char* hotspot_ssid = "Huyang_AP";
const char* hotspot_password = "huyangpassword";</div>`;

            document.getElementById('btnWifiMode').addEventListener('click', () => {
                configDetails.innerHTML = configWifiContent;
            });

            document.getElementById('btnHotspotMode').addEventListener('click', () => {
                configDetails.innerHTML = configHotspotContent;
            });
            
            // --- Access Toggles ---
            const accessDetails = document.getElementById('access-details');
            const accessWifiContent = `
                <h4 class="font-semibold text-lg mb-2">Connecting in Wi-Fi Mode</h4>
                <ol class="list-decimal list-inside space-y-1 text-slate-600">
                    <li>Find the IP address in the Serial Monitor (e.g., 192.168.1.50).</li>
                    <li>Ensure your device is on the same Wi-Fi network.</li>
                    <li>Enter the IP address into your browser.</li>
                    <li>If you changed the port, add it like this: \`http://192.168.1.50:8080\`</li>
                </ol>`;

            const accessHotspotContent = `
                <h4 class="font-semibold text-lg mb-2">Connecting in Hotspot Mode</h4>
                <ol class="list-decimal list-inside space-y-1 text-slate-600">
                    <li>Connect your device to the "Huyang_AP" Wi-Fi network.</li>
                    <li>Use the password "huyangpassword" (or your custom one).</li>
                    <li>In your browser, go to \`http://192.168.10.1\`.</li>
                    <li>If you changed the port, add it like this: \`http://192.168.10.1:123\`</li>
                </ol>`;

            document.getElementById('btnAccessWifi').addEventListener('click', () => {
                accessDetails.innerHTML = accessWifiContent;
            });
            
             document.getElementById('btnAccessHotspot').addEventListener('click', () => {
                accessDetails.innerHTML = accessHotspotContent;
            });

        });
    </script>

</body>
</html>
