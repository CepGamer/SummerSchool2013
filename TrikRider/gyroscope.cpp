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

void Gyroscope::setConnection()
{
    ke_gyroFd = open(ke_path.toAscii().data(), O_SYNC, O_RDONLY);
    if (ke_gyroFd == -1)
    {
        qDebug() << "Cannot open gyroscope input file " << ke_path << ", reason: " << errno;
    } else
    {
        qDebug() << "File opened successfully";
    }
    ke_SocketNotifier = new QSocketNotifier(ke_gyroFd, QSocketNotifier::Read, this);
    connect(ke_SocketNotifier, SIGNAL(activated(int)), this, SLOT(readGyroEvent()));
    ke_SocketNotifier->setEnabled(true);
}

void Gyroscope::setPort(int port)
{
    ke_port = port;
}

void Gyroscope::setPath(QString path)
{
    ke_path = path;
    // "/dev/input/event2"
}

void Gyroscope::readGyroEvent()
{
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
        fullMessage.sprintf("VALUE:%d %d %d\r\n",
                           static_cast<int>(m_tiltX),
                           static_cast<int>(m_tiltY),
                           static_cast<int>(m_tiltZ));
        break;
    case EV_SYN:
        qDebug() << fullMessage;
    }
}
