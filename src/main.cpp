#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>
#include <GyverTimer.h>
#include <MyJSONparser.h>
#include <stripEffects.h>
#include <mqtt.h>

#define LED_PIN 4
#define NUM_LEDS 32
#define DATA_PIN 2
#define FPS 100
#define COLOR_ORDER RGB
#define LED_TYPE WS2811

CRGB leds[NUM_LEDS];
Segment Segment1(0, NUM_LEDS);

MDNSResponder mDNS;
ESP8266WebServer server(80);
GTimer autoSaveSettingsTimer(MS, 3000);
GTimer effectTimer(MS, 50);

WiFiClient wclient;
PubSubClient client(wclient);

String domain_name = "myled";
String ssidAP = "";
String ssid = "";
String pass = "";

String color = "";
String specific_color = "";
String effect = "";
String brightness = "";
String ledstate = "";
String newSettings = "";

const char *mqtt_server = "";
const int mqtt_port = 1883;
const char *mqtt_user = "";
const char *mqtt_pass = "";

void saveSettings(){
  File settings = LittleFS.open("/settings.json", "w");
  newSettings = MyJSONparser_jsonToStr();
  settings.print(newSettings);
  settings.close();
  Serial.println(newSettings);
  autoSaveSettingsTimer.stop();
}

void setSavedColor()
{
  unsigned int icolor = 0;
  for(unsigned int i = 2; i < color.length() - 1; i++)
  {
    if(color[i] < 97)
      icolor += color[i] - 48 << ((color.length() - i - 2) * 4);
    else
      icolor += color[i] - 87 << ((color.length() - i - 2) * 4);
  }
  for(int i = 0; i < NUM_LEDS; i++)
    leds[i] = icolor;

  Segment1.setColor(color.substring(1, 7));
}

//----------Server handlers----------

String getContentType(String filename){                                
  if (filename.endsWith(".html")) return "text/html";                   
  else if (filename.endsWith(".css")) return "text/css";                
  else if (filename.endsWith(".js")) return "application/javascript";   
  else if (filename.endsWith(".png")) return "image/png";               
  else if (filename.endsWith(".jpg")) return "image/jpeg";              
  else if (filename.endsWith(".gif")) return "image/gif";               
  else if (filename.endsWith(".ico")) return "image/x-icon";            
  return "text/plain";                                                  
}

bool handleFileRead(String path){                                       
  if(path.endsWith("/")) path += "index.html";                          
  String contentType = getContentType(path);                            
  if(LittleFS.exists(path)){                                              
    File file = LittleFS.open(path, "r");                                 
    server.streamFile(file, contentType);                   
    file.close();                                                       
    return true;                                                        
  }
  return false;                                                         
}

void handleOnNotFound(){
  if(!handleFileRead(server.uri()))                                       
      server.send(404, "text/plain", "Not Found");
}

void handleToggleLed(){
  ledstate = server.arg("state");
  if(ledstate == "true")
    digitalWrite(LED_PIN, 0);
  else digitalWrite(LED_PIN, 1);
  MyJSONparser_set_value("led_state", ledstate);
  server.send(200, "text/plain", ledstate);
  autoSaveSettingsTimer.start();
}

void handleSetBrightness(){
  brightness = server.arg("brightness");
  FastLED.setBrightness(brightness.toInt());
  MyJSONparser_set_value("brightness", brightness);  
  server.send(200, "text/plain", brightness);
  autoSaveSettingsTimer.start();
}

void handleSetColor(){
  color = server.arg("color");
  //Segment1.setColor("#" + color);
  color = "\"#" + color + "\"";
  setSavedColor();  

  MyJSONparser_set_value("color", color);
  server.send(200, "text/plain", color);
  autoSaveSettingsTimer.start();
}

void handleSetSpecificColor(){
  specific_color = server.arg("specific_color");
  specific_color = "\"" + specific_color + "\"";
  if(specific_color == "\"none\"")
    setSavedColor();

  MyWS2812B_SetSpecificColor(leds, NUM_LEDS, specific_color);
  MyJSONparser_set_value("specific_color", specific_color);
  server.send(200, "text/plain", specific_color);
  autoSaveSettingsTimer.start();
}

void handleSetEffect(){
  effect = server.arg("effect");
  effect = "\"" + effect + "\"";
  if((effect == "\"none\""))
  {
    setSavedColor();
    MyWS2812B_SetSpecificColor(leds, NUM_LEDS, specific_color);
    FastLED.setBrightness(brightness.toInt());
  }
  MyJSONparser_set_value("effect", effect);
  server.send(200, "text/plain", effect);
  autoSaveSettingsTimer.start();
}


