#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QStandardItemModel>
#include <QHeaderView>

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

private slots:
    void Display_CurrentTime();

private:    
    Ui::MainWindow *ui;
    QTimer *timer_sec;
    ads1120 *m_adc;
    slf3s1300f *m_flowSensor;

};

#endif // MAINWINDOW_H
