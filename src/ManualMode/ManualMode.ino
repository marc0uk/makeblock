#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <MeOrion.h>

MeDCMotor dc;
MeInfraredReceiver irP6(6);
MeUltrasonicSensor usP3(3);

int minSpeed = 45;
int moveSpeed = 150;
int randnum = 0;
bool manual = true;
bool leftFlag = false;
bool rightFlag = false;
double distance;

void forward();
void backward();
void turnLeft();
void turnRight();
void halt();
void buzz();

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
  dc.run(-moveSpeed);
}

void backwardLeft()
{
  dc.reset(M1);
  dc.run(moveSpeed/2);
  dc.reset(M2);
  dc.run(-moveSpeed);
}

void backwardRight()
{
  dc.reset(M1);
  dc.run(moveSpeed);
  dc.reset(M2);
  dc.run(-moveSpeed/2);
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

void buzz()
{
  buzzerOn();
  delay(100);
  buzzerOff();
}

void setup() {
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  delay(300);
  digitalWrite(13,LOW);
  Serial.begin(115200);
  //
  delay(500);
  buzzerOn();
  delay(100);
  buzzerOff();
  delay(500);
  //
  irP6.begin();
}

uint8_t irCode()
{
  irP6.loop();
  return irP6.getCode();
}

void manualLoop()
{
  switch(irCode())
  {
    case IR_BUTTON_PLUS:
      forward();
      break;
    case IR_BUTTON_MINUS:
      backward();
      break;
    case IR_BUTTON_PREVIOUS:
      turnLeft();
      break;
    case IR_BUTTON_NEXT:
      turnRight();
      break;
    case IR_BUTTON_A:
      manual = false;
      break;
    default:
      halt();
      break;
  }
}

void autonomousLoop()
{
  switch(irCode())
  {
    case IR_BUTTON_C:
      manual = true;
      halt();
      break;
  }
  distance = usP3.distanceCm();
  randomSeed(analogRead(A4));
  if((distance > 10) && (distance < 40))
  {
    randnum = random(300);
    if ((randnum > 190) && (!rightFlag))
    {
      leftFlag = true;
      turnLeft();
    }
    else
    {
      rightFlag = true;
      turnRight();
    }
  }
  else if (distance < 10)
  {
    randnum=random(300);
    if(randnum > 190)
    {
      backwardLeft();
    }
    else
    {
      backwardRight();
    }
  }
  else
  {
    leftFlag = false;
    rightFlag = false;
    forward();
  }
}

void loop() {
  if (manual)
  {
    manualLoop();
  }
  else
  {
    autonomousLoop();
  }
}

void _loop()
{
  irP6.loop();
}

