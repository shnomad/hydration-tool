#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QProcess>
#include <QtNetwork/QNetworkInterface>
#include "builddatetime.h"
#include "adc/ads1120.h"
#include "flow-sensor/slf3s1300f.h"
#include "expander/iopi.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Init();
    System_Information();
    Set_Peripheral();

   /*ADC operation test*/

#if 1
    m_adc = new ads1120;

    m_adc->begin();
    m_adc->setGain(1);
    m_adc->setDataRate(0x0);
    m_adc->setOpMode(0x0);
    m_adc->setConversionMode(0x0);
    m_adc->setVoltageRef(0x1);
    m_adc->setDRDYmode(0);
    m_adc->setTemperatureMode(0x0);

    /* CH0 : 0x8
     * CH1 : 0x9
     * CH2 : 0xA
     * CH3 : 0xB
    */

    m_adc->setMultiplexer(0x08);
    quint16 value_a0 = m_adc->readADC_Single();

    qDebug()<<"read adc Value :" <<value_a0;

    m_adc->setMultiplexer(0x09);
    quint16 value_a1 = m_adc->readADC_Single();

    qDebug()<<"read adc Value :" <<value_a1;

#endif

}

void MainWindow::Init()
{
    /* Create MainWindow display label*/
    QFont font_type_1, font_type_2;
    font_type_1.setPointSize(font_type_1.pointSize()*0.5);
    font_type_2.setPointSize(font_type_2.pointSize()*0.8);

    ui->hydration_status->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");
    ui->hydration_countdown->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");
    ui->hydration_start_time->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");
    ui->hydration_end_time->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");

    ui->reboot->setStyleSheet("color: rgb(58, 134, 255); background-color: white; border: 2px solid rgb(58, 134, 255)");
    ui->power_off->setStyleSheet("color: rgb(251, 86, 7); background-color: white; border: 2px solid rgb(251, 86, 7)");

    /*home tab*/
    /* sensor card hydration status table */
    QStringList table_header;

    /* CH 1 ~ CH 8 */
    ui->ch_gr_1->setColumnCount(3);
    ui->ch_gr_1->setColumnWidth(0,95);
    ui->ch_gr_1->setColumnWidth(1,150);
    ui->ch_gr_1->setColumnWidth(2,200);

    table_header<<"Channel"<<"Bias(mV)"<<"Status";
    ui->ch_gr_1->setHorizontalHeaderLabels(table_header);
    ui->ch_gr_1->horizontalHeader()->setStyleSheet("QHeaderView { font-size: 12pt; }");

    ui->ch_gr_1->setRowCount(8);
    for(quint8 row_count=0; row_count<8; row_count++)
    {
       ui->ch_gr_1->setRowHeight(row_count, 40);       
    }

    ui->ch_gr_1->verticalHeader()->setVisible(false);
    ui->ch_gr_1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ch_gr_1->setFocusPolicy(Qt::NoFocus);
    ui->ch_gr_1->setSelectionMode(QAbstractItemView::NoSelection);

    for(quint8 row_count=0; row_count<8; row_count++)
    {
        QTableWidgetItem *pCell = new QTableWidgetItem;
        pCell->setText(QString::number(row_count+1));
        pCell->setTextAlignment(Qt::AlignCenter);
        ui->ch_gr_1->setItem(row_count, 0, pCell);
    }

    /* CH 2 ~ CH 16 */
    ui->ch_gr_2->setColumnCount(3);
    ui->ch_gr_2->setColumnWidth(0,95);
    ui->ch_gr_2->setColumnWidth(1,150);
    ui->ch_gr_2->setColumnWidth(2,200);

    table_header<<"Channel"<<"Bias(mV)"<<"Status";
    ui->ch_gr_2->setHorizontalHeaderLabels(table_header);
    ui->ch_gr_2->horizontalHeader()->setStyleSheet("QHeaderView { font-size: 12pt; }");

    ui->ch_gr_2->setRowCount(8);
    for(quint8 row_count=0; row_count<8; row_count++)
    {
       ui->ch_gr_2->setRowHeight(row_count, 40);
    }

    ui->ch_gr_2->verticalHeader()->setVisible(false);
    ui->ch_gr_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ch_gr_2->setFocusPolicy(Qt::NoFocus);
    ui->ch_gr_2->setSelectionMode(QAbstractItemView::NoSelection);

    for(quint8 row_count=0; row_count<8; row_count++)
    {
        QTableWidgetItem *pCell = new QTableWidgetItem;
        pCell->setText(QString::number(row_count+9));
        pCell->setTextAlignment(Qt::AlignCenter);
        ui->ch_gr_2->setItem(row_count, 0, pCell);
    }

    /*settings hydration time*/
    ui->set_hydration_time->setMaximumTime(QTime(48,00));
    ui->set_hydration_time->setDisplayFormat("hh:mm");
    ui->set_hydration_time->setTime(QTime(12,00));

    /*test mode setting*/

    /*Create flow status table */

    /*1 sec timer for Current Clock*/
    timer_sec = new QTimer(this);                           //display system time
    QObject::connect(timer_sec, SIGNAL(timeout()), this, SLOT(Display_CurrentTime()));
    timer_sec->start(1000);

    /*1 sec timer for hydration count down*/
    m_hydration_count_down = new QTimer(this);
    QObject::connect(m_hydration_count_down, SIGNAL(timeout()), this, SLOT(Display_Hydration_CountDown()));

}

