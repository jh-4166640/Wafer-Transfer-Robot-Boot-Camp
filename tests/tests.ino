#include <Adafruit_NeoPixel.h>
#include <PRIZM_PRO.h>
#include <notes.h>
#include <pixelPatterns.h>
//#include <rp2040_pio.h>
#include <soundEffects.h>
#include "AServoCAN.h"
PRIZM prizm; // instantiate a PRIZM Pro object “prizm” so we can
#define DELAY_T 300
#define STOP    while(1)
#define DELAY_LINE 400

HardwareSerial mySerial(1);
AServoCAN aservo_can;

int32_t Aservo_actualPosition[4] = {0,};
int32_t Aservo_actualVelocity[4] = {0,};
static int last_axis_number = (int)AServoAxis::J4;

uint8_t Axis4_ServoON(void);
uint8_t Axis4_SetPostionMode(void);
uint8_t Axis4_SetHommingMode(void);
uint8_t Axis4_Move( int32_t axis1_position, int32_t axis1_velocity, int32_t axis2_position, int32_t axis2_velocity, int32_t axis3_position, int32_t axis3_velocity, int32_t axis4_position, int32_t axis4_velocity);
// put your setup code here, to run once:


inline void buzz() { prizm.setSoundNote(3000,75); prizm.setSoundOff(); }
void lstop(int dir, int16_t n, double _at);

/* -- 왼쪽에 웨이퍼가 X O X O X 에 위치 할 떄--*/
void Lget_even(uint8_t floor);
void L2Lput_even();

/* -- 왼쪽에 웨이퍼가 O X O X O 에 위치 할 떄--*/
void LInit_odd();
void Lget_odd(uint8_t floor);
void L2Lput_odd();  // 왼쪽 라인에서 왼쪽 라인으로만 옮기는 함수

/* -- 오른쪽에 웨이퍼가 X O X O X 에 위치 할 떄--*/
void RInit_even();
void Rget_even(uint8_t floor);
void Rput_even();

inline void armop(int32_t p1, int32_t v1, int32_t p2, int32_t v2, int32_t p3, int32_t v3, int32_t p4, int32_t v4)
{
  while(!Axis4_Move(p1,v1, p2,v2, p3,v3, p4,v4)); 
  delay(DELAY_T);
}


int32_t cur_m1, cur_m2, cur_m3, cur_m4;
void setup() 
{
  Serial.begin(115200);
  prizm.PrizmBegin();
  aservo_can.begin(mySerial, 38400, 15, 16);
  Serial.println("Robowell");
  Serial.println("Start up");
  
  while(!Axis4_SetPostionMode()); // 위치 원점으로 초기화 
  buzz();
  delay(100);
  buzz();
}

void loop() 
{ 
  // armop(0,0,-6000000,1000,0,0,0,0);
  // delay(5000);
  // // armop(0,0,0,0,124000,120,0,0);
  // // delay(5000);
  // // armop(0,0,0,0,0,120,0,0);
  // // delay(5000);

  LEFT_work();
  Right_work();

  STOP;
}

