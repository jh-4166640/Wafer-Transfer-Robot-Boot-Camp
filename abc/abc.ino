#include <PRIZM_PRO.h> // include PRIZM Pro library
PRIZM prizm; // instantiate a PRIZM Pro object “prizm” so we can
#include "AServoCAN.h"

//HardwareSerial mySerial(2);
HardwareSerial mySerial(1);

AServoCAN aservo_can;

int32_t Aservo_actualPosition[4] = {0,};
int32_t Aservo_actualVelocity[4] = {0,};

static int last_axis_number = (int)AServoAxis::J4;

uint8_t Axis4_ServoON(void);
uint8_t Axis4_SetPostionMode(void);
uint8_t Axis4_SetHommingMode(uint8_t last_axis_number);
uint8_t Axis4_Homming(uint8_t last_axis_number);
uint8_t Axis4_Move( int32_t axis1_position, int32_t axis1_velocity, int32_t axis2_position, int32_t axis2_velocity, int32_t axis3_position, int32_t axis3_velocity, int32_t axis4_position, int32_t axis4_velocity);
// put your setup code here, to run once:
uint8_t Axis4_MoveRel(int32_t axis1_position, int32_t axis1_velocity,
                      int32_t axis2_position, int32_t axis2_velocity,
                      int32_t axis3_position, int32_t axis3_velocity,
                      int32_t axis4_position, int32_t axis4_velocity);


void setup() 
{

  Serial.begin(115200);
  prizm.PrizmBegin();

  aservo_can.begin(mySerial, 38400, 15, 16);

  while(!Axis4_SetPostionMode());

}

int32_t arm(int32_t ang)
{
  return (int32_t)(ang/0.00225);
}

int32_t fork(int32_t ang)
{
  return (int32_t)(ang/0.009);
}

int32_t updown(int32_t cm)
{
  return (int32_t)(cm*600000);
}

void loop()
{
  static bool initialized = false;

  if (!initialized)
  {
    Axis4_ActualPosition();
    while (!Axis4_MoveRel(0, 0, -updown(10), 1000, 0,0, 0,0));
    
    /*while (!Axis4_MoveRel(arm(-15), 10, 0, 0, arm(50), 50, fork(-30), 20));
    delay(500);
    while (!Axis4_MoveRel(arm(-5), 10, 0, 0, arm(20), 50, fork(-10), 20));

    while (!Axis4_MoveRel(0, 0, -updown(2), 600, 0, 0, 0, 0));
    delay(500);
    while (!Axis4_MoveRel(arm(5), 10, 0, 0, arm(-20), 50, fork(10), 20));

    while (!Axis4_MoveRel(arm(15), 10, 0, 0, arm(-50), 50, fork(30), 20));*/
    
    initialized = true;
  }

  while (1);
}









/*
 1축(Turn) : 16배율, 원점을 기준으로 1Pulse 당 0.00225도 
 2축(상하) : 12배율, 120,000 Pulse 당 2mm, 
 3축(Arm) : 16배율, 원점을 기준으로 1Pulse 당 0.00225도
 4축(fork) : 4배율, 원점을 기준으로 1Pulse 당 0.009도
*/


void base_move(int speed)
{
  prizm.setMotorSpeeds(speed,-speed);
}

uint8_t Axis4_ActualPosition(void)
{
  bool allReady = true;

  for(int i = 1; i <= last_axis_number; i++)
  {
    allReady &= aservo_can.GetActaulPositionAndVelocity((AServoAxis)i, Aservo_actualPosition[i-1], Aservo_actualVelocity[i-1]); 
  }
  if(allReady) 
  {
    for(int i = 1; i <= last_axis_number; i++)
    {
      Serial.print(i);Serial.print("번 축: ");Serial.print(Aservo_actualPosition[i-1]);
    }
    Serial.println("");
    return 1;
  }
  return 0;
}

uint8_t Axis4_ServoON(void)
{
  bool allReady = true;
  bool allTargetReached = true;
  for(int i = 1; i <= last_axis_number; i++)
  {
    if(aservo_can.ResetFault((AServoAxis)i))
    {
      allReady &=  aservo_can.ServoOn((AServoAxis)i);
    }
  }
  if(allReady) return 1;
  Serial.println("ServoON ReTry");
  return 0;
}


uint8_t Axis4_ServoOFF(void)
{
  bool allReady = true;
  bool allTargetReached = true;
  for(int i = 1; i <= last_axis_number; i++)
  {
    if(aservo_can.ResetFault((AServoAxis)i))
    {
      allReady &=  aservo_can.ServoOff((AServoAxis)i);
    }
  }
  if(allReady) return 1;
  Serial.println("ServoON ReTry");
  return 0;
}

