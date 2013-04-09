#include "programsettings.h"

ProgramSettings::ProgramSettings(QObject *parent) :
    QSettings("VARControl.ini", QSettings::IniFormat, parent)
{
}

// Имена ключей для хранения таблицы маршрутизации в ini-файле
const char strKeyName_RouteTable[] = "Route/Table";
const char strKeyName_RouteTable_addrBitBus[] = "addrBitBus";
const char strKeyName_RouteTable_addrCAN[] = "addrCAN";

TRouteTable ProgramSettings::RouteTable()
{
    QMap<ushort, uchar> table;
    int size = beginReadArray(strKeyName_RouteTable);
    for (int i = 0; i < size; ++i) {
        setArrayIndex(i);

        // Чтение полей очередной записи таблицы маршрутизации
        uchar ucCanAddr = value(strKeyName_RouteTable_addrCAN).toUInt();
        ushort usDeviceID = value(strKeyName_RouteTable_addrBitBus).toUInt();
        // Добавление в таблицу очередного устройства
        table[usDeviceID] = ucCanAddr;
    }
    endArray();

    return table;
}

void ProgramSettings::setRouteTable(TRouteTable tableNew)
{

}

