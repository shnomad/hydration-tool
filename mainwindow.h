#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ads1120;
class slf3s1300f;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();    
    void Init();
    void Set_Peripheral();
    void Function_Disable(bool);
    QString Seconds_To_Time(quint64);

signals:

private slots:
    void Display_CurrentTime();
    void Read_FlowSensor();
    void Display_FlowSensor(char *);
    void Display_Hydration_CountDown();

    void on_hydration_start_clicked();
    void on_hydration_stop_clicked();
    void on_set_hydration_time_userTimeChanged(const QTime &time);

    void on_adc_cal_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer_sec;
    ads1120 *m_adc;
    slf3s1300f *m_flowSensor;
    QTimer *m_flow_sensor, *m_adc_read, *m_hydration_count_down;

    quint32 hydration_count_down_sec =0;
    quint64 hydration_start_time_sec =0, hydration_end_time_sec=0;

};

#endif // MAINWINDOW_H
