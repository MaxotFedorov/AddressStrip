#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>
#include <GyverTimer.h>
#include <MyJSONparser.h>
#include <MyEffectsForWS2812B.h>

#define LED_PIN 2
#define NUM_LEDS 60
#define DATA_PIN 4
#define FPS 100

CRGB leds[NUM_LEDS];
MDNSResponder mDNS;
ESP8266WebServer server(80);
GTimer autosaveSettingsTimer(MS, 3000);
GTimer effectTimer(MS, 50);

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

void saveSettings(){
  File settings = LittleFS.open("/settings.json", "w");
  newSettings = MyJSONparser_jsonToStr();
  settings.print(newSettings);
  settings.close();
  Serial.println(newSettings);
  autosaveSettingsTimer.stop();
}

void set_saved_color()
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
}

//Server handlers

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


void handle_OnNotFound(){
  if(!handleFileRead(server.uri()))                                       
      server.send(404, "text/plain", "Not Found");
}

void handle_ToggleLed(){
  ledstate = server.arg("state");
  if(ledstate == "true")
    digitalWrite(LED_PIN, 0);
  else digitalWrite(LED_PIN, 1);
  MyJSONparser_set_value("led_state", ledstate);
  server.send(200, "text/plain", ledstate);
  autosaveSettingsTimer.start();
}

void handle_SetBrightness(){
  brightness = server.arg("brightness");
  FastLED.setBrightness(brightness.toInt());
  MyJSONparser_set_value("brightness", brightness);  
  server.send(200, "text/plain", brightness);
  autosaveSettingsTimer.start();
}

void handle_SetColor(){
  color = server.arg("color");
  color = "\"#" + color + "\"";
  set_saved_color();

  MyJSONparser_set_value("color", color);
  server.send(200, "text/plain", color);
  autosaveSettingsTimer.start();
}

void handle_SetSpecificColor(){
  specific_color = server.arg("specific_color");
  specific_color = "\"" + specific_color + "\"";
  if(specific_color == "\"none\"")
    set_saved_color();

  MyWS2812B_SetSpecificColor(leds, NUM_LEDS, specific_color);
  MyJSONparser_set_value("specific_color", specific_color);
  server.send(200, "text/plain", specific_color);
  autosaveSettingsTimer.start();
}

void handle_SetEffect(){
  effect = server.arg("effect");
  effect = "\"" + effect + "\"";
  if((effect == "\"none\""))
  {
    set_saved_color();
    MyWS2812B_SetSpecificColor(leds, NUM_LEDS, specific_color);
    FastLED.setBrightness(brightness.toInt());
  }
  MyJSONparser_set_value("effect", effect);
  server.send(200, "text/plain", effect);
  autosaveSettingsTimer.start();
}


void handle_GetSettings(){
  File settings = LittleFS.open("/settings.json", "r");
  server.streamFile(settings, "application/json");
  settings.close();
}

//-----------------------------------------
void handle_SaveWifiSettings(){
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

void setup_saved_settings(){
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
    
  set_saved_color();
  MyWS2812B_SetSpecificColor(leds, NUM_LEDS, specific_color);
}

void setup_wifi(){
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


void setup() {
  autosaveSettingsTimer.stop();
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("");

  //Get saved settings
  LittleFS.begin();
  setup_saved_settings();
  Serial.println(newSettings);

  //WiFi connect
  setup_wifi();

  //Server paths
  server.onNotFound(handle_OnNotFound);
  server.on("/save_wifi_settings", handle_SaveWifiSettings);
  server.on("/toggle_led", handle_ToggleLed);
  server.on("/set_brightness", handle_SetBrightness);
  server.on("/set_color", handle_SetColor);
  server.on("/set_specific_color", handle_SetSpecificColor);
  server.on("/set_effect", handle_SetEffect);
  server.on("/get_settings", handle_GetSettings);

  server.begin();


  mDNS.addService("http", "tcp", 80);
}
 
void loop() {
  server.handleClient();
  mDNS.update();

  if(effectTimer.isReady())
    MyWS2812B_SetEffect(leds, NUM_LEDS, effect);
  FastLED.show();
  FastLED.delay(1000 / FPS);

  if (autosaveSettingsTimer.isReady()){
    saveSettings();
  }
}