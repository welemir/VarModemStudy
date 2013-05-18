/* CommandCode.h - �������������� ������ � ������� �� ���
*/

#ifndef _COMMAND_CODE_H_
#define _COMMAND_CODE_H_

typedef enum
{ // ���� ������
  // �������� 0x00 - 0x0d �������������� ��� ������� FirmwareUpdater'�
  eSystemShutDown = 0x0e,
  eFlashErase     = 0x0f,
  eDeviceType     = 0x20,
  eHwVersion      = 0x21,
  eSwVersion      = 0x22,
  eGetManufacturer= 0x23,
  eGetUUID        = 0x24,

  // ������� ���������
  eCAN_TL_ItemSet = 0x30, // ��������� �������� ������� ���������� CAN-���������
  eCAN_TL_ItemGet = 0x31, // ������ �������� ������� ���������� CAN-���������
  eCAN_RouteAdd   = 0x32, // ���������� ������ � ������� ������������� CAN-���������
//  eCAN_ChangeSpeed= 0x33, // ��������� �������� ������ �� CAN-����������

/* ���� ������ �� 0x40 �������� ����������-���������� � ����� ��������� � ��������� ������������ ������ � ������� ����������� ��������� */

  eReadConfiguration = 0x40,
  eWriteConfiguration= 0x41,
  eDiagControl    = 0x42,
  eDeviceIdSet    = 0x43,
  eDeviceIdGet    = 0x44,
  eDumpRouteTable = 0x45,
  eGetParameter   = 0x46,
  eSetParameter   = 0x47,
  eCommand_Sniffer = 0x48,
  eCommand_Simulator_Send_Packet = 0x49, // ��������� ����� �� CAN-����.
  eGetTransmitterParam = 0x50,
  eSetTransmitterParam = 0x51,
  eLogGetMessage = 0x52,// �������� ������ ������ �� ���� ��������� ���������
  eLogReset = 0x53, // ������ ������� ���� � ������� ���������
  eLogEnd = 0x54, // ����� ����� �� �����.

}TDeviceCommand;
typedef enum
{
  eDisable  = 0,
  eEnable   = 1,
}TCommandParam;

typedef enum
{ // ���� ���� ���������� ��� ������� �� �������
  eAnswerMark     = 0x80, // ������ ����������� � ���� ������� ��� ��������� ���� ��������� ������
  eAnsDeviceType  = eAnswerMark + eDeviceType,
  eAnsHwVersion   = eAnswerMark + eHwVersion,
  eAnsSwVersion   = eAnswerMark + eSwVersion,
  eAnsManufacturer = eAnswerMark + eGetManufacturer,
  eAnsUUID        = eAnswerMark + eGetUUID,
  eAnsCAN_TL_Item = eAnswerMark + eCAN_TL_ItemGet,
  eAnsIdRogerThat = eAnswerMark + eDeviceIdSet,
  eAnsDumpRouteTable = eAnswerMark + eDumpRouteTable,
  eAnsParameter   = eAnswerMark + eGetParameter,
  eSnifferData = eAnswerMark + eCommand_Sniffer,
  eAnsLogGetLogMessage = eAnswerMark + eLogGetMessage,
  eAnsLogReset = eAnswerMark + eLogReset,
  eAnsLogEnd = eAnswerMark + eLogEnd,
  eAnsGetTransmitterParam = eAnswerMark + eGetTransmitterParam,
  eAnsSetTransmitterParam = eAnswerMark + eSetTransmitterParam,
  ePCSmarker = 0x64,          // ����� ������ �� ���
}TInfoType;

#endif // _COMMAND_CODE_H_
