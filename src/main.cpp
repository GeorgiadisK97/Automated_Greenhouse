#include <Arduino.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "Utility.h"

// WiFi credentials
const char *SSID = "SSID";
const char *PASSWORD = "PASSWORD";

// The String below "webpage" contains the complete HTML code that is sent to the client whenever someone connects to the webserver
String webpage = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no' /><style>h1 {text-align: center;}p {text-align: center;}</style><title>Page Title</title></head><body style='background-color: #EEEEEE;'><span style='color: #003366;'><h1>Greenhouse</h1><p>Current Temperature Threshold: <span id='DHT_THRESHOLD'>-</span></p><p>Current Humidity Threshold: <span id='HMD_THRESHOLD'>-</span></p><p><label for='TEMP_THRESHOLD'>Temperature Threshold:</label><input type='number' id='TEMP_THRESHOLD' name='TEMP_THRESHOLD'<br><br></p><p><label for='HUMID_THRESHOLD'> Humidity Threshold:</label><input type='number' id='HUMID_THRESHOLD' name='HUMID_THRESHOLD'<br><br></p><p><button type='button' id='BTN_SEND_BACK'>Submit</button></p><p>PUMP_STATE: <span id='PUMP_STATE'>-</span></p><p>FAN_STATE: <span id='FAN_STATE'>-</span></p><p>WINDOW_STATE: <span id='WINDOW_STATE'>-</span></p></span></body><script> var Socket; document.getElementById('BTN_SEND_BACK').addEventListener('click', button_send_back); function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function button_send_back() { var msg = {DHT_THRESHOLD : document.getElementById('TEMP_THRESHOLD').value,HMD_THRESHOLD : document.getElementById('HUMID_THRESHOLD').value};Socket.send(JSON.stringify(msg)); } function processCommand(event) {var obj = JSON.parse(event.data);document.getElementById('DHT_THRESHOLD').innerHTML = obj.DHT_THRESHOLD;document.getElementById('HMD_THRESHOLD').innerHTML = obj.HMD_THRESHOLD;document.getElementById('PUMP_STATE').innerHTML = obj.PUMP_STATE;document.getElementById('FAN_STATE').innerHTML = obj.FAN_STATE;document.getElementById('WINDOW_STATE').innerHTML = obj.WINDOW_STATE; } window.onload = function(event) { init(); }</script></html>";

// We want to periodically send values to the clients, so we need to define an "interval" and remember the last time we sent data to the client (with "previousMillis")
int interval = 1000;              // Send data to the client every 1000ms -> 1s
unsigned long previousMillis = 0; // We use the "millis()" command for time reference and this will output an unsigned long

// Initialization of webserver and websocket
WebServer server(80);                              // The server uses port 80 (standard port for websites
WebSocketsServer webSocket = WebSocketsServer(81); // The websocket uses port 81 (standard port for websockets

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length);

// Sensors Declaration
DHT_L dht{DHT_DATA_PIN, DHTTYPE};
HMD hmd{HMD_DATA_PIN, HMD_POWER_PIN};

// Motors Initialization
Fan fan{FAN_RELAY_PIN};
Pump pump{PUMP_RELAY_PIN};
Servo_motor window{_HZ, _SERVO_PIN, _MIN_PULSE_WIDTH, _MAX_PULSE_WIDTH};

int DHT_THRESHOLD{24};
int HMD_THRESHOLD{60};
int previous_temp_data{0};
int previous_hmd_data{0};
int current_temp_data{0};
int current_hmd_data{0};

// Flags for the loop() preventing the execution of the same code
bool temp_flag{true};
bool hmd_flag{true};

