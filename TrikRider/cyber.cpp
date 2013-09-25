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

#include "cyber.h"

Cyber::Cyber(QObject *parent) :
    QObject(parent)
{
    settings = new QSettings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
//    curr = new gyro_pos;
    gyro = new Gyroscope(this);
    mainTimer = new QTimer(this);
    wheels = new QList<Motor *>();
    conServer = new QTcpServer ();
    buffer = new QString();

    conServer->listen(QHostAddress::Any, settings->value("ConnectionPort").toInt());
    connect(conServer, SIGNAL(newConnection()), this, SLOT(setConnection()));

    position.x = position.y = \
            direction.x = direction.y = 0;      //  Resetting values
//            acceleration.x = acceleration.y = 0;
}

vector operator *(matrix a, vector b)
{
    vector toRet;   //  Plain math, multiplying matrix 2x2 & vector
    toRet.x = a._11 * b.x + a._12 * b.y;
    toRet.y = a._21 * b.x + a._22 * b.y;
    return toRet;
}

vector operator*(qreal a, vector b)
{
    vector toRet;
    toRet.x = b.x * a;
    toRet.y = b.y * a;
    return toRet;
}

vector operator +(vector a, vector b)
{
    vector toRet;   //  Summing vectors
    toRet.x = a.x + b.x;
    toRet.y = a.y + b.y;
    return toRet;
}

/*vector operator =(vector a, vector b)
{
    a.x = b.x;
    a.y = b.y;
}
*/

vector normalize(vector a)
{
    vector b;   //  Resulting vector
    //  Plain math here, nothing to look at, move along
    qreal mod = a.x * a.x + a.y * a.y;
    b.x = a.x / mod;
    b.y = a.y / mod;
    return b;
}

matrix setAngle(qreal radAngle)
{
    matrix toRet;       //  Set spin matrix
    toRet._11 = cos(radAngle);
    toRet._12 = -sin(radAngle);
    toRet._21 = sin(radAngle);
    toRet._22 = cos(radAngle);
    return toRet;
}

vector setVAngle(qreal radAngle)
{
    vector toRet;
    toRet.x = qCos(radAngle);
    toRet.y = qSin(radAngle);
    return toRet;
}

Cyber::~Cyber()
{
    while(connection->CloseConnection() != 0);

    delete connection;
    delete conSocket;
    delete conServer;
    delete mainTimer;
    delete settings;
    delete wheels;
    delete buffer;
    delete guide;
    delete gyro;
}

void Cyber::turn (qreal degree)
{
    if (degree > 0)
    {
        turnRight(degree);      //  If we turn
    }
    else if (degree < 0)
        turnLeft(-degree);
}

void Cyber::stop()
{
    for (int i = 0; i < wheelSize; i++)
        wheels->at(i)->setPower(0);      //  Fullstop
}

void Cyber::startOMNI()
{
    if(settings->allKeys().count() != 0)
    {
        char temp;
        qDebug() << "Load previous settings? Y/N";
        scanf("%c", &temp);
        if(temp == 'y' || temp == 'Y')
            loadFromSaved();
        else if (temp != 'n' && temp != 'N')
            qDebug() << "Please press either 'Y' or 'N'";
        else setSettings();
    } else setSettings();

    gyro->setConnection();          //  Start reading gyro
//    test();
    calibrate();                    //  Calibrating gyro (& accel, when ready (& if needed))
}

void Cyber::moveByVector(vector toMove)
{
    if (mainTimer->isActive()) disconnect(mainTimer, SIGNAL(timeout()), this, SLOT(moveVectorSlot()));
    direction.x = direction.y = 0;
    moving.x = toMove.x;
    moving.y = toMove.y;        //  Resetting values

    connect(mainTimer, SIGNAL(timeout()), this, SLOT(moveVectorSlot()));
    mainTimer->start(1000 / checksPerSecond);
}

/*void Cyber::firstLaunch()
{
    //  This is the main func to provide first user experience. Must be somehow rewritten
    direction.x = direction.y = 0;
//    gyro->setConnection();
    //  Set calibration
    integrand = count = 0;
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(firstLaunchSlot()));
    mainTimer->start(1000 / checksPerSecond);
}*/

void Cyber::turnLeft(qreal degree)
{
    //qDebug() << "Start engines";
/*    wheels->at(0)->spin(100);   //  spinning dem wheels
    wheels->at(1)->spin(100);
    wheels->at(2)->spin(100);*/
    leftRad = degree / 180 * Pi;
    //  Again, some code after gyro
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(turnLeftSlot()));
    mainTimer->start(1000 / checksPerSecond);
}

