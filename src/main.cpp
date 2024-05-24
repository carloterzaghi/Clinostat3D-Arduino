#include <Arduino.h>
#include "Servo.h"
#include <SoftwareSerial.h>

// Create a new servo object:
Servo myservo;

//Pinos de comunicacao serial do modulo RS485
#define Pino_RS485_RX    10
#define Pino_RS485_TX    11

//Pino de controle transmissao/recepcao
#define SSerialTxControl 3

#define RS485Transmit    HIGH
#define RS485Receive     LOW

// Definindo todas as funções, sendo servo até dado recebido via Serial pelo Esp32
#define servoPin 9
const int ledPin = 8;
const int buttonPin = 7;
int buttonState = 0;
String ReciveDataSerial = "";

//Cria a serial por sofware para conexao com modulo RS485
SoftwareSerial RS485Serial(Pino_RS485_RX, Pino_RS485_TX);

void setup() {
  Serial.begin(9600);
  // Attach the Servo variable to a pin:
  myservo.attach(servoPin);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  pinMode(SSerialTxControl, OUTPUT);

  //Coloca o modulo RS485 em modo de recepcao
  digitalWrite(SSerialTxControl, RS485Receive);

  digitalWrite(ledPin, LOW);

  RS485Serial.begin(9600);
}

void loop() {

  // Leitura do Esp32
  if (RS485Serial.available())
  {
    while (RS485Serial.available())
    {
      char inChar = (char)RS485Serial.read();
      
      if (inChar != '\0') 
      {
        ReciveDataSerial += inChar;
      }
      
      if (inChar == '\n')
      {
        //Mostra no Serial Monitor a string recebida
        Serial.print(ReciveDataSerial);
        
        String nova_str = ReciveDataSerial.substring(0, ReciveDataSerial.length() - 2);
        String estadoLigar, Gravidade;
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
        ReciveDataSerial = "";
      }
    }
  }
  
  buttonState = digitalRead(buttonPin);
  
  digitalWrite(SSerialTxControl, RS485Transmit);
  RS485Serial.println(String(buttonState)+","+String(random(100))); // Send data to esp32 -> buttonState, RandomNumber
  digitalWrite(SSerialTxControl, RS485Receive);

  delay(300);
}