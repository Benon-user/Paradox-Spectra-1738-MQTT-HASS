#include <WiFi.h>
#include <PubSubClient.h>


// Replace with your network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT Broker settings
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883; // Default MQTT port
const char* mqtt_user = "YOUR_MQTT_USERNAME";
const char* mqtt_password = "YOUR_MQTT_PASSWORD";

// Globals
WiFiClient espClient;
PubSubClient client(espClient);
HardwareSerial mySerial(1);  // Use UART1 for serial communication

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

// Event structure
struct Event {
    int EventId;
    int EventCategory;
    const char* EventName;
};

// Zone structure
struct Zone {
    int CategoryId;
    const char* ZoneName;
    bool IsZoneOpen;
    unsigned long ZoneEventTime;
};

// Event Category constants
const int ZONE = 1;
const int STATUS = 2;
const int TROUBLE = 3;
const int ACCESS_CODE = 4;
const int SPECIAL_ALARM = 5;
const int SPECIAL_ARM = 6;
const int SPECIAL_DISARM = 7;
const int NON_REPORT_EVENTS = 8;
const int SPECIAL_REPORT = 9;
const int REMOTE_CONTROL = 10;

Event events[] = {
    {0, ZONE, "Zone OK"},
    {1, ZONE, "Zone Open"},
    {2, STATUS, "Partition Status"},
    {5, NON_REPORT_EVENTS, "Non-Reportable Events"},
    {6, REMOTE_CONTROL, "Arm/Disarm with Remote Control"},
    {7, REMOTE_CONTROL, "Button Pressed on Remote (B)"},
    {8, REMOTE_CONTROL, "Button Pressed on Remote (C)"},
    {9, REMOTE_CONTROL, "Button Pressed on Remote (D)"},
    {10, ACCESS_CODE, "Bypass programming"},
    {11, ACCESS_CODE, "User Activated PGM"},
    {12, ZONE, "Zone with delay is breached"},
    {13, ACCESS_CODE, "Arm"},
    {14, SPECIAL_ARM, "Special Arm"},
    {15, ACCESS_CODE, "Disarm"},
    {16, ACCESS_CODE, "Disarm after Alarm"},
    {17, ACCESS_CODE, "Cancel Alarm"},
    {18, SPECIAL_DISARM, "Special Disarm"},
    {19, ZONE, "Zone Bypassed on arming"},
    {20, ZONE, "Zone in Alarm"},
    {21, ZONE, "Fire Alarm"},
    {22, ZONE, "Zone Alarm restore"},
    {23, ZONE, "Fire Alarm restore"},
    {24, SPECIAL_ALARM, "Special alarm"},
    {25, ZONE, "Auto zone shutdown"},
    {26, ZONE, "Zone tamper"},
    {27, ZONE, "Zone tamper restore"},
    {28, TROUBLE, "System Trouble"},
    {29, TROUBLE, "System Trouble restore"},
    {30, SPECIAL_REPORT, "Special Reporting"},
    {31, ZONE, "Wireless Transmitter Supervision Loss"},
    {32, ZONE, "Wireless Transmitter Supervision Loss Restore"},
    {33, ZONE, "Arming with a Keyswitch"},
    {34, ZONE, "Disarming with a Keyswitch"},
    {35, ZONE, "Disarm after Alarm with a Keyswitch"},
    {36, ZONE, "Cancel Alarm with a Keyswitch"},
    {37, ZONE, "Wireless Transmitter Low Battery"},
    {38, ZONE, "Wireless Transmitter Low Battery Restore"}
};

Zone zones[] = {
    {1, "ZONE 1", false, 0},
    {2, "ZONE 2", false, 0},
    {3, "ZONE 3", false, 0},
    {4, "ZONE 4", false, 0},
    {5, "ZONE 5", false, 0},
    {6, "ZONE 6", false, 0},
    {7, "ZONE 7", false, 0},
    {8, "ZONE 8", false, 0},
    {9, "ZONE 9", false, 0},
    {10, "ZONE 10", false, 0},
    {11, "ZONE 11", false, 0},
    {12, "ZONE 12", false, 0},
    {13, "ZONE 13", false, 0},
	{14, "ZONE 14", false, 0},
	{15, "ZONE 15", false, 0},
	{16, "ZONE 16", false, 0}
};

