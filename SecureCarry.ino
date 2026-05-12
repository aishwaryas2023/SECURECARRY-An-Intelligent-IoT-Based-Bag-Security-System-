#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <TinyGPS++.h>

#define SS_PIN 5
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN);
Servo lockServo;

#define SERVO_PIN 12
#define LED_PIN 4
#define RED_LED 21
#define BUZZER 15
#define WATER_SENSOR 34
#define BUTTON 13

// WIFI + TELEGRAM
const char* ssid = "xxyy";
const char* passwordWifi = "****"; 

#define BOTtoken "****" 
#define CHAT_ID "****" 

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

// Keypad
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};

byte rowPins[ROWS] = {32,33,25,26};
byte colPins[COLS] = {27,14,17};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void keypadSetup() {
  keypad.setDebounceTime(20);
  keypad.setHoldTime(500);
}

// Password
String password = "1234";
String input = "";

// Authorized RFID
String authorizedTag = "e28ee600";

bool rfidOK = false;
bool waterAlert = false;
bool emergencyUsed = false;
unsigned long ledTimer = 0;
unsigned long redLedTimer = 0;
bool redLedActive = false;

int wrongAttempts = 0;
int wrongRFID = 0;
bool lockDown = false;
unsigned long lockDownStart = 0;
unsigned long telegramTimer = 0;
float lastLat = 0.0;
float lastLng = 0.0;
bool gpsFix = false;

void sendAlert(String message)
{
if(WiFi.status() == WL_CONNECTED)
{
bot.sendMessage(CHAT_ID, message, "");
}
}

// LOCATION FUNCTION
void sendLocation()
{
  float lat = 0.0;
  float lng = 0.0;

  if (gps.location.isValid())
  {
    lat = gps.location.lat();
    lng = gps.location.lng();

    lastLat = lat;
    lastLng = lng;
    gpsFix = true;

    String locationMsg = "📍 Pouch Location:\n";
    locationMsg += "https://maps.google.com/?q=";
    locationMsg += String(lat,6);
    locationMsg += ",";
    locationMsg += String(lng,6);

    sendAlert(locationMsg);
  }

  else
  {
    sendAlert("📍 Pouch Location:\nhttps://maps.app.goo.gl/n4DjhqF1d9QJBygC9?g_st=aw");
  }
}
void setup()
{
Serial.begin(115200);
SPI.begin(18,19,23,5);
rfid.PCD_Init();
rfid.PCD_SetAntennaGain(rfid.RxGain_max);
keypadSetup();
gpsSerial.begin(9600, SERIAL_8N1, 16, -1);

Serial.println("GPS Started");

pinMode(LED_PIN, OUTPUT);
pinMode(RED_LED, OUTPUT);
pinMode(BUZZER, OUTPUT);
pinMode(WATER_SENSOR, INPUT);
pinMode(BUTTON, INPUT_PULLUP);

// Keypad pullups
pinMode(32, INPUT_PULLUP);
pinMode(33, INPUT_PULLUP);
pinMode(25, INPUT_PULLUP);
pinMode(26, INPUT_PULLUP);

lockServo.attach(SERVO_PIN);

lockPouch();

WiFi.begin(ssid, passwordWifi);

while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}

client.setInsecure();

Serial.println("");
Serial.println("WiFi connected");

bot.sendMessage(CHAT_ID, "SecureCarry System Online", "");

Serial.println("SecureCarry Ready");
}

void loop()
{
    // --- Feed GPS continuously ---
    while (gpsSerial.available())
    {
        gps.encode(gpsSerial.read());
    }
    if(gps.location.isUpdated())
{
  Serial.println("GPS UPDATED");
  Serial.println(gps.location.lat(),6);
  Serial.println(gps.location.lng(),6);
}

    checkRFID();
    checkKeypad();
    checkButton();
    checkWater();
    checkRedLED();

    if(millis() - telegramTimer > 2000)
    {
        checkTelegram();
        telegramTimer = millis();
    }

    delay(5);
}

// LOCK
void lockPouch()
{
lockServo.write(20);
digitalWrite(LED_PIN,LOW);
Serial.println("Pouch Locked");

sendAlert("🔒 Pouch closed");
}

// UNLOCK
void unlockPouch()
{
Serial.println("Access Granted");

lockServo.write(100);
digitalWrite(LED_PIN,HIGH);

sendAlert("🔓 Pouch opened");

delay(10000);

lockPouch();

rfidOK = false;
input = "";
}

