#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


#define LABEL_FONT_SIZE_RESETTTING 1.5
#define TAB_FONT_SIZE_RESETTTING   1.6

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void Display_CurrentTime();


private:
    Ui::MainWindow *ui;
    QTimer *timer_sec;

};
#endif // MAINWINDOW_H
