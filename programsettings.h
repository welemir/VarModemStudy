#ifndef PROGRAMSETTINGS_H
#define PROGRAMSETTINGS_H

#include <QSettings>
#include <QMap>

typedef QMap<ushort,uchar> TRouteTable;
class ProgramSettings : public QSettings
{
    Q_OBJECT
    Q_PROPERTY(TRouteTable mapRouteTable READ RouteTable WRITE setRouteTable)

public:
    explicit ProgramSettings(QObject *parent = 0);

    // Методы для работы с таблицей маршрутизаии устройств
    TRouteTable RouteTable();
    void setRouteTable(TRouteTable tableNew);

signals:
    
public slots:
    
};

#endif // PROGRAMSETTINGS_H
