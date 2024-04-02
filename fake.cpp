// #include <SPI.h>
// #include <TFT_eSPI.h> // Hardware-specific library
// #include <Wire.h>
// #include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SS_PIN  21  
#define RST_PIN 22

const char* ssid = "Atom";
const char* password = "12345678";

// MFRC522 rfid(SS_PIN, RST_PIN);
// TFT_eSPI tft = TFT_eSPI(); 
String currentID = "";
String zeroOrSpace;
String apiKey = "B9N3YB53DRL3NG2C";
WiFiClient client;

unsigned long thingSpeakCHID = 2486697;
const char* serverName = "http://api.thingspeak.com/update";
volatile bool scan = false;

// void drawWaitingAnimation() {
//   int dotSize = 10;
//   // Increased spacing between dots by multiplying dotSize with a factor
//   int spacingFactor = 3;  // Adjust this value to control spacing
//   int centerX = (TFT_WIDTH / 2) - spacingFactor*5;
//   int centerY = TFT_HEIGHT / 2;

//   for (int i = 0; i < 3; i++) {
//     tft.fillCircle(centerX + dotSize * (i * spacingFactor - 1), centerY, dotSize, TFT_WHITE);
//     delay(50);  // Adjust delay between dots
//   }

//   // Repeat for a bouncing effect (optional)
//   for (int i = 2; i >= 0; i--) {
//     tft.fillCircle(centerX + dotSize * (i * spacingFactor - 1), centerY, dotSize, TFT_WHITE);
//     delay(50);  // Adjust delay between dots
//   }

//   delay(200);  // Adjust delay between animation frames
//     tft.fillScreen(TFT_BLACK);
// }

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

void setup(void){
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    // tft.init();
    // tft.setRotation(2);
    // tft.fillScreen(TFT_BLACK);
    // tft.setCursor(0, 0, 2);

    Serial.begin(115200);
    // SPI.begin(); // init SPI bus
    // rfid.PCD_Init(); // init MFRC522
}

void loop(){
  httpPostToThingSpeak("6352",13.5264893,100.236585446);
}