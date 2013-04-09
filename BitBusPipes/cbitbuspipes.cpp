#include "cbitbuspipes.h"

CBitBusPipes* CBitBusPipes::m_pInstance = NULL;

CBitBusPipes* CBitBusPipes::GetInstance()
{
    static QMutex mutex;
    if (!m_pInstance)
    {
        mutex.lock();
        if (!m_pInstance)
            m_pInstance = new CBitBusPipes;
        mutex.unlock();
    }




    return m_pInstance;
}

void CBitBusPipes::slotWriteDataToPipe(const QByteArray buff, unsigned short usDestAddr, TPipeType PipeID)
{
}

//    void slotConnectSerialPort(SerialPort newSP);


