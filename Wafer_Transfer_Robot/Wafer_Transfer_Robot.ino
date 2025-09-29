#include <PRIZM_PRO.h> // include PRIZM Pro library
PRIZM prz; // instantiate a PRIZM Pro object “prizm” so we can
#include "AServoCAN.h"


#define   PI              3.14
#define   UNIT_DISTANCE   17       // 모터가 한바퀴 돌 때 이동한 거리 17cm
#define   DEGperSEC       360     
#define   ENCODER_UNIT    1440    // 한바퀴 ENCODER 갑 
#define   DEG_UNIT        360

#define rgt1              1
#define rgt2              1
#define rgt3              1
#define rgt4              1



HardwareSerial mySerial(1);
AServoCAN aservo_can;

static int last_axis_number = (int)AServoAxis::J4;

uint8_t Axis4_ServoON(void);
uint8_t Axis4_SetPostionMode(void);
uint8_t Axis4_SetHommingMode(void);
uint8_t Axis4_Move( int32_t axis1_position, int32_t axis1_velocity, int32_t axis2_position, int32_t axis2_velocity, int32_t axis3_position, int32_t axis3_velocity, int32_t axis4_position, int32_t axis4_velocity);
// put your setup code here, to run once:

inline void buzz() { prz.setSoundNote(3000,75); prz.setSoundOff(); }
void lstop(int dir, int16_t n, double _at);


void setup() 
{

  Serial.begin(115200);
  prz.PrizmBegin();
  aservo_can.begin(mySerial, 38400, 15, 16);
  Serial.println("Robowell");
  Serial.println("Start up");
  
  while(!Axis4_SetPostionMode()); // 위치 원점으로 초기화 
}

  // put your main code here, to run repeatedly:
void loop() 
{
  buzz();
  int16_t mcp = 0;
  int16_t continues = 0;
  switch(mcp)
  {
  case -4:
    break;
  case -3:
    break;
  case -2:
    while(!Axis4_Move(70000,250,0,0,0,0,0,0));
    buzz();
    while(!Axis4_Move(140000,250,0,0,0,0,0,0));

    break;
  case -1:
    while(!Axis4_Move(140000,250,0,0,0,0,0,0));
    break;

  case 0:
    
    while(!Axis4_Move(70000,250,0,0,250000,180,62000,60));
    //while(!Axis4_Move(125000,250,0,0,140000,0,62500,0));
     buzz();
      
    while(!Axis4_Move(130000,250,0,0,0,0,0,0));
    buzz();
    buzz();

    //delay(1000);
    
    if(continues == 1) mcp++;
    else prz.PrizmEnd();
    break;
  case 1:
   
    

    if(continues == 1) mcp++;
    else prz.PrizmEnd();
    break;
  case 2:

    

    if(continues == 1) mcp++;
    else prz.PrizmEnd();
    break;
  case 3:


    if(continues == 1) mcp++;
    else prz.PrizmEnd();
    break;
  }
  

}


void lstop(int dir, int16_t n, double _at) //dir 앞뒤, n읽을 라인 개수, _at 라인읽고 멈출 거리 ms
{
  int16_t max_p =25;
  if(dir < 0) dir = -1;
  if(dir >= 0) dir = 1;
  int16_t cnt = 0;
  for(int i = 0; i<=max_p;i++)  
  {
    prz.setMotorPowers(i*dir,-i*dir);
    delay(i<=10?40:20);
  }
  while(1){
    if(prz.getLineSensor(1)){
      buzz();
      cnt++;
      if(cnt==n) break;
      delay(800);
    }
  }
  delay(_at);
  for(int i = max_p; i>=0;i--)  
  {
    prz.setMotorPowers(i*dir,-i*dir);
    delay(i>=10?20:40);
  }
  prz.setMotorPowers(125,125);
  prz.setMotorPowers(0,0);
}

// 벨로시티 기준 : RPM 

// 1번 : 기어비 25, 1000 각도 → 호밍 : 포토센서 // 무조건 왼쪽으로 회전 // 검 01
// 2번 : Z축, 1회전에 2mm → 호밍 : 하단으로 이동 // 갈 02 
// 3번 : 기어비 25, 1000 각도 →시작각도 // 빨 03
// 4번 : 기어비 25, 1000 각도 →시작각도 // 주 04


uint8_t Axis4_ServoReadyCheck(void)
{
  // bool allReady = true;
  // bool allTargetReached = true;
  // for(int i = 1; i <= last_axis_number; i++)
  // {
  //    allReady &= aservo_can.CanStartMotion((AServoAxis)i, AServoMode::Position);
  // }
  // if(allReady) return 1;
  // Serial.println("ServoCheck ReTry");
  // return 0;
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

uint8_t Axis4_SetHommingMode(void)
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


uint8_t Axis4_Homming(void)
{
  bool allReady = true;
  bool allTargetReached = true;
  for(int i = 1; i <= last_axis_number; i++)
  {
    aservo_can.MoveHomming((AServoAxis)i);
  }
  while(1)
  {
    for(int i = 1; i <= last_axis_number; i++)
    {
      allTargetReached &= aservo_can.IsHommingReached((AServoAxis)i);
    } 
    if(allTargetReached)return 1;
  }  
  return 0;
}

uint8_t Axis_Homming(int nunber)
{
  bool allReady = true;
  bool allTargetReached = true;
    aservo_can.MoveHomming((AServoAxis)nunber);
  while(1)
  {
      allTargetReached &= aservo_can.IsHommingReached((AServoAxis)nunber);
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
