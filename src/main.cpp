#include <Arduino.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "Utility.h"

// WiFi credentials
const char *SSID = "SSID";
const char *PASSWORD = "PASSWORD";

// The String below "webpage" contains the complete HTML code that is sent to the client whenever someone connects to the webserver
String webpage = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'><style>h1 {text-align: center;}p {text-align: center;}</style><title>Page Title</title></head><body style='background-color: #EEEEEE;'><span style='color: #003366;'><h1>Automated Greenhouse</h1><p>Current Temperature Threshold: <span id='DHT_THRESHOLD'>-</span></p><p>Current Humidity Threshold: <span id='HMD_THRESHOLD'>-</span></p><p><label for='TEMP_THRESHOLD'>Temperature Threshold:</label><input type='number' id='TEMP_THRESHOLD' name='TEMP_THRESHOLD'<br><br></p><p><label for='HUMID_THRESHOLD'> Humidity Threshold:</label><input type='number' id='HUMID_THRESHOLD' name='HUMID_THRESHOLD'<br><br></p><p><button type='button' id='BTN_SEND_BACK'>Submit</button></p></span></body><script> var Socket; document.getElementById('BTN_SEND_BACK').addEventListener('click', button_send_back); function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function button_send_back() { var msg = {DHT_THRESHOLD : document.getElementById('TEMP_THRESHOLD').value,HMD_THRESHOLD : document.getElementById('HUMID_THRESHOLD').value};Socket.send(JSON.stringify(msg)); } function processCommand(event) {var obj = JSON.parse(event.data);document.getElementById('DHT_THRESHOLD').innerHTML = obj.DHT_THRESHOLD;document.getElementById('HMD_THRESHOLD').innerHTML = obj.HMD_THRESHOLD; } window.onload = function(event) { init(); }</script></html>";

// We want to periodically send values to the clients, so we need to define an "interval" and remember the last time we sent data to the client (with "previousMillis")
int interval = 1000;              // send data to the client every 1000ms -> 1s
unsigned long previousMillis = 0; // we use the "millis()" command for time reference and this will output an unsigned long

// Initialization of webserver and websocket
WebServer server(80);                              // the server uses port 80 (standard port for websites
WebSocketsServer webSocket = WebSocketsServer(81); // the websocket uses port 81 (standard port for websockets

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length);

// Sensors Initialization
int DHT_THRESHOLD{24};
int HMD_THRESHOLD{60};
DHT_L dht{DHT_DATA_PIN, DHTTYPE};
HMD hmd{HMD_DATA_PIN, HMD_POWER_PIN};

int previous_temp_data{0};
int previous_hmd_data{0};
int current_temp_data{0};
int current_hmd_data{0};

bool temp_flag{true};
bool hmd_flag{true};

// Motors Initialization
Fan fan{FAN_RELAY_PIN};
Pump pump{PUMP_RELAY_PIN};
Servo_motor window{_HZ, _SERVO_PIN, _MIN_PULSE_WIDTH, _MAX_PULSE_WIDTH};

void setup()
{
  Serial.begin(115200);
  Serial.print("Establishing network connection.(" + String(SSID) + ")");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.print("Connected to network IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {                     // define here what the webserver needs to do
    server.send(200, "text/html", webpage); //    -> it needs to send out the HTML string "webpage" to the client
  });
  server.begin(); // start server

  webSocket.begin();                 // start websocket
  webSocket.onEvent(webSocketEvent); // define a callback function -> what does the ESP32 need to do when an event from the websocket is received? -> run function "webSocketEvent()"

  pinMode(DHT_DATA_PIN, INPUT);
  pinMode(HMD_DATA_PIN, INPUT);
  pinMode(HMD_POWER_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  window.attach();
  delay(100);

  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, droplet);
  lcd.createChar(1, thermometer);

  // Initialize device.
  dht.begin();

  init_flags();
  print_UI();

  delay(2000);
  current_temp_data = dht.read(); // READ DHT SENSOR DATA
  display_temperature(current_temp_data);
  previous_temp_data = current_temp_data;
  if (!isnan(current_temp_data))
  {
    if (dht.check_data(DHT_THRESHOLD))
    {
      temp_flag = false;
      window.open();
      fan.open();
    }
    else
    {
      temp_flag = true;
      window.close();
      fan.close();
    }
  }
  else
  {
    Serial.println("Error reading temperature");
  }

  current_hmd_data = hmd.read();
  display_moisture(current_hmd_data);
  previous_hmd_data = current_hmd_data;
  if (!isnan(current_hmd_data))
  {
    if (hmd.check_data(HMD_THRESHOLD))
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
  print_UI();
}

