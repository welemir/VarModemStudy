/* CommandCode_RadioModem.h - идентификаторы команд и ответных сообщений специфичных для проекта радиомодем
*/

#ifndef _COMMAND_CODE_RADIOMODEM_H_
#define _COMMAND_CODE_RADIOMODEM_H_

#include "CommandCode.h"

typedef enum
{
/* Коды команд от 0x40 являются устройство-зависимыми и могут находится в отдельных заголовочных файлах в проекте конкретного устройсва */

  eWorkModeSet         = 0x40, // Задать режим работы (0 - Режим приёма; 1 - Режим передачи; 3 - Режим ожидания (энергосбережения))
  eWorkModeGet         = 0x41,
  eModulationTypeSet   = 0x42, // Задать тип модуляции (0 - OOK; 1 - FSK)
  eModulationTypeGet   = 0x43,
  eModulationSpeedSet  = 0x44, // Задать скорость модуляции
  eModulationSpeedGet  = 0x45,
  eTxPowerSet          = 0x46, // Задать выходную мощность передатчика
  eTxPowerGet          = 0x47,
  ePreambleLengthSet   = 0x48, // Установить длину преамбулы битовой синхронизации в байтах
  ePreambleLengthGet   = 0x49,
  eSyncroStartParamSet = 0x4a,  // Задать параметры синхропосылки в начале пакета
  eSyncroStartParamGet = 0x4b,
  eDataFieldSizeSet    = 0x4c,  // Задать размер поля данных пакета
  eDataFieldSizeGet    = 0x4d,
  eCrcModeSet          = 0x4e,  // Задать режим контроля/коррекции ошибок
  eCrcModeGet          = 0x4f,
  eFrequencySet        = 0x50,
  eFrequencyGet        = 0x51,
  eDeviationSet        = 0x52,
  eDeviationGet        = 0x53,
  eBandwidthSet        = 0x54,
  eBandwidthGet        = 0x55,
  eModemStatusGet      = 0x60,
  eSubmitRawData       = 0x61,  // Задать ID для получения "сырых" данных приема
}TCommand_RadioModem;

typedef enum
{ // Коды типа информации для ответов на команды
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
