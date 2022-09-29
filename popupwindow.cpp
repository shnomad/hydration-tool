#include "popupwindow.h"
#include "ui_popupwindow.h"


//popupwindow::popupwindow(QString msg, QWidget *parent) :
popupwindow::popupwindow(popup_event event,QString msg, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::popupwindow)
{
    ui->setupUi(this);    

    QFont font_type_1, font_type_2;
    font_type_1.setPointSize(font_type_1.pointSize()*0.5);
    font_type_2.setPointSize(font_type_2.pointSize()*3);

    ui->pop->setStyleSheet("color: rgb(58, 134, 255); background-color: white; border: 2px solid rgb(58, 134, 255)");
    ui->pushButton->setStyleSheet("color: black; background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px;");
    ui->pushButton->setText("Close");

    title = new QLabel(this);
    title->setAlignment(Qt::AlignCenter);

    if(event == HYDRATION_PROGRESS)
    {
        title->setGeometry(40,10,240,30);
        title->setText(msg);

        countdown = new QLabel(this);
        countdown->setGeometry(20,90,280,60);
        countdown->setAlignment(Qt::AlignCenter);
        countdown->setFont(font_type_2);
    }
}

void popupwindow::receive_popup_msg(QString msg)
{
    countdown->setText(msg);
}

popupwindow::~popupwindow()
{
    delete ui;
}

void popupwindow::on_pushButton_clicked()
{
    popupwindow::close();
}
