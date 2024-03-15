#include <Arduino.h>
#include "Servo.h"

// Create a new servo object:
Servo myservo;

// Definindo todas as funções, sendo servo até dado recebido via Serial pelo Esp32
#define servoPin 9
const int ledPin = 8;
const int buttonPin = 7;
int buttonState = 0;
String ReciveDataSerial = "";
String estadoLigar, Gravidade;

void setup() {
  Serial.begin(9600);
  // Attach the Servo variable to a pin:
  myservo.attach(servoPin);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  digitalWrite(ledPin, LOW);
}

void loop() {
  ReciveDataSerial = Serial.readString(); // Faz a leitura das informações enviadas pelo Esp32 ao Arduino via Serial
  if (ReciveDataSerial!="") {
    String nova_str = ReciveDataSerial.substring(0, ReciveDataSerial.length() - 2);
    int pos = nova_str.indexOf(",");

    // Se tiver virgula separa os dados pela posição
    if (pos != -1) {
      estadoLigar = nova_str.substring(0, pos); 
      Gravidade = nova_str.substring(pos + 1); 
    }

    // Se não tiver virgula é porque é um dado sozinho, como o "off" para desligar o led
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

  // Se for para ligar o led, envie dados para o ESP32
  if (estadoLigar == "on"){
    Serial.println(String(buttonState)+","+String(random(100))); // Send data to esp32 -> buttonState, RandomNumber
  }

  delay(300);
}
