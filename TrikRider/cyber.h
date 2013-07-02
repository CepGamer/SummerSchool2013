#ifndef CYBER_H
#define CYBER_H

#include <QObject>
#include <QSharedPointer>
#include "wheel.h"

struct vector
{
    int x, y;
};

struct matrix
{
    int _11;
    int _12;
    int _21;
    int _22;
};

inline vector operator*(matrix *a, vector *b) const;
inline vector operator+(vector *a, vector *b) const;
inline int operator*(vector *a, vector *b) const;

class Cyber : public QObject
{
    Q_OBJECT
public:
    explicit Cyber(QObject *parent = 0);
    inline void turn (float degree);    //  NOTE:
    //  We allow turning for more than 360 degree (that's may be bad because of float precision)
    void stop();
    void moveByVector (float speed);

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
