#include "cyber.h"
#include <QCoreApplication>
#include <QtNetwork/QTcpServer>
#include <QByteArray>
#include <QDataStream>

enum progtype {ANDROID_CONTROL, AUTO_MODE, PC_CONTROL, OTHER};

int main (int argc, char ** argv)
{
    QCoreApplication * a = new QCoreApplication (argc, argv);
    /*QTcpServer * reciever = new QTcpServer;
    QByteArray * buffer;
    QDataStream * numbers;*/
    vector c;
    qreal x;
    progtype type = OTHER;
//    int toRet;
    Cyber * b = new Cyber(I2C);
    qDebug() << "Engines is going to run";
    if (argc == 2)
    {
        if (strcmp("android", argv[1]) == 0)
            type = ANDROID_CONTROL;
        else if(strcmp("auto", argv[1]) == 0)
            type = AUTO_MODE;
        else if(strcmp("pc", argv[1]) == 0)
            type = PC_CONTROL;
    }
/*    switch (type) {
    case ANDROID_CONTROL:

        break;
    case AUTO_MODE:

        break;
    case PC_CONTROL:
        while(true)
        {
            if (numbers != NULL)
                delete numbers;
            reciever = new QUdpSocket();
            reciever->connectToHost("192.168.0.1", 22);
            numbers = new QDataStream(reciever->read(16));
            *numbers >> c.x >> c.y >> x;
            qDebug() << c.x << '\t' << c.y << 't' << x;
            b->moveByVector(30 + 40 * sqrt(x / 125000.0), normalize(c));
        }
        break;
    default:
        while(scanf("%f", &x))
            switch(qRound(x))
            {
                case 0:
                    b->stop();
                    break;
                case 360:
                    b->~Cyber();
                    break;
                case 500:
                    b->turn(15);
                    break;
                default:
                    qDebug() << "Degree is:\t" << qRound(x);
                    c.x = cos ( x * Pi / 180);
                    c.y = sin ( x * Pi / 180);
                    qDebug() << "Moving vector is:\t" << (qreal)c.x << '\t' << (qreal)c.y;
                    b->moveByVector(50, normalize(c));
                break;
            }
        break;
    }*/
    if(type == ANDROID_CONTROL)
        5;
    else if(type == AUTO_MODE)
        5;
    /*else if (type == PC_CONTROL)
    {
        while(true)
        {
            if (numbers != NULL)
                delete numbers;
            if(buffer != NULL)
                delete buffer;
            reciever->listen(QHostAddress("192.168.0.126"), 22);
            reciever->nextPendingConnection();
            buffer = new QByteArray(reciever->read(16));
            if(buffer && buffer->isEmpty())
                continue;
            numbers = new QDataStream(*buffer);
            *numbers >> c.x >> c.y >> x;
            qDebug() << c.x << '\t' << c.y << '\t' << x;
            b->moveByVector(30 + 40 * sqrt(x / 125000.0), normalize(c));
        }
    }*/
    else
    {
//        while(scanf("%f", &x))
//        scanf("%f", &x);
        x = 600.2;
//        qDebug() << x;
            switch(qRound(x))
            {
            case 0:
                b->stop();
                break;
/*            case 500:
                b->turn(360);
                break;*/
            case 600:
                b->calibrate();
                break;
            default:
/*                qDebug() << "Degree is:\t" << qRound(x);
                c.x = cos ( x * Pi / 180);
                c.y = sin ( x * Pi / 180);
                qDebug() << "Moving vector is:\t" << (qreal)c.x << '\t' << (qreal)c.y;*/
                b->turn(qRound(x));
            break;
            }
    }
//    delete b;
//    qDebug() << "Exited program";
    return a->exec();
}
