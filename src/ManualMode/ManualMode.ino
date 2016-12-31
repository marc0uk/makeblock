#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <MeOrion.h>

MeDCMotor dc;
MeInfraredReceiver irP6;

uint8_t irRead = 0;

int minSpeed = 45;
int moveSpeed = 150;

void forward()
{
  dc.reset(M1);
  dc.run(-moveSpeed);
  dc.reset(M2);
  dc.run(moveSpeed);
}

void backward()
{
  dc.reset(M1);
  dc.run(moveSpeed);
  dc.reset(M2);
  dc.run(moveSpeed);
}

void turnLeft()
{
  dc.reset(M1);
  dc.run(moveSpeed);
  dc.reset(M2);
  dc.run(moveSpeed);
}

void turnRight()
{
  dc.reset(M1);
  dc.run(-moveSpeed);
  dc.reset(M2);
  dc.run(-moveSpeed);
}

void halt()
{
  dc.reset(M1);
  dc.run(0);
  dc.reset(M2);
  dc.run(0);
}

void irProcess()
{
  irP6.loop();
  irRead = irP6.getCode();
  switch(irRead)
  {
    case IR_BUTTON_PLUS:
      forward();
      break;
    case IR_BUTTON_MINUS:
      backward();
      break;
    case IR_BUTTON_PREVIOUS: // Left
      turnLeft();
      break;
    case IR_BUTTON_NEXT:
      turnRight();
      break;
    case IR_BUTTON_TEST: // D
      halt();
      while (irP6.buttonState() != 0)
      {
        irP6.loop();
      }
      break;
    default:
      halt();
      break;
  }
}


void setup() {
  irP6.begin();
}

void loop() {
  irProcess();
}