// TELEGRAM
void checkTelegram()
{

int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

for (int i = 0; i < numNewMessages; i++)
{
String text = bot.messages[i].text;

if(text == "/lock")
{
lockDown = true;
lockDownStart = millis();

Serial.println("Manual lockdown activated via Telegram");

sendAlert("🔒 Manual lockdown activated for 5 minutes.");
}

if(text == "/unlock")
{
lockDown = false;
wrongAttempts = 0;
wrongRFID = 0;

Serial.println("Lockdown manually cancelled via Telegram");

sendAlert("✅ Lockdown cancelled. System active.");
}

if(text == "/location")
{
sendLocation();
}

}
}

// WATER SENSOR
void checkWater()
{
int water = analogRead(WATER_SENSOR);

if(water > 1000 && !waterAlert)
{
Serial.println("Water Detected!");

digitalWrite(BUZZER,HIGH);
delay(100);
digitalWrite(BUZZER,LOW);

sendAlert("💧 Water detected near pouch!");

waterAlert = true;
}

if(water < 500)
{
waterAlert = false;
}
}

void checkRedLED()
{
if(redLedActive && millis() - redLedTimer >= 5000)
{
digitalWrite(RED_LED,LOW);
redLedActive = false;
}
}

// EMERGENCY BUTTON
void checkButton()
{
if(digitalRead(BUTTON)==LOW && !emergencyUsed)
{
Serial.println("Emergency button pressed — unlocking");

lockServo.write(100);
digitalWrite(LED_PIN,HIGH);

sendAlert("🚨 Emergency button pressed! Pouch opened.");

sendLocation();

ledTimer = millis();

rfidOK=false;
input="";

emergencyUsed = true;
}

if(emergencyUsed && millis() - ledTimer >= 5000)
{
digitalWrite(LED_PIN,LOW);
}
}

// KEYPAD
void checkKeypad()
{

if(lockDown)
{
if(millis() - lockDownStart < 300000)
{
return;
}
else
{
lockDown = false;
wrongAttempts = 0;

Serial.println("Lockdown ended - System active again");
sendAlert("✅ Lockdown ended. System active again.");
}
}

char key = keypad.getKey();

if(key)
{

Serial.print("Key: ");
Serial.println(key);

if(!rfidOK)
{
Serial.println("Scan RFID first");
return;
}

if(key == '#')
{

if(input == password)
{
Serial.println("Correct PIN");
unlockPouch();
wrongAttempts = 0;
digitalWrite(RED_LED,LOW);
}

else
{
Serial.println("Wrong PIN");

digitalWrite(RED_LED,HIGH);
redLedTimer = millis();
redLedActive = true;

digitalWrite(BUZZER,HIGH);
delay(100);
digitalWrite(BUZZER,LOW);

sendAlert("❌ Wrong PIN attempt detected!");

wrongAttempts++;

if(wrongAttempts >= 3)
{
lockDown = true;
lockDownStart = millis();

Serial.println("LOCKDOWN ACTIVATED");

sendAlert("🚨 SECURITY ALERT: 3 Wrong PIN attempts. System locked for 5 minutes.");

sendLocation();
}

rfidOK = false;
input="";

return;
}

input="";
}

else
{
input += key;
}

}
}

// RFID
void checkRFID()
{
if(lockDown)
{
return;
}

if(!rfid.PICC_IsNewCardPresent()) return;
if(!rfid.PICC_ReadCardSerial()) return;

String tag="";

for(byte i=0;i<rfid.uid.size;i++)
{
if(rfid.uid.uidByte[i] < 0x10) tag += "0";
tag += String(rfid.uid.uidByte[i],HEX);
}

tag.toLowerCase();

Serial.print("UID: ");
Serial.println(tag);

if(tag == authorizedTag)
{
Serial.println("RFID Accepted");

rfidOK = true;
wrongRFID = 0;

digitalWrite(RED_LED,LOW);
}

else
{
wrongRFID++;

if(wrongRFID >= 3)
{
lockDown = true;
lockDownStart = millis();

Serial.println("RFID LOCKDOWN ACTIVATED");

sendAlert("🚨 SECURITY ALERT: 3 Unauthorized RFID scans. System locked for 5 minutes.");

sendLocation();

wrongRFID = 0;
}

Serial.println("Unauthorized Card");

digitalWrite(RED_LED,HIGH);
redLedTimer = millis();
redLedActive = true;

digitalWrite(BUZZER,HIGH);
delay(100);
digitalWrite(BUZZER,LOW);

sendAlert("❌ Unauthorized RFID card scanned!");
}

rfid.PICC_HaltA();
rfid.PCD_StopCrypto1();
}