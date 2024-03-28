#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <Wire.h>
#include <TinyGPS++.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <TridentTD_LineNotify.h>

#define SS_PIN  21  
#define RST_PIN 22 
#define TFT_GREY 0x5AEB // New colour
#define RXD2 16
#define TXD2 17
#define LINE_TOKEN "5jbTrnjtkPoK04oaLkOhbL83vyoI400BDmOgWCHdhsy"

MFRC522 rfid(SS_PIN, RST_PIN);
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
HardwareSerial neogps(1);
TinyGPSPlus gps;
WiFiClient client;
String currentID = "";
String zeroOrSpace;

volatile bool scan = false;
const char* ssid = "Atom";
const char* password = "12345678";
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

unsigned long thingSpeakCHID = 2486697;
String apiKey = "B9N3YB53DRL3NG2C";
const char* serverName = "http://api.thingspeak.com/update";

void setup(void) {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  LINE.setToken(LINE_TOKEN);

  //Begin serial communication Neo6mGPS
  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 2);

  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522

  delay(2000);
}

void httpPostToThingSpeak(String rfids,float lat, float longi) {
  HTTPClient http;

  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencode");
  String httpRequestData = "api_key=" + apiKey + "&field1=" + String(rfids) + "&field2=" + String(lat) + "&field3=" + String(longi);
  int httpResponseCode = http.POST(httpRequestData);
  if(httpResponseCode == 200){
    Serial.println("Channel update successful.");
  }else if(httpResponseCode == -1){
    Serial.println("Channel update failed.");
  }
  http.end();
}

unsigned int halfSecond = 0;

void print_speed(){       
  if (gps.location.isValid())
  { 
    tft.println("Lat: ");
    double lat = gps.location.lat();
    tft.println(lat,6);

    tft.println("Lng: ");
    double lng = gps.location.lat();
    tft.println(lng,6);

    tft.println("Speed: ");
    tft.println(gps.speed.kmph());
    
    tft.println("SAT:");
    tft.println(gps.satellites.value());

    tft.println("ALT:");
    tft.println(gps.altitude.meters(), 0);

    if (WiFi.status() == WL_CONNECTED and halfSecond >=30){
      halfSecond = 0;
      httpPostToThingSpeak(currentID,lat,lng);
    }
    else{
      Serial.println("WiFi Disconnected");
    }
  }
  else
  {
    tft.println("No Signal From Satellite");
  }  
}

void drawWaitingAnimation() {
  int dotSize = 10;
  // Increased spacing between dots by multiplying dotSize with a factor
  int spacingFactor = 3;  // Adjust this value to control spacing
  int centerX = (TFT_WIDTH / 2) - spacingFactor*5;
  int centerY = TFT_HEIGHT / 2;

  for (int i = 0; i < 3; i++) {
    tft.fillCircle(centerX + dotSize * (i * spacingFactor - 1), centerY, dotSize, TFT_WHITE);
    delay(50);  // Adjust delay between dots
  }

  // Repeat for a bouncing effect (optional)
  for (int i = 2; i >= 0; i--) {
    tft.fillCircle(centerX + dotSize * (i * spacingFactor - 1), centerY, dotSize, TFT_WHITE);
    delay(50);  // Adjust delay between dots
  }

  delay(200);  // Adjust delay between animation frames
    tft.fillScreen(TFT_BLACK);
}

void loop() {
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(2);

  while (scan){
    tft.setCursor(0, 0, 2);

    boolean newData = false;
    for (unsigned long start = millis(); millis() - start < 1000;){
      while (neogps.available())
      {
        if (gps.encode(neogps.read()))
        {
          newData = true;
        }
      }
    }

    //If newData is true
    if(newData == true)
    {
      newData = false;
      tft.print("Satellite : ");
      tft.println(gps.satellites.value());
      print_speed();
    }
    else
    {
      tft.println("Error : Check Wiring");
    }  
    
    if(rfid.PICC_IsNewCardPresent()){
      if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      tft.print("RFID/NFC: ");
      tft.println(rfid.PICC_GetTypeName(piccType));

      // print UID in tft Monitor in the hex format
      tft.print("UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        tft.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        tft.print(rfid.uid.uidByte[i], HEX);
      }
      tft.println();

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
      currentID = "";
      scan = !scan;
      LINE.notify("Your Package Arrived");
      }
    }
  }

  while (!rfid.PICC_IsNewCardPresent()) { // new tag is available
    drawWaitingAnimation();   
  }

  tft.fillScreen(TFT_BLACK);

  if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
    tft.setCursor(0, 0, 2);
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    tft.print("ประเภทของ RFID/NFC: ");
    tft.println(rfid.PICC_GetTypeName(piccType));

    // print UID in tft Monitor in the hex format
    tft.print("UID:");
    for (int i = 0; i < rfid.uid.size; i++) {
      zeroOrSpace = rfid.uid.uidByte[i] < 0x10 ? " 0" : " ";
      currentID = currentID + zeroOrSpace;
      currentID = currentID + rfid.uid.uidByte[i];
    }
    Serial.println(currentID);
    tft.println(currentID);
    tft.println(scan);

    rfid.PICC_HaltA(); // halt PICC
    rfid.PCD_StopCrypto1(); // stop encryption on PCD

    LINE.notify("Your Package is on the way");

    scan = !scan;
    delay(2000);
    tft.fillScreen(TFT_BLACK);
  }
}
