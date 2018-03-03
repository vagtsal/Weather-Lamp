// Wifi Smart Lamp Project

// Vagtsal, 14/7/2017

// ESP8266 code
// Creates an access point to submit network settings (ssid "esp8266", pass: "****", address page: 192.168.4.1)
// Creates a web server with a simple html interface to control smart lamp. Password protected (admin/admin default)
// Sends commands to arduino mini through serial connection to control led strip
// Downloads weather forecast from openweather.
// Stores current state in EEPROM

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <string.h>
    

#define NETWORK_SETTINGS_ADDR   0
#define ADMIN_SETTINGS_ADDR     100
#define WEATHER_SETTINGS_ADDR   150

#define BRIGHT_ADDR     200
#define RED_ADDR        201
#define GREEN_ADDR      202
#define BLUE_ADDR       203
#define EFFECT_ADDR     204
#define WEATHER_ADDR    205

#define CLEAR         1
#define CLOUDS        2
#define RAIN          3
#define THUNDERSTORM  4
#define SNOW          5

#define NO_EFFECT    0
#define FIRE         1
#define WEATHER      2

#define HOURS_TO_REQUEST_WEATHER 6

String openWeatherID = "****";
String googleMapsID = "****";

String msg;
int addr;
int login_counter = 20;                               // Number of login tries

const char* ssidAP = "esp8266";
const char* passwordAP = "****";

String cookieID = "*****";

String username;
String pass;

String latitude;
String longitude;
String forecast;

String ssid;
String password;
String ip1;
String ip2;
String ip3;
String ip4;
String gateway1;
String gateway2;
String gateway3;
String gateway4;
String subnet1;
String subnet2;
String subnet3;
String subnet4;

// button states
bool red, green, blue;
int effect; 
int weather;
int brightness;

unsigned long timer = 0;

ESP8266WebServer server(80);                          // SERVER PORT


void eeprom_write_string(String string){
    int string_length =string.length();
    char temp_char[string_length + 1];
    string.toCharArray(temp_char, string_length + 1);
    EEPROM.write(addr, string_length);
    for (int i = 0 ; i < string_length ; i++){
      addr++;
      EEPROM.write(addr, temp_char[i]);
    }
    addr++;
    EEPROM.commit();
}

String eeprom_read_string(){
  String string;
  int string_length = EEPROM.read(addr);
  for (int i=0;i<string_length;i++){
      addr++;
      char b = EEPROM.read(addr);
      string += b;
  }
  addr++;
  return string;
}



bool is_authentified(){                               // COOKIE CHECK
  if (server.hasHeader("Cookie")){
    String cookie = server.header("Cookie");
    //Serial.println(cookie);
    if (cookie.indexOf(cookieID) != -1) {
      return true;
    }
  }
  return false;
}



