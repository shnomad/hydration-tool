#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QWidget>
#include <QLabel>
#include "common.h"

namespace Ui {
class popupwindow;
}

class popupwindow : public QWidget
{
    Q_OBJECT

public:

//    explicit popupwindow(QString, QWidget *parent = nullptr);
     explicit popupwindow(popup_event,QString, QWidget *parent = nullptr);
    ~popupwindow();

signals:    

private slots:
    void on_pushButton_clicked();
    void receive_popup_msg(QString);

private:
    Ui::popupwindow *ui;
    popup_event m_event;
    QLabel *title, *countdown;
};

#endif // POPUPWINDOW_H
