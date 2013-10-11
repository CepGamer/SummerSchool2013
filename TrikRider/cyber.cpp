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

#include "cyber.h"

Cyber::Cyber(QObject *parent) :
    QObject(parent)
{
    speed = 0;
    settings = new QSettings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
//    curr = new gyro_pos;
    gyro = new Gyroscope(this);
    mainTimer = new QTimer(this);
    wheels = new QList<Motor *>();
    conServer = new QTcpServer ();
    buffer = new QString();
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

void Cyber::stop(bool full)
{
//    if (full) speed = 0;
    speed = 0;
    for (int i = 0; i < wheelSize; i++)
        wheels->at(i)->setPower(speed);      //  Fullstop
}

void Cyber::startOMNI()
{
    if(settings->allKeys().count() != 0)
    {
        char temp;
        printf ("Load previous settings? Y/N\n");
        scanf("%c", &temp);
        while(temp != 'y' && temp != 'Y' && \
              temp != 'n' && temp != 'n')
        {
            printf("Please press either 'Y' or 'N'\n");
            scanf("%c", &temp);
        }
        if (temp == 'y' || temp == 'Y')
            loadFromSaved();
        else
            setSettings();
    } else setSettings();

    gyro->setConnection();          //  Start reading gyro

//    test();
    calibrate();                    //  Calibrating gyro (& accel, when ready (& if needed))
}

void Cyber::initialiseMove()
{
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(moveVectorSlot()));
    mainTimer->start(1000 / checksPerSecond);
}

void Cyber::calibrate()
{
    count = correction = integrand = 0; //  Tune the calibration
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(calibrateSlot()));
    mainTimer->start(1000 / checksPerSecond);
}

void Cyber::loadFromSaved()
{
    wheelSize = settings->value("WheelSize").toInt();
    controlMode = (controlType) settings->value("ControlMode").toInt();
    guide = new vector[wheelSize];      //  Guiding vectors for all
    setWheels();
}

void Cyber::setSettings()
{
    char temp;
    settings->setValue("DevPath", "/dev/i2c-2");
    settings->setValue("DevId", 72);        //0x48
    settings->setValue("ConnectionPort", 4444);
    settings->setValue("Period", 0x1000);

    printf("How many wheels do you have on your OMNI?");
    scanf("\n%c", &wheelSize);
    wheelSize -= '0';
    settings->setValue("WheelSize", wheelSize);
    guide = new vector[wheelSize];                  //  Guiding vectors for all
    for (int i = 1; i <= wheelSize; i++)
        settings->setValue("Wheel" + QString::number(i), i);

    printf("Enter type of connection: (A)ndroid, (P)C or A(u)to control?");
    scanf("\n%c", &temp);
    if(temp == 'A' || temp == 'a')
        controlMode = ANDROID_CONTROL;
    else if (temp == 'P' || temp == 'p')
        controlMode = PC_CONTROL;
    else if (temp == 'U' || temp == 'u')
        controlMode = AUTO_MODE;
    else printf ("\nPlease press either 'A', 'P' or 'u': ");

    settings->setValue("ControlMode", controlMode);
    setWheels();
}

void Cyber::setWheels()
{
    connection = new I2cConnection(settings->value("DevPath").toString(), settings->value("DevId").toInt());

    guide[0].x = qCos(1 * 2 * Pi / wheelSize + Pi / 4);
    guide[0].y = qSin(1 * 2 * Pi / wheelSize + Pi / 4);
    wheels->append( new Motor ((char) (settings->value("Wheel" + QString::number(1)).toInt()), \
                                       settings->value("Period").toInt(), \
                                       connection));

    guide[1].x = qCos(0 * 2 * Pi / wheelSize + Pi / 4);
    guide[1].y = qSin(0 * 2 * Pi / wheelSize + Pi / 4);
    wheels->append( new Motor ((char) (settings->value("Wheel" + QString::number(2)).toInt()), \
                                       settings->value("Period").toInt(), \
                                       connection));
    wheels->at(1)->revertClockwise();

    guide[2].x = qCos(3 * 2 * Pi / wheelSize + Pi / 4);
    guide[2].y = qSin(3 * 2 * Pi / wheelSize + Pi / 4);
    wheels->append( new Motor ((char) (settings->value("Wheel" + QString::number(4)).toInt()), \
                                       settings->value("Period").toInt(), \
                                       connection));

    guide[3].x = qCos(2 * 2 * Pi / wheelSize + Pi / 4);
    guide[3].y = qSin(2 * 2 * Pi / wheelSize + Pi / 4);
    wheels->append( new Motor ((char) (settings->value("Wheel" + QString::number(3)).toInt()), \
                                       settings->value("Period").toInt(), \
                                       connection));
}

void Cyber::test()
{
    qDebug() << "Enter test";
    qint32 prev, now = 1;
    prev = 1;
    while (now != -1)
    {
        scanf("%d", &now);
        if (now != -1) break;
        qDebug() << "Enter change wheel #" << now;
        wheels->at(prev)->setPower(0);
        wheels->at(now)->setPower(100);
        prev = now;
    }
}

void Cyber::andrControl()
{
    initialiseMove();
    conServer->listen(QHostAddress::Any, settings->value("ConnectionPort").toInt());
    connect(conServer, SIGNAL(newConnection()), this, SLOT(setConnection()));
}

void Cyber::pcControl()
{
    qreal angle;
    printf("Enter angle (in deg): ");
    scanf("\n%g", &angle);
    moving.x = qCos(angle * Pi / 180);
    moving.y = qSin(angle * Pi / 180);
    speed = 100;
    initialiseMove();
}

