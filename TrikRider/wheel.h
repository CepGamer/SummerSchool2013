#ifndef WHEEL_H
#define WHEEL_H

#include <QObject>

class wheel : public QObject
{
    Q_OBJECT
public:
    explicit wheel(int wheelNumber, QObject *parent = 0);
    void spinForw(float speed, float msecs);
    void spinBackw(float speed, float msecs);
    void spinForw(float speed);
    void spinBackw(float speed);
    void stop();
signals:
    
public slots:
    
};

#endif // WHEEL_H
