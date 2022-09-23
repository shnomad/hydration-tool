#include <QDebug>
#include "slf3s1300f.h"

slf3s1300f::slf3s1300f(QObject *parent) : QObject(parent)
{
    if(bcm2835_init())
       qDebug()<<"bcm2835 init success";
    else
       exit(0);

   if(bcm2835_i2c_begin())
      qDebug()<<"bcm2835 I2C init success";
   else
      exit(0);   
}

void slf3s1300f::init()
{
    bcm2835_i2c_setSlaveAddress(SLAVE_ADDRESS);  //I2C address
    bcm2835_i2c_setClockDivider(CLOCK_DIVIDE);
}

quint32 slf3s1300f::operation(command cmd)
{
    quint8 result =0;

    switch(cmd)
    {
        case START_MEASURE_WATER:

             cmd_buf[0] = 0x36;
             cmd_buf[1] = 0x08;

             cmd_length = 2;

            break;

        case START_MEASURE_ALCHOL:

            cmd_buf[0] = 0x36;
            cmd_buf[1] = 0x15;

            cmd_length = 2;

            break;

        case STOP_MEASURE:

            cmd_buf[0] = 0x3F;
            cmd_buf[1] = 0xF9;

            cmd_length = 2;

            break;

        case SOFT_RESET:

            bcm2835_i2c_setSlaveAddress(0x0);  //I2C address

            cmd_buf[0] = 0x06;

            cmd_length = 1;

            break;

        case READ_PRODUCT_ID:

            cmd_buf[0] = 0x36;
            cmd_buf[1] = 0x7C;
            cmd_buf[2] = 0xE1;
            cmd_buf[3] = 0x02;

            cmd_length = 2;

         break;

        default:
            break;
    }

    if(cmd == READ_PRODUCT_ID)
    {
        result = bcm2835_i2c_write(cmd_buf, cmd_length);
        result = bcm2835_i2c_write(cmd_buf+2, cmd_length);

        bcm2835_delay(10);

        result = bcm2835_i2c_read(recv_buf,18);
        product_number = recv_buf[0] <<24  | recv_buf[1] << 16  | recv_buf[3] << 8  | recv_buf[4];
        serial_number = recv_buf[6] << 56  | recv_buf[7] << 48  | recv_buf[9] << 40 | recv_buf[10] << 32 \
                     | recv_buf[12] << 24  | recv_buf[13] << 16 | recv_buf[15] << 8 | recv_buf[16];
    }

    if(cmd == SOFT_RESET)
    {
        result = bcm2835_i2c_write(cmd_buf, cmd_length);
        bcm2835_i2c_setSlaveAddress(SLAVE_ADDRESS);  //I2C address

        bcm2835_delay(100);
    }

    if(cmd == START_MEASURE_WATER || cmd == START_MEASURE_ALCHOL)
    {                
        result = bcm2835_i2c_write(cmd_buf, cmd_length);

        bcm2835_delay(300);
        result = bcm2835_i2c_read(recv_buf,9);

        flow_rate = recv_buf[0]<< 8 | recv_buf[1];
        Temp = recv_buf[3]<< 8 | recv_buf[4];
        air_in_line_flag = recv_buf[7] & 0x01;
        high_flow_flag = recv_buf[7] & 0x02;
        exp_smoothing_active = recv_buf[7] & 0x10;

        qDebug()<<"flow_rate :"<<flow_rate<<"\n";
        qDebug()<<"Temperature :"<<Temp<<"\n";
        qDebug()<<"air_in_line_flag :"<<air_in_line_flag<<"\n";
        qDebug()<<"high_flow_flag :"<<high_flow_flag<<"\n";
        qDebug()<<"exp_smoothing_active :"<<exp_smoothing_active<<"\n";

        emit sig_flow_sensor_read(recv_buf);
    }

    memset(cmd_buf,0x0, sizeof (cmd_buf));
    memset(recv_buf,0x0, sizeof (recv_buf));

    return result;
}

void slf3s1300f::write_command()
{

}

void slf3s1300f::read_data()
{

}

slf3s1300f::~slf3s1300f()
{
    bcm2835_i2c_end();
    bcm2835_close();
}
