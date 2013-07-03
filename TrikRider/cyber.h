#ifndef CYBER_H
#define CYBER_H

#include <QObject>
#include <QSharedPointer>

#include "wheel.h"

const float degPerSecMCoef = 15;

class Cyber : public QObject
{
    Q_OBJECT
public:
    explicit Cyber(QObject *parent = 0);
    inline void turn (float degree);    //  NOTE:
    //  We allow turning for more than 360 degree (that's may be bad because of float precision)
    void stop();
    void moveByVector (float speed, vector guide);

private:
    wheel _1;
    wheel _2;
    wheel _3;
    void turnLeft (float degree);
    void turnRight (float degree);

signals:
    
public slots:
    
};

#endif // CYBER_H
