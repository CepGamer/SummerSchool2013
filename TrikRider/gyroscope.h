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
#include <QTime>
//#include <QUdpSocket>

const qreal gyroConvConst = 820.846931;     //  Constant from quadrocopter

struct gyro_pos
{
    qreal m_tiltX;  //  Roll, тангаж
    qreal m_tiltY;  //  Pitch, крен
    qreal m_tiltZ;  //  Yaw, рысканье
};

class Gyroscope : public QObject
{
    Q_OBJECT
public:
    Gyroscope(QObject *parent = 0);
    ~Gyroscope();
    inline void setPort(int port = 22){ ke_port = port; }
    inline void setPath(QString path = QString("/dev/input/event2")) {ke_path = path;}  //  May be event1
    inline qreal getTiltX() {return m_tiltX / gyroConvConst;}
    inline qreal getTiltY() {return m_tiltY / gyroConvConst;}
    inline qreal getTiltZ() {return m_tiltZ / gyroConvConst;}

signals:
    
public slots:
    void readGyroEvent();
    void setConnection();

private:
    int                 ke_port;
    int                 ke_gyroFd;
    qreal               m_tiltX;
    qreal               m_tiltY;
    qreal               m_tiltZ;
    QString             ke_path;
    QString             fullMessage;
    QSocketNotifier    *ke_SocketNotifier;
    quint32             count;
    QTime              *begin;

//    QUdpSocket        ke_udpSocket;
};

#endif // GYROSCOPE_H