void setup()
{
  // LCD initialization
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, droplet);     // Custom character for Humidity
  lcd.createChar(1, thermometer); // Custom character for Temperature
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  Serial.begin(115200);
  Serial.print("Establishing network connection.(" + String(SSID) + ")");

  // Establishing WiFi network connection.
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.print("Connected to network IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {                     // Define here what the webserver needs to do
    server.send(200, "text/html", webpage); //    -> it needs to send out the HTML string "webpage" to the client
  });
  server.begin(); // start server

  webSocket.begin();                 // Start websocket
  webSocket.onEvent(webSocketEvent); // Define a callback function -> what does the ESP32 need to do when an event from the websocket is received? -> run function "webSocketEvent()"

  // Pin configuration
  pinMode(DHT_DATA_PIN, INPUT);
  pinMode(HMD_DATA_PIN, INPUT);
  pinMode(HMD_POWER_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print(WiFi.localIP());

  dht.begin();
  window.attach();

  current_temp_data = dht.read();         // Read the data from the DHT
  display_temperature(current_temp_data); // Display on the LCD

  if (!isnan(current_temp_data)) // Data validation
  {
    if (current_temp_data >= DHT_THRESHOLD)
    {
      temp_flag = false;
      fan.open();
      window.open();
    }
    else
    {
      temp_flag = true;
      fan.close();
      window.close();
    }
  }
  else
  {
    Serial.println("Error reading temperature");
  }

  current_hmd_data = hmd.read();
  display_moisture(current_hmd_data); // Display on the LCD
  if (!isnan(current_hmd_data))
  {
    if (current_hmd_data >= HMD_THRESHOLD)
    {
      hmd_flag = false;
      pump.close();
    }
    else
    {
      hmd_flag = true;
      pump.open();
    }
  }
  else
  {
    Serial.println("Error reading humidity.");
  }
}

void loop()
{
  server.handleClient(); // Needed for the webserver to handle all clients
  webSocket.loop();

  unsigned long now = millis(); // Read out the current "time" ("millis()" gives the time in ms since the Arduino started)
  if ((unsigned long)(now - previousMillis) > interval)
  { // check if "interval" ms has passed since last time the clients were updated

    String jsonString = "";                   // Create a JSON string for sending data to the client
    StaticJsonDocument<200> doc;              // Create a JSON container
    JsonObject object = doc.to<JsonObject>(); // Create a JSON Object
    object["DHT_THRESHOLD"] = DHT_THRESHOLD;  // Write data into the JSON object
    object["HMD_THRESHOLD"] = HMD_THRESHOLD;
    object["PUMP_STATE"] = pump.state();
    object["FAN_STATE"] = fan.state();
    object["WINDOW_STATE"] = window.state();
    serializeJson(doc, jsonString);     // Convert JSON object to string
    Serial.println(jsonString);         // Print JSON string to console for debug purposes
    webSocket.broadcastTXT(jsonString); // Send JSON string to clients

    previousMillis = now;

    current_temp_data = dht.read(); // Read the data from the DHT
    display_temperature(current_temp_data);
    if (!isnan(current_temp_data)) // Data validation
    {
      if (current_temp_data >= DHT_THRESHOLD) 
      {
        if (temp_flag)
        {
          temp_flag = false;
          fan.open();
          window.open();
        }
      }
      else if (current_temp_data <= DHT_THRESHOLD - 1)
      {
        if (!temp_flag)
        {
          temp_flag = true;
          fan.close();
          window.close();
        }
      }
    }
    else
    {
      Serial.println("Error reading temperature");
    }

    current_hmd_data = map(hmd.read(), 0, 380, 0, 99);
    display_moisture(current_hmd_data);
    if (!isnan(current_hmd_data))
    {
      if (current_hmd_data >= HMD_THRESHOLD)
      {
        if (hmd_flag)
        {
          hmd_flag = false;
          pump.close();
        }
      }
      else
      {
        if (!hmd_flag)
        {
          hmd_flag = true;
          pump.open();
        }
      }
    }
    else
    {
      Serial.println("Error reading humidity");
    }
  }
}

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length)
{ // the parameters of this callback function are always the same -> num: id of the client who send the event, type: type of message, payload: actual data sent and length: length of payload
  switch (type)
  {                         // Switch on the type of information sent
  case WStype_DISCONNECTED: // If a client is disconnected, then type == WStype_DISCONNECTED
    Serial.println("Client " + String(num) + " disconnected");
    break;
  case WStype_CONNECTED: // If a client is connected, then type == WStype_CONNECTED
    Serial.println("Client " + String(num) + " connected");
    break;
  case WStype_TEXT: // If a client has sent data, then type == WStype_TEXT
    // try to decipher the JSON string received
    StaticJsonDocument<200> doc; // create a JSON container
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    else
    {
      // JSON string was received correctly, so information can be retrieved:
      DHT_THRESHOLD = doc["DHT_THRESHOLD"];
      HMD_THRESHOLD = doc["HMD_THRESHOLD"];
    }
    Serial.println("");
    break;
  }
}