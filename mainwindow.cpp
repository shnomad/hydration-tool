#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "adc/ads1120.h"
#include "flow-sensor/slf3s1300f.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Init();

    Set_Peripheral();

    /*ADC operation test*/
#if 0
    m_adc = new ads1120;

    m_adc->begin(ADS1120_SPI_CS, ADS1120_SPI_DRDY);
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

    ui->tabWidget->setFont(font_type_2);

    ui->system_current_time->setFont(font_type_1);
    ui->hydration_start_label->setFont(font_type_2);
    ui->hydration_end_label->setFont(font_type_2);
    ui->hydration_start_time->setFont(font_type_2);
    ui->hydration_end_time->setFont(font_type_2);

    ui->hydration_status->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");
    ui->hydration_countdown->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");
    ui->hydration_start_time->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");
    ui->hydration_start_label->setText("Start Time");

    ui->hydration_end_time->setStyleSheet("background-color: rgb(255, 255, 255); border-style: solid; border-color: rgb(100,100,100); border-width: 2px; border-radius: 10px;");
    ui->hydration_end_label->setText("End Time");

    ui->hydration_start->setFont(font_type_2);
    ui->hydration_stop->setFont(font_type_2);


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
//  ui->flow_rate->setFont(font_type_1);
//  ui->flow_rate->setStyleSheet("background-color: rgb(251, 176, 206);");

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
    /*Create Timer for every peripheral*/
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
    connect(m_flowSensor,SIGNAL(sig_flow_sensor_read(char *)), this, SLOT(Display_FlowSensor(char *)));

}

void MainWindow::Display_CurrentTime()
{
    ui->system_current_time->setText("Current Time: " + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));
}

void MainWindow::Read_FlowSensor()
{
    m_flowSensor->operation(slf3s1300f::START_MEASURE_WATER);
}

void MainWindow::Display_FlowSensor(char *flow_data)
{
    float flow_rate, Temp;

    quint16  flow_rate_tmp = flow_data[0]<< 8 | flow_data[1];
    quint16  Temp_tmp = flow_data[3]<< 8 | flow_data[4];
    bool   air_in_line_flag = flow_data[7] & 0x01;
    bool   high_flow_flag = flow_data[7] & 0x02;
    bool   exp_smoothing_active = flow_data[7] & 0x10;

    if(flow_rate_tmp<32768)
    {
        flow_rate = (float)(flow_rate_tmp);
        flow_rate = flow_rate/500;
    }
    else
    {
        flow_rate_tmp = ~(flow_rate_tmp);
        flow_rate_tmp +=1;
        flow_rate = (float)(flow_rate_tmp);
        flow_rate = -(flow_rate/500);
    }

    if(Temp_tmp<32768)
    {
        Temp = (float)Temp_tmp;
        Temp = Temp/200;
    }
    else
    {
        Temp_tmp = ~(Temp_tmp);
        Temp_tmp +=1;
        Temp = (float)(Temp_tmp);
        Temp = -(Temp/200);
    }

    ui->flow->setText("Flow : " + QString::number(flow_rate));
    ui->temp->setText("Temp : " + QString::number(Temp));
    ui->high_flow->setText("High Flow Signal :" + QString::number(air_in_line_flag));
    ui->air_in_line->setText("Air-in-line flag :" + QString::number(high_flow_flag));
    ui->exp_smoothing->setText("Exponential smoothing active :" +QString::number(exp_smoothing_active));
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

MainWindow::~MainWindow()
{
    delete ui;
}
