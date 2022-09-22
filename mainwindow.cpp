#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Init();

    timer_sec = new QTimer(this);                           //display system time
    QObject::connect(timer_sec, SIGNAL(timeout()), this, SLOT(Display_CurrentTime()));
    timer_sec->start(1000);
}

void MainWindow::Init()
{

    /* Create MainWindow display label*/
    QFont font_type_1, font_type_2;
    font_type_1.setPointSize(font_type_1.pointSize()*0.5);
    font_type_2.setPointSize(font_type_2.pointSize()*0.8);

    ui->tabWidget->setFont(font_type_2);

    ui->system_current_time->setFont(font_type_1);
    ui->hydration_start_time->setFont(font_type_2);
    ui->hydration_end_time->setFont(font_type_2);

    ui->hydration_status->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");
    ui->hydration_countdown->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");
    ui->hydration_start_time->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");
    ui->hydration_start_time->setText("Start Time: " + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));

    ui->hydration_end_time->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");
    ui->hydration_end_time->setText("End Time: " + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));

    ui->hydration_start->setFont(font_type_2);
    ui->hydration_stop->setFont(font_type_2);


    /*home tab*/
    /* sensor card hydration status table */
    QStringList table_header;

    /* CH 1 ~ 8 */
    ui->ch_gr_1->setColumnCount(3);
    ui->ch_gr_1->setColumnWidth(0,95);
    ui->ch_gr_1->setColumnWidth(1,150);
    ui->ch_gr_1->setColumnWidth(2,200);

    table_header<<"Channel"<<"Bias(mV)"<<"Status";
    ui->ch_gr_1->setHorizontalHeaderLabels(table_header);

    ui->ch_gr_1->setRowCount(8);
    for(quint8 row_count=0; row_count<8; row_count++)
    {
       ui->ch_gr_1->setRowHeight(row_count, 40);
    }

    ui->ch_gr_1->verticalHeader()->setVisible(false);
    ui->ch_gr_1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ch_gr_1->setFocusPolicy(Qt::NoFocus);
    ui->ch_gr_1->setSelectionMode(QAbstractItemView::NoSelection);

    /* CH 2 ~ 16 */
    ui->ch_gr_2->setColumnCount(3);
    ui->ch_gr_2->setColumnWidth(0,95);
    ui->ch_gr_2->setColumnWidth(1,150);
    ui->ch_gr_2->setColumnWidth(2,200);

    table_header<<"Channel"<<"Bias(mV)"<<"Status";
    ui->ch_gr_2->setHorizontalHeaderLabels(table_header);

    ui->ch_gr_2->setRowCount(8);
    for(quint8 row_count=0; row_count<8; row_count++)
    {
       ui->ch_gr_2->setRowHeight(row_count, 40);
    }

    ui->ch_gr_2->verticalHeader()->setVisible(false);
    ui->ch_gr_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ch_gr_2->setFocusPolicy(Qt::NoFocus);
    ui->ch_gr_2->setSelectionMode(QAbstractItemView::NoSelection);

    /*settings tab*/
    ui->set_hydration_time->setMaximumTime(QTime(48,00));
    ui->set_hydration_time->setDisplayFormat("hh:mm");
    ui->set_hydration_time->setTime(QTime(12,00));

}


void MainWindow::Display_CurrentTime()
{
    ui->system_current_time->setText("Current Time: " + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

