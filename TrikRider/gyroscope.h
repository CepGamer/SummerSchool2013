#ifndef GYROSCOPE_H
#define GYROSCOPE_H

#include <QObject>
#include <QSocketNotifier>
#include <QUdpSocket>

class Gyroscope : public QObject
{
    Q_OBJECT
public:
    Gyroscope(QObject *parent = 0);
    void setPort(int port);
    void setPath(QString path);

signals:
    
public slots:
    void readGyroEvent();
    void setConnection();

private:
    int               ke_port;
    int               ke_gyroFd;
    double            m_tiltX;
    double            m_tiltY;
    double            m_tiltZ;
    QString           ke_path;
    QString fullMessage;
    QSocketNotifier   *ke_SocketNotifier;
    QUdpSocket        ke_udpSocket;
};

#endif // GYROSCOPE_H