void LEFT_work() // 왼쪽 초록색 트레이들을 빨간색으로 옮긴거
{
  double setLinePosFWRD = 1120;
  double setLinePosBCK = 300;
  
  // 왼쪽 트레이들 이동하기
  lstop(1,1,500);
  delay(DELAY_LINE);
  lstop(-1,1,setLinePosBCK);
  LInit_odd(); // 첫번쨰만
  Lget_odd(5);
  lstop(1,1,setLinePosFWRD);
  delay(DELAY_LINE);
  L2Lput_odd();

  lstop(-1,1,setLinePosBCK);
  Lget_odd(3);
  lstop(1,1,setLinePosFWRD);
  delay(DELAY_LINE);
  L2Lput_odd();
  
  lstop(-1,1,setLinePosBCK);
  Lget_odd(1);
  lstop(1,1,setLinePosFWRD);
  delay(DELAY_LINE);
  L2Lput_odd();
  //////////
  lstop(1,2,500);
  delay(500);
  lstop(-1,1,setLinePosBCK);
  Lget_odd(2);
  lstop(1,1,setLinePosFWRD);
  delay(DELAY_LINE);
  L2Lput_odd();
  
  lstop(-1,1,setLinePosBCK);
  Lget_odd(4);
  lstop(1,1,setLinePosFWRD);
  delay(DELAY_LINE);
  L2Lput_odd();
  
  lstop(-1,1,setLinePosBCK);
  Lget_odd(6);
  lstop(1,1,setLinePosFWRD);
  delay(DELAY_LINE);
  L2Lput_odd();
}
void Right_work()
{
  double setLinePosFWRD = 1100;
  double setLinePosBCK = 150;

  RInit_even();
  Rget_even(6);
  lstop(-1,1,setLinePosBCK);
  delay(400);
  Rput_even();

  lstop(1,1,setLinePosFWRD);
  delay(400);
  Rget_even(4);
  lstop(-1,1,setLinePosBCK);
  delay(400);
  Rput_even();

  lstop(1,1,setLinePosFWRD);
  delay(400);
  Rget_even(2);
  lstop(-1,1,setLinePosBCK);
  delay(400);
  Rput_even();

  lstop(-1,1,200);  
  Rget_even(1);
  lstop(-1,1,setLinePosBCK);
  delay(400);
  Rput_even();

  lstop(1,1,setLinePosFWRD);
  delay(400);
  Rget_even(3);
  lstop(-1,1,setLinePosBCK);
  delay(400);
  Rput_even();

  lstop(1,1,setLinePosFWRD);
  delay(400);
  Rget_even(5);
  lstop(-1,1,setLinePosBCK);
  delay(400);
  Rput_even();
}

void RInit_even()
{
  //int32_t nm1 = 20000,  nm3 = 124000, nm4= 11500;  
  int32_t nm1 = 145000 , nm3 = 124000, nm4 = 23500;
  int32_t nm2 = cur_m2;
  armop(nm1,100, nm2,0, cur_m3,0, cur_m4, 0);
  armop(nm1,0, nm2,0, nm3,150, nm4, 50);

  cur_m1 = nm1;
  cur_m4 = nm4;
  cur_m3 = nm3;
}
void Rput_even()
{
  int32_t nm1 = cur_m1, nm2 = cur_m2, nm3 = cur_m3, nm4 = cur_m4;
  // 밀어넣기 
  nm1 = nm1 + 11000;   
  nm2 = nm2;         
  nm3 = nm3 - 12000;  
  nm4 = nm4;   
  armop(nm1,30, nm2,0, nm3, 20, nm4,5);

  // 더 밀어넣기 
  nm1 = nm1 + 14000;   
  nm2 = nm2;         
  nm3 = nm3 - 21000;  
  nm4 = nm4 + 1000;   
  armop(nm1,20, nm2,0, nm3, 30, nm4, 10);
  
  // 놓기
  nm1 = nm1; //65200
  nm2 = nm2 + 1500000; //
  nm3 = nm3; // 98500
  nm4 = nm4; // 17000
  armop(nm1,0, nm2 ,2000, nm3,0, nm4,0);
  delay(1000);

  // 빼기
  nm1 = nm1 - 14000;   
  nm2 = nm2;         
  nm3 = nm3 + 21000;  
  nm4 = nm4 ;   
  armop(nm1,20, nm2,0, nm3, 30, nm4,0);

  // 더 빼기 
  nm1 = 185000;   
  nm2 = nm2;         
  nm3 = 124000;  
  nm4 = 25000;   
  armop(nm1,30, nm2,0, nm3, 20, nm4,35);
  //int32_t nm1 = 130000 , nm3 = 124000, nm4 = 25000;
  cur_m1 = nm1;
  cur_m2 = nm2;
  cur_m3 = nm3;
  cur_m4 = nm4;
}

