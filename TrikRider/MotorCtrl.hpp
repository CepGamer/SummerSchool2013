#pragma once
#include <QDebug>
#include "I2cConnection.hpp"


class Motor : public QObject
{
	Q_OBJECT
public:
	Motor(char jack,int period, I2cConnection* i2cCon);
    ~Motor();

    int setPower(int _power);
	int setPeriod(int _period);
    inline int getPower() {return m_power * clockwise;}
	char getPowerRegister(char jack);
	char getPeriodRegister(char jack);
    inline void revertClockwise() {clockwise = -clockwise;}


public slots:

signals:
	int toLog(QString msg);

protected:
	unsigned char m_jack; 
	int m_power;
	I2cConnection* m_i2cCon;
    qint8 clockwise;
    static const char min_Period = 200;
};
