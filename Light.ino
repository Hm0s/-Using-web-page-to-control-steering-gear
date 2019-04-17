/*
 * DATE：201903
 * ID：NIT_Hm0s
 * 用ESP81266内置网页控制舵机转动
 * 舵机接D4引脚
*/
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "填入你的WIFI名称"
#define STAPSK  "填入你的WIFI密码"
#endif

Servo myservo;
const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

String html = "<!DOCTYPE html><html lang=\"en\"><head><meta charest=\"UTF-8\"><title>Document</title></head><body><a href=\"./pin?light=on\"><input type=\"button\" value=\"&#x5F00;&#x706F;\"></a><a href=\"./pin?light=off\"><input type=\"button\" value=\"&#x5173;&#x706F;\"></a></body></html>";

void pin(){
  if(server.arg("light")=="on"){
     digitalWrite(D4, LOW);
     server.send(200, "text/html", html);
     myservo.write(45);// tell servo to go to position
     delay(1000); 
     myservo.write(90);  
    }else if(server.arg("light")=="off"){
      digitalWrite(D4, HIGH);
      server.send(200, "text/html", html);      
      myservo.write(135);// tell servo to go to position
      delay(1000); 
      myservo.write(90);       
      }
  }

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html", html);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(D4, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  myservo.attach(D4);


  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/pin", HTTP_GET, pin);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
