#include "programsettings.h"

ProgramSettings::ProgramSettings(QObject *parent) :
    QSettings("VARControl.ini", QSettings::IniFormat, parent)
{
}

// ����� ������ ��� �������� ������� ������������� � ini-�����
const char strKeyName_RouteTable[] = "Route/Table";
const char strKeyName_RouteTable_addrBitBus[] = "addrBitBus";
const char strKeyName_RouteTable_addrCAN[] = "addrCAN";

TRouteTable ProgramSettings::RouteTable()
{
    QMap<ushort, uchar> table;
    int size = beginReadArray(strKeyName_RouteTable);
    for (int i = 0; i < size; ++i) {
        setArrayIndex(i);

        // ������ ����� ��������� ������ ������� �������������
        uchar ucCanAddr = value(strKeyName_RouteTable_addrCAN).toUInt();
        ushort usDeviceID = value(strKeyName_RouteTable_addrBitBus).toUInt();
        // ���������� � ������� ���������� ����������
        table[usDeviceID] = ucCanAddr;
    }
    endArray();

    return table;
}

void ProgramSettings::setRouteTable(TRouteTable tableNew)
{

}

