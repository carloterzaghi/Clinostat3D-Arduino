#include <Arduino.h>
#include "Servo.h"

// Create a new servo object:
Servo myservo;

// Define the servo pin:
#define servoPin 9
const int ledPin = 8;
const int buttonPin = 7;
int buttonState = 0;
String ReciveDataSerial = "";

void setup() {
  Serial.begin(9600);
  // Attach the Servo variable to a pin:
  myservo.attach(servoPin);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  digitalWrite(ledPin, LOW);
}

void loop() {
  ReciveDataSerial = Serial.readString();
  if (ReciveDataSerial!="") {
    String nova_str = ReciveDataSerial.substring(0, ReciveDataSerial.length() - 2);
    String estadoLigar, Gravidade;
    int pos = nova_str.indexOf(",");
    
    if (pos != -1) {
      estadoLigar = nova_str.substring(0, pos); 
      Gravidade = nova_str.substring(pos + 1); 
    }
    else{
      estadoLigar = nova_str;
    }

    if (estadoLigar == "off"){
      digitalWrite(ledPin, LOW);
    }
    if (estadoLigar == "on"){
      digitalWrite(ledPin, HIGH);
    } 

    if (Gravidade != ""){
      myservo.write(atoi(Gravidade.c_str()));
    } 

  }

  buttonState = digitalRead(buttonPin);

  Serial.println(String(buttonState)+","+String(random(100))); // Send data to esp32 -> buttonState, RandomNumber

  delay(300);
}