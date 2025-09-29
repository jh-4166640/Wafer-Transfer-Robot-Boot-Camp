#include <PRIZM_PRO.h> // include PRIZM Pro library
PRIZM prizm; // instantiate a PRIZM Pro object “prizm” so we can
#include "AServoCAN.h"

HardwareSerial mySerial(1);
AServoCAN aservo_can;

//static int last_axis_number = (int)AServoAxis::J4;
static int last_axis_number = (int)AServoAxis::J1;


uint8_t Axis4_ServoON(void);
uint8_t Axis4_SetPostionMode(void);
uint8_t Axis4_SetHommingMode(void);
uint8_t Axis4_Move( int32_t axis1_position, int32_t axis1_velocity, int32_t axis2_position, int32_t axis2_velocity, int32_t axis3_position, int32_t axis3_velocity, int32_t axis4_position, int32_t axis4_velocity);
// put your setup code here, to run once:

void setup() 
{

  Serial.begin(115200);
  prizm.PrizmBegin();
  aservo_can.begin(mySerial, 38400, 15, 16);
  Serial.println("Robowell");


  Serial.println("Start up");
  
  while(!Axis4_SetPostionMode()); // 위치 원점으로 이동
//  Axis4_SetHommingMode(); // 모든 원점 이동
//  Axis_Homming(1); // 개별 호밍

}

  // put your main code here, to run repeatedly:
void loop() 
{
  
  while(!Axis4_Move(0,0, 0,0, 0,0, 0,0));
  while(!Axis4_Move(10000,60, 0,0, 0,0, 0,0));
    delay(1000);
  while(!Axis4_Move(0,60, 0,0, 0,0, 0,0));


}
