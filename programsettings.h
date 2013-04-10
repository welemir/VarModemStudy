#ifndef PROGRAMSETTINGS_H
#define PROGRAMSETTINGS_H

#include <QSettings>
#include <QMap>

class ProgramSettings : public QSettings
{
    Q_OBJECT

public:
    explicit ProgramSettings(QObject *parent = 0);

signals:
    
public slots:
    
};

#endif // PROGRAMSETTINGS_H