void handleRootSettings() {                                               // NETWORK SETTINGS PAGE
  if (server.hasArg("RESTART")){
    ESP.restart();
    return;
  }
  
  if (server.hasArg("SSID") && server.hasArg("PASSWORD") && server.hasArg("IP1") && server.hasArg("IP2") && server.hasArg("IP3") && server.hasArg("IP4") && server.hasArg("GATEWAY1") && server.hasArg("GATEWAY2") && server.hasArg("GATEWAY3") && server.hasArg("GATEWAY4") && server.hasArg("SUBNET1") && server.hasArg("SUBNET2") && server.hasArg("SUBNET3") && server.hasArg("SUBNET4")){
    ssid = server.arg("SSID");
    password = server.arg("PASSWORD");
    ip1 = server.arg("IP1");
    ip2 = server.arg("IP2");
    ip3 = server.arg("IP3");
    ip4 = server.arg("IP4");
    gateway1 = server.arg("GATEWAY1");
    gateway2 = server.arg("GATEWAY2");
    gateway3 = server.arg("GATEWAY3");
    gateway4 = server.arg("GATEWAY4");
    subnet1 = server.arg("SUBNET1");
    subnet2 = server.arg("SUBNET2"); 
    subnet3 = server.arg("SUBNET3"); 
    subnet4 = server.arg("SUBNET4"); 

    addr = NETWORK_SETTINGS_ADDR;
    eeprom_write_string(ssid);  
    eeprom_write_string(password);
    eeprom_write_string(ip1);
    eeprom_write_string(ip2);
    eeprom_write_string(ip3);
    eeprom_write_string(ip4);
    eeprom_write_string(gateway1);
    eeprom_write_string(gateway2);
    eeprom_write_string(gateway3);
    eeprom_write_string(gateway4);
    eeprom_write_string(subnet1);  
    eeprom_write_string(subnet2);
    eeprom_write_string(subnet3);  
    eeprom_write_string(subnet4);  
    
    msg = "Save Successfull! Please <a href='/?RESTART=YES'>restart</a> device.";
  }
  
  addr = NETWORK_SETTINGS_ADDR;
  ssid = eeprom_read_string(); 
  password = eeprom_read_string();
  ip1 = eeprom_read_string();
  ip2 = eeprom_read_string();
  ip3 = eeprom_read_string();
  ip4 = eeprom_read_string();
  gateway1 = eeprom_read_string();
  gateway2 = eeprom_read_string();
  gateway3 = eeprom_read_string();
  gateway4 = eeprom_read_string();
  subnet1 = eeprom_read_string();
  subnet2 = eeprom_read_string();
  subnet3 = eeprom_read_string();
  subnet4 = eeprom_read_string();
  
  String content = "<meta http-equiv='content-type' content='text/html;charset=utf-8' />";
  content += "<meta name='viewport' content='width=device-width; initial-scale=1.0; maximum-scale=1.0;'>";
  content += "<html><body style='text-align:center'><form action='' method='POST'> Network Settings:<br>";
  content += "SSID:<input type='text' name='SSID' value='" + ssid + "' maxlength='20'><br>";
  content += "Password:<input type='password' name='PASSWORD' value='" + password + "' maxlength='20'><br><br>";
  content += "IP Address:<input type='text' name='IP1' value='" + ip1 + "' maxlength='3' size='3'>.";
  content += "<input type='text' name='IP2' value='" + ip2 + "' maxlength='3' size='4'>.";
  content += "<input type='text' name='IP3' value='" + ip3 + "' maxlength='3' size='4'>.";
  content += "<input type='text' name='IP4' value='" + ip4 + "' maxlength='3' size='4'><br>";
  content += "Gateway:<input type='text' name='GATEWAY1' value='" + gateway1 + "' maxlength='3' size='4'>.";
  content += "<input type='text' name='GATEWAY2' value='" + gateway2 + "' maxlength='3' size='4'>.";
  content += "<input type='text' name='GATEWAY3' value='" + gateway3 + "' maxlength='3' size='4'>.";
  content += "<input type='text' name='GATEWAY4' value='" + gateway4 + "' maxlength='3' size='4'><br>";
  content += "Subnet:<input type='text' name='SUBNET1' value='" + subnet1 + "' maxlength='3' size='4'>.";
  content += "<input type='text' name='SUBNET2' value='" + subnet2 + "' maxlength='3' size='4'>.";
  content += "<input type='text' name='SUBNET3' value='" + subnet3 + "' maxlength='3' size='4'>.";
  content += "<input type='text' name='SUBNET4' value='" + subnet4 + "' maxlength='3' size='4'><br>";
  content += "<input type='submit' name='SUBMIT' value='Save'></form><br>";
  content += msg + "</body></html>";
  server.send(200, "text/html", content);

  msg = "";
}



