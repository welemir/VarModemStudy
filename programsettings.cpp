#include "programsettings.h"

ProgramSettings::ProgramSettings(QObject *parent) :
    QSettings("VarModemStudy.ini", QSettings::IniFormat, parent)
{
}