void setup() {
    Serial.begin(9600);
    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);

    // Initialize UART1 (Serial1) for communication on pins 16 (RX) and 17 (TX)
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    if (Serial2.available() >= 4) {
        byte DataStream[4];
        for (byte i = 0; i < 4; i++) {
            DataStream[i] = Serial2.read();
        }

        int msb = DataStream[2];
        int lsb = DataStream[3];

        // Getting minute and hour with shift operations
        int hour = msb >> 3;
        int minute = ((msb & 3) << 4) + (lsb >> 4);
        char paradoxTime[6];
        sprintf(paradoxTime, "%02d:%02d", hour, minute);

        int EventId = DataStream[0] >> 2;
        int CategoryId = ((DataStream[0] & 3) << 4) + (DataStream[1] >> 4);

        const char* Event = getEventName(EventId);
        int EventCategory = getEventCategory(EventId);

        String Message = String(CategoryId);

        bool isZoneEvent = EventCategory == ZONE;
        bool isStatus = EventCategory == STATUS;
        bool isTrouble = EventCategory == TROUBLE;
        bool isAccessCode = EventCategory == ACCESS_CODE;
        bool isSpecialAlarm = EventCategory == SPECIAL_ALARM;
        bool isSpecialArm = EventCategory == SPECIAL_ARM;
        bool isSpecialDisarm = EventCategory == SPECIAL_DISARM;
        bool isNonReportEvents = EventCategory == NON_REPORT_EVENTS;
        bool isSpecialReport = EventCategory == SPECIAL_REPORT;
        bool isRemoteControl = EventCategory == REMOTE_CONTROL;

        if (isZoneEvent) {
            bool IsZoneOpen = false;
            if (EventId == 1) IsZoneOpen = true;
            updateZoneStatus(CategoryId, IsZoneOpen);
            Message = getZoneName(CategoryId);
        }
        if (isStatus) Message = getPartitionStatus(CategoryId);
        if (isTrouble) Message = getSystemTrouble(CategoryId);
        if (isSpecialAlarm) Message = getSpecialAlarm(CategoryId);
        if (isSpecialArm) Message = getSpecialArm(CategoryId);
        if (isSpecialDisarm) Message = getSpecialDisarm(CategoryId);
        if (isNonReportEvents) Message = getNonReportableEvent(CategoryId);
        if (isSpecialReport) Message = getSpecialReporting(CategoryId);
        if (isRemoteControl) Message = String("Remote_") + CategoryId;
        if (isAccessCode) Message = getAccessCode(CategoryId);

        // Creating the payload message for MQTT (without timestamp)
        String mqttPayload = String(Event) + ", " + Message;
        
        // Creating the payload message for UART (with timestamp)
        String uartPayload = String(paradoxTime) + " " + Event + ", " + Message;
        
        // Publish to MQTT
        client.publish("home/alarms/paradox", mqttPayload.c_str());

        // Print to UART
        Serial.println(uartPayload);
    }
}

const char* getEventName(int eventId) {
    for (int i = 0; i < sizeof(events) / sizeof(Event); i++) {
        if (events[i].EventId == eventId) {
            return events[i].EventName;
        }
    }
    return "Unknown Event";
}

int getEventCategory(int eventId) {
    for (int i = 0; i < sizeof(events) / sizeof(Event); i++) {
        if (events[i].EventId == eventId) {
            return events[i].EventCategory;
        }
    }
    return -1;
}

void updateZoneStatus(int categoryId, bool isZoneOpen) {
    for (int i = 0; i < sizeof(zones) / sizeof(Zone); i++) {
        if (zones[i].CategoryId == categoryId) {
            zones[i].IsZoneOpen = isZoneOpen;
            zones[i].ZoneEventTime = millis();
        }
    }
}

