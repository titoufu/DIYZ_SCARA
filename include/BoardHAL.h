#pragma once
#include <Arduino.h>
#include "BoardPins.h"
namespace Diyz {
class BoardHAL {
public:
 static void beginSafe(){
  // Primeiro fixa nível LOW no latch, depois configura saída, sem PWM.
  constexpr uint8_t motorPins[]={Pins::J1_IN1,Pins::J1_IN2,Pins::J2_IN1,Pins::J2_IN2,Pins::PUMP_IN1,Pins::PUMP_IN2};
  for(uint8_t pin:motorPins){digitalWrite(pin,LOW);pinMode(pin,OUTPUT);}
  // Servo NÃO é associado a PWM; mantém sinal inativo.
  digitalWrite(Pins::SERVO_Z,LOW);pinMode(Pins::SERVO_Z,OUTPUT);
  // SPI/CS, LED RGB e fins de curso serão ensaiados somente na Fase 1.
 }
};
}