void handleAdminSettings() {                                                                // ADMIN SETTINGS PAGE
  if (!is_authentified()){                                                                        // IF COOKIE IS WRONG, LOAD LOGIN PAGE
    String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  
  if (server.hasArg("USERNAME") && server.hasArg("PASS")) {
    username = server.arg("USERNAME");
    pass = server.arg("PASS");

    addr = ADMIN_SETTINGS_ADDR;
    eeprom_write_string(username);  
    eeprom_write_string(pass);
   
    msg = "Save Successfull!";
  }
  addr = ADMIN_SETTINGS_ADDR;
  username = eeprom_read_string();
  pass = eeprom_read_string();

  String content = "<meta http-equiv='content-type' content='text/html;charset=utf-8' />";
  content += "<meta name='viewport' content='width=device-width; initial-scale=1.0; maximum-scale=1.0;'>";
  content += "<html><body style='text-align:center'><form action='' method='POST'> Admin Settings:<br>";
  content += "User name:<input type='text' name='USERNAME' value='" + username + "' maxlength='20'><br>";
  content += "Password:<input type='password' name='PASS' value='" + pass + "' maxlength='20'><br><br>";
  content += "<input type='submit' name='SUBMIT' value='Save'></form><br>";
  content += msg + "<br><br>";
  content += "<a href='/'>Back</a></body></html>";
  server.send(200, "text/html", content);

  msg = "";
}

void handleWeatherSettings() {                                                                // WEATHER SETTINGS PAGE
  if (!is_authentified()){                                                                        // IF COOKIE IS WRONG, LOAD LOGIN PAGE
    String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  
  if (server.hasArg("Latitude") && server.hasArg("Longitude") && server.hasArg("Forecast")) {
    latitude = server.arg("Latitude");
    longitude = server.arg("Longitude");
    forecast = server.arg("Forecast");

    addr = WEATHER_SETTINGS_ADDR;
    eeprom_write_string(latitude);  
    eeprom_write_string(longitude);
    eeprom_write_string(forecast);
    
    msg = "Save Successfull!";

    request_weather();
  }

  String content = "<meta http-equiv='content-type' content='text/html;charset=utf-8' />";
  content += "<meta name='viewport' content='width=device-width; initial-scale=1.0; maximum-scale=1.0;'>";
  content += "<html><body style='text-align:center'><form action='' method='POST'> Weather Settings <br>";
  content += "Latitude:    <input type='text' id='Lat' name='Latitude' value='" + latitude + "' maxlength='11'><br>";
  content += "Longitude:<input type='text' id='Lng' name='Longitude' value='" + longitude + "' maxlength='11'><br>";
  
  content += "Forecast:  <select name=\"Forecast\">";
  content += "<option " + String((forecast == "0")?"selected ":"") + "value=\"0\">Now</option>";
  content += "<option " + String((forecast == "6")?"selected ":"") + "value=\"6\">+6  hours</option>";
  content += "<option " + String((forecast == "12")?"selected ":"") + "value=\"12\">+12 hours</option>";
  content += "<option " + String((forecast == "18")?"selected ":"") + "value=\"18\">+18 hours</option>";
  content += "<option " + String((forecast == "24")?"selected ":"") + "value=\"24\">+24 hours</option>";
  content += "<option " + String((forecast == "48")?"selected ":"") + "value=\"48\">+2  days</option>";
  content += "<option " + String((forecast == "72")?"selected ":"") + "value=\"72\">+3  days</option>";
  content += "<option " + String((forecast == "96")?"selected ":"") + "value=\"96\">+4  days</option>";
  content += "</select>";
  
  content += "<br><br><input type='submit' name='SUBMIT' value='Save'></form>";
  content += "<a href='/'>Back</a><br>";
  content += msg + "<br><br>";
  
  content += "<div id=\"googleMap\" style=\"width:100%;height:60%;text-align:center\"></div>";
  content += "<script> function myMap() {";
  content += "var mapProp= {center:new google.maps.LatLng(" + latitude + "," + longitude + "),zoom:11,};";
  content += "var map=new google.maps.Map(document.getElementById(\"googleMap\"),mapProp);";
  content += "google.maps.event.addListener(map, \"click\", function(event) {var lat = event.latLng.lat();var lng = event.latLng.lng();";
  content += "document.getElementById(\"Lat\").value=lat; document.getElementById(\"Lng\").value=lng;});}</script>";
  content += "<script src=\"https://maps.googleapis.com/maps/api/js?key=" + googleMapsID + "&callback=myMap\"></script>";
  
  content += "</body></html>";
  server.send(200, "text/html", content);

  msg = "";
  
}


void handleLogin(){                                   // LOGIN PAGE
  if (server.hasArg("DISCONNECT")){
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: " + cookieID + "; Expires=Thu, 01-Jan-1970 00:00:00 GMT\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == username && server.arg("PASSWORD") == pass){                                              // USER AND PASS
      String header = "HTTP/1.1 301 OK\r\nSet-Cookie: " + cookieID + "; Expires=Wed, 09-Jun-2021 10:18:14 GMT\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      return;
    }
    
    login_counter--;
    if (login_counter == 0) {
      while (true) {
        delay(1000);
      }
    }
    msg = String(login_counter) + " tries remain.";
  }
  
  String content = "<meta http-equiv='content-type' content='text/html;charset=utf-8' />";
  content += "<meta name='viewport' content='width=device-width; initial-scale=1.0; maximum-scale=1.0;'>";
  content += "<html><body style='text-align:center'><form action='/login' method='POST'> Log in:<br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "</body></html>";
  server.send(200, "text/html", content);
}




void handleRoot(){                                                                                // MAIN PAGE  
  if (!is_authentified()){                                                                        // IF COOKIE IS WRONG, LOAD LOGIN PAGE
    String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }

  if (server.hasArg("Color")){
    if (server.arg("Color") == "Red"){
      if (red == 0){
        Serial.print("r\n");
        red = 1;
      }
      else{
        Serial.print("nr\n");
        red = 0;
      }
      EEPROM.write(RED_ADDR, red);
      EEPROM.commit();
    }
    else if (server.arg("Color") == "Green"){
      if (green == 0){
        Serial.print("g\n");
        green = 1;
      }
      else{
        Serial.print("ng\n");
        green = 0;
      }
      EEPROM.write(GREEN_ADDR, green);
      EEPROM.commit();
    }
    else if (server.arg("Color") == "Blue"){
      if (blue == 0){
        Serial.print("b\n");
        blue = 1;
      }
      else{
        Serial.print("nb\n");
        blue = 0;
      }
      EEPROM.write(BLUE_ADDR, blue);
      EEPROM.commit();
    }
  }
  if (server.hasArg("Effect")){
    if (server.arg("Effect") == "Fire"){
      if (effect != FIRE){
        Serial.print("f\n");
        effect = FIRE;
      }
      else{
        Serial.print("nf\n");
        effect = NO_EFFECT;
      }
      EEPROM.write(EFFECT_ADDR, effect);
      EEPROM.commit();
    }
    if (server.arg("Effect") == "Weather"){
      if (effect != WEATHER){
        Serial.print("w\n");
        effect = WEATHER;
      }
      else{
        Serial.print("nw\n");
        effect = NO_EFFECT;
      }
      EEPROM.write(EFFECT_ADDR, effect);
      EEPROM.commit();
    }
  }
  if (server.hasArg("Brightness")){
    if (server.arg("Brightness") == "+"){
      if (brightness <= 75){brightness += 25; Serial.print("bu\n");} else {brightness = 100;}
    }
    else if (server.arg("Brightness") == "-"){
      if (brightness >= 25){brightness -= 25; Serial.print("bd\n");} else {brightness = 0;}
    }
    EEPROM.write(BRIGHT_ADDR, brightness);
    EEPROM.commit();
  }

  
  String content = "<meta http-equiv='content-type' content='text/html;charset=utf-8' />";
  content += "<meta name='viewport' content='width=device-width; initial-scale=1.0; maximum-scale=1.0;'>";
  content += "<html><body style='text-align:center;'><form method='POST' action=''>";
  content += "<span style='font-size:30px'>Colors</span><br>";
  content += "<input type='Submit' name='Color' value='Red' style='height:50px; width:120px; margin:10px; font-size:0px; background-color:red; color:white; opacity:";
  content += (red==0)?"0.5;'":"1.0;'";
  content += (effect!=NO_EFFECT)?" disabled>":">";
  content += "<input type='Submit' name='Color' value='Green' style='height:50px; width:120px; margin:10px; font-size:0px; background-color:green; color:white; opacity:";
  content += (green==0)?"0.5;'":"1.0;'";
  content += (effect!=NO_EFFECT)?" disabled>":">";
  content += "<br>";
  content += "<input type='Submit' name='Color' value='Blue' style='height:50px; width:120px; margin:10px; font-size:0px; background-color:blue; color:white; opacity:";
  content += (blue==0)?"0.5;'":"1.0;'";
  content += (effect!=NO_EFFECT)?" disabled>":">";
  content += "<br><br>";
  content += "<span style='font-size:30px'>Effects</span><br>";
  content += "<input type='Submit' name='Effect' value='Fire' style=\"height:50px; width:120px; margin:10px; font-size:0px; background-image:url('https://s3.amazonaws.com/spoonflower/public/design_thumbnails/0167/7306/rrFirePattern-01_shop_preview.png'); color:white; opacity:";
  content += (effect==FIRE)?"1.0;\">":"0.5;\">";
  content += "<input type='Submit' name='Effect' value='Weather' style=\"height:50px; width:120px; margin:10px; font-size:0px; background-image:url('http://images.all-free-download.com/images/graphiclarge/transparent_water_drops_design_background_vector_542481.jpg'); color:white; opacity:";
  content += (effect==WEATHER)?"1.0;\">":"0.5;\">";
  content += "<br><br>";
  content += "<span style='font-size:30px'>Brightness<br>";
  content += "<input type='Submit' name='Brightness' value='+' style='height:50px; width:120px; margin:10px; font-size:30px;'><br>";
  content += brightness;
  content += "%<br></span><input type='Submit' name='Brightness' value='-' style='height:50px; width:120px; margin:10px; font-size:30px;'>";
  content += "</form><br>";
  content += "<a href='/admin_settings'>Admin Settings</a><br>";
  content += "<a href='/weather_settings'>Weather Settings</a><br><br>";
  content += "<a href='/login?DISCONNECT=YES'>Disconnect</a></body></html>";
  server.send(200, "text/html", content);
}


void request_weather(){
  int previousWeather = weather;
  int noOfBrackets;
  
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/forecast?lat=" + latitude + "&lon=" + longitude + "&APPID=" + openWeatherID;
  http.begin(url);
  http.GET();

  if (forecast == "0"){
    noOfBrackets = 1;
  }
  else if (forecast == "6"){
    noOfBrackets = 3;
  }
  else if (forecast == "12"){
    noOfBrackets = 5;
  }
  else if (forecast == "18"){
    noOfBrackets = 7;
  }
  else if (forecast == "24"){
    noOfBrackets = 9;
  }
  else if (forecast == "48"){
    noOfBrackets = 17;
  }
  else if (forecast == "72"){
    noOfBrackets = 25;
  }
  else if (forecast == "96"){
    noOfBrackets = 33;
  }
  
  // parse json (forecast, 24h later)
  String payload = http.getString();
  //Serial.println(payload);
  int pos = payload.indexOf('[');
  if (pos == -1){
    return;
  }
  for (int i=0; i<noOfBrackets; i++){
    pos = payload.indexOf('[', pos + 1 );
  }
  for (int i=0; i<5; i++){
    pos = payload.indexOf("\"", pos + 1 );
  }
  String weatherString =  payload.substring(pos+1, payload.indexOf("\"", pos+2));
  //Serial.println(weatherString);

  if (weatherString == "Clear"){
    weather = CLEAR;
    Serial.print("ws\n");
  }
  else if (weatherString == "Clouds"){
    weather = CLOUDS;
    Serial.print("wc\n");
  }
  else if (weatherString == "Rain" || weatherString == "Drizzle"){
    weather = RAIN;
    Serial.print("wr\n");
  }
  else if (weatherString == "Thunderstorm"){
    weather = THUNDERSTORM;
    Serial.print("wt\n");
  }
  else if (weatherString == "Snow"){
    weather = SNOW;
    Serial.print("wx\n");
  }

  if (previousWeather != weather){
    EEPROM.write(WEATHER_ADDR, weather);
    EEPROM.commit();
  }
  
  http.end();
}



void handleNotFound(){                              // NOT FOUND PAGE
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}




void setup(){
  delay(2000);

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  
  Serial.begin(115200);
  EEPROM.begin(512);

  addr = NETWORK_SETTINGS_ADDR; 
 
  /*eeprom_write_string("****");
  eeprom_write_string("****");
  eeprom_write_string("191");
  eeprom_write_string("168");
  eeprom_write_string("1");
  eeprom_write_string("173");
  eeprom_write_string("191");
  eeprom_write_string("168");
  eeprom_write_string("1");
  eeprom_write_string("1");
  eeprom_write_string("255");
  eeprom_write_string("255");
  eeprom_write_string("255");
  eeprom_write_string("0");*/

  
  ssid = eeprom_read_string(); 
  password = eeprom_read_string();
  ip1 = eeprom_read_string();
  ip2 = eeprom_read_string();
  ip3 = eeprom_read_string();
  ip4 = eeprom_read_string();
  gateway1 = eeprom_read_string();
  gateway2 = eeprom_read_string();
  gateway3 = eeprom_read_string();
  gateway4 = eeprom_read_string();
  subnet1 = eeprom_read_string();
  subnet2 = eeprom_read_string();
  subnet3 = eeprom_read_string();
  subnet4 = eeprom_read_string();
  
  // when eeprom is new, set default values
     
    /*addr = ADMIN_SETTINGS_ADDR;
    eeprom_write_string("admin");
    eeprom_write_string("admin");

    addr = WEATHER_SETTINGS_ADDR;
    eeprom_write_string("50");
    eeprom_write_string("50");
    eeprom_write_string("24");
   
    EEPROM.write(BRIGHT_ADDR, 50);
    EEPROM.write(RED_ADDR, 1);
    EEPROM.write(GREEN_ADDR, 1);
    EEPROM.write(BLUE_ADDR, 1);
    EEPROM.write(EFFECT_ADDR, NO_EFFECT);
    EEPROM.write(WEATHER_ADDR, CLEAR);
    EEPROM.commit();*/
  
  
  addr = ADMIN_SETTINGS_ADDR;
  username = eeprom_read_string();
  pass = eeprom_read_string();

  addr = WEATHER_SETTINGS_ADDR;
  latitude = eeprom_read_string();
  longitude = eeprom_read_string();
  forecast = eeprom_read_string();

  brightness = EEPROM.read(BRIGHT_ADDR);
  red = EEPROM.read(RED_ADDR);
  green = EEPROM.read(GREEN_ADDR);
  blue = EEPROM.read(BLUE_ADDR);
  effect = EEPROM.read(EFFECT_ADDR);
  weather = EEPROM.read(WEATHER_ADDR);
  

  IPAddress ip(atoi(ip1.c_str()),atoi(ip2.c_str()),atoi(ip3.c_str()),atoi(ip4.c_str()));
  IPAddress gateway(atoi(gateway1.c_str()),atoi(gateway2.c_str()),atoi(gateway3.c_str()),atoi(gateway4.c_str()));
  IPAddress subnet(atoi(subnet1.c_str()),atoi(subnet2.c_str()),atoi(subnet3.c_str()),atoi(subnet4.c_str()));
  
  WiFi.begin(ssid.c_str(), password.c_str());
  WiFi.config(ip,gateway,subnet);      // STATIC IP

  WiFi.mode(WIFI_STA);

  int connection_counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    connection_counter++;
    
    if (connection_counter > 15){                                                       // ENTER NETWORK SETTINGS MODE (CONNECT TO "ESP8266" WLAN AND ENTER "192.168.4.1" IN A BROWSER)
      Serial.println("Settings Mode");
      WiFi.mode(WIFI_AP);
      delay(1000);
      WiFi.softAP(ssidAP, passwordAP);
      IPAddress myIP = WiFi.softAPIP();
    
      server.on("/",handleRootSettings);
      server.begin();
      return;
    }
  }
  
  Serial.println("Connected");

  server.on("/", handleRoot);                                                                   // HANDLING
  server.on("/login", handleLogin);
  server.on("/admin_settings", handleAdminSettings);
  server.on("/weather_settings", handleWeatherSettings);
  server.onNotFound(handleNotFound);

  const char* headerkeys[] = {"Cookie"};                                                        // USED HEADERKEYS
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  server.collectHeaders(headerkeys, headerkeyssize);

  server.begin();

  request_weather();
}



void loop(){
  unsigned long now = millis();
  if (now > HOURS_TO_REQUEST_WEATHER*3600000 && ((now - timer) > HOURS_TO_REQUEST_WEATHER*3600000 || (now - timer) < 0)){
    timer = millis();
    request_weather();
  }
  server.handleClient();
}