void Cyber::turnRight(qreal degree)
{
    //qDebug() << "Start engines";
    /*wheels->at(0)->spin(-100);
    wheels->at(1)->spin(-100);
    wheels->at(2)->spin(-100);*/
    leftRad = degree / 180 * Pi;
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(turnRightSlot())); //  Start timer
    mainTimer->start(1000 / checksPerSecond);
}

void Cyber::calibrate()
{
    qDebug () << "Sensor Calibration Initiated";
    count = correction = integrand = 0; //  Tune the calibration
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(calibrateSlot()));
    mainTimer->start(1000 / checksPerSecond);
}

void Cyber::loadFromSaved()
{
    wheelSize = settings->value("WheelSize").toInt();
    qDebug() << wheelSize;
    guide = new vector[wheelSize];                  //  Guiding vectors for all
    setWheels();
}

void Cyber::setSettings()
{
    char temp;
    settings->setValue("DevPath", "/dev/i2c-2");
    settings->setValue("DevId", 72);        //0x48
    settings->setValue("ConnectionPort", 4444);

    qDebug() << "How many wheels do you have on your OMNI?";
    scanf("\n%c", &wheelSize);
    wheelSize -= '0';
    settings->setValue("WheelSize", wheelSize);
    guide = new vector[wheelSize];                  //  Guiding vectors for all
    for (int i = 1; i <= wheelSize; i++)
        settings->setValue("Wheel" + QString::number(i), i);

    qDebug() << "Enter type of connection: (A)ndroid, (P)C or A(u)to control?";
//    scanf("%c", &temp);
    temp = 'A';
    if(temp == 'A' || temp == 'a')
        controlMode = ANDROID_CONTROL;
    else if (temp == 'P' || temp == 'p')
        controlMode = PC_CONTROL;
    else if (temp == 'U' || temp == 'u')
        controlMode = AUTO_MODE;
    else qDebug() << "Please press either 'A', 'P' or 'u'";

    setWheels();
}

void Cyber::setWheels()
{
    connection = new I2cConnection(settings->value("DevPath").toString(), settings->value("DevId").toInt());

    guide[0].x = qCos(Pi / 4);
    guide[0].y = qSin(Pi / 4);
    wheels->append( new Motor ((char) (settings->value("Wheel" + QString::number(1)).toInt()), \
                                       settings->value("Period").toInt(), \
                                       connection));

    guide[1].x = qCos(2 * Pi / wheelSize + Pi / 4);
    guide[1].y = qSin(2 * Pi / wheelSize + Pi / 4);
    wheels->append( new Motor ((char) (settings->value("Wheel" + QString::number(2)).toInt()), \
                                       settings->value("Period").toInt(), \
                                       connection));

    guide[2].x = -qCos(2 * 2 * Pi / wheelSize + Pi / 4);
    guide[2].y = -qSin(2 * 2 * Pi / wheelSize + Pi / 4);
    wheels->append( new Motor ((char) (settings->value("Wheel" + QString::number(4)).toInt()), \
                                       settings->value("Period").toInt(), \
                                       connection));

    guide[3].x = qCos(3 * 2 * Pi / wheelSize + Pi / 4);
    guide[3].y = qSin(3 * 2 * Pi / wheelSize + Pi / 4);
    wheels->append( new Motor ((char) (settings->value("Wheel" + QString::number(3)).toInt()), \
                                       settings->value("Period").toInt(), \
                                       connection));
}

void Cyber::test()
{
    /*for (int i = 0; i < wheelSize; i++)
    {
        wheels->at(i)->setPower(100);
    }*/
    qDebug() << "Enter test";
    qint32 prev, now = 1;
    prev = 1;
    while (now != -1)
    {
        scanf("%d", &now);
        qDebug() << "Enter change wheel #" << now;
        wheels->at(prev)->setPower(0);
        wheels->at(now)->setPower(100);
        prev = now;
    }
}

void Cyber::andrControl()
{
    qDebug() << "Entered android Control";
    vector c;
    c.y = 0;
    c.x = 1;
    this->moveByVector(c);
}

