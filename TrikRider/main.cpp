#include "cyber.h"
#include <QCoreApplication>

int main (int argc, char ** argv)
{
    QCoreApplication * a = new QCoreApplication (argc, argv);
    vector c;
    float x;
//    int toRet;
    Cyber * b = new Cyber();
    qDebug() << "Engines is going to run";
    while(scanf("%f", &x))
    {
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
                b->moveByVector(70, normalize(c));
            break;
        }
    }
    qDebug() << "Exited program";
    return a->exec();
}
