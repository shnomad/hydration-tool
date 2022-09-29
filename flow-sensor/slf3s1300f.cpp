#include <QDebug>
#include "slf3s1300f.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include<fcntl.h>
#include <linux/i2c-dev.h>

slf3s1300f::slf3s1300f(QObject *parent) : QObject(parent)
{

#ifndef USE_BCM2835_LIBRARY

    i2cfd  = open("/dev/i2c-0", O_RDWR);

   if(i2cfd<0)
   {
       Log()<<"I2C-0 fail!!";
       exit(0);
   }
   else
   {
       Log()<<"I2C-0 Success!!";
       ioctl(i2cfd, I2C_SLAVE, SLAVE_ADDRESS);
   }

#else
    if(bcm2835_init())
       qDebug()<<"bcm2835 init success";
    else
       exit(0);

   if(bcm2835_i2c_begin())
      qDebug()<<"bcm2835 I2C init success";
   else
      exit(0);      
#endif

   qRegisterMetaType<flow_info>();
}

void slf3s1300f::init()
{
#ifndef USE_BCM2835_LIBRARY


#else
    bcm2835_i2c_setSlaveAddress(SLAVE_ADDRESS);  //I2C address
    bcm2835_i2c_setClockDivider(CLOCK_DIVIDE);
#endif
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

#ifndef USE_BCM2835_LIBRARY

#else
            bcm2835_i2c_setSlaveAddress(0x0);  //I2C address
#endif

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

#ifndef USE_BCM2835_LIBRARY

         write(i2cfd,cmd_buf,cmd_length);
         write(i2cfd,cmd_buf+2,cmd_length);

         read(i2cfd,recv_buf,18);

#else
        result = bcm2835_i2c_write(cmd_buf, cmd_length);
        result = bcm2835_i2c_write(cmd_buf+2, cmd_length);

        bcm2835_delay(10);

        result = bcm2835_i2c_read(recv_buf,18);
#endif

        product_number = recv_buf[0] <<24  | recv_buf[1] << 16  | recv_buf[3] << 8  | recv_buf[4];
        serial_number = recv_buf[6] << 56  | recv_buf[7] << 48  | recv_buf[9] << 40 | recv_buf[10] << 32 \
                     | recv_buf[12] << 24  | recv_buf[13] << 16 | recv_buf[15] << 8 | recv_buf[16];
    }

    if(cmd == SOFT_RESET)
    {
#ifndef USE_BCM2835_LIBRARY

        ioctl(i2cfd, I2C_SLAVE, 0x0);
        write(i2cfd,cmd_buf,cmd_length);

        ioctl(i2cfd, I2C_SLAVE, SLAVE_ADDRESS);
#else
        result = bcm2835_i2c_write(cmd_buf, cmd_length);
        bcm2835_i2c_setSlaveAddress(SLAVE_ADDRESS);  //I2C address
        bcm2835_delay(100);
#endif
    }

    if(cmd == START_MEASURE_WATER || cmd == START_MEASURE_ALCHOL)
    {

#ifndef USE_BCM2835_LIBRARY

        write(i2cfd,cmd_buf,cmd_length);
        read(i2cfd,recv_buf,9);

#else
        result = bcm2835_i2c_write(cmd_buf, cmd_length);

        bcm2835_delay(300);
        result = bcm2835_i2c_read(recv_buf,9);
#endif

        float flow_rate, Temp;

        quint16  flow_rate_tmp = recv_buf[0]<< 8 | recv_buf[1];
        quint16  Temp_tmp = recv_buf[3]<< 8 | recv_buf[4];
        bool   air_in_line_flag = recv_buf[7] & 0x01;
        bool   high_flow_flag = recv_buf[7] & 0x02;
        bool   exp_smoothing_active = recv_buf[7] & 0x10;

        if(flow_rate_tmp<32768)
        {
            flow_rate = (float)(flow_rate_tmp);
            flow_rate = flow_rate/500;
        }
        else
        {
            flow_rate_tmp = ~(flow_rate_tmp);
            flow_rate_tmp +=1;
            flow_rate = (float)(flow_rate_tmp);
            flow_rate = -(flow_rate/500);
        }

        if(Temp_tmp<32768)
        {
            Temp = (float)Temp_tmp;
            Temp = Temp/200;
        }
        else
        {
            Temp_tmp = ~(Temp_tmp);
            Temp_tmp +=1;
            Temp = (float)(Temp_tmp);
            Temp = -(Temp/200);
        }

        m_flow_result.flow = flow_rate;
        m_flow_result.temp = Temp;
        m_flow_result.air_in_line_flag = air_in_line_flag;
        m_flow_result.high_flow_flag = high_flow_flag;
        m_flow_result.exp_smoothing_active = exp_smoothing_active;

        emit sig_flow_sensor_read(m_flow_result);
    }

    memset(cmd_buf,0x0, sizeof (cmd_buf));
    memset(recv_buf,0x0, sizeof (recv_buf));

    return result;
}

slf3s1300f::~slf3s1300f()
{
#ifndef USE_BCM2835_LIBRARY

    close(i2cfd);

#else
    bcm2835_i2c_end();
    bcm2835_close();

#endif
}
