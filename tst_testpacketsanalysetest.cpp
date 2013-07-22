#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include "ckernel.h"

class TestPacketsAnalyseTest : public QObject
{
  Q_OBJECT

public:
  TestPacketsAnalyseTest();

private Q_SLOTS:
  void initTestCase();
  void cleanupTestCase();

  void testComparePackets();
  void testComparePackets_data();

  void benchmarkComparePackets();
  void benchmarkComparePackets_data();

private:
  void comparePackets_data();
private:
  CKernel *m_pKernel;
};

TestPacketsAnalyseTest::TestPacketsAnalyseTest()
{
}

void TestPacketsAnalyseTest::initTestCase()
{
  m_pKernel = CKernel::GetInstance();
}

void TestPacketsAnalyseTest::cleanupTestCase()
{
  m_pKernel->deleteLater();
}

void TestPacketsAnalyseTest::testComparePackets()
{
  QFETCH(QByteArray, packetIn);
  QFETCH(QByteArray, packetOut);
  QFETCH(int, iErrorExpected);

  int iErrorCounterBytes(0), iErrorCounterBits(0);
  m_pKernel->comparePackets(packetIn, packetOut, &iErrorCounterBytes, &iErrorCounterBits);

  if(0 <= iErrorExpected){
    QCOMPARE(iErrorCounterBits, iErrorExpected);
    }
  else{
      QString str = "Error bytes " + QString("%1").arg(iErrorCounterBytes) + " bits " + QString("%1").arg(iErrorCounterBits);
      QWARN(str.toAscii());
    }

}
void TestPacketsAnalyseTest::benchmarkComparePackets()
{
  QFETCH(QByteArray, packetIn);
  QFETCH(QByteArray, packetOut);
  int iErrorCounterBytes(0), iErrorCounterBits(0);

  QBENCHMARK(m_pKernel->comparePackets(packetIn, packetOut, &iErrorCounterBytes, &iErrorCounterBits));
}

void TestPacketsAnalyseTest::testComparePackets_data(){
  comparePackets_data();
}
void TestPacketsAnalyseTest::benchmarkComparePackets_data(){
  comparePackets_data();
}

void TestPacketsAnalyseTest::comparePackets_data()
{
  qsrand(QTime::currentTime().msec());

  QTest::addColumn<QByteArray>("packetOut");
  QTest::addColumn<QByteArray>("packetIn");
  QTest::addColumn<int>("iErrorExpected");

  int iPacketDataLength = 50;
  QByteArray newPacket1, newPacket2;
  for( int i = 0; i < iPacketDataLength; i++){
    newPacket1.append(qrand());
    newPacket2.append(qrand());
  }
//  m_Transmitter->appendCrc(&newPacket);

  QTest::newRow("Identical Packets") << newPacket1 << newPacket1 << 0;
  QTest::newRow("Random Different Packets") << newPacket1 << newPacket2 << -1;
  newPacket1.clear();
  newPacket2.clear();
  for( int i = 0; i < iPacketDataLength; i++){
      if(0 == i%2){
          newPacket1.append(qrand());
          newPacket2.append(qrand());
        }else{
          newPacket1.append(i);
          newPacket2.append(i);
        }
  }
  QTest::newRow("Random2 Different Packets") << newPacket1 << newPacket2 << -1;
}

QTEST_MAIN(TestPacketsAnalyseTest)

#include "tst_testpacketsanalysetest.moc"
