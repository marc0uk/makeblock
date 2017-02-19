//
// Autonomous obstacle avoidance driver for the 3-wheeled robot with
// fancier functionality:
// - go to maximum speed when distance is larger than threshold
// - slow down to default speed when distance is shorter than threshold
// - avoidance maneuver involves a 90 degree random turn left/right 
//
// Usage:
//   A: start the robot
//   B: randomly turn left/right for up to 10 seconds
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

//---- Program constances ----
// Default speed
const int16_t defaultSpeed = 100;

// Number of milliseconds for a turn
const unsigned long turnMillis = 1300;

// Minimum distance for the fast speed
const double distanceSpeedThreshold = 100;

// Minimum distance to continue moving forward
const double distanceTurnThreshold = 50;

// Halting distance
const double distanceHaltThreshold = 10;

// ---- State for the autonomous mode ----
// Current motor speed
int16_t speed;

// Distance measured by usP3
double distance;

// True when the robot is moving forward
bool forwards

// True when the robot is turning
bool turning;

// Time since the start of turning maneuver
unsigned long previousMillis = 0;

// Number of millisecond for the special maneuver
unsigned long maneuverMillis = 0;

void forward()
{
  turning = false;
  dcRun(M1, speed);
  dcRun(M2, -speed);
  forwards = true
}

void slow() {
  speed = defaultSpeed;
  forward();
}

void fast() {
  speed = 255;
  forward();
}

void halt() {
  speed = 0;
  dcRun(M1, 0);
  dcRun(M2, 0);
  forwards = false;
  turning = false;
}

void left()
{
  previousMillis = millis();
  forwards = false;
  turning = true;
  dcRun(M1, -defaultSpeed);
  dcRun(M2, 0);
}

void right()
{
  previousMillis = millis();
  forwards = false;
  turning = true;
  dcRun(M1, 0);
  dcRun(M2, defaultSpeed);
}

void turn(ulong durationMillis)
{
  maneuverMillis = durationMillis
  if (random(1, 101) < 50) {
    left();
  } else {
    right();
  }
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

  // Seed the random number generator
  randomSeed(analogRead(A4));
  
  // Start IR sensor
  irP6.begin();
  
  // User-defined parameters
  speed = 75;
}

void loop() {
  switch(irCode())
  {
    case IR_BUTTON_A:
      forward();
      break;
    case IR_BUTTON_C:
      halt();
      break;
    case IR_BUTTON_B:
      turn(random(50,5000));
      break;
  }
  autonomous();
}

void autonomous()
{
  if (turning) {
    evaluateManeuver();
  } else {
    distance = usP3.distanceCm();
    if ((distance > max_speed_distance) && forwards) {
      fast();
    }
    if ((distance < max_speed_distance) && forwards) {
      slow();
    }
    if (distance < turn_distance && distance > halt_distance) {
      turn(turnMillis); 
    }
    if (distance < halt_distance) {
      halt();
    }
  }
}

// Decide whether the (turn) maneuver should be completed
void evaluateManeuver()
{
  if (millis() - previousMillis) >= maneuverMillis) {
    // We have turned for the requested amount
    previousMillis = 0;
    maneuverMillies = 0;
    forward();
  }
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

