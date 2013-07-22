#ifndef GYROSCOPE_H
#define GYROSCOPE_H

/*
 *  This class was created by Matvey Bryksin (github: @mbryksin) and partly modified by Sergey Bolotov:
 *  Commented lines containing "QUdpSocket" in .h file.
 *  Also changed some QString to char * casts, fulfilled func, minor changes in debug
 */

#include <QObject>
#include <QDebug>
#include <QSocketNotifier>
//#include <QUdpSocket>

const qreal gyroConvConst = 1;

struct gyro_pos
{
    float m_tiltX;  //  Roll, тангаж
    float m_tiltY;  //  Pitch, крен
    float m_tiltZ;  //  Yaw, рысканье
};

class Gyroscope : public QObject
{
    Q_OBJECT
public:
    Gyroscope(QObject *parent = 0);
    void setPort(int port);
    void setPath(QString path);
    float getTiltX();
    float getTiltY();
    float getTiltZ();

signals:
    
public slots:
    void readGyroEvent();
    void setConnection();

private:
    int                 ke_port;
    int                 ke_gyroFd;
    double              m_tiltX;
    double              m_tiltY;
    double              m_tiltZ;
    QString             ke_path;
    QString             fullMessage;
    QSocketNotifier    *ke_SocketNotifier;
//    QUdpSocket        ke_udpSocket;
};

#endif // GYROSCOPE_H
