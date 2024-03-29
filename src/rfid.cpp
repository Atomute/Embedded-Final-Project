#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <Wire.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <TridentTD_LineNotify.h>

#define SS_PIN  21  
#define RST_PIN 22
#define LINE_TOKEN "5jbTrnjtkPoK04oaLkOhbL83vyoI400BDmOgWCHdhsy"

const char* ssid = "Atom";
const char* password = "12345678";

MFRC522 rfid(SS_PIN, RST_PIN);
TFT_eSPI tft = TFT_eSPI(); 
String currentID = "";
String zeroOrSpace;

volatile bool scan = false;

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

    LINE.setToken(LINE_TOKEN);

    tft.init();
    tft.setRotation(2);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 2);

    Serial.begin(115200);
    SPI.begin(); // init SPI bus
    rfid.PCD_Init(); // init MFRC522
}

void loop(){
    while (!rfid.PICC_IsNewCardPresent()) { // new tag is available
        drawWaitingAnimation();   
    }
    tft.fillScreen(TFT_BLACK);

    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
        tft.setCursor(0, 0, 2);
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        tft.print("RFID/NFC: ");
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
        tft.println("Start Tracking");
        LINE.notify("Start Tracking");

        rfid.PICC_HaltA(); // halt PICC
        rfid.PCD_StopCrypto1(); // stop encryption on PCD

        scan = !scan;

        delay(2000);
        tft.fillScreen(TFT_BLACK);
    }

    while(scan){
        tft.println("Lat: 13.821454");

        tft.println("Lng: 100.515308");

        tft.println("Speed: 3.37");
        
        tft.println("SAT: 4");

        tft.println("ALT: 19");

        delay(5000);
        tft.fillScreen(TFT_BLACK);

        tft.println("Lat: 13.821298");

        tft.println("Lng: 100.515062");

        tft.println("Speed: 2.11");
        
        tft.println("SAT: 4");

        tft.println("ALT: 40");

        delay(5000);
        tft.fillScreen(TFT_BLACK);

        tft.println("Lat: 13.821209");

        tft.println("Lng: 100.514915");

        tft.println("Speed: 1.19");
        
        tft.println("SAT: 4");

        tft.println("ALT: 56");

        delay(5000);
        tft.fillScreen(TFT_BLACK);

        tft.println(currentID);
        LINE.notify("Package Arrived At Destination");
        currentID = "";
        scan = !scan;
        delay(2000);
        tft.fillScreen(TFT_BLACK);
    }
}