uint8_t Axis4_SetPostionMode(void)
{
  bool allReady = true;
  bool allTargetReached = true;

  for(int i = 1; i <= last_axis_number; i++)
  {
    if(aservo_can.ResetFault((AServoAxis)i))
    {
      Serial.print(1);Serial.println("Fault oK");
      if(aservo_can.SetMode((AServoAxis)i, AServoMode::Position))
      {
        Serial.print(1);Serial.println("Mode oK");
        if(aservo_can.ServoOn((AServoAxis)i))
        {
          allReady &= aservo_can.CanStartMotion((AServoAxis)i, AServoMode::Position);
        }
      }
    }
    else 
    {
      Serial.println("ResetFault Fail");
      if(aservo_can.SetMode((AServoAxis)i, AServoMode::Position))
      {
        Serial.println("##1 SET Servo Position Mode");
        if(aservo_can.ServoOn((AServoAxis)i))
        {
          Serial.println("##2 Servo ON");
          if(aservo_can.ServoOff((AServoAxis)i))
          {
            Serial.println("##3 Servo OFF");
            if(aservo_can.ServoOn((AServoAxis)i))
            {
              Serial.println("##4 Servo ON");
              allReady &= aservo_can.CanStartMotion((AServoAxis)i, AServoMode::Position);
            }
          }
        } 
      }
    }
  }
  if(allReady) 
  {
    Serial.println("SetPostionMode OK");
    return 1;
  }
  Serial.println("SetPostionMode ReTry");
  return 0;
}

uint8_t Axis4_SetHommingMode(uint8_t last_axis_number)
{
  bool allReady = true;
  bool allTargetReached = true;

  for(int i = 1; i <= last_axis_number; i++)
  {
    if(aservo_can.ResetFault((AServoAxis)i))
    {
      Serial.print(1);Serial.println("Fault oK");
      if(aservo_can.SetMode((AServoAxis)i, AServoMode::Homing))
      {
        Serial.print(1);Serial.println("Mode oK");
        if(aservo_can.ServoOn((AServoAxis)i))
        {
          allReady &= aservo_can.CanStartMotion((AServoAxis)i, AServoMode::Homing);
        }
      }
    }
    else 
    {
      Serial.println("ResetFault Fail");
      if(aservo_can.SetMode((AServoAxis)i, AServoMode::Homing))
      {
        Serial.println("##1 SET Servo SetHomming Mode");
        if(aservo_can.ServoOn((AServoAxis)i))
        {
          Serial.println("##2 Servo ON");
          if(aservo_can.ServoOff((AServoAxis)i))
          {
            Serial.println("##3 Servo OFF");
            if(aservo_can.ServoOn((AServoAxis)i))
            {
              Serial.println("##4 Servo ON");
              allReady &= aservo_can.CanStartMotion((AServoAxis)i, AServoMode::Homing);
            }
          }
        } 
      }
    }
  }
  if(allReady) 
  {
   Serial.println("SetHommingMode OK");
    return 1;
  }
  Serial.println("SetHommingMode ReTry");
  return 0;
}

uint8_t Axis4_Homming(uint8_t last_axis_number)
{
  bool allReady = true;
  bool allTargetReached = true;
  for(int i = 1; i <= last_axis_number; i++)
  {
    aservo_can.MoveHomming((AServoAxis)i);
  }
  if(allTargetReached)
  {
    for(int i = 1; i <= last_axis_number; i++)
    {
      allTargetReached &= aservo_can.IsHommingReached((AServoAxis)i);
      Serial.println(aservo_can.IsHommingReached((AServoAxis)i));
    } 
    if(allTargetReached)return 1;
  }
  return 0;
}


uint8_t Axis4_Move( int32_t axis1_position, int32_t axis1_velocity, int32_t axis2_position, int32_t axis2_velocity, int32_t axis3_position, int32_t axis3_velocity, int32_t axis4_position, int32_t axis4_velocity)
{
  bool allReady = true;
  bool allTargetReached = true;
  int32_t axis_position[4] = {axis1_position, axis2_position, axis3_position, axis4_position};
  int32_t axis_velocity[4] = {axis1_velocity, axis2_velocity, axis3_velocity, axis4_velocity};

  for(int i = 1; i <= last_axis_number; i++)
  {
    allTargetReached &= aservo_can.IsTargetReached((AServoAxis)i);
  }
  if(allTargetReached)
  {
    for(int i = 1; i <= last_axis_number; i++)
    {
      aservo_can.MoveAbs((AServoAxis)i, axis_position[i-1], axis_velocity[i-1]);
    }
    return 1;
  }
  return 0;
}
uint8_t Axis4_MoveRel(int32_t axis1_position, int32_t axis1_velocity,
                      int32_t axis2_position, int32_t axis2_velocity,
                      int32_t axis3_position, int32_t axis3_velocity,
                      int32_t axis4_position, int32_t axis4_velocity)
{
  bool allReady = true;
  bool allTargetReached = true;
  int32_t axis_position[4] = {axis1_position, axis2_position, axis3_position, axis4_position};
  int32_t axis_velocity[4] = {axis1_velocity, axis2_velocity, axis3_velocity, axis4_velocity};

  for(int i = 1; i <= last_axis_number; i++)
  {
    allTargetReached &= aservo_can.IsTargetReached((AServoAxis)i);
  }
  if(allTargetReached)
  {
    for(int i = 1; i <= last_axis_number; i++)
    {
      aservo_can.MoveRel((AServoAxis)i, axis_position[i-1], axis_velocity[i-1]);
    }
    return 1;
  }
  return 0;
}
