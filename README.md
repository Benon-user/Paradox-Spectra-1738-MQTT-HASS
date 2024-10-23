# Paradox Spectra 1738 Interface via WIFI to MQTT in Home Assistant

Reading the serial stream from Paradox Spectra 1738 Security System Serial Output with ESP32, and sending ower Wifi to MQTT server in Home Assistant. 

![image](https://github.com/user-attachments/assets/139da3f6-5b41-4e8d-b6fd-3a532aab6416)


**Equipment used:**

**ESP32 Development Board WiFi+Bluetooth Ultra-Low Power Consumption Dual Core ESP-32S with terminal adapter** https://www.aliexpress.com/item/1005005958763057.html

**LM2596 DC-DC Input 4V-35V Output 1.23V-30V Adjustable Step-down Regulator module** https://www.aliexpress.com/item/2035753977.html

**Electrical Connections**

DC/DC step-down converter must be set output voltage to 5V before conneting ESP32 board.
![Connection Diagram](https://github.com/user-attachments/assets/3c20b173-1026-4cd1-9d6c-a3f54486b4d7)


**Code Explanation:**

Using the knowledge from reverse engineering in repository https://github.com/nplourde/Paradox-Spectra-1738-SerialOutput ,  created the scatch in Arduion IDE for ESP-32S. 
Programm reads serial data from Paradox Spectra 1738 v.2.1. then decodes it to user friendly text messages and sends ower Wifi to MQTT broker in Home Assistant. 


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
 The Serial2.begin(9600, SERIAL_8N1, 17, 16) initializes the serial communication with a baud rate of 9600 on pins 17 (RX) and 16 (TX). Only RX is connected in this application.


Reading Serial Data and Publishing to MQTT and UART:

 The main loop() function reads data from the serial port, processes it, and publishes the resulting message to the MQTT broker under the topic home/alarms/paradox.
 It also prints the message to the UART (serial monitor) using Serial.println(uartPayload).
 The payload for MQTT is created without the paradoxTime: String mqttPayload = String(Event) + ", " + Message;.
 The payload for UART includes the paradoxTime: String uartPayload = String(paradoxTime) + " " + Event + ", " + Message;.


Helper Functions:

 Various helper functions (getEventName, getZoneName, etc.) are used to convert raw data into human-readable strings.


Note:

 Replace placeholders (YOUR_SSID, YOUR_WIFI_PASSWORD, YOUR_MQTT_BROKER_IP, YOUR_MQTT_USERNAME, YOUR_MQTT_PASSWORD) with your actual WiFi credentials and MQTT broker details.
 Ensure that your ESP32 is correctly connected to the serial device (e.g., Paradox Spectra 1738 alarm system). You may need to adjust the pin numbers and settings if you're using different serial pins or configurations.



**Home Assistant setup**

Ensure MQTT is Set Up in Home Assistant 

Configure MQTT sensor in configuration.yaml.

    mqtt:
      sensor:
        - name: "Paradox Event and Message"
          state_topic: "home/alarms/paradox"