void Rget_even(uint8_t floor) // 고정우
{
  int32_t start_m1 = 145000, start_m4 = 25000;
  int32_t start_m2 = 0, start_m3 = 124000;

  if(floor == 6) start_m2 = 1288000;  
  else if(floor == 5) start_m2 = 1288000 + 1350000;
  else if(floor == 4) start_m2 = 4438000;
  else if(floor == 3) start_m2 = 4438000 + 1350000;
  else if(floor == 2) start_m2 = 7350000;
  else if(floor == 1) start_m2 = 7320000 + 1010000;
  int32_t nm1 = start_m1, nm2 = start_m2, nm3 = cur_m3, nm4= start_m4;
  //int32_t nm1 = 130000 , nm3 = 124000, nm4 = 25000;
  // 밀어넣기 
  nm1 = nm1 + 10500;  // 130000 + 10000
  nm2 = nm2;         
  nm3 = nm3 - 3850;  
  nm4 = nm4 - 2300;   
  armop(nm1,30, nm2,2000, nm3, 20, nm4,5);
  delay(5000-DELAY_T);

  // 더 밀어넣기 
  nm1 = nm1 + 16000;    // 17000 -> 16000
  nm2 = nm2;         
  nm3 = nm3 - 26500; 
  nm4 = nm4 + 1600;   
  armop(nm1,20, nm2,0, nm3, 33, nm4, 8);

  // 들기
  nm1 = nm1; //65200
  nm2 = nm2 - 1500000; //
  nm3 = nm3; // 98500
  nm4 = nm4; // 17000
  armop(nm1,0, nm2 ,2000, nm3,0, nm4,0);

  // 빼기
  nm1 = nm1 - 15000;   //16000 -> 17000
  nm2 = nm2;         
  nm3 = nm3 + 26500;  
  nm4 = nm4 - 1600;   
  armop(nm1,20, nm2,0, nm3, 33, nm4,5);

  // 더 빼기 
  nm1 = nm1 - 10500;   
  nm2 = nm2;         
  nm3 = nm3 + 4200;  
  nm4 = nm4 + 2070;   
  armop(nm1,30, nm2,0, nm3, 20, nm4,35);

  cur_m1 = nm1; //128000
  cur_m2 = nm2;
  cur_m3 = nm3;
  cur_m4 = nm4;
}

