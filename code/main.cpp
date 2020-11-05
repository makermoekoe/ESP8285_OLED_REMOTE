#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define OLED_RST 10 //U8X8_PIN_NONE
U8G2_SSD1306_64X32_1F_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ OLED_RST);


const char* ssid = "XX";
const char* password = "XX";

WiFiServer server(1023);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const int button_on = 13;
const int button_left = 12;
const int button_right = 5;
const int latch = 4;

int counter = 0;


int X(int textgroesse, int textwidth, int n) {
  //gibt die X koordinate aus, damit text mit n zeichen mittig ist
  return (0.5 * (64 - textgroesse * (textwidth * n - 1)));
}//end int X

int Y(int textgroesse, int textheight, float f) {
  //gibt die Y koordinate aus, damit text mittig ist
  return (f * 48 + (textgroesse * textheight/2));
}//end int Y

void setup() {
  pinMode(latch, OUTPUT);
  digitalWrite(latch, HIGH);

  Wire.begin(2, 14);
  Serial.begin(115200);
  Serial.println("start");

  pinMode(OLED_RST, OUTPUT);
  pinMode(button_on, INPUT);
  pinMode(button_left, INPUT);
  pinMode(button_right, INPUT);

  u8g2.begin();
  u8g2.clearBuffer();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_inb16_mf);
  u8g2.setCursor(X(1,14,3), Y(1,16,0.5));
  u8g2.print("HEY");
  u8g2.sendBuffer();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int stat = 0;
  while ((WiFi.status() != WL_CONNECTED)) {
    
    /*for(int i=0; i<3; i++){
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_inb16_mf);
      u8g2.setCursor(X(1,14,1), Y(1,16,0.5));
      if(stat == 0) u8g2.print("H");
      else if(stat == 1) u8g2.print("E");
      else if(stat == 2) u8g2.print("Y");
      stat++;
      if(stat > 2) stat = 0;
      u8g2.sendBuffer();
      delay(300);
    }*/
    delay(100);
  }

  timeClient.begin();
  timeClient.setTimeOffset(3600);
}

unsigned long t_time = millis();

void loop() {
  //Serial.println(("Button 1: " + String(digitalRead(button_left)) + ", Button 2: " + String(digitalRead(button_on)) + ", Button 3: " + String(digitalRead(button_right))));


  timeClient.update();
  if(millis() >= t_time + 100){
    String ntp = timeClient.getFormattedTime();
    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFont(u8g2_font_6x12_tn );  // choose a suitable font
    u8g2.setCursor(X(1,6,8), Y(1,12,0.3));
    u8g2.print(ntp);

    String c = String(counter);
    int len = c.length();
    u8g2.setCursor(X(1,6,len), Y(1,12,0.7));
    u8g2.print(c);
    u8g2.sendBuffer();
    t_time = millis();
  }
  

  if(digitalRead(button_right) == 1){
    if(counter < 10) counter++;
    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFont(u8g2_font_inb16_mf );  // choose a suitable font
    if(counter < 10) u8g2.setCursor(X(1,14,1), Y(1,16,0.5));
    else u8g2.setCursor(X(1,14,2), Y(1,16,0.5));
    u8g2.print(counter);
    u8g2.sendBuffer();
    while(digitalRead(button_right) == 1) delay(20);
  }
  if(digitalRead(button_left) == 1){
    if(counter > 0) counter--;
    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFont(u8g2_font_inb16_mf );  // choose a suitable font
    if(counter < 10) u8g2.setCursor(X(1,14,1), Y(1,16,0.5));
    else u8g2.setCursor(X(1,14,2), Y(1,16,0.5));
    u8g2.print(counter);
    u8g2.sendBuffer();
    while(digitalRead(button_left) == 1) delay(20);
  }

  if(digitalRead(button_on) == 1){
    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFont(u8g2_font_inb16_mf );  // choose a suitable font
    u8g2.setCursor(X(1,14,3), Y(1,16,0.5));
    u8g2.print("BYE");
    u8g2.sendBuffer();

    delay(2000);
    digitalWrite(latch, LOW);
    delay(1000);
  }


}