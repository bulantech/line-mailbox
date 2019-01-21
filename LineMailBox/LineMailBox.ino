#include <ESP8266WiFi.h>

// Config connect WiFi
#define WIFI_SSID "PP-RD-FL4"
#define WIFI_PASSWORD "ppetech1"

// Line config
#define LINE_TOKEN "ez4tu8UFv2Yw7ELVCeyrfSfFqOYTUlb6QqTfZJotMp3"

#define IR_SW D8
#define LED D0

String msg_you_have_got_mail = "%E0%B8%84%E0%B8%B8%E0%B8%93%E0%B8%A1%E0%B8%B5%E0%B8%88%E0%B8%94%E0%B8%AB%E0%B8%A1%E0%B8%B2%E0%B8%A2 ";
String msg_pieces = " %E0%B8%89%E0%B8%9A%E0%B8%B1%E0%B8%9A";

int mail_counter;
boolean line_send = false;
unsigned long time_ms_buffer;


void Line_Notify_Send(void)
{
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;   
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + msg_you_have_got_mail + mail_counter + msg_pieces).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + msg_you_have_got_mail + mail_counter + msg_pieces;
  // Serial.println(req);
  client.print(req);
    
  delay(20);
  line_send = false;

  Serial.println("-------------");
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    Serial.println(line);
  }
  Serial.println("-------------");
}

void IntSwitch(void)
{
    if((millis() - time_ms_buffer) > 3000)   // wait for 3 sec
    {
      mail_counter++;
      line_send = true;
      digitalWrite(LED, 0); //on 
    }
    
    time_ms_buffer = millis();
}

void setup() 
{
  Serial.begin(115200);
  pinMode(IR_SW,INPUT_PULLUP);
  pinMode(LED,OUTPUT);
  digitalWrite(LED, 1); //off 
  
  attachInterrupt(digitalPinToInterrupt(IR_SW), IntSwitch, FALLING);
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
}

void loop() 
{
  if(line_send==true) { 
    Line_Notify_Send(); 
    digitalWrite(LED, 1); 
  }
  delay(10);
}
