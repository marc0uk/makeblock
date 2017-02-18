//
// A simple autonomous obstacle avoidance driver for the 3-wheeled robot.
//
// Usage:
//   A: start the robot
//   C: stop the robot
//   UP: increase the turn delay by 0.1 seconds
//   DOWN: decrease the turn delay by 0.1 seconds
//   B: reset the turn delay to the default
//
// Descriptions:
//   When distance is between 10 and 40 the robot will randomly turn left
//   or right and start a timer. If the distance is still in the same range
//   but the robot is already turning, no action will be taken.
//   If the distance is now larger than 40 and the robot has been turning
//   for more than the threshold value defined by the turnDelay variable,
//   then the robot will stop turning and resume going forward.
//   If the distance is smaller than 10 then robot will randomly reverse
//   left or right and start a timer. 
//   The robot will stop the reverse-turn action and resume moving forward 
//   once the distance has increased and the turning lasted more than the 
//   turnDelay threshold.
//
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <MeOrion.h>

MeDCMotor dc;
MeInfraredReceiver irP6(6);
MeUltrasonicSensor usP3(3);

// Current motor speed
double speed;

// Distance measured by usP3
double distance;

// true when the robot is stopped
bool halted;

// true when the robot is turning left
bool leftFlag;

// true when the robot is turning right
bool rightFlag;

// random number to decide which way to turn
int randnum = 0;

// current time
double currentTime = 0;

// last measured time
double lastTime = 0;

// delay to apply in the distance detection when turning
double turnDelay;

double getLastTime(){
      return currentTime = millis()/1000.0 - lastTime;
}

uint8_t irCode()
{
  irP6.loop();
  return irP6.getCode();
}

// Run the specified motor at the specified speed
void dcRun(uint8_t motor, int16_t motorSpeed)
{
  dc.reset(motor);
  dc.run(motorSpeed);
}

// Move the robot forward
void forward()
{
  dcRun(M1, speed);
  dcRun(M2, -speed);
  leftFlag = false;
  rightFlag = false;
}

// Move the robot backward
void backward()
{
  dcRun(M1, -speed);
  dcRun(M2, speed);
  leftFlag = false;
  rightFlag = false;
}

// Stop the robot
void halt()
{
  dcRun(M1, 0);
  dcRun(M2, 0);
  halted = true;
}

// Turn the robot left (tank style)
void left()
{
  dcRun(M1, speed);
  dcRun(M2, speed);
  leftFlag = true;
  rightFlag = false;
}

// Turn the robot right (tank style)
void right()
{
  dcRun(M1, -speed);
  dcRun(M2, -speed);
  leftFlag = false;
  rightFlag = true;
}

// Reverse and turn left
void backwardLeft()
{
  dcRun(M1, -0.5 * speed);
  dcRun(M2, 1.25 * speed);
  leftFlag = true;
  rightFlag = false;
}

// Reverse and turn right
void backwardRight()
{
  dcRun(M1, -1.25 * speed);
  dcRun(M2, -0.5 * speed);
  leftFlag = false;
  rightFlag = true;
}

void setup() {
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  delay(300);
  digitalWrite(13,LOW);
  Serial.begin(9600);
  delay(500);
  buzzerOn();
  delay(100);
  buzzerOff();
  delay(500);
  randomSeed(analogRead(A4));
  //
  irP6.begin();
  //
  halted = true;
  speed = 150;
  turnDelay = 0.3;
}

void printDelay()
{
  Serial.print("Turn delay: ");
  Serial.print(turnDelay, DEC);
  Serial.println(" sec");
}

void loop() {
  switch(irCode())
  {
    case IR_BUTTON_A:
      halted = false;
      forward();
      Serial.println("Running");
      break;
    case IR_BUTTON_C:
      halt();
      break;
    case IR_BUTTON_B:
      turnDelay = 0.3;
      printDelay();
      break;
    case IR_BUTTON_PLUS:
      turnDelay += 0.1;
      printDelay();
      break;
    case IR_BUTTON_MINUS:
      if (turnDelay > 0)
      {
        turnDelay -= 0.1;
        printDelay();
      }
      break;
  }
  autonomous();
}

void autonomous()
{
  if (halted)
  {
    return;
  }
  distance = usP3.distanceCm();
  //Serial.print("Distance: ");
  //Serial.println(distance, DEC);
  if (distance > 10 && distance < 30)
  {
    if (!leftFlag && !rightFlag)
    {
      lastTime = millis()/1000.0;
      randnum = random(300);
      Serial.print("Distance ");
      Serial.print(distance, DEC);
      if (randnum > 190 && !rightFlag)
      {
        Serial.println(" turning left");
        left();
      }
      else
      {
        Serial.println(" turning right");
        right();
      }
    }
  }
  else if (distance < 10)
  {
    if (!leftFlag && !rightFlag)
    {
      lastTime = millis()/1000.0;
      randnum = random(300);
      Serial.print("Distance ");
      Serial.print(distance, DEC);
      if (randnum > 190 && !rightFlag)
      {
        Serial.println(" reversing left");
        backwardLeft();
      }
      else
      {
        Serial.println(" reversing right");
        backwardRight();
      }
    }
  }
  else
  {
    if (getLastTime() > turnDelay)
    {
      Serial.print("Distance: ");
      Serial.print(distance, DEC);
      Serial.print(" delay: ");
      Serial.print(currentTime - lastTime, DEC);
      Serial.println(" going forward");
      forward();
    }
  }
}
