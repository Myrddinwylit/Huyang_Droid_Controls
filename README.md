Huyang Remote Control
🚀 Overview

This project based off JeanetteMueller's Huyang_Remote_Control -'THIS IS A WORK IN PROGRESS' provides the software for an ESP8266 microcontroller to remotely control your DroidDivision Huyang unit. It enables browser and mobile device access, utilizing two GC9A01 TFT displays for visual feedback. This solution is designed to run on most ESP8266 boards. Inside the /Wiring Diagrams folder, you can find instructions on how to connect two GC9A01 Displays to common boards.
<img width="1378" height="1128" alt="Screenshot 2025-07-11 at 10 15 49 AM" src="https://github.com/user-attachments/assets/1631418d-ad9d-43f2-986b-f2ebc041178d" />

https://myrddinwylit.github.io/Huyang_Droid_Controls/

✨ Getting This Running!
Follow these detailed steps to get Huyang Remote Control up and running on your ESP8266.

1. Configure config.h
Open the config.h file in the project directory and edit the configuration as you need it. You can configure Huyang to be accessible via its own Wi-Fi network (hotspot mode) or let it connect to your home Wi-Fi.

2. Install Board Library
To enable your Arduino IDE to recognize and program ESP boards:

Open Arduino IDE.

Go to File > Preferences (or Arduino > Preferences on macOS).

In the Additional boards manager URLs field, add the following URL on a new line:

http://arduino.esp8266.com/stable/package_esp8266com_index.json

Click OK and restart your Arduino IDE.

Navigate to Tools > Board > Boards Manager....

Search for "esp" and install either esp32 by Espressif or ESP8266 Boards (3.1.0) (choose the one appropriate for your board).

Finally, select your specific board under Tools > Board (e.g., Generic ESP8266 Module or ESP32 Dev Module).

3. Install Software Libraries
Install the following libraries via the Arduino IDE's Library Manager. If any dependencies are prompted, be sure to install them as well.

Go to Tools > Manage Libraries....

Search for and install each of the following:

ESPAsyncWebServer (by Iacamera)

Adafruit PWM Servo Driver Library (by Adafruit)

Adafruit NeoPixel (by Adafruit)

GFX Library for Arduino (by Moon On Our Nation)

DFRobotDFPlayerMini (by DFRobot)

ArduinoJson (by Benoit Blanchon)

4. Install Arduino IDE 2 Plugin
Install the arduino-littlefs-upload-1.5.0.vsix plugin by following the instructions provided on Random Nerd Tutorials:
https://randomnerdtutorials.com/arduino-ide-2-install-esp8266-littlefs/#installing

🚀 Running the Software
Once all the prerequisites are installed and configured:

Build and upload the code to your ESP8266 board using the Arduino IDE.

Huyang communicates via Serial (USB) on port 115200. Open the Serial Monitor in the Arduino IDE to view debug messages and the assigned IP address (in Wi-Fi mode).

Accessing Huyang
You have two main ways to connect to your Huyang:

Using Local Wi-Fi Mode
If you configured Huyang to connect to your home Wi-Fi:

Check Serial Monitor: Read the current IP Address displayed on the Serial Monitor.

Connect Device: Ensure your computer, phone, or other device is connected to the same Wi-Fi network as Huyang.

Open Browser: Enter the IP Address from the Serial Monitor into your browser's address bar.

Example: http://192.168.10.1

Custom Port: If you changed the WebServerPort from the default 80, you'll need to specify it:

Example: http://192.168.10.1:8080 (if your custom port is 8080)

Using Hotspot Mode
If you configured Huyang to create its own Wi-Fi hotspot:

Connect Device: Connect your device directly to the Wi-Fi network created by Huyang.

Open Browser: Enter the default hotspot IP address into your browser's address bar.

Example: http://192.168.10.1

Custom Port: If you changed the WebServerPort from the default 80, you'll need to specify it:

Example: http://192.168.10.1:123 (if your custom port is 123)

🤝 Contributing
Contributions are welcome! If you have suggestions for improvements, bug fixes, or new features, please feel free to:

Fork the repository.

Create a new branch (git checkout -b feature/YourFeature).

Make your changes.

Commit your changes (git commit -m 'Add some feature').

Push to the branch (git push origin feature/YourFeature).

Open a Pull Request.

📄 License
This project is licensed under the MIT License - see the LICENSE file for details.
