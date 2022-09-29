#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QThread>
#include "common.h"
#include "popupwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ads1120;
class slf3s1300f;

#define VALVE_CONTROL_GPIO          RPI_BPLUS_GPIO_J8_37
#define MOTOR_ONOFF_CONTROL_GPIO    RPI_BPLUS_GPIO_J8_35
#define MOTOR_PWM_CONTROL_GPIO      RPI_BPLUS_GPIO_J8_12
#define MOTOR_ENABLE                0x0
#define MOTOR_DISABLE               0x1
#define PWM_CHANNEL                 0x0
#define PWM_RANGE                   1024

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();    
    void Init();
    void System_Information();
    void Set_Peripheral();
    void Function_Disable(bool);
    QString Seconds_To_Time(quint64);
    void show_popup(popup_event,QString);

signals:

    void sig_popup_window_mgs(QString);

private slots:
    void Display_CurrentTime();
    void Read_FlowSensor();
    void Display_FlowSensor(flow_info);
    void Display_Hydration_CountDown();
    void on_hydration_start_clicked();
    void on_hydration_stop_clicked();
    void on_set_hydration_time_userTimeChanged(const QTime &time);
    void on_adc_cal_clicked();
    void on_reboot_clicked();
    void on_power_off_clicked();

    void on_valve_open_clicked();
    void on_valve_close_clicked();

    void on_pump_start_clicked();
    void on_pump_stop_clicked();

    void on_bias_on_clicked();
    void on_bias_off_clicked();

    void on_pwm_value_set_valueChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QTimer *timer_sec;
    ads1120 *m_adc;
    slf3s1300f *m_flowSensor;
    QTimer *m_flow_sensor, *m_adc_read, *m_hydration_count_down;

    quint32 hydration_count_down_sec =0;
    quint64 hydration_start_time_sec =0, hydration_end_time_sec=0;

    quint16 pwm_data = 1, clock_divide=16;
    bool pump_working=false;

    popupwindow * m_popupwindow;

    bool system_init_done = false;
};

#endif // MAINWINDOW_H
