/* CommandCode.h - идентификаторы команд и ответов на них
*/

#ifndef _COMMAND_CODE_H_
#define _COMMAND_CODE_H_

typedef enum
{ // Коды команд
  // Диапазон 0x00 - 0x0d зарезервирован под команды FirmwareUpdater'а
  eSystemShutDown = 0x0e,
  eFlashErase     = 0x0f,
  eDeviceType     = 0x20,
  eHwVersion      = 0x21,
  eSwVersion      = 0x22,
  eGetManufacturer= 0x23,
  eGetUUID        = 0x24,

  // Команды настройки
  eCAN_TL_ItemSet = 0x30, // Установка элемента массива описателей CAN-сообщений
  eCAN_TL_ItemGet = 0x31, // Запрос элемента массива описателей CAN-сообщений
  eCAN_RouteAdd   = 0x32, // Добавление строки в таблицу маршрутизации CAN-устройств
//  eCAN_ChangeSpeed= 0x33, // Изменение скорости обмена по CAN-интерфейсу

/* Коды команд от 0x40 являются устройство-зависимыми и могут находится в отдельных заголовочных файлах в проекте конкретного устройсва */

  eReadConfiguration = 0x40,
  eWriteConfiguration= 0x41,
  eDiagControl    = 0x42,
  eDeviceIdSet    = 0x43,
  eDeviceIdGet    = 0x44,
  eDumpRouteTable = 0x45,
  eGetParameter   = 0x46,
  eSetParameter   = 0x47,
  eCommand_Sniffer = 0x48,
  eCommand_Simulator_Send_Packet = 0x49, // отправить пакет на CAN-шину.
  eGetTransmitterParam = 0x50,
  eSetTransmitterParam = 0x51,
  eLogGetMessage = 0x52,// получить первую запись из лога системных сообщений
  eLogReset = 0x53, // начать вычитку лога с первого сообщения
  eLogEnd = 0x54, // чтобы никто не занял.

}TDeviceCommand;
typedef enum
{
  eDisable  = 0,
  eEnable   = 1,
}TCommandParam;

typedef enum
{ // Коды типа информации для ответов на команды
  eAnswerMark     = 0x80, // Маркер добавляемый к коду команды для получения типа сообщения ответа
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
  ePCSmarker = 0x64,          // пакет ответа от СДК
}TInfoType;

#endif // _COMMAND_CODE_H_