void LInit_odd()
{
  int32_t nm1 = 20000,  nm3 = 124000, nm4= 13000;
  armop(nm1,80, 0,0, 0,0, 0, 0);

  armop(nm1,0, 0,0, nm3,250, 0,0);

  nm1 = 62000;
  armop(nm1,100, 0,0, nm3,0, 0,0);

  armop(nm1,0, 0,0, nm3,0, nm4,50);

  cur_m1 = nm1;
  cur_m2 = 0;
  cur_m3 = nm3;
  cur_m4 = nm4;
}
void L2Lput_odd()
{
  int32_t nm1 = cur_m1, nm2 = cur_m2;
  int32_t nm3 = cur_m3, nm4 = cur_m4;

  nm1 = nm1-200; //62100
  nm2 = nm2; //
  nm3 = nm3-12000; // 112000
  nm4 = nm4+1400; // 14400
  armop(nm1,20, nm2,0, 112000,80, nm4,20);

  // 완전 넣기
  nm1 = nm1+3150; //65200
  nm2 = nm2; //
  nm3 = nm3-13500; // 98500
  nm4 = nm4+2600; // 17000
  armop(nm1,20, nm2,0, nm3,75, nm4,15);


  // 놓기
  nm1 = nm1; //65200
  nm2 = nm2 + 1500000; //
  nm3 = nm3; // 98500
  nm4 = nm4; // 17000
  armop(nm1,0, nm2 ,2000, nm3,0, nm4,0);
  delay(3000-DELAY_T);

  // 빼기
  nm1 = nm1-3150; //62100
  nm2 = nm2; //
  nm3 = nm3+13500; // 112000
  nm4 = nm4-2600; // 14400
  armop(nm1,25, nm2,0, nm3,75, nm4,15);

  // 완전 빼기
  nm1 = 62000; //63000
  nm2 = nm2; //
  nm3 = 124000; // 124000
  nm4 = 13000; // 11500
  armop(nm1,10, nm2,0, nm3,80, nm4,18);
  //int32_t nm1 = 63350,  nm3 = 124000, nm4= 11500;
  cur_m1 = nm1;
  cur_m2 = nm2;
  cur_m3 = nm3;
  cur_m4 = nm4;

}
void Lget_odd(uint8_t floor)
{
  int32_t start_m1 = 62000, start_m4 = 13000;
  int32_t start_m2 = 0, start_m3 = 124000;
  
  if(floor == 6) start_m2 = 1288000;  
  else if(floor == 5) start_m2 = 1288000 + 1350000;
  else if(floor == 4) start_m2 = 4438000;
  else if(floor == 3) start_m2 = 4438000 + 1350000;
  else if(floor == 2) start_m2 = 7350000;
  else if(floor == 1) start_m2 = 7320000 + 1010000;
  int32_t nm1 = start_m1, nm2 = start_m2, nm3 = start_m3, nm4= start_m4;
  //int32_t nm1 = 63350,  nm3 = 124000, nm4= 11500;

  nm1 = nm1; //63350
  nm2 = nm2; //
  nm3 = nm3; // 124000
  nm4 = nm4; // 11500

  armop(nm1,0, nm2,2000, nm3,0, nm4,0);
  delay(5000-DELAY_T);

  // 넣기 
  nm1 = nm1; //63350 //넣을 때 살짝 오른쪽으로 움직이는거 지움
  nm2 = nm2; //
  nm3 = nm3-12000; // 112000
  nm4 = nm4+3600; // 14400
  armop(nm1,0, nm2,0, 112000,80, nm4,20);

  // 완전 넣기
  nm1 = nm1+2050; //66900
  nm2 = nm2; //
  nm3 = nm3-16500; // 95500
  nm4 = nm4+2800; // 17600
  armop(nm1,20, nm2,0, nm3,75, nm4,15);

  // 들기
  nm1 = nm1; //66900
  nm2 = nm2 - 1500000; //
  nm3 = nm3; // 95500
  nm4 = nm4; // 17600
  armop(nm1,0, nm2 ,2000, nm3,0, nm4,0);

  // 빼기
  nm1 = nm1-2050; //63750
  nm2 = nm2; //
  nm3 = nm3+16500; // 112000
  nm4 = nm4-2800; // 14400
  armop(nm1,25, nm2,0, nm3,75, nm4,15);

  // 완전 빼기
  nm1 = nm1; //64150
  nm2 = nm2; //
  nm3 = nm3+12000; // 124000
  nm4 = nm4-3600; // 11500
  armop(nm1,10, nm2,0, nm3,80, nm4,18);
  //int32_t nm1 = 64000,  nm3 = 124000, nm4= 13000;
  cur_m1 = nm1;
  cur_m2 = nm2;
  cur_m3 = nm3;
  cur_m4 = nm4;
}

void lstop(int dir, int16_t n, double _at) //dir 앞뒤, n읽을 라인 개수, _at 라인읽고 멈출 거리 ms
{
  int16_t max_p =60;
  if(dir < 0)       dir = 1;
  else if(dir >= 0) dir = -1;
  int16_t cnt = 0;
  for(int i = 10; i<=max_p;i++)  
  {
    prizm.setMotorPowers(i*dir,-i*dir);
    delay(3);
  }
  while(1){
    if(prizm.getLineSensor(3)){
      buzz();
      cnt++;
      if(cnt==n) break;
      delay(800);
    }
  }
  delay(_at);
  for(int i = max_p; i>=0;i--)  
  {
    prizm.setMotorPowers(i*dir,-i*dir);
    delay(i>=10?5:20);
  }
  prizm.setMotorPowers(125,125);
  prizm.setMotorPowers(0,0);
  return;
}

// 벨로시티 기준 : RPM 

// 1번 : 기어비 25, 1000 각도 → 호밍 : 포토센서 // 무조건 왼쪽으로 회전 // 검 01
// 2번 : Z축, 1회전에 2mm → 호밍 : 하단으로 이동 // 갈 02 
// 3번 : 기어비 25, 1000 각도 →시작각도 // 빨 03
// 4번 : 기어비 25, 1000 각도 →시작각도 // 주 04


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