#include "AServoCAN.h"
#include "SerialToCAN.h"
//#include <SoftwareSerial.h>
#include <stdio.h>
#include <stdint.h>

#define	CIA_402_CONTROL_WORD								0x6040
#define	CIA_402_SWITCH_ON										0x0001
#define	CIA_402_ENABLE_VOLTAGE							0x0002
#define	CIA_402_QUICK_STOP									0x0004
#define	CIA_402_ENABLE_OPERATION						0x0008
#define	CIA_402_FAULT_RESET									0x0080
#define	CIA_402_HALT_BIT						        0x0100
#define	CIA_402_HOMING_START				        0x0010			//	Homing Mode
#define	CIA_402_NEW_SETPOINT				        0x0010			//	Profile Position Mode
#define	CIA_402_RELATIVE_BIT							  0x0040

#define	CIA_402_STATUS_WORD									0x6041	
#define	CIA_402_STATUS_OPERATION_ENABLED		0x0027
#define	CIA_402_TARGET_REACHED						  0x0400
#define	CIA_402_STATUS_SET_POINT_ACK				0x1000

#define	CIA_402_TARGET_POSITION							0x607A
#define	CIA_402_TARGET_VELOCITY							0x60FF
#define	CIA_402_PROFILE_VELOCITY						0x6081	
#define	CIA_402_MODES_OF_OPERATION					0x6060
#define	CIA_402_MODES_OF_OPERATION_DISPLAY	0x6061
#define	CIA_402_ACTUAL_VELOCITY							0x606C
#define	CIA_402_ACTUAL_POSITION							0x6064


#define CIA_402_HOMING_COMPLETED						0x609C         //   Unsigned 8bit,   RO         (0 : Homing이 필요함, 1 : Homing 정상 완료)

#define	HOMING_STATUS_COMPLETE								0x08
#define	CIA_402_STATUS_HOMING_ERROR						0x2000			//	Homing Error
#define	CIA_402_STATUS_HOMING_ATTAINED				0x1000			//	Homing Mode

#define debug_mode 1

#define dbg_println(...) do { if (debug_mode) Serial.println(__VA_ARGS__); } while(0)
#define dbg_print(...)   do { if (debug_mode) Serial.print(__VA_ARGS__); } while(0)

void AServoCAN::begin(HardwareSerial &serial, unsigned long baud, int rxPin, int txPin) 
{
 // begin(HardwareSerial &serial, unsigned long baud, int rxPin, int txPin) 
   _can.begin(serial, baud, rxPin, txPin);
    
}

// void AServoCAN::begin(SoftwareSerial& serial, unsigned long baud)
// {
//     _can.begin(serial, baud);
// }

// void AServoCAN::begin(HardwareSerial& serial, unsigned long baud)
// {
//     _can.begin(serial, baud);
// }

bool AServoCAN::IsValid(AServoAxis axis)
{
  switch(axis)
  {
    case AServoAxis::J1 :
    case AServoAxis::J2 :
    case AServoAxis::J3 :
    case AServoAxis::J4 :
      return true;

    default :
      return false;
  }
}

bool AServoCAN::GetStatusWord(AServoAxis axis, int32_t& value)
{
  if(!IsValid(axis)) return false;
  if(GetParameter((uint8_t)axis, CIA_402_STATUS_WORD, 0, value) == AServoCANError::NoError)
  {
    _statusword[(uint8_t)axis] = value;
    return true;
  }
  return false;
}

bool AServoCAN::SetControlWord(AServoAxis axis, int32_t value)
{
  if(!IsValid(axis)) return false;
  if(SetParameter((uint8_t)axis, CIA_402_CONTROL_WORD, 0, value) == AServoCANError::NoError)
  {
    _controlword[(uint8_t)axis] = value;
    return true;
  }
  return false;
}

bool AServoCAN::AndControlWord(AServoAxis axis, int32_t mask)
{
  if(!IsValid(axis)) return false;
  int32_t value = _controlword[(uint8_t)axis] & mask;
  return SetControlWord(axis, value);
}

bool AServoCAN::OrControlWord(AServoAxis axis, int32_t mask)
{
  if(!IsValid(axis)) return false;
  int32_t value = _controlword[(uint8_t)axis] | mask;
  return SetControlWord(axis, value);
}

