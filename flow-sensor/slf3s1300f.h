#ifndef SLF3S1300F_H
#define SLF3S1300F_H

#include <QObject>
#include <QTimer>
//#include "bcm2835.h"
#include "common.h"

#define SLAVE_ADDRESS 0x08
#define CLOCK_DIVIDE  BCM2835_I2C_CLOCK_DIVIDER_2500

class slf3s1300f : public QObject
{
    Q_OBJECT
public:
    explicit slf3s1300f(QObject *parent = nullptr);
    ~slf3s1300f();
    void init();

    enum command{

        START_MEASURE_WATER =0x0,
        START_MEASURE_ALCHOL,
        STOP_MEASURE,
        SOFT_RESET,
        READ_PRODUCT_ID
    };

    quint32 operation(command);    

signals:
      void sig_flow_sensor_read(flow_info);

public slots:

private:
    int i2cfd =0;

    quint32 cmd_length, product_number=0;
    quint64 serial_number = 0;
    char recv_buf[18] = {0x0,};
    char cmd_buf[4] = {0x0,};

    float flow_rate, Temp;
    bool air_in_line_flag, high_flow_flag, exp_smoothing_active;

    flow_info m_flow_result;
};

#endif // SLF3S1300F_H