const char* getZoneName(int categoryId) {
    for (int i = 0; i < sizeof(zones) / sizeof(Zone); i++) {
        if (zones[i].CategoryId == categoryId) {
            return zones[i].ZoneName;
        }
    }
    return "Unknown Zone";
}

const char* getPartitionStatus(int categoryId) {
    switch (categoryId) {
        case 0: return "System not ready";
        case 1: return "System ready";
        case 2: return "Steady alarm";
        case 3: return "Pulsed alarm";
        case 4: return "Pulsed or Steady Alarm";
        case 5: return "Alarm in partition restored";
        case 6: return "Bell Squawk Activated";
        case 7: return "Bell Squawk Deactivated";
        case 8: return "Ground start";
        case 9: return "Disarm partition";
        case 10: return "Arm partition";
        case 11: return "Entry delay started";
        default: return "Unknown Status";
    }
}

const char* getSystemTrouble(int categoryId) {
    switch (categoryId) {
        case  1: return "AC Loss";
        case  2: return "Battery Failure";
        case  3: return "Auxiliary current overload";
        case  4: return "Bell current overload";
        case  5: return "Bell disconnected";
        case  6: return "Timer Loss";
        case  7: return "Fire Loop Trouble";
        case  8: return "Future use";
        case  9: return "Module Fault";
        case 10: return "Printer Fault";
        case 11: return "Fail to Communicate";
        default: return "Unknown Trouble";
    }
}

const char* getNonReportableEvent(int categoryId) {
    switch (categoryId) {
        case  0: return "Telephone Line Trouble";
        case  1: return "Reset smoke detectors";
        case  2: return "Instant arming";
        case  3: return "Stay arming";
        case  4: return "Force arming";
        case  5: return "Fast Exit (Force & Regular Only)";
        case  6: return "PC Fail to Communicate";
        case  7: return "Midnight";
        default: return "Unknown Non-Reportable Event";
    }
}

const char* getSpecialAlarm(int categoryId) {
    switch (categoryId) {
        case  0: return "Emergency, keys [1] [3]";
        case  1: return "Auxiliary, keys [4] [6]";
        case  2: return "Fire, keys [7] [9]";
        case  3: return "Recent closing";
        case  4: return "Auto Zone Shutdown";
        case  5: return "Duress alarm";
        case  6: return "Keypad lockout";
        default: return "Unknown Special Alarm";
    }
}

const char* getSpecialReporting(int categoryId) {
    switch (categoryId) {
        case  0: return "System power up";
        case  1: return "Test report";
        case  2: return "WinLoad Software Access";
        case  3: return "WinLoad Software Access finished";
        case  4: return "Installer enters programming mode";
        case  5: return "Installer exits programming mode";
        default: return "Unknown Special Reporting";
    }
}

const char* getSpecialDisarm(int categoryId) {
    switch (categoryId) {
        case  0: return "Cancel Auto Arm (timed/no movement)";
        case  1: return "Disarm with WinLoad Software";
        case  2: return "Disarm after alarm with WinLoad Software";
        case  3: return "Cancel Alarm with WinLoad Software";
        default: return "Unknown Special Disarm";
    }
}

const char* getSpecialArm(int categoryId) {
    switch (categoryId) {
        case  0: return "Auto arming (timed/no movement)";
        case  1: return "Late to Close (Auto-Arming failed)";
        case  2: return "No Movement Auto-Arming";
        case  3: return "Partial Arming (Stay, Force, Instant, Bypass)";
        case  4: return "One-Touch Arming";
        case  5: return "Arm with WinLoad Software";
        case  7: return "Closing Delinquency";
        default: return "Unknown Special Arm";
    }
}

String getAccessCode(int code) {
    switch (code) {
        case  1: return "Master code";
        case  2: return "Master Code 1";
        case  3: return "Master Code 2";
        case 48: return "Duress Code";
        default: return "User Code " + String(code);
    }
}
