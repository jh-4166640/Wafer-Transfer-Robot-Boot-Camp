#include <PRIZM_PRO.h>
/* 모든 값의 기준은 cm단위 */
#define   PI              3.14
#define   UNIT_DISTANCE   17       // 모터가 한바퀴 돌 때 이동한 거리 17cm
#define   DEGperSEC       360     
#define   ENCODER_UNIT    1440    // 한바퀴 ENCODER 갑 
#define   DEG_UNIT        360


PRIZM prz;
inline void buzz() { prz.setSoundNote(3000,75); prz.setSoundOff(); }

void setup() {
  
  prz.PrizmBegin();
}

void loop() {

  uint8_t test = 5;
  prz.resetEncoders();
  switch(test){
    case 0:
      
      delay(3000);
      break;
    case 1:
    // 100%에서 delay 1000이면 42cm
    // 100% delay 2000 72.5cm

      for(int i = 0; i<=100;i++)  
      {
        prz.setMotorPowers(i,-i);
        delay(i<=20?4:2);
      }
      delay(2000);
      for(int i = 100; i>=0;i--)  
      {
        prz.setMotorPowers(i,-i);
        delay(i<=20?4:2);
      }
      prz.setMotorPowers(0,0);
      delay(100000);
      test = 100;
      break;
    case 2:
      EStart2(40,30,10,10);
      delay(100000);
      break;
    case 3:
      
      prz.setMotorSpeeds(360,360);
      delay(1000);
      prz.resetEncoders();
      while(prz.getEncoderDegrees(1) <= 360);
      prz.setMotorSpeeds(0,0);
      

      delay(100000);
      break;
    case 4:
      prz.setRedLED(HIGH);
      prz.resetEncoders();
      prz.setMotorSpeeds(160,-160);
      while(1)
      {
        
        if(prz.getEncoderDegrees(1) > 360) break;
      }
      prz.setMotorSpeeds(0,0);
      prz.setRedLED(LOW);
      delay(10000);
      break;

      case 5:
      LineStop(1,2,0.004);
      delay(1000);
      LineStop(-1,2,0.05);
      delay(100000);
      break;
  }
  //prz.PrizmEnd();
}


void EStart(double y, int16_t v) // Encode 제어
{
  prz.resetEncoders();
  double N =  y / UNIT_DISTANCE;
  int16_t dir = y>0? 1:-1;
  int16_t speed = v*DEGperSEC*dir;
  int32_t encoder_val = ENCODER_UNIT*(int32_t)N + (ENCODER_UNIT/10)*(N*10 - ((int32_t)N)*10);
  prz.setMotorTargets(speed, encoder_val, speed, -1*encoder_val);
}

void EStart2(double y, int16_t v, int16_t a, int16_t _a) // Encode 제어
{
  
  double N = (y - a - _a) / UNIT_DISTANCE;
  double a_N = a / UNIT_DISTANCE;
  double _a_N = a / UNIT_DISTANCE;
  //double N =  y / UNIT_DISTANCE;
  int16_t dir = y>0? 1:-1;
  int16_t speed = v*DEGperSEC*dir; // target speed
  int32_t encoder_val = ENCODER_UNIT*(int32_t)N + (ENCODER_UNIT/10)*(N*10 - ((int32_t)N)*10);               // 정상상태 속도에서 갈 거리만큼의 엔코더 값
  int32_t a_encoder_val = ENCODER_UNIT*(int32_t)a_N + (ENCODER_UNIT/10)*(a_N*10 - ((int32_t)a_N)*10);       // 가속 거리만큼의 엔코더 값
  int32_t _a_encoder_val = ENCODER_UNIT*(int32_t)_a_N + (ENCODER_UNIT/10)*(_a_N*10 - ((int32_t)_a_N)*10);   // 감속 거리만큼의 엔코더 값

  int32_t acc_step = a_encoder_val / a;                     // 가속 진행할 엔코더 Step 값
  int32_t _acc_step = _a_encoder_val / _a;                  // 감속 진행할 엔코더 step 값
  int16_t acc_speed_step = speed / a;                       // 가속 step 값
  int16_t _acc_speed_step = speed / _a;                     // 감속 step 값

  int16_t cur_speed = acc_speed_step;                                    // 현재 속도
  int32_t sum_encoder = 0;                                    // 현재 속도
  prz.resetEncoders();
  prz.setMotorSpeeds(cur_speed,-cur_speed);
  while(prz.getEncoderDegrees(1) <= a_encoder_val){
    cur_speed += acc_speed_step;
    prz.setMotorSpeeds(cur_speed,-cur_speed);
    delay(10);
  };
  
  //prz.setMotorSpeeds(speed,-speed);
  //prz.setMotorTargets(speed, encoder_val, speed, -1*encoder_val);
  //prz.resetEncoders();
  //while(prz.getEncoderDegrees(1) <= encoder_val){}
  //cur_speed=speed;
  //prz.resetEncoders();
  // while(prz.getEncoderDegrees(1)< =_a_encoder_val){
  //   cur_speed -= _acc_speed_step;
  //   prz.setMotorSpeeds(cur_speed,-cur_speed);
  //   delay(10);
  // }
  prz.setMotorSpeeds(0,0);
}

void LineStop(int dir, int16_t n, double _at)
{
  int16_t max_p =90;
  if(dir < 0) dir = -1;
  if(dir >= 0) dir = 1;
  int16_t cnt = 0;
  uint32_t stop_t =(uint32_t)(_at*1000);
  for(int i = 0; i<=max_p;i++)  
  {
    prz.setMotorPowers(i*dir,-i*dir);
    delay(i<=20?4:2);
  }
  while(1){
    if(prz.getLineSensor(1)){
      buzz();
      cnt++;
      if(cnt==n) break;
      delay(800);
    }
  }
  delay(stop_t);
  for(int i = max_p; i>=0;i--)  
  {
    prz.setMotorPowers(i*dir,-i*dir);
    delay(i<=20?4:2);
  }
  prz.setMotorPowers(125,125);
}

// void LineStop(int16_t v,int16_t _a)
// {
//   double setdeg = _a * 360 / UNIT_DISTANCE;
//   int16_t delay_time = 500/v;
//   int16_t cur_v = 360*v;
//   for(int16_t i=0; i<=v*2; i++) 
//   {
//         // 속도를 선형 증가 (0 → v)
//         prz.setMotorSpeeds(i*160,-1*i*160); 
//         delay(delay_time);
//   }
//   prz.setMotorSpeeds(v*360, -v*360);
//   while(!prz.getLineSensor(1));
//   prz.setRedLED(HIGH);
//   delay(10);
//   prz.resetEncoders();
//   prz.setRedLED(HIGH);
//   while(prz.getEncoderDegrees(1)<=setdeg)
//   {
//     delay(1);
//   }
//   prz.setRedLED(LOW);

//   //while(prz.getEncoderDegrees(1) > -setdeg);
//   // {
//   //   // prz.setMotorSpeeds(cur_v-360,cur_v-360);  
//   //   // delay(delay_time);
//   //   // cur_v -= 360;
//   //   // if(cur_v <= 5*360) cur_v = 5*360;
//   // }
//   prz.setMotorSpeeds(0,0);
// }