void Cyber::checkPosition()
{
    vector mov;
    qreal angVelocityC = angVelocity / ((wheels->at(1)->getPower() > 75) ? 2 : 1);  //  Get speed - divide speed in 2 types
    vector on50percentSpeed;
//    currRad = currRad % (2 * Pi);
    on50percentSpeed.x = qCos(angVelocityC * 2 * Pi);
    on50percentSpeed.y = qSin(angVelocityC * 2 * Pi);
    mov.x = mov.y = 0;
    for (int i = 0; i < wheelSize; i++)
        mov = mov + guide[i] * (wheels->at(i)->getPower() / 100);
/*    for (int i=0; i<3; i++)
        angVelocity += wheels->at(i)->getSpeed();
        angVelocity = convertSpeed();   //  convert percents into speed. Or based on an encoder.
*/
    previous.tiltX = angles.tiltX;
    previous.tiltY = angles.tiltY;
    previous.tiltZ = angles.tiltZ;
//    gyro->readGyroEvent();
    angles.tiltX = gyro->getTiltX();  //  Get gyro statements
    angles.tiltY = gyro->getTiltY();
    angles.tiltZ = gyro->getTiltZ();
/*    qDebug() << "Pos is:\t" << position.x << '\t' << position.y << '\n'\
             << "Dir is:\t" << direction.x << '\t' << direction.y;
    position = position + ( (kalmanCoef / (checksPerSecond * checksPerSecond * 2)) * acceleration \
                            + ((1 - kalmanCoef) / checksPerSecond) * (moving));*/
    //  Kalman filter values
    direction = direction + \
            (kalmanCoef / checksPerSecond) * (setAngle( angles.tiltZ - correction ) * direction) + \
            ((1 - kalmanCoef) / checksPerSecond ) * (on50percentSpeed + direction);
    //  Change direction
    absolute.tiltZ = kalmanCoef * (angles.tiltZ - correction) + (1 - kalmanCoef) * angVelocityC;
    currRad += absolute.tiltZ / checksPerSecond;
    if (count == 10)
        while (currRad > 2 * Pi) currRad -= 2 * Pi;
}

void Cyber::moveVectorSlot()
{
    count++;
    count %= checksPerSecond / 5;

    checkPosition();
//    moving = normalize(setAngle(-currRad) * moving);     //  Calibrate moving vector

    if(count == 0)
    {
//        qDebug() << "CurrRad is " << currRad;
//        qDebug() << "Wheel #1 speed is " << ((normalize(setAngle(-currRad) * moving) * guide[0]) * 70);
        for (int i = 0; i < wheelSize; i++)
            wheels->at(i)->setPower((normalize(setAngle(-currRad) * moving) * guide[i]) * 70);  //  Get those wheels spinning
    }
}

void Cyber::turnLeftSlot()
{
    if(leftRad < 0.2)
    {
        mainTimer->stop();
        mainTimer->disconnect(mainTimer, SIGNAL(timeout()), this, SLOT(turnLeftSlot()));
        stop();
    }
    else if(leftRad < 2 * Pi)
        for(int i = 0; i < wheelSize; i++)
            wheels->at(i)->setPower(50);
    checkPosition();
    leftRad -= qAbs(absolute.tiltZ / checksPerSecond);
}

void Cyber::turnRightSlot()
{
    count++;
    count %= checksPerSecond;
    if(count == 0)
        qDebug() << "leftRad is:\t" << QString::number(leftRad, 'f');
    if(leftRad < 0.2)
    {
        mainTimer->stop();
        disconnect(mainTimer, SIGNAL(timeout()), this, SLOT(turnRightSlot()));
        stop();
    }
    else if(leftRad < 2 * Pi)
        for(int i = 0; i < wheelSize; i++)
            wheels->at(i)->setPower(-50);
    checkPosition();
    leftRad -= qAbs(absolute.tiltZ / checksPerSecond);
}

void Cyber::calibrateSlot()
{
    count++;
    if (count == (2 * checksPerSecond))
    {
        qDebug() << "Integrand: " << integrand;
        mainTimer->stop();
        disconnect(mainTimer, SIGNAL(timeout()), this, SLOT(calibrateSlot()));
        correction = integrand / (2 * checksPerSecond);
        integrand = count = 0;
        andrControl();
//        moving.x = 1;
//        moving.y = 0;
//        moveByVector(moving);
    }
    checkPosition();
    integrand += angles.tiltZ;
}

void Cyber::movingState()
{

}

void Cyber::pauseState()
{
    stop();
}

void Cyber::tcpDisconnected()
{
    qDebug() << "TCP connection failed";
    pauseState();
    conSocket->disconnectFromHost();
}

bool Cyber::setConnection()
{
    if(conSocket->isValid()) qDebug () << "Replacing current connection";
    conSocket = conServer->nextPendingConnection();
    conSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(conSocket, SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
    connect(conSocket, SIGNAL(readyRead()), this, SLOT(readTcp()));
    return true;
}

bool Cyber::readTcp()
{
    if (!conSocket->isValid()) return false;
    while (conSocket->bytesAvailable() > 0)
    {
        buffer->append(conSocket->readLine(100));

    }

    buffer->clear();
    return true;
}
