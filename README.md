# -Paradox-Spectra-1738-MQTT--HASS

Base pn the https://github.com/nplourde/Paradox-Spectra-1738-SerialOutput created code for ESP32 to read serial data from Paradox Spectra 1738 v.2.0

Equipment used:

**ESP32 Development Board WiFi+Bluetooth Ultra-Low Power Consumption Dual Core ESP-32S with terminal adapter**
https://www.aliexpress.com/item/1005005958763057.html?spm=a2g0o.order_detail.order_detail_item.3.b04b43ceKjKK9C

**LM2596 DC-DC Input 4V-35V Output 1.23V-30V Adjustable Step-down Regulator module**
https://www.aliexpress.com/item/2035753977.html?spm=a2g0o.order_detail.order_detail_item.11.123ef19cTXfBVt

**Code Explanation:**

The MQTT payload is published using client.publish("home/alarms/paradox", mqttPayload.c_str());.
The UART payload is printed using Serial.println(uartPayload);.
WiFi and MQTT Connection:

The WiFi library is used to connect the ESP32 to a WiFi network.
The PubSubClient library handles MQTT communication.
Connecting to WiFi:

The setup_wifi() function connects the ESP32 to the specified WiFi network.
MQTT Setup and Reconnection:

The MQTT client is set up with the client.setServer(mqtt_server, mqtt_port) function.
The reconnect() function ensures the ESP32 stays connected to the MQTT broker using the provided username and password.
Setting up Serial Communication on Pins 17 and 16:

The Serial2 object is defined to use the second hardware serial port (UART1) on the ESP32.
The Serial2.begin(9600, SERIAL_8N1, 17, 16) initializes the serial communication with a baud rate of 9600 on pins 17 (RX) and 16 (TX).
Reading Serial Data and Publishing to MQTT and UART:

The main loop() function reads data from the serial port, processes it, and publishes the resulting message to the MQTT broker under the topic home/alarms/paradox.
It also prints the message to the UART (serial monitor) using Serial.println(uartPayload).
The payload for MQTT is created without the paradoxTime: String mqttPayload = String(Event) + ", " + Message;.
The payload for UART includes the paradoxTime: String uartPayload = String(paradoxTime) + " " + Event + ", " + Message;.

**Helper Functions:**

Various helper functions (getEventName, getZoneName, etc.) are used to convert raw data into human-readable strings.

**Note:**
Replace placeholders (YOUR_SSID, YOUR_WIFI_PASSWORD, YOUR_MQTT_BROKER_IP, YOUR_MQTT_USERNAME, YOUR_MQTT_PASSWORD) with your actual WiFi credentials and MQTT broker details.
Ensure that your ESP32 is correctly connected to the serial device (e.g., Paradox Spectra 1738 alarm system).
You may need to adjust the pin numbers and settings if you're using different serial pins or configurations.
