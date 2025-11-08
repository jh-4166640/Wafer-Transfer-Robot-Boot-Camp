#include <Adafruit_NeoPixel.h>
#include <PRIZM_PRO.h>
#include <notes.h>
#include <pixelPatterns.h>
//#include <rp2040_pio.h>
#include <soundEffects.h>

#include <PRIZM_PRO.h> // include PRIZM Pro library
PRIZM prizm; // instantiate a PRIZM Pro object “prizm” so we can
#include "AServoCAN.h"



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
void Lget_odd(uint8_t floor);
void L2Lput_odd();


int32_t cur_m1, cur_m2, cur_m3, cur_m4;
void setup() 
{
  Serial.begin(115200);
  prizm.PrizmBegin();
  aservo_can.begin(mySerial, 38400, 15, 16);
  Serial.println("Robowell");
  Serial.println("Start up");
  
  while(!Axis4_SetPostionMode()); // 위치 원점으로 초기화 
}

/* 왼쪽 웨이퍼 꺼내기 위한 세팅 동작
double setLinePos = 370;
  lstop(1,1,500);
  delay(1000);
  lstop(-1,1,setLinePos);
  Lget_x();
  lstop(1,2,500);
  delay(1000);
  lstop(-1,1,setLinePos);
  Lput_x();
*/
void loop() 
{ 
  
  double setLinePos = 370;
  lstop(1,1,500);
  delay(1000);
  lstop(-1,1,setLinePos);
  while(!Axis4_Move(20000,100, 0,100, 0,100, 0, 100)); 
  Lget_odd(1);
  lstop(1,2,500);
  delay(1000);
  lstop(-1,1,setLinePos);
  L2Lput_odd();
}
void L2Lput_odd()
{
  int32_t nm1 = cur_m1, nm2 = cur_m2;
  int32_t nm3 = cur_m3, nm4 = cur_m4;

  nm1 = nm1-900; //62100
  nm2 = nm2; //
  nm3 = nm3-12000; // 112000
  nm4 = nm4+2900; // 14400
  while(!Axis4_Move(nm1,60, nm2,0, 112000,80, nm4,20)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  // 완전 넣기
  nm1 = nm1+3100; //65200
  nm2 = nm2; //
  nm3 = nm3-13500; // 98500
  nm4 = nm4+2600; // 17000
  while(!Axis4_Move(nm1,20, nm2,0, nm3,75, nm4,15)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);


  // 놓기
  nm1 = nm1; //65200
  nm2 = nm2 + 1200500; //
  nm3 = nm3; // 98500
  nm4 = nm4; // 17000

  while(!Axis4_Move(nm1,0, nm2 ,1000, nm3,0, nm4,0));
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(8000);

  // 빼기
  nm1 = nm1-3100; //62100
  nm2 = nm2; //
  nm3 = nm3+13500; // 112000
  nm4 = nm4-2600; // 14400


  while(!Axis4_Move(nm1,25, nm2,0, nm3,75, nm4,15)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  // 완전 빼기
  nm1 = nm1+900; //63000
  nm2 = nm2; //
  nm3 = nm3+12000; // 124000
  nm4 = nm4-2900; // 11500

  while(!Axis4_Move(nm1,10, nm2,0, nm3,80, nm4,18)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  cur_m1 = nm1;
  cur_m2 = nm2;
  cur_m3 = nm3;
  cur_m4 = nm4;

}
void Lget_odd(uint8_t floor)
{
  int32_t start_m1 = 20000, start_m4 = 11500;
  int32_t start_m2 = 0, start_m3 = 124000;
  if(floor == 1) start_m2 = 1249000;
  else if(floor == 2) start_m2 = 4399000;
  else if(floor == 3) start_m2 = 7299000;
  int32_t nm1 = start_m1, nm2 = start_m2, nm3 = start_m3, nm4= start_m4;

  while(!Axis4_Move(nm1,100, 0,0, 0,0, 0, 0)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);
  

  while(!Axis4_Move(nm1,0, 0,0, nm3,250, 0,0)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  nm1 = 63000;
  while(!Axis4_Move(nm1,100, 0,0, nm3,0, 0,0)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  nm1 = nm1; //63000
  nm2 = nm2; //
  nm3 = nm3; // 124000
  nm4 = nm4; // 11500
  while(!Axis4_Move(nm1,0, nm2,1000, nm3,0, nm4,50)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(8000);

  // 넣기 
  nm1 = nm1-900; //62100
  nm2 = nm2; //
  nm3 = nm3-12000; // 112000
  nm4 = nm4+2900; // 14400
  while(!Axis4_Move(nm1,60, nm2,0, 112000,80, nm4,20)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  // 완전 넣기
  nm1 = nm1+3100; //65200
  nm2 = nm2; //
  nm3 = nm3-13500; // 98500
  nm4 = nm4+2600; // 17000
  while(!Axis4_Move(nm1,20, nm2,0, nm3,75, nm4,15)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  // 들기
  nm1 = nm1; //65200
  nm2 = nm2 - 1200000; //
  nm3 = nm3; // 98500
  nm4 = nm4; // 17000

  while(!Axis4_Move(nm1,0, nm2 ,1000, nm3,0, nm4,0));
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(8000);

  // 빼기
  nm1 = nm1-3100; //62100
  nm2 = nm2; //
  nm3 = nm3+13500; // 112000
  nm4 = nm4-2600; // 14400


  while(!Axis4_Move(nm1,25, nm2,0, nm3,75, nm4,15)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  // 완전 빼기
  nm1 = nm1+900; //63000
  nm2 = nm2; //
  nm3 = nm3+12000; // 124000
  nm4 = nm4-2900; // 11500

  while(!Axis4_Move(nm1,10, nm2,0, nm3,80, nm4,18)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  cur_m1 = nm1;
  cur_m2 = nm2;
  cur_m3 = nm3;
  cur_m4 = nm4;
}

void L2Lput_even()
{
  int32_t nm1 = cur_m1, nm2 = cur_m2;
  int32_t nm3 = cur_m3, nm4 = cur_m4;

  nm1 = nm1 - 1700;   // 61300
  nm2 = nm2;          // 
  nm3 = nm3 + 15000;  // 15000
  nm4 = nm4 - 3000;   // 34500

  while(!Axis4_Move(nm1,9, nm2,0, nm3, 65, nm4,15)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  nm1 = nm1 - 2000;   // 59300
  nm2 = nm2;          //  
  nm3 = nm3 + 11000;  // 26000
  nm4 = nm4 - 1900;   // 32400

  while(!Axis4_Move(nm1,15, nm2,0, nm3,60, nm4,10));
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  // 놓기
  nm1 = nm1;   // 59300
  nm2 = nm2 + 1200000;          //  
  nm3 = nm3;  // 26000
  nm4 = nm4;   // 32400

  while(!Axis4_Move(nm1,0, nm2 ,1000, nm3,0, nm4,0));
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(8000);


  // 뺴기

  nm1 = nm1 + 2200;   // 61500
  nm2 = nm2;          //  
  nm3 = nm3 - 11000;  // 15000
  nm4 = nm4 + 2100;   // 34500

  while(!Axis4_Move(nm1,11, nm2,0, nm3,50, nm4,11));
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  // 완전뺴기

  nm1 = nm1 + 1500;   // 63000
  nm2 = nm2;          //  
  nm3 = nm3 - 15000;  // 0
  nm4 = nm4 + 3000;   // 37500


  while(!Axis4_Move(nm1,20, nm2,0, nm3,50, nm4,8)); // end
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  cur_m1 = nm1;
  cur_m2 = nm2;
  cur_m3 = nm3;
  cur_m4 = nm4;
}

void Lget_even(uint8_t floor)
{
  int32_t start_m1 = 63000, start_m4 = 37500;
  int32_t start_m2 = 0, start_m3 = 0;
  if(floor == 1) start_m2 = 1249000;
  else if(floor == 2) start_m2 = 4399000;
  else if(floor == 3) start_m2 = 7299000;
  int32_t nm1 = start_m1, nm2 = start_m2, nm3 = start_m3, nm4= start_m4;
  
  while(!Axis4_Move(nm1, 80, 0,0, 0,0, nm4, 150));
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  while(!Axis4_Move(nm1,0, nm2 ,1000, 0,0, nm4,0));
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(8000);

  // 밀어넣기 
  nm1 = nm1 - 2000;   // 61000
  nm2 = nm2;          //  
  nm3 = nm3 + 15000;  // 15000
  nm4 = nm4 - 3000;   // 34500

  while(!Axis4_Move(nm1,9, nm2,0, nm3, 65, nm4,15)); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  // 더 밀어넣기
  nm1 = nm1 - 2000;   // 59000
  nm2 = nm2;          //  
  nm3 = nm3 + 11000;  // 26000
  nm4 = nm4 - 2100;   // 32600

  while(!Axis4_Move(nm1,15, nm2,0, nm3,60, nm4,10));
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  // 들기
  nm1 = nm1;   // 59000
  nm2 = nm2 - 1200000;          //  
  nm3 = nm3;  // 26000
  nm4 = nm4;   // 32600

  while(!Axis4_Move(nm1,0, nm2 ,1000, nm3,0, nm4,0));
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(8000);

  // 뺴기

  nm1 = nm1 + 2500;   // 61500
  nm2 = nm2;          //  
  nm3 = nm3 - 11000;  // 15000
  nm4 = nm4 + 1900;   // 34500

  while(!Axis4_Move(nm1,11, nm2,0, nm3,50, nm4,11));
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

  // 완전뺴기

  nm1 = nm1 + 1500;   // 63000
  nm2 = nm2;          //  
  nm3 = nm3 - 15000;  // 0
  nm4 = nm4 + 3000;   // 37500


  while(!Axis4_Move(nm1,20, nm2,0, nm3,50, nm4,8)); // end
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)4, Aservo_actualPosition[0], Aservo_actualVelocity[0])); 
  while(!aservo_can.GetActaulPositionAndVelocity((AServoAxis)3, Aservo_actualPosition[1], Aservo_actualVelocity[1])); 
  Serial.println(Aservo_actualPosition[0]);
  Serial.println(Aservo_actualPosition[1]);
  delay(3000);

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
    delay(5);
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