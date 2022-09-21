#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFont label_font;
    label_font = ui->current_time->font();
    label_font.setPixelSize(label_font.pointSize()*LABEL_FONT_SIZE_RESETTTING);
    ui->current_time->setFont(label_font);

    QFont time_font;
    time_font = ui->start_time->font();
    time_font.setPixelSize(time_font.pointSize()*LABEL_FONT_SIZE_RESETTTING);
    ui->start_time->setFont(time_font);

    time_font = ui->end_time->font();
    time_font.setPixelSize(time_font.pointSize()*LABEL_FONT_SIZE_RESETTTING);
    ui->end_time->setFont(time_font);

    time_font = ui->residual_time->font();
    time_font.setPixelSize(time_font.pointSize()*LABEL_FONT_SIZE_RESETTTING);
    ui->residual_time->setFont(time_font);

    time_font = ui->status->font();
    time_font.setPixelSize(time_font.pointSize()*LABEL_FONT_SIZE_RESETTTING);
    ui->status->setFont(time_font);

    QFont tab_font;
    tab_font = ui->tabWidget->font();
    tab_font.setPixelSize(tab_font.pointSize()*TAB_FONT_SIZE_RESETTTING);
    ui->tabWidget->setFont(tab_font);

    timer_sec = new QTimer(this);                          //display system time
    QObject::connect(timer_sec, SIGNAL(timeout()), this, SLOT(Display_CurrentTime()));
    timer_sec->start(1000);


#if 0
    /* channel group 1 */
    QStringList table_header;
    table_header<<"Channel"<<"Bias(mV)"<<"Status";

    for(quint8 col_count=0; col_count<3; col_count++)
        ui->ch_gr_1->insertColumn(col_count);
    ui->ch_gr_1->setColumnWidth(0,96);
    ui->ch_gr_1->setColumnWidth(1,160);
    ui->ch_gr_1->setColumnWidth(2,190);
    ui->ch_gr_1->setHorizontalHeaderLabels(table_header);

    for(quint8 row_count=0; row_count<8; row_count++)
    {
        ui->ch_gr_1->insertRow(row_count);
        ui->ch_gr_1->setRowHeight(row_count,40);
    }

    ui->ch_gr_1->verticalHeader()->hide();

    /* channel group 2 */
    for(quint8 col_count=0; col_count<3; col_count++)
        ui->ch_gr_2->insertColumn(col_count);
    ui->ch_gr_2->setColumnWidth(0,96);
    ui->ch_gr_2->setColumnWidth(1,160);
    ui->ch_gr_2->setColumnWidth(2,190);
    ui->ch_gr_2->setHorizontalHeaderLabels(table_header);

    for(quint8 row_count=0; row_count<8; row_count++)
    {
        ui->ch_gr_2->insertRow(row_count);
        ui->ch_gr_2->setRowHeight(row_count,40);
    }

    ui->ch_gr_2->verticalHeader()->hide();

#endif


}


void MainWindow::Display_CurrentTime()
{
    ui->current_time->setText("Current Time: " + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

