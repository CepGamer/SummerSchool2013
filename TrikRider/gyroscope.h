/*
Copyright (c) 2013, Bolotov Sergey
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Saint-Petersburg State University nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Bolotov Sergey BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

struct gyroPos
{
    qreal tiltX;  //  Roll, тангаж
    qreal tiltY;  //  Pitch, крен
    qreal tiltZ;  //  Yaw, рысканье
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
