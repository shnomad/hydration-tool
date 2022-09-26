#ifndef COMMON_H
#define COMMON_H

#include <QDebug>
#include <QDateTime>

#define Log() qDebug() <<"["<<QDateTime::currentDateTime().toString("MM.dd hh:mm:ss") << __PRETTY_FUNCTION__ << __LINE__ << "]"

struct flow_info
{
   float flow = 0.0;
   float temp = 0.0;
   bool air_in_line_flag =false;
   bool high_flow_flag =false;
   bool exp_smoothing_active =false;
}; Q_DECLARE_METATYPE(flow_info)

#endif // COMMON_H