bool AServoCAN::XorControlWord(AServoAxis axis, int32_t mask)
{
  if(!IsValid(axis)) return false;
  int32_t value = _controlword[(uint8_t)axis] ^ mask;
  return SetControlWord(axis, value);
}

bool AServoCAN::ServoOn(AServoAxis axis)
{
  if(!SetControlWord(axis, CIA_402_ENABLE_VOLTAGE | CIA_402_QUICK_STOP)) return false;
  if(!OrControlWord(axis, CIA_402_SWITCH_ON)) return false;
  if(!OrControlWord(axis, CIA_402_ENABLE_OPERATION)) return false;
  return true;
}

bool AServoCAN::ServoOff(AServoAxis axis)
{
  return SetControlWord(axis, 0);
}

bool AServoCAN::ResetFault(AServoAxis axis)
{
  return OrControlWord(axis, CIA_402_FAULT_RESET);
}

bool AServoCAN::SetMode(AServoAxis axis, AServoMode mode)
{
  int32_t value = 0;
  switch(mode)
  {
    case AServoMode::Velocity :
      value = 3;
      break;
      
    case AServoMode::Position : 
      value = 1;
      break;
      
    case AServoMode::Homing : 
      value = 6;
      break;

    default :
      return false;
  }

  if(SetParameter((uint8_t)axis, CIA_402_MODES_OF_OPERATION, 0, value) != AServoCANError::NoError) return false;
  return true;
}

AServoMode AServoCAN::GetMode(AServoAxis axis)
{
  int32_t value = 0;
  if(GetParameter((uint8_t)axis, CIA_402_MODES_OF_OPERATION_DISPLAY, 0, value) != AServoCANError::NoError) return AServoMode::Unknown;

  if(value == 3) return AServoMode::Velocity;
  else if(value == 1) return AServoMode::Position;
  else if(value == 6) return AServoMode::Homing;
  else return AServoMode::Unknown;
}

bool AServoCAN::CanStartMotion(AServoAxis axis, AServoMode mode)
{
  int32_t value = 0;
  if(GetMode(axis) != mode) return false;
  if(!GetStatusWord(axis, value)) return false;
  if(value & CIA_402_STATUS_OPERATION_ENABLED != CIA_402_STATUS_OPERATION_ENABLED ) return false;
  return true;
}

bool AServoCAN::Stop(AServoAxis axis)
{
  return OrControlWord(axis, CIA_402_HALT_BIT);
}

bool AServoCAN::Speed(AServoAxis axis, int32_t velocity)
{
  if(SetParameter((uint8_t)axis, CIA_402_TARGET_VELOCITY, 0, velocity) != AServoCANError::NoError) return false;
  return true;
}


bool AServoCAN::MoveHomming(AServoAxis axis)
{
  
  if(!AndControlWord(axis, ~(CIA_402_FAULT_RESET | CIA_402_HOMING_START | CIA_402_HALT_BIT | CIA_402_RELATIVE_BIT))) return false;
  if(!OrControlWord(axis, CIA_402_HOMING_START)) return false;
  if(!AndControlWord(axis, ~CIA_402_HOMING_START)) return false;
  return true;
}

bool AServoCAN::MoveAbs(AServoAxis axis, int32_t position)
{
  if(SetParameter((uint8_t)axis, CIA_402_TARGET_POSITION, 0, position) != AServoCANError::NoError) return false;
  if(!AndControlWord(axis, ~(CIA_402_FAULT_RESET | CIA_402_NEW_SETPOINT | CIA_402_HALT_BIT | CIA_402_RELATIVE_BIT))) return false;
  if(!OrControlWord(axis, CIA_402_NEW_SETPOINT)) return false;
  if(!AndControlWord(axis, ~CIA_402_NEW_SETPOINT)) return false;
  return true;
}

bool AServoCAN::MoveAbs(AServoAxis axis, int32_t position, int32_t velocity)
{
  if(SetParameter((uint8_t)axis, CIA_402_PROFILE_VELOCITY, 0, velocity) != AServoCANError::NoError) return false;
  return MoveAbs(axis, position);
}

