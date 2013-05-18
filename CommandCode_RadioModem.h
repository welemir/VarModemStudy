/* CommandCode_RadioModem.h - �������������� ������ � �������� ��������� ����������� ��� ������� ����������
*/

#ifndef _COMMAND_CODE_RADIOMODEM_H_
#define _COMMAND_CODE_RADIOMODEM_H_

#include "CommandCode.h"

typedef enum
{
/* ���� ������ �� 0x40 �������� ����������-���������� � ����� ��������� � ��������� ������������ ������ � ������� ����������� ��������� */

  eWorkModeSet         = 0x40, // ������ ����� ������ (0 - ����� �����; 1 - ����� ��������; 3 - ����� �������� (����������������))
  eWorkModeGet         = 0x41,
  eModulationTypeSet   = 0x42, // ������ ��� ��������� (0 - OOK; 1 - FSK)
  eModulationTypeGet   = 0x43,
  eModulationSpeedSet  = 0x44, // ������ �������� ���������
  eModulationSpeedGet  = 0x45,
  eTxPowerSet          = 0x46, // ������ �������� �������� �����������
  eTxPowerGet          = 0x47,
  ePreambleLengthSet   = 0x48, // ���������� ����� ��������� ������� ������������� � ������
  ePreambleLengthGet   = 0x49,
  eSyncroStartParamSet = 0x4a,  // ������ ��������� ������������� � ������ ������
  eSyncroStartParamGet = 0x4b,
  eDataFieldSizeSet    = 0x4c,  // ������ ������ ���� ������ ������
  eDataFieldSizeGet    = 0x4d,
  eCrcModeSet          = 0x4e,  // ������ ����� ��������/��������� ������
  eCrcModeGet          = 0x4f,
  eFrequencySet        = 0x50,
  eFrequencyGet        = 0x51,
  eDeviationSet        = 0x52,
  eDeviationGet        = 0x53,
  eBandwidthSet        = 0x54,
  eBandwidthGet        = 0x55,
  eModemStatusGet      = 0x60,
  eSubmitRawData       = 0x61,  // ������ ID ��� ��������� "�����" ������ ������
}TCommand_RadioModem;

typedef enum
{ // ���� ���� ���������� ��� ������� �� �������
  eAnsWorkMode         = (unsigned char)eAnswerMark + eWorkModeGet,
  eAnsModulationType   = (unsigned char)eAnswerMark + eModulationTypeGet,
  eAnsModulationSpeed  = (unsigned char)eAnswerMark + eModulationSpeedGet,
  eAnsTxPower          = (unsigned char)eAnswerMark + eTxPowerGet,
  eAnsPreambleLength   = (unsigned char)eAnswerMark + ePreambleLengthGet,
  eAnsSyncroStartParam = (unsigned char)eAnswerMark + eSyncroStartParamGet,
  eAnsDataFieldSize    = (unsigned char)eAnswerMark + eDataFieldSizeGet,
  eAnsCrcModeGet       = (unsigned char)eAnswerMark + eCrcModeGet,
  eAnsModemStatusGet  = (unsigned char)eAnswerMark + eModemStatusGet,
}TInfoType_RadioModem;

#endif // _COMMAND_CODE_RADIOMODEM_H_
