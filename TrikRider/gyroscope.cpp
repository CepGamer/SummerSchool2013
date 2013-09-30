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

#include "gyroscope.h"

#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <linux/input.h>

Gyroscope::Gyroscope(QObject *parent) :
    QObject(parent),
    m_tiltX(0),
    m_tiltY(0),
    m_tiltZ(0),
    fullMessage("")
{
}

Gyroscope::~Gyroscope()
{
    delete ke_SocketNotifier;
    delete begin;
}

void Gyroscope::setConnection()
{
    begin = new QTime();
    begin->start();
    setPort();                   //  Standart port
    setPath(QString("/dev/input/by-path/platform-spi_davinci.1-event"));    //  Standart path
    ke_gyroFd = open(ke_path.toStdString().data(), O_SYNC, O_RDONLY);
    if (ke_gyroFd == -1)
    {
        qDebug() << "Cannot open gyroscope input file " << ke_path << ", reason: " << errno;
    } else
    {
//        qDebug() << "File opened successfully";
    }
    ke_SocketNotifier = new QSocketNotifier(ke_gyroFd, QSocketNotifier::Read, this);
    connect(ke_SocketNotifier, SIGNAL(activated(int)), this, SLOT(readGyroEvent()));
    ke_SocketNotifier->setEnabled(true);
}

void Gyroscope::readGyroEvent()
{
    quint32 checks = 1000;
    count++;
    count %= checks;
    if(count == 0)
    {
        begin->start();
    }

    struct input_event event;

    if (read(ke_gyroFd, reinterpret_cast<char*>(&event), sizeof(event)) != sizeof(event))
    {
        qDebug() << "Incomplete gyroscope data read";
        return;
    }
    switch (event.type)
    {
    case EV_ABS:
        switch (event.code)
        {
            case ABS_X: m_tiltX = event.value; break;
            case ABS_Y: m_tiltY = event.value; break;
            case ABS_Z: m_tiltZ = event.value; break;
        }
        break;
    case EV_SYN:
//        qDebug() << fullMessage;
        break;
    }
}