bool AServoCAN::MoveRel(AServoAxis axis, int32_t position)
{
  if(SetParameter((uint8_t)axis, CIA_402_TARGET_POSITION, 0, position) != AServoCANError::NoError) return false;
  if(!AndControlWord(axis, ~(CIA_402_FAULT_RESET | CIA_402_NEW_SETPOINT | CIA_402_HALT_BIT))) return false;
  if(!OrControlWord(axis, CIA_402_NEW_SETPOINT | CIA_402_RELATIVE_BIT)) return false;
  if(!AndControlWord(axis, ~CIA_402_NEW_SETPOINT)) return false;
  return true;
}

bool AServoCAN::MoveRel(AServoAxis axis, int32_t position, int32_t velocity)
{
  if(SetParameter((uint8_t)axis, CIA_402_PROFILE_VELOCITY, 0, velocity) != AServoCANError::NoError) return false;
  return MoveRel(axis, position);
}

bool AServoCAN::GetActualVelocity(AServoAxis axis, int32_t& actualVelocity)
{
  if(GetParameter((uint8_t)axis, CIA_402_ACTUAL_VELOCITY, 0, actualVelocity) != AServoCANError::NoError) return false;
  return true;
}

bool AServoCAN::GetActualPosition(AServoAxis axis, int32_t& actualPosition)
{
  if(GetParameter((uint8_t)axis, CIA_402_ACTUAL_POSITION, 0, actualPosition) != AServoCANError::NoError) return false;
  return true;
}

bool AServoCAN::IsTargetReached(AServoAxis axis)
{
  int32_t value = 0;
  if(!GetStatusWord(axis, value)) return false;
  return ((value & CIA_402_TARGET_REACHED) == CIA_402_TARGET_REACHED && (value & CIA_402_STATUS_SET_POINT_ACK) != CIA_402_STATUS_SET_POINT_ACK);
}


bool AServoCAN::IsHommingReached(AServoAxis axis)
{
  // int32_t value = 0;
  // if(!GetStatusWord(axis, value)) return false;
  // return ((value & CIA_402_HOMING_MODE) == CIA_402_HOMING_MODE && (value & CIA_402_CONTROL_ENABLE_OPERATION) != CIA_402_CONTROL_ENABLE_OPERATION);
  int32_t value = 0;
  if (!GetStatusWord(axis, value)) return false;
  // int32_t value = CIA_402_HOMING_MODE;
	// if (!GetStatusWord(axis, value)) return false;
	
	// value = CIA_402_CONTROL_ENABLE_OPERATION;
	// if (!GetStatusWord(axis, value)) return false;

  // value = CIA_402_STATUS_HOMING_ERROR;
	// if (!GetStatusWord(axis, value)) return false;

	// value = CIA_402_CONTROL_ENABLE_OPERATION | CIA_402_CONTROL_HOMING_START;
	// if (!GetStatusWord(axis, value)) return false;
  
  // value = CIA_402_CONTROL_ENABLE_OPERATION | CIA_402_CONTROL_HOMING_START;
	// if (!GetStatusWord(axis, value)) return false;
	return ((value & CIA_402_HOMING_COMPLETED) == CIA_402_HOMING_COMPLETED && (value & CIA_402_STATUS_HOMING_ERROR) != CIA_402_STATUS_HOMING_ERROR);
}

bool AServoCAN::GetActaulPositionAndVelocity(AServoAxis axis, int32_t& actualPosition, int32_t& actualVelocity)
{
  if(GetParameter((uint8_t)axis, CIA_402_ACTUAL_POSITION, 0, actualPosition) != AServoCANError::NoError) return false;
  if(GetParameter((uint8_t)axis, CIA_402_ACTUAL_VELOCITY, 0, actualVelocity) != AServoCANError::NoError) return false;
  return true;
}