void MainWindow::Set_Peripheral()
{

#ifndef USE_BCM2835_LIBRARY


#else
    /*bcm2835 Library Init*/
    if(bcm2835_init())
       qDebug()<<"bcm2835 init success";
    else
       exit(0);
#endif

   /*Create Timer for flow sensor*/
    m_flow_sensor= new QTimer;
    QObject::connect(m_flow_sensor, SIGNAL(timeout()), this, SLOT(Read_FlowSensor()));

   /*Create IO Plus*/

   /*Create flow sensor Control*/
    m_flowSensor = new slf3s1300f;
    m_flowSensor->init();
    m_flowSensor->operation(slf3s1300f::SOFT_RESET);
    m_flowSensor->operation(slf3s1300f::READ_PRODUCT_ID);

   /*Create ADC Control*/

   /*Create PWM Motor Control*/

   /*Create Signal/Slot Connection*/
    connect(m_flowSensor,SIGNAL(sig_flow_sensor_read(flow_info)), this, SLOT(Display_FlowSensor(flow_info)));
}

void MainWindow::System_Information()
{
    /* Network Information*/
    QString eth_ip, wlan_ip;

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    int eth0_num = QNetworkInterface::interfaceIndexFromName("eth0");
    int wlan_num = QNetworkInterface::interfaceIndexFromName("wlan0");

    if(eth0_num > 0 &&  eth0_num <= interfaces.count())
    {
        QList<QNetworkAddressEntry> entries = interfaces.at(eth0_num-1).addressEntries();

        for (int j = 0; j < entries.count(); j++)
        {
            if (entries.at(j).ip().protocol() == QAbstractSocket::IPv4Protocol)
            {
                eth_ip = entries.at(j).ip().toString();
                qDebug() << eth_ip;
                ui->eth->setText("ETH0 IP : "+ eth_ip);
            }
        }
    }
    else
    {
         ui->eth->setText("ETH0 IP : ");
    }

    if(wlan_num > 0 &&  wlan_num <= interfaces.count())
    {
        QList<QNetworkAddressEntry> entries = interfaces.at(wlan_num-1).addressEntries();

        for (int j = 0; j < entries.count(); j++)
        {
            if (entries.at(j).ip().protocol() == QAbstractSocket::IPv4Protocol)
            {
                wlan_ip = entries.at(j).ip().toString();
                qDebug() << wlan_ip;
                ui->wlan->setText("WLAN0 IP : "+ wlan_ip);
            }
        }
    }
    else
    {
         ui->wlan->setText("WLAN0 IP : ");
    }

    /* Application build date*/
    ui->build_date->setText(build_date);
}


void MainWindow::Display_CurrentTime()
{
    ui->system_current_time->setText("Current Time: " + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));
}

void MainWindow::Read_FlowSensor()
{
    m_flowSensor->operation(slf3s1300f::START_MEASURE_WATER);
}

void MainWindow::Display_FlowSensor(flow_info flow_sensing_result)
{
    ui->flow->setText("Flow : " + QString::number(flow_sensing_result.flow));
    ui->temp->setText("Temp : " + QString::number(flow_sensing_result.temp));
    ui->high_flow->setText("High Flow Signal :" + QString::number(flow_sensing_result.high_flow_flag));
    ui->air_in_line->setText("Air-in-line flag :" + QString::number(flow_sensing_result.air_in_line_flag));
    ui->exp_smoothing->setText("Exponential smoothing active :" +QString::number(flow_sensing_result.exp_smoothing_active));
}