void loop()
{
  server.handleClient(); // Needed for the webserver to handle all clients
  webSocket.loop();

  unsigned long now = millis(); // read out the current "time" ("millis()" gives the time in ms since the Arduino started)
  if ((unsigned long)(now - previousMillis) > interval)
  { // check if "interval" ms has passed since last time the clients were updated

    String jsonString = "";                   // create a JSON string for sending data to the client
    StaticJsonDocument<200> doc;              // create a JSON container
    JsonObject object = doc.to<JsonObject>(); // create a JSON Object
    object["DHT_THRESHOLD"] = DHT_THRESHOLD;  // write data into the JSON object -> I used "rand1" and "rand2" here, but you can use anything else
    object["HMD_THRESHOLD"] = HMD_THRESHOLD;
    serializeJson(doc, jsonString);     // convert JSON object to string
    Serial.println(jsonString);         // print JSON string to console for debug purposes (you can comment this out)
    webSocket.broadcastTXT(jsonString); // send JSON string to clients

    previousMillis = now;

    current_temp_data = dht.read(); // READ DHT SENSOR DATA

    if (!isnan(current_temp_data)) // CHECK IF THE DATA IS VALID
    {
      display_temperature(current_temp_data);
      if (dht.check_data(DHT_THRESHOLD)) // IF THE DATA FROM THE SENSOR IS GREATER THAN THE THRESHOLD
      {
        if (temp_flag) // EXECUTE ONLY IF THE DATA IS DIFFERENT FROM THE PREVIOUS
        {
          temp_flag = false;
          window.open();
          fan.open();
          previous_temp_data = current_temp_data;
          print_UI();
        }
      }
      else
      {
        if (!temp_flag) // EXECUTE ONLY IF THE DATA IS DIFFERENET FROM THE PREVIOUS
        {
          temp_flag = true;
          window.close();
          fan.close();
          previous_temp_data = current_temp_data;
          print_UI();
        }
      }
    }
    else
    {
      Serial.println("Error reading temperature");
    }

    current_hmd_data = map(hmd.read(), 0, 360, 0, 100);

    if (!isnan(current_hmd_data))
    {
      display_moisture(current_hmd_data);
      if (hmd.check_data(HMD_THRESHOLD))
      {
        if (current_hmd_data != previous_hmd_data && hmd_flag)
        {
          hmd_flag = false;
          pump.close();

          previous_hmd_data = current_hmd_data;
          print_UI();
        }
      }
      else
      {
        if (current_hmd_data != previous_hmd_data && !hmd_flag)
        {
          hmd_flag = true;
          pump.open();
          previous_hmd_data = current_hmd_data;
          print_UI();
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
  {                         // switch on the type of information sent
  case WStype_DISCONNECTED: // if a client is disconnected, then type == WStype_DISCONNECTED
    Serial.println("Client " + String(num) + " disconnected");
    break;
  case WStype_CONNECTED: // if a client is connected, then type == WStype_CONNECTED
    Serial.println("Client " + String(num) + " connected");
    // optionally you can add code here what to do when connected
    break;
  case WStype_TEXT: // if a client has sent data, then type == WStype_TEXT
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