void Cyber::autoControl()
{
    moving.x = qCos(0);
    moving.y = qSin(0);
    speed = 100;
    initialiseMove();
}

void Cyber::parseSignalAndroid(QStringList signal)
{
    if(signal.at(0) == "pad")
        switch (signal.at(1).toInt()) {
        case 1:
            if(signal.at(2).trimmed() == "up")
                moving.x = moving.y = 0;
            else
            {
                double x, y;
                x = signal.at(2).trimmed().toDouble();
                y = signal.at(3).trimmed().toDouble();
                moving.x = y;
                moving.y = -x;
                speed = qSqrt(x * x + y * y);
            }
            break;
        case 2:
            if(signal.at(2).trimmed() == "up")
                angVelocity = 0;
            else
            {
                qreal x, y;
                x = signal.at(2).trimmed().toDouble();
                y = signal.at(3).trimmed().toDouble();
                if (y <= 0)
                    break;
                angVelocity = round10( -x / qSqrt((x * x) + (y * y)) * 100);
            }
            break;
        default:
            break;
        }
    else if (signal.at(0) == "btn")
        switch (signal.at(1).toInt()) {
        case 1:
            stop(true);
            break;
        case 2:
            stop(true);
            disconnect(mainTimer, SIGNAL(timeout()), this, SLOT(moveVectorSlot()));
            calibrate();
            break;
        default:
            break;
        }
}

qint8 Cyber::round10(qreal number)
{
    quint8 x = (qint8) number;
    return x >= 20 ? x : 0;    // "/ 10) * 10;"
}

void Cyber::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_F1:
        qDebug() << "F1, stopping";
        stop(true);
        break;
    case Qt::Key_F2:
        stop(false);
        qDebug() << "Going to PC Control";
        pcControl();
        break;
    case Qt::Key_F3:
        stop(false);
        qDebug() << "Going to andrControl";
        andrControl();
        break;
    default:
        break;
    }
}

void Cyber::checkPosition()
{
    previous.tiltX = angles.tiltX;
    previous.tiltY = angles.tiltY;
    previous.tiltZ = angles.tiltZ;

    angles.tiltX = gyro->getTiltX();  //  Get gyro statements
    angles.tiltY = gyro->getTiltY();
    angles.tiltZ = gyro->getTiltZ();

    //  Kalman filter values
    direction = direction + \
            (kalmanCoef / checksPerSecond) * (setAngle( angles.tiltZ - correction ) * direction) + \
            ((1 - kalmanCoef) / checksPerSecond ) * (/*angVelocity -> new vector + */ direction);
    //  Change direction
    absolute.tiltZ = kalmanCoef * (angles.tiltZ - correction) + (1 - kalmanCoef) * angVelocity;
    currRad += absolute.tiltZ / checksPerSecond;
    if (count == 0)
    {
        while (currRad > ( 2 * Pi)) currRad -= 2 * Pi;
        while (currRad < (-2 * Pi)) currRad += 2 * Pi;
    }
}

void Cyber::moveVectorSlot()
{
    count++;
    count %= checksPerSecond / 5;

    checkPosition();

    if(count == 0)
    {
        for (int i = 0; i < wheelSize; i++)
        {
            quint8 mx = 100;
            if (speed >= 20 && angVelocity > 50)
            {
                mx = 80;
                wheels->at(i)->setPower((round10((setAngle(-currRad) * moving * guide[i]) * qMin(speed, mx)) + (angVelocity / 5)));
                //  Get those wheels spinning
            }
            else if (angVelocity > 50)
            {
                wheels->at(i)->setPower(angVelocity);
            }
            else if (speed >= 20)
                wheels->at(i)->setPower((round10((setAngle(-currRad) * moving * guide[i]) * qMin(speed, mx))));
            else stop(false);
        }
        qDebug()    << currRad\
                    << wheels->at(0)->getPower() << wheels->at(1)->getPower()\
                    << wheels->at(2)->getPower() << wheels->at(3)->getPower();
    }
}

void Cyber::calibrateSlot()
{
    count++;
    if (count == (2 * checksPerSecond))
    {
        mainTimer->stop();
        disconnect(mainTimer, SIGNAL(timeout()), this, SLOT(calibrateSlot()));
        correction = integrand / (2 * checksPerSecond);
        qDebug() << "Correction is:\t" + QString::number(correction);
        angles.tiltX = angles.tiltY = angles.tiltZ = \
                absolute.tiltX = absolute.tiltY = absolute.tiltZ = \
                currRad = integrand = count = 0;
        switch (controlMode) {
        case ANDROID_CONTROL:
            andrControl();
            break;
        case PC_CONTROL:
            pcControl();
        case AUTO_MODE:
            autoControl();
            break;
        default:
            break;
        }
    }
    checkPosition();
    integrand += angles.tiltZ;
}

void Cyber::tcpDisconnected()
{
    qDebug() << "TCP connection failed";
    stop(true);
    conSocket->disconnectFromHost();
}

void Cyber::setConnection()
{
    if(conSocket && conSocket->isValid()) qDebug () << "Replacing current connection";
    conSocket = conServer->nextPendingConnection();
    conSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(conSocket, SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
    connect(conSocket, SIGNAL(readyRead()), this, SLOT(readTcp()));
}

void Cyber::readTcp()
{
    if (!conSocket->isValid()) return ;
    while (conSocket->bytesAvailable() > 0)
    {
        char data [100];
        conSocket->readLine(data, 100);
        buffer->append(data);
        parseSignalAndroid(QString(data).split(" ", QString::SkipEmptyParts));
    }
    buffer->clear();
    return ;
}