AServoCANError AServoCAN::SetParameter(uint8_t node_id, uint16_t index, uint8_t subIndex, const int32_t value)
{
  uint8_t can_txbuff[8] = {0,};
  uint16_t can_id = 0x600+ node_id;
  can_txbuff[0] = 0x22;	// 2F - 1byte, 2B - 2byte, 23 - 4byte, 22 - Not defined
  can_txbuff[1] = (uint8_t)(index>>0);
  can_txbuff[2] = (uint8_t)(index>>8);
  can_txbuff[3] = (uint8_t)(subIndex);
  can_txbuff[4] = (uint8_t)(value >> 0);
  can_txbuff[5] = (uint8_t)(value >> 8);
  can_txbuff[6] = (uint8_t)(value >> 16);
  can_txbuff[7] = (uint8_t)(value >> 24);

  unsigned char can_rxbuff[8]={0,};
  unsigned long can_rxid = 0x580 + node_id;
  for(int i = 0; i < _retry_count; i++)
  {
    _can.send(can_id, 0, 0, 8, can_txbuff);
    unsigned long time = millis();
    while(millis() - time < _timeout_ms)
    {
      if(_can.recv( &can_rxid, can_rxbuff))
      {
        if ((_can.can_id & 0xF80) == 0x580 && _can.can_len > 4) 
        {
          uint8_t ccs = can_rxbuff[0];
          uint16_t index_    = *(uint16_t *)&can_rxbuff[1];
          uint8_t  subIndex_ = can_rxbuff[3];

          if (index == index_ && subIndex == subIndex_) 
          {
            if (ccs == 0x60) 
            {
              return AServoCANError::NoError;
            }
            // Abort SDO Protocol (in case of error)
            else if (ccs == 0xC0)
            {	
              // unsigned long abortCode = *(long *)&can_rxbuff[4];
              dbg_println("set: Abort SDO Protocol.");
              dbg_println(can_rxbuff[4]);
              return AServoCANError::AbortCode;
            }
          }
        }
      } 
      delay(1);
    }
    dbg_print(node_id); dbg_print("축 ");
    dbg_print("Timeout.. ");dbg_print(i+1);dbg_print(" retry");dbg_print(" IndexCode");dbg_println(index);
  }
  //delay(5);
  return AServoCANError::Timeout;
}

AServoCANError AServoCAN::GetParameter(uint8_t node_id, uint16_t index, uint8_t subIndex, int32_t& value)
{
  uint8_t can_txbuff[8] = {0,};
  uint16_t buff_id =0; 
  uint8_t buff_len=0;
  uint16_t can_id = 0x600 + node_id;

  can_txbuff[0] = 0x40;	// 2F - 1byte, 2B - 2byte, 23 - 4byte, 22 - Not defined
  can_txbuff[1] = (uint8_t)(index>>0);
  can_txbuff[2] = (uint8_t)(index>>8);
  can_txbuff[3] = (uint8_t)(subIndex);
  can_txbuff[4] = (uint8_t)(value >> 0);
  can_txbuff[5] = (uint8_t)(value >> 8);
  can_txbuff[6] = (uint8_t)(value >> 16);
  can_txbuff[7] = (uint8_t)(value >> 24);

  unsigned char can_rxbuff[8]={0,};
  unsigned long can_rxid = 0x580 + node_id;
  for(int i = 0; i < _retry_count; i++)
  {
    _can.send( can_id, 0, 0, 8, can_txbuff);

    unsigned long time = millis();
    while(millis() - time < _timeout_ms)
    {
      if(_can.recv(&can_rxid, can_rxbuff))
      {
        if (((_can.can_id & 0xF80) == 0x580) && (_can.can_len > 4)) 
        {
          uint8_t ccs = can_rxbuff[0];
          uint16_t index_ = *(uint16_t *)&can_rxbuff[1];
          uint8_t  subIndex_ = can_rxbuff[3];

          if (index == index_ && subIndex == subIndex_) 
          {
            if (ccs == 0x4F) // Receiving data 1byte
            {		
              value = *(int8_t *)&can_rxbuff[4];
              return AServoCANError::NoError;
            }
            else if (ccs == 0x4B) // Receiving data 2byte
            {	
              value = *(int16_t *)&can_rxbuff[4];
              return AServoCANError::NoError;
            }
            else if (ccs == 0x43) // Receiving data 4byte 
            {	
              value = *(int32_t *)&can_rxbuff[4];
              return AServoCANError::NoError;
            }

            else if (ccs == 0xC0) // Abort SDO Protocol (in case of error)
            {
              // unsigned long abortCode = *(long *)&can_rxbuff[4];
              dbg_println("get: Abort SDO Protocol.");
              dbg_println(can_rxbuff[4]);
              return AServoCANError::AbortCode;
            }
          }
        }
      }
      delay(1);
    }
    dbg_print(node_id); dbg_print("축 ");
    dbg_print("Timeout.. ");dbg_print(i+1);dbg_print(" retry");dbg_print(" IndexCode");dbg_println(index);
  }
  //delay(5);
  return AServoCANError::Timeout;
}


