/*
Copyright 2013 Bolotov Sergey

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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
