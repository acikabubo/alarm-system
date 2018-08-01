/*
 * Alarm system
 * 
 * Made by Aleksandar Krsteski
 * 
 */
 
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

const int buzzerPin = 7;

const int redPin = 6;
const int greenPin = 5;
const int bluePin = 3;

const int DOOR_SENSOR = 4;
const int MOTION_SENSOR = 2;

bool DEACTIVATED = false;

void setup() {
  setColor(0, 0, 0);

  SPI.begin();        // Initiate  SPI bus
  mfrc522.PCD_Init(); // Initiate MFRC522

  pinMode(buzzerPin, OUTPUT);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(DOOR_SENSOR, INPUT_PULLUP);
  pinMode(MOTION_SENSOR, INPUT_PULLUP);
  
  Serial.begin(9600);
}

void loop() {
  if (DEACTIVATED) {
    Serial.println("ALARM DEACTIVATED");
    String content = getTagID();
    if (content.substring(1) == "43 EB E5 2E") {
       DEACTIVATED = false;
       setColor(0, 0, 100); // Blue led
       delay(500);
       setColor(0, 0, 0); // Off led
       for(int i=15; i>0; i--) {
         // TODO: This message will be shown in LCD 
         Serial.print("Alarm will be activated in ");
         Serial.print(i);
         Serial.println(" seconds");
         delay(1000);
       }
       // buzzerBeep(100);
       setColor(0, 100, 0); // Green led
       delay(1000);
       setColor(0, 0, 0); // Off led
    }
    delay(500);
  } else {
    Serial.println("ALARM_ACTIVATED");
    if (digitalRead(DOOR_SENSOR)) {
      if (digitalRead(MOTION_SENSOR)) {
        for(int i=1; i<=5; i++) {
          String content = getTagID();
          if (content.substring(1) == "43 EB E5 2E") {
            setColor(0, 100, 0); // Green led
            delay(3000);
            setColor(0, 0, 0); // Off led
            DEACTIVATED = true;
            // TODO: This message should be shown in LCD
            Serial.print("DEACTIVATED: ");
            Serial.println(DEACTIVATED);
            // buzzerBeep(100);
            return;
          }
          // TODO: This message should be shown in LCD
          Serial.println(i);
          setColor(0, 0, 100); // Blue led
          delay(500);
          setColor(0, 0, 0); // Off led
          delay(500);
        }
        setColor(100, 0, 0); // Red led
         buzzerBeep(100);
        delay(2000);
        setColor(0, 0, 0); // Off led
        
      }
      Serial.println("OPENED");
    } else {
      Serial.println("CLOSED");
    }
  
    String content = getTagID();
    if (content != "") {
      DEACTIVATED = false;
      // TODO: This message should be shown in LCD
      Serial.print("DEACTICATED: ");
      Serial.println(DEACTIVATED);
      setColor(0, 0, 100); // Blue led
      delay(500);
      setColor(0, 0, 0); // Off led
    }
    
    delay(150);
  }
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

String getTagID() {
  String content = "";
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return content;
  }
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    Serial.println("SECOND");
    return content;
  }
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  // TODO: This message should be shown in LCD
  Serial.print("CONTENT: ");
  Serial.println(content);

  return content;
}

void buzzerBeep(int sec) {
  digitalWrite(buzzerPin, HIGH);
  delay(sec);
  digitalWrite(buzzerPin, LOW);
}

