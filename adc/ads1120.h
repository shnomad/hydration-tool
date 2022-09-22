#ifndef ADS1120_H
#define ADS1120_H

#include <QObject>
#include "bcm2835.h"

#define SPI_MASTER_DUMMY   0xFF
// Commands for the ADC
#define CMD_RESET 0x07
#define CMD_START_SYNC 0x08
#define CMD_PWRDWN 0x03
#define CMD_RDATA 0x1f
#define CMD_RREG 0x20
#define CMD_WREG 0x40

// Configuration registers
#define CONFIG_REG0_ADDRESS 0x00
#define CONFIG_REG1_ADDRESS 0x01
#define CONFIG_REG2_ADDRESS 0x02
#define CONFIG_REG3_ADDRESS 0x03

// Register masks for settings
// Register 0
#define REG_MASK_MUX 0xF0
#define REG_MASK_GAIN 0x0E
#define REG_MASK_PGA_BYPASS 0x01

// Register 1
#define REG_MASK_DATARATE 0xE0
#define REG_MASK_OP_MODE 0x18
#define REG_MASK_CONV_MODE 0x04
#define REG_MASK_TEMP_MODE 0x02
#define REG_MASK_BURNOUT_SOURCES 0x01

// Register 2
#define REG_MASK_VOLTAGE_REF 0xC0
#define REG_MASK_FIR_CONF 0x30
#define REG_MASK_PWR_SWITCH 0x08
#define REG_MASK_IDAC_CURRENT 0x07

// Register 3
#define REG_MASK_IDAC1_ROUTING 0xE0
#define REG_MASK_IDAC2_ROUTING 0x1C
#define REG_MASK_DRDY_MODE 0x02
#define REG_MASK_RESERVED 0x01

/*CS, DRDY pin assign*/
#define ADS1120_SPI_CS      RPI_BPLUS_GPIO_J8_38
#define ADS1120_SPI_DRDY    RPI_BPLUS_GPIO_J8_40

#define USE_BCM2835_LIBRARY true

class ads1120 : public QObject
{
    Q_OBJECT
public:
    explicit ads1120(QObject *parent = nullptr);
    void writeRegister(quint8 address, quint8 value);
    quint8 readRegister(quint8 address);
    void begin(quint8 cs_pin, quint8 drdy_pin);
    bool isDataReady(void);
    quint16 readADC(void);
    quint8 * readADC_Array(void);
    quint16 readADC_Single(void);
    double readADC_SingleTemp(void);
    quint8 * readADC_SingleArray(void);
    quint16 convertToValue(quint8 * data);
    double convertToTemp(quint8 * data);
    void sendCommand(quint8 command);
    void reset(void);
    void startSync(void);
    void powerDown(void);
    void rdata(void);
    void writeRegisterMasked(quint8 value, quint8 mask, quint8 address);
    void setMultiplexer(quint8 value);
    void setGain(quint8 gain);
    void setPGAbypass(bool value);
    void setDataRate(quint8 value);
    void setOpMode(quint8 value);
    void setConversionMode(quint8 value);
    void setTemperatureMode(quint8 value);
    void setBurnoutCurrentSources(bool value);
    void setVoltageRef(quint8 value);
    void setFIR(quint8 value);
    void setPowerSwitch(quint8 value);
    void setIDACcurrent(quint8 value);
    void setIDAC1routing(quint8 value);
    void setIDAC2routing(quint8 value);
    void setDRDYmode(quint8 value);

signals:

public slots:

private:
};

#endif // ADS1120_H
