#ifndef COMMUNICATIONSTRUCTS_H
#define COMMUNICATIONSTRUCTS_H

#define BITBUS_MARK 0x7e
/******************************************************************************
Формат пакета для обмена
1. BITBUS_MARK  - 1 байт    - Маркер начала пакета (необязателен в каналах связи,
   0x7E                       позволяющих однозначно определить момент начала и
                              окончания передачи пакета).
2. Length       - 1 байт    - Длина пакета данных (Количество байт
                              между полями Длина и CRC
!!! Поскольку стандартный маркер BitBus равен 0x7E(126) и анализируется
!!! алгоритмом поиска синхронизации приёма
!!! максимальный размер пакета не должени превышать 125 байт

3. ID_Recipient - 2 байта   - Идентификатор устройства-получателя сообщения
4. ID_Sender    - 2 байта   - Идентификатор устройства-отправителя сообщения
5. PipeAddress  - 1 байт    - Адрес канала обмена
6. Дата         - Length-5  - Информация пакета
7. CRC          - 1 байт    - Контрольная сумма пакета (учитываются байты
                              между полями Length и CRC
8. BITBUS_MARK  - 1 байт    - Маркер окончания пакета (необязателен в каналах связи,
                              позволяющих однозначно определить момент начала и
                              окончания передачи пакета).
******************************************************************************/
#pragma pack(push)
#pragma pack(1)
typedef struct PacketStruct
{
  static const int c_iMaxDataSize = 248;
  unsigned char   ucLength;       // Необходимый порядок двух первых байт для попадания
  unsigned char   ucPipeAddress;  // в границу слова при выравнивании
  unsigned short  usID_Recipient;
  unsigned short  usID_Sender;
  unsigned char   ucData[c_iMaxDataSize]; // Максимально возможный буфер
  unsigned char   ucCRC;          // Поле повторяется в ucData при немаксимальной длине пакета
  /* Вспомогательные методы */
  static unsigned char ServiceHeaderSize() { struct PacketStruct *p;
    return (sizeof(p->ucPipeAddress)+sizeof(p->usID_Recipient)+sizeof(p->usID_Sender));};
  // Определение смещения в данных на контрольную сумму
  unsigned char CrcDeltaFromLen() { return (ucLength-(sizeof(usID_Recipient)+sizeof(usID_Sender)+sizeof(ucPipeAddress)));};
}TPacket;
#pragma pack(pop)

/*******************************************************************************
  Вспомогательный класс для подсчёта CRC
*******************************************************************************/
class CCRC_Checker
{
public:
  enum{ // Стандартные полиномы для подсчёта CRC
    eCRC8_iButton = 0x8cl,
    eCRC16_CRC_16 = 0x8005l,
    eCrc16_IBM    = 0x8005l,
    eCrc16_CCIT   = 0x1021l,
    eCRC32_V42    = 0xedb88320l,
    eCRC32_ARCH   = 0x04c11db7l,
  };

public:
  // Конструктор, задаёт полином и размер контрольной суммы
  CCRC_Checker(unsigned long ulPolinom):
    mc_ulPolinom(ulPolinom), m_ulCRC_value(0l){};

  // Сброс, вызывается при повторном расчёте одним объектом.
  // Позволяет проинициализировать поле CRC (начать не с 0)
  void Reset(unsigned long ulInitValue = 0){m_ulCRC_value = ulInitValue;};
  // Расчитывает CRC для очередной порции данных, размер определяется конструктором
  // Возвращает текущее значение CRC
  unsigned long Add(char cData){return Add((unsigned char)cData);};
  unsigned long Add(short sData){return Add((unsigned short)sData);};
  unsigned long Add(int iData){return Add((unsigned int)iData);};
  unsigned long Add(long lData){return Add((unsigned long)lData);};
  unsigned long Add(unsigned char ucData){
         return Add((unsigned long)ucData, 8*sizeof(unsigned char));};
  unsigned long Add(unsigned short usData){
         return Add((unsigned long)usData, 8*sizeof(unsigned short));};
  unsigned long Add(unsigned int uiData){
         return Add((unsigned long)uiData, 8*sizeof(unsigned int));};
  unsigned long Add(unsigned long ulData, unsigned int uiBitDataSize=32)
  { m_ulCRC_value ^= ulData;
    for (unsigned int j=uiBitDataSize;j>0;j--)
      if (m_ulCRC_value&0x01)
        m_ulCRC_value =(m_ulCRC_value>>1)^mc_ulPolinom;
      else
        m_ulCRC_value = m_ulCRC_value>>1;

    return m_ulCRC_value;};
  // Оператор, позволяющий считать текущее значение через присваивание объекта
  operator unsigned long (){return m_ulCRC_value;};

private:
  const unsigned long mc_ulPolinom;
  unsigned long m_ulCRC_value;
};

#endif // COMMUNICATIONSTRUCTS_H