void MainWindow::Display_Hydration_CountDown()
{
    if(hydration_count_down_sec == 0)
    {
        on_hydration_stop_clicked();
    }
    else
    {
        hydration_count_down_sec--;
        ui->hydration_countdown->setText(Seconds_To_Time(hydration_count_down_sec));

        emit sig_popup_window_mgs(Seconds_To_Time(hydration_count_down_sec));
    }
}

void MainWindow::on_hydration_start_clicked()
{
    m_flow_sensor->start(1000);
    m_hydration_count_down->start(1000);

    ui->hydration_start->setStyleSheet("background-color:rgb(0,100,0)");
    ui->hydration_stop->setStyleSheet("default");

    /*Update Hydration Start Time*/
     ui->hydration_start_time->setText(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));
     hydration_start_time_sec = QDateTime::currentDateTime().toSecsSinceEpoch();

    /*Update Hydration End Time*/
     hydration_end_time_sec = hydration_start_time_sec +  hydration_count_down_sec;
     ui->hydration_end_time->setText(QDateTime::fromTime_t(hydration_end_time_sec).toLocalTime().toString("yyyy.MM.dd hh:mm:ss ap"));

     /*Funcion Setting disable */
     Function_Disable(true);

     /* Create popup window*/
     show_popup(HYDRATION_PROGRESS,"Hydration processing");

     /* Main and popup Object connect */
     QObject::connect(this, SIGNAL(sig_popup_window_mgs(QString)), m_popupwindow, SLOT(receive_popup_msg(QString)));

}

void MainWindow::on_hydration_stop_clicked()
{
    m_flow_sensor->stop();
    m_hydration_count_down->stop();
    ui->hydration_start->setStyleSheet("default");

    /*Funcion Setting disable */
    Function_Disable(false);

    /*Get last hydration Time value and restore*/
    ui->hydration_countdown->setText(ui->set_hydration_time->time().toString());
    hydration_count_down_sec =QTime(0,0).secsTo(ui->set_hydration_time->time());
}

void MainWindow::on_adc_cal_clicked()
{

}

void MainWindow::on_set_hydration_time_userTimeChanged(const QTime &time)
{

    QObject *obj = this->sender();

    q_check_ptr(obj);
    qDebug() << "objClass ::" << obj->metaObject()->className();

    hydration_count_down_sec = QTime(0,0).secsTo(time);
    ui->hydration_countdown->setText(time.toString());

   /*Update Hydration Start Time*/
    ui->hydration_start_time->setText(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));
    hydration_start_time_sec = QDateTime::currentDateTime().toSecsSinceEpoch();

   /*Update Hydration End Time*/
    hydration_end_time_sec = hydration_start_time_sec +  hydration_count_down_sec;
    ui->hydration_end_time->setText(QDateTime::fromTime_t(hydration_end_time_sec).toLocalTime().toString("yyyy.MM.dd hh:mm:ss ap"));
}

QString MainWindow::Seconds_To_Time(quint64 sec)
{
    int seconds = sec % 60;
    int minutes = (sec / 60) % 60;
    int hours = (sec / 60 / 60);

    QString timeString = QString("%1:%2:%3")
      .arg(hours, 2, 10, QChar('0'))
      .arg(minutes, 2, 10, QChar('0'))
      .arg(seconds, 2, 10, QChar('0'));

    return timeString;
}

void MainWindow::Function_Disable(bool OnOff)
{
    ui->set_hydration_time->setDisabled(OnOff);
    ui->adc_cal->setDisabled(OnOff);
    ui->bias_on->setDisabled(OnOff);
    ui->bias_off->setDisabled(OnOff);
    ui->pump_start->setDisabled(OnOff);
    ui->pump_stop->setDisabled(OnOff);
    ui->valve_open->setDisabled(OnOff);
    ui->valve_close->setDisabled(OnOff);
    ui->flow_cal->setDisabled(OnOff);
}

void MainWindow::show_popup(popup_event event,QString msg)
{
    m_popupwindow = new popupwindow(event,msg,this);
    m_popupwindow->move(352, 100);
    m_popupwindow->show();
}

void MainWindow::on_reboot_clicked()
{
      QProcess::startDetached("sudo reboot");
}

void MainWindow::on_power_off_clicked()
{
      QProcess::startDetached("sudo poweroff");
}

MainWindow::~MainWindow()
{
    delete ui;
}

