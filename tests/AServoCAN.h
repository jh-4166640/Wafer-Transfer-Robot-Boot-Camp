#pragma once

#include "SerialToCAN.h"
#include "Arduino.h"

#define AXIS_J1  1
#define AXIS_J2  2
#define AXIS_J3  3
#define AXIS_J4  4

enum class AServoAxis : uint8_t
{
  J1 = 1,
  J2 = 2,
  J3 = 3,
  J4 = 4,
};

enum class AServoCANError : int8_t
{
  NoError = 0,
  AbortCode = 1,
  Timeout = 2,
};

enum class AServoMode : uint8_t
{
  Unknown = 0,
  Velocity = 1,
  Position = 2,
  Homing = 3,
};

class AServoCAN
{
  public:
    //void begin(int can_tx_pin, int can_rx_pin, unsigned long baud);
    //void begin(SoftwareSerial& serial, unsigned long baud);
    void begin(HardwareSerial &serial, unsigned long baud, int rxPin, int txPin);
    //void begin(HardwareSerial& serial, unsigned long baud);
  
    bool ServoOn(AServoAxis axis);  // 작동 ON 
    bool ServoOff(AServoAxis axis); // 작동 OFF
    bool ResetFault(AServoAxis axis); // 에러 리셋
    bool SetMode(AServoAxis axis, AServoMode mode); //  1번 : 포지션, 3번 : 파워회전, 6번 : 호밍
    AServoMode GetMode(AServoAxis axis); // 현재 모드 상태

    bool CanStartMotion(AServoAxis axis, AServoMode mode);

    bool Stop(AServoAxis axis);
    bool Speed(AServoAxis axis, int32_t velocity);
    
    bool MoveHomming(AServoAxis axis);

    bool MoveAbs(AServoAxis axis, int32_t position);
    bool MoveAbs(AServoAxis axis, int32_t position, int32_t velocity);

    bool MoveRel(AServoAxis axis, int32_t position);
    bool MoveRel(AServoAxis axis, int32_t position, int32_t velocity);

    bool IsTargetReached(AServoAxis axis);
    bool IsHommingReached(AServoAxis axis);

    bool GetActualPosition(AServoAxis axis, int32_t& actualPosition);
    bool GetActualVelocity(AServoAxis axis, int32_t& actualVelocity);
    bool GetActaulPositionAndVelocity(AServoAxis axis, int32_t& actualPosition, int32_t& actualVelocity);
    
  private:
    const uint8_t _retry_count = 3;
    const uint8_t _timeout_ms = 100;
    int32_t _controlword[5] = {0, };
    int32_t _statusword[5] = {0, };
    Serial_CAN  _can;

    bool GetStatusWord(AServoAxis axis, int32_t& value);

    bool SetControlWord(AServoAxis axis, int32_t value);
    bool AndControlWord(AServoAxis axis, int32_t mask);
    bool OrControlWord(AServoAxis axis, int32_t mask);
    bool XorControlWord(AServoAxis axis, int32_t mask);

    bool IsValid(AServoAxis axis);
    
    AServoCANError SetParameter(uint8_t node_id, uint16_t index, uint8_t subIndex, const int32_t value);
    AServoCANError GetParameter(uint8_t node_id, uint16_t index, uint8_t subIndex, int32_t& value);
};