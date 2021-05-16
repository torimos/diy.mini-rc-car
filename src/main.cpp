#include <Arduino.h>
#include "rc_car.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>

byte incomingByte = 0;
byte incomingByte1 = 0;

bool forwardsPressed = false;
bool backwardsPressed = false;
bool rightPressed = false;
bool leftPressed = false;
bool speedChanged = false;

const int FORWARDS_PRESSED = 1;
const int FORWARDS_RELEASED = 2;
const int BACKWARDS_PRESSED = 3;
const int BACKWARDS_RELEASED = 4;
const int RIGHT_PRESSED = 5;
const int RIGHT_RELEASED = 6;
const int LEFT_PRESSED = 7;
const int LEFT_RELEASED = 8;

byte packetBuffer[512];
int speed = 255;
int max_steering = 45;
const int offset_steer = -5;

WiFiMulti WiFiMulti;
WiFiUDP udp;

void setup() {
  Serial.begin(115200);

  rcc_init();
  rcc_set_steering(offset_steer);
  rcc_set_speed(0,0);

  WiFiMulti.addAP("zza-net", "@@@@@@@@@@@@@@");
  
  Serial.println();
  Serial.println();
  Serial.print("Waiting for WiFi... ");

  while(WiFiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  uint16_t port = 8888;
  udp.begin(port);
}

void detectKeyPresses() {
  if (incomingByte == FORWARDS_PRESSED) {
    forwardsPressed = true;
    speedChanged = true;
  }
  else if (incomingByte == BACKWARDS_PRESSED) {
    backwardsPressed = true;
    speedChanged = true;
  }

  if (incomingByte == FORWARDS_RELEASED) {
    forwardsPressed = false;
    speedChanged = true;
  }
  else if (incomingByte == BACKWARDS_RELEASED) {
    backwardsPressed = false;
    speedChanged = true;
  }

  if (incomingByte == RIGHT_PRESSED) {
    rightPressed = true;
  }
  else if (incomingByte == LEFT_PRESSED) {
    leftPressed = true;
  }

  if (incomingByte == RIGHT_RELEASED) {
    rightPressed = false;
  }
  else if (incomingByte == LEFT_RELEASED) {
    leftPressed = false;
  }
  else if (incomingByte == 0xF0)
  {
    speed = incomingByte1;
  }
  else if (incomingByte == 0xF1)
  {
    max_steering = incomingByte1;
  }
}

void handlePinOutputs() {
  if (forwardsPressed == true) {
    rcc_set_speed(speed,speed);
  }
  else if (backwardsPressed == true) {
    rcc_set_speed(-speed,-speed);
  }
  else {
    rcc_set_speed(0,0);
  }

  if (rightPressed == true) {
    rcc_set_steering(max_steering+offset_steer);
  }
  else if (leftPressed == true) {
    rcc_set_steering(-max_steering-offset_steer);
  }
  else {
    rcc_set_steering(0+offset_steer);
  }
  speedChanged = false;
}

void loop() {
  int noBytes = udp.parsePacket();
  String received_command = "";

  if ( noBytes ) {
    Serial.print(millis() / 1000);
    Serial.print(":Packet of ");
    Serial.print(noBytes);
    Serial.print(" received from ");
    Serial.print(udp.remoteIP());
    Serial.print(":");
    Serial.println(udp.remotePort());

    udp.read(packetBuffer,noBytes);
    Serial.println();

    Serial.println(packetBuffer[0]);
    incomingByte = packetBuffer[0];
    if (noBytes == 4)
    {
      incomingByte1 = (packetBuffer[1] - 0x30)*100 + (packetBuffer[2] - 0x30)*10 + (packetBuffer[3] - 0x30);
      Serial.println(incomingByte1);
    }
    else
    {
      incomingByte1 = 0;
    }
    Serial.println();
  }
  detectKeyPresses();
  handlePinOutputs();
}