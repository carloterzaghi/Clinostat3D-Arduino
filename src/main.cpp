#include <Arduino.h>
#include <SoftwareSerial.h>
#include "AccelStepper.h"

//Pinos de comunicacao serial do modulo RS485
#define Pino_RS485_RX    10
#define Pino_RS485_TX    11

//Pino de controle transmissao/recepcao
#define SSerialTxControl 3

#define RS485Transmit    HIGH
#define RS485Receive     LOW

// Definindo todas as funções, sendo servo até dado recebido via Serial pelo Esp32
const int ledPin = 8;
const int buttonPin = 7;
int buttonState = 0;
String ReciveDataSerial = "", Gravidade ="";

// Pinos diginais do Motor de Passo e as configurações
// Pinos
#define MotorDirPin 2
#define MotorStepPin 4
#define MotorEnable 9
// Configurações
#define MotorInterfaceType 1
#define MotorLigar LOW
#define MotorDesligar HIGH

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(MotorInterfaceType, MotorStepPin, MotorDirPin);

// Função que transforma de RPM para Passos por Segundo
float RPM_to_PPS(float RPM){
  return RPM*20/3;
}


//Cria a serial por sofware para conexao com modulo RS485
SoftwareSerial RS485Serial(Pino_RS485_RX, Pino_RS485_TX);

void setup() {
  Serial.begin(9600);

  // Configarções Init motor de passo
  pinMode(MotorEnable, OUTPUT);
  digitalWrite(MotorEnable, MotorDesligar); 
  stepper.setMaxSpeed(RPM_to_PPS(150)); // Velocidade Máxima imposta será de 150 RPM


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
        Serial.println(ReciveDataSerial);
        
        String nova_str = ReciveDataSerial.substring(0, ReciveDataSerial.length() - 2);
        String estadoLigar, whereIsSend;

        Gravidade = "";
        int firstComma = nova_str.indexOf(',');
        int secondComma = nova_str.indexOf(',', firstComma + 1);

        // Se tiver virgula separa os dados pela posição
        whereIsSend = nova_str.substring(0, firstComma); 
        
        // Se não tiver virgula é porque é um dado sozinho, como o "off" para desligar o led
        if (secondComma != -1) {
          Gravidade = nova_str.substring(secondComma+1); 
          estadoLigar = nova_str.substring(firstComma+1,secondComma); 
        }
        else estadoLigar = nova_str.substring(firstComma+1); 

        Serial.println(whereIsSend);
        Serial.println(estadoLigar);
        Serial.println(Gravidade);

        if (whereIsSend == "ESP-Master"){
          if (estadoLigar == "off"){
            digitalWrite(MotorEnable, MotorDesligar);  // Desligar o motor
            digitalWrite(ledPin, LOW);
          }
          if (estadoLigar == "on"){
            digitalWrite(MotorEnable, MotorLigar);  // Liga o motor
            digitalWrite(ledPin, HIGH);
            delay(200);  // Aguarda um pouco para estabilizar
          }
        }
         
        ReciveDataSerial = "";
      }
    }
  }

  if (Gravidade != ""){
    stepper.setSpeed(atoi(Gravidade.c_str())); // Coloca a velocidade imposta pelo usuário
    stepper.runSpeed(); // Inicia o funcionamento
  } 

  buttonState = digitalRead(buttonPin);
  
  static unsigned long lastSerialTime = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - lastSerialTime >= 900) { // Intervalo de 1000ms
    lastSerialTime = currentMillis;
    buttonState = digitalRead(buttonPin);
    digitalWrite(SSerialTxControl, RS485Transmit);
    RS485Serial.println(String(buttonState) + "," + String(random(100))); // Envia dados ao ESP32
    digitalWrite(SSerialTxControl, RS485Receive);
  }
}