void handleGetSettings(){
  File settings = LittleFS.open("/settings.json", "r");
  server.streamFile(settings, "application/json");
  settings.close();
}

//-----------------------------------------
void handleSaveWifiSettings(){
  ssid = server.arg("ssid");
  ssid = "\"" + ssid + "\"";
  pass = server.arg("pass");
  pass = "\"" + pass + "\"";
  MyJSONparser_set_value("ssid", ssid);
  MyJSONparser_set_value("pass", pass);
  saveSettings();
  server.send(200, "text/plain", ssid + pass);
  ESP.restart();
}

void setupSavedSettings(){
  File settings = LittleFS.open("/settings.json", "r");
  while(settings.available())
    newSettings += (char)settings.read();
  settings.close();
  MyJSONparser_start(newSettings);
  color = MyJSONparser_get_value("color");
  brightness = MyJSONparser_get_value("brightness");
  ledstate = MyJSONparser_get_value("led_state");
  specific_color = MyJSONparser_get_value("specific_color");
  effect = MyJSONparser_get_value("effect");

  ssid = MyJSONparser_get_value("ssid");
  ssid.remove(0, 1);
  ssid.remove(ssid.length()-1, 1);

  pass = MyJSONparser_get_value("pass");
  pass.remove(0, 1);
  pass.remove(pass.length()-1, 1);

  ssidAP = MyJSONparser_get_value("ssidAP");
  ssidAP.remove(0, 1);
  ssidAP.remove(ssidAP.length()-1, 1);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness.toInt());
  
  if(ledstate == "true")
    digitalWrite(LED_PIN, 0);
  else
    digitalWrite(LED_PIN, 1);
    
  setSavedColor();
  MyWS2812B_SetSpecificColor(leds, NUM_LEDS, specific_color);
}

void setupWifi(){
  int enable_softAP = 0;
  int attempts = 5;
  if(!enable_softAP)
  {
    WiFi.begin(ssid, pass);
    while((WiFi.status() != WL_CONNECTED) && (attempts >= 0)) {
      delay(2000);
      Serial.print(".");
      attempts--;
    };
    if(WiFi.status() == WL_CONNECTED){
      Serial.println();
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      if (mDNS.begin(domain_name, WiFi.localIP()))
        Serial.println("MDNS responder started");
    }
    else enable_softAP = 1;
  }

  if(enable_softAP)
  {
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    //WiFi.softAPConfig(IPAddress(9,9,9,9), IPAddress(9,9,9,9), IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssidAP); 
    Serial.println();
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
    if (mDNS.begin(domain_name, WiFi.softAPIP()))
      Serial.println("MDNS responder started");
  }
}

void mqttConnection() {
  if (WiFi.status() == WL_CONNECTED) { // if wi-fi is connected
    if (!client.connected()) { // if haven't mqtt connection
      Serial.println("MQTT - none");
      String clientId = "ESP8266Client-";
      clientId += String(random(0xffffffff), HEX);
      if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
        Serial.println("MQTT - ok");
      } else {
        Serial.println("MQTT - error");   // if not connected
      }
    }

    if (client.connected()) { // if connected
      client.loop();
    }
  }
}

void setup() {
  autoSaveSettingsTimer.stop();
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("");
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  //Get saved settings
  LittleFS.begin();
  setupSavedSettings();
  Serial.println(newSettings);

  //WiFi connect
  setupWifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  //Server paths
  server.onNotFound(handleOnNotFound);
  server.on("/save_wifi_settings", handleSaveWifiSettings);
  server.on("/toggle_led", handleToggleLed);
  server.on("/set_brightness", handleSetBrightness);
  server.on("/set_color", handleSetColor);
  server.on("/set_specific_color", handleSetSpecificColor);
  server.on("/set_effect", handleSetEffect);
  server.on("/get_settings", handleGetSettings);

  server.begin();

  mDNS.addService("http", "tcp", 80);

  
  Segment1.setEffect("Fill");
  Segment1.setColor("#ff00ff");
}
 
void loop() {
  server.handleClient();
  mDNS.update();

  //if(effectTimer.isReady())
  //  MyWS2812B_SetEffect(leds, NUM_LEDS, effect);
  Segment1.drawEffect(leds);
  FastLED.show();
  FastLED.delay(1000 / FPS);

  //mqttConnection();

  if (autoSaveSettingsTimer.isReady()){
    saveSettings();
  }
}