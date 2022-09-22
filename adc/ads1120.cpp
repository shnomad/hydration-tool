#include <QDebug>
#include "ads1120.h"

ads1120::ads1120(QObject *parent) : QObject(parent)
{
    if(bcm2835_init())
       qDebug()<<"bcm2835 init success";
    else
       exit(0);

   if(bcm2835_spi_begin())
      qDebug()<<"bcm2835 SPI init success";
   else
      exit(0);
}

void ads1120::writeRegister(uint8_t address, uint8_t value)
{
#ifndef USE_BCM2835_LIBRARY
  digitalWrite(ads1120_CS_PIN, LOW);
  delay(5);
  SPI.transfer(CMD_WREG|(address<<2)); // What not setting num bytes?
  SPI.transfer(value);
  delay(5);
  //startSync(); // Send start/sync for continuous conversion mode
  //delayMicroseconds(1); // Delay a minimum of td(SCCS)
  digitalWrite(ads1120_CS_PIN, HIGHwriteRegister);
#else

//  quint32 reg_data = CMD_WREG|(address<<2);
//  bcm2835_spi_write((quint16)(CMD_WREG|(address<<2)));
//  char cmd_data[2] = {0x0,};

//  cmd_data[0] = (CMD_WREG|(address<<2));
//  cmd_data[1] = value;

    bcm2835_gpio_write(ADS1120_SPI_CS, HIGH);

    bcm2835_gpio_write(ADS1120_SPI_CS, LOW);

    bcm2835_delay(5);

    bcm2835_spi_transfer(CMD_WREG|(address<<2));
    bcm2835_spi_transfer(value);

    bcm2835_delay(5);

    bcm2835_gpio_write(ADS1120_SPI_CS, HIGH);

//  bcm2835_spi_transfer(quint8(CMD_WREG|(address<<2)));
//  bcm2835_spi_transfer(value);

#endif
}

quint8 ads1120::readRegister(uint8_t address)
{
#ifndef USE_BCM2835_LIBRARY
  digitalWrite(ads1120_CS_PIN,LOW);
  delay(5);
  SPI.transfer(CMD_RREG|(address<<2)); // What not setting num bytes?
  uint8_t data = SPI.transfer(SPI_MASTER_DUMMY);
  delay(5);
  digitalWrite(ads1120_CS_PIN,HIGH);

  return data;
#else

//  char *cmd_addr, *data;
//  cmd_addr[0] = address;
//  cmd_addr[1] = CMD_RREG;
//  data[0] = SPI_MASTER_DUMMY;
//  bcm2835_spi_transfern(cmd_addr, 0x2);
//  quint32 reg_data = CMD_RREG|(address<<2);
//  bcm2835_spi_transfer(quint8(CMD_RREG|(address<<2)));
//  quint8 read_reg = bcm2835_spi_transfer(SPI_MASTER_DUMMY);
//  char cmd_data[2] = {0x0,};
//  cmd_data[0] = (CMD_RREG|(address<<2));
//  bcm2835_spi_writenb(cmd_data,0x1);

    bcm2835_gpio_write(ADS1120_SPI_CS, HIGH);

    bcm2835_gpio_write(ADS1120_SPI_CS, LOW);

    bcm2835_spi_transfer(CMD_RREG|(address<<2));

    quint8 read_reg = bcm2835_spi_transfer(SPI_MASTER_DUMMY);

    bcm2835_gpio_write(ADS1120_SPI_CS, HIGH);

    return read_reg;

#endif

}

void ads1120::begin(uint8_t cs_pin, uint8_t drdy_pin)
{

#ifndef USE_BCM2835_LIBRARY
  // Set pins up
  ads1120_CS_PIN = cs_pin;
  ads1120_DRDY_PIN = drdy_pin;

  // Configure the SPI interface (CPOL=0, CPHA=1)
  SPI.begin();
  SPI.setDataMode(SPI_MODE1);

  // Configure chip select as an output
  pinMode(ads1120_CS_PIN, OUTPUT);

  // Configure DRDY as as input (mfg wants us to use interrupts)
  pinMode(ads1120_DRDY_PIN, INPUT);

  digitalWrite(ads1120_CS_PIN, LOW); // Set CS Low
  delayMicroseconds(1); // Wait a minimum of td(CSSC)
  reset(); // Send reset command
  delayMicroseconds(1);; BCM2835_SPI_CS0// Delay a minimum of 50 us + 32 * tclk

  // Sanity check read back (optional)

  startSync(); // Send start/sync for continuous conversion mode
  delayMicroseconds(1); // Delay a minimum of td(SCCS)
  digitalWrite(ads1120_CS_PIN, HIGH); // Clear CS to high
#else
    /*DRDY pin select, pull up*/
    bcm2835_gpio_fsel(drdy_pin, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(drdy_pin, BCM2835_GPIO_PUD_UP);

    /*CS Pin select*/
    bcm2835_gpio_fsel(cs_pin, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(cs_pin, HIGH);

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536); // The default
//  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);                  // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS_NONE, LOW);  // the default
//  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default

    reset();
    bcm2835_delay(10);

#endif
}

bool ads1120::isDataReady()
{
#ifndef USE_BCM2835_LIBRARY
    if (digitalRead(ads1120_DRDY_PIN) == HIGH)
    {
      return false;
    }
#else

    if(bcm2835_gpio_lev(ADS1120_SPI_DRDY))
    {
        return false;
    }

#endif
  return true;
}

quint16 ads1120::readADC()
{
  quint8 *data = readADC_SingleArray();
  return convertToValue(data);
}

quint8 * ads1120::readADC_Array()
{

#ifndef USE_BCM2835_LIBRARY
    digitalWrite(ads1120_CS_PIN, LOW); // Take CS low
  delayMicroseconds(1); // Minimum of td(CSSC)

  static quint8 dataarray[2];
  for (int x = 0; x < 2 ; x++)
  {
    dataarray[x] = SPI.transfer(SPI_MASTER_DUMMY);
  }
  delayMicroseconds(1); // Minimum of td(CSSC)
  digitalWrite(ads1120_CS_PIN, HIGH);

  return dataarray;
#else



    static quint8 dataarray[2];
    for (int x = 0; x < 2 ; x++)
    {
        dataarray[x] = bcm2835_spi_transfer(SPI_MASTER_DUMMY);
    }

    return dataarray;

#endif

}

//Single Conversion read modes
quint16 ads1120::readADC_Single()
{
  quint8 * data = readADC_SingleArray();
  return convertToValue(data);
}

double ads1120::readADC_SingleTemp()
{
  quint8 * data = readADC_SingleArray();
  return convertToTemp(data);
}

quint8 * ads1120::readADC_SingleArray()
{

#ifndef USE_BCM2835_LIBRARY
  digitalWrite(ads1120_CS_PIN, LOW); // Take CS low
  delayMicroseconds(1); // Minimum of td(CSSC)
  SPI.transfer(0x08);

  while(digitalRead(ads1120_DRDY_PIN) == HIGH)
  {
    //Wait to DRDY goes down
    //Not a good thing
    //Code could be stuck here
    //Need a improve later
  }

  static quint8 dataarray[2];
  for (int x = 0; x < 2 ; x++)
  {
    dataarray[x] = SPI.transfer(SPI_MASTER_DUMMY);
  }
  delayMicroseconds(1); // Minimum of td(CSSC)
  digitalWrite(ads1120_CS_PIN, HIGH);

  return dataarray;
#else

//   bcm2835_spi_transfer(CMD_START_SYNC);
//   char cmd[1];
//   cmd[0] = CMD_START_SYNC;
//   bcm2835_spi_writenb(cmd,0x1);

    bcm2835_gpio_write(ADS1120_SPI_CS, HIGH);

    bcm2835_gpio_write(ADS1120_SPI_CS, LOW);

    bcm2835_delay(1);

    bcm2835_spi_transfer(CMD_START_SYNC);

//  while(bcm2835_gpio_lev(ads1120_SPI_DRDY))
//  {
//  }

    bcm2835_delay(100);

    bcm2835_spi_transfer(CMD_RDATA);

    static quint8 dataarray[2];

    for (int x = 0; x < 2 ; x++)
    {
      dataarray[x] = bcm2835_spi_transfer(SPI_MASTER_DUMMY);
    }

    bcm2835_delay(1);

    bcm2835_gpio_write(ADS1120_SPI_CS, HIGH);

    return dataarray;

#endif
  bcm2835_delay(10);
}

quint16 ads1120::convertToValue(quint8 * data)
{
  return (data[0])<<8 | (data[1]); //Moving MSB and LSB to 16 bit
}

double ads1120::convertToTemp(quint8 * data)
{
  // 14-bit result that is left-justified within the conversion result
  uint16_t conversion = ((data[0])<<8 | (data[1])) >> 2;

  // Negative numbers are represented in binary twos complement format
  if(conversion >= 8192) {
    conversion = (~(conversion-1)) ^ 0xC000;
    return conversion * -0.03125;
  }

  return conversion * 0.03125;
}

void ads1120::sendCommand(quint8 command)
{

#ifndef USE_BCM2835_LIBRARY
  // Following Protocentral's code, not sure exactly what's going on here.
  digitalWrite(ads1120_CS_PIN, LOW);
  delay(2);
  digitalWrite(ads1120_CS_PIN, HIGH);
  delay(2);
  digitalWrite(ads1120_CS_PIN, LOW);
  delay(2);
  SPI.transfer(command);
  delay(2);
  digitalWrite(ads1120_CS_PIN, HIGH);
#else

    bcm2835_gpio_write(ADS1120_SPI_CS, HIGH);

    bcm2835_gpio_write(ADS1120_SPI_CS, LOW);

    bcm2835_delay(5);

    bcm2835_spi_transfer(command);

    bcm2835_delay(5);

    bcm2835_gpio_write(ADS1120_SPI_CS, HIGH);

#endif
}

void ads1120::writeRegisterMasked(uint8_t value, uint8_t mask, uint8_t address)
{
  // Write the value to a register using the mask to leave the rest of the
  // register untouched. This does not shift the value, so it shoudl be provided
  // shifted to the appropriate positions.

  // Read what's in the register now
  uint8_t register_contents = readRegister(address);

  // Flip the mask so that it's zeros where we'll put data and zero out that
  // part of the register's contents
  register_contents = register_contents & ~mask;

  // OR in the value to be written
  register_contents = register_contents | value;

  // Write it back out
  writeRegister(address, register_contents);
}

void ads1120::setMultiplexer(uint8_t value)
{
  /* Set multiplexer
  | Value | AINp | AINn |
  | ----- | ---- | ---- |
  | 0x00  | AIN0 | AIN1 |
  | 0X01  | AIN0 | AIN2 |
  | 0X02  | AIN0 | AIN3 |
  | 0X03  | AIN1 | AIN2 |
  | 0X04  | AIN1 | AIN3 |
  | 0X05  | AIN2 | AIN3 |
  | 0X06  | AIN1 | AIN0 |
  | 0X07  | AIN3 | AIN2 |
  | 0X08  | AIN0 | AVSS |
  | 0X09  | AIN1 | AVSS |
  | 0X0A  | AIN2 | AVSS |
  | 0X0B  | AIN3 | AVSS |
  | 0X0C  |  REF/4 MON  |
  | 0X0D  | APWR/4 MON  |
  | 0X0E  |   SHORTED   |
  */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x0E)
  {
    value = 0x00;
  }

  value = value << 4; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_MUX, CONFIG_REG0_ADDRESS);
}

void ads1120::setGain(uint8_t gain)
{
  /* Sets ADC gain. Possible values are 1, 2, 4, 8, 16, 32, 64, 128. */
  uint8_t value = 0x00;
  switch(gain) {
    case 1:
      value = 0x00;
      break;
    case 2:
      value = 0x01;
      break;
    case 4:
      value = 0x02;
      break;
    case 8:
      value = 0x03;
      break;
    case 16:
      value = 0x04;
      break;
    case 32:
      value = 0x05;
      break;
    case 64:
      value = 0x06;
      break;
    case 128:
      value = 0x07;
      break;
    default:
      value = 0x00;
      break;
  }
  value = value << 1; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_GAIN, CONFIG_REG0_ADDRESS);
}

void ads1120::setPGAbypass(bool value)
{
  /* Bypasses the PGA if true.
     PGA can only be disabled for gains 1, 2, 4.
  */
  writeRegisterMasked(value, REG_MASK_PGA_BYPASS, CONFIG_REG0_ADDRESS);
}

void ads1120::setDataRate(quint8 value)
{
  /* Sets the data rate for the ADC. See table 18 in datasheet for datarates
     in various operating modes. */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x07)
  {
    value = 0x00;
  }

  value = value << 5; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_DATARATE, CONFIG_REG1_ADDRESS);
}

void ads1120::setOpMode(quint8 value)
{
  /* Sets the ADC operating mode:
     0 - Normal mode
     1 - Duty-cycle mode
     2 - Turbo mode
  */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x02)
  {
    value = 0x00;
  }
  value = value << 3; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_OP_MODE, CONFIG_REG1_ADDRESS);
}

void ads1120::setConversionMode(quint8 value)
{
  /* Sets the ADC conversion mode.
     0 - Single shot mode
     1 - continuous conversion mode
  */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x01)
  {
    value = 0x00;
  }
  value = value << 2; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_CONV_MODE, CONFIG_REG1_ADDRESS);
}

void ads1120::setTemperatureMode(quint8 value)
{
  /* Controls the state of the internal temperature sensor.
     0 - Disables temperature sensor
     1 - Enables temperature sensor
  */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x01) {
    value = 0x00;
  }
  value = value << 1; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_TEMP_MODE, CONFIG_REG1_ADDRESS);
}

void ads1120::setBurnoutCurrentSources(bool value)
{
  /* Turns the 10uA burn-out current sources on or off. */
  writeRegisterMasked(value, REG_MASK_BURNOUT_SOURCES, CONFIG_REG1_ADDRESS);
}

void ads1120::setVoltageRef(quint8 value)
{
  /* Sets the voltage reference used by the ADC.
     0 - Internal 2.048 V
     1 - External on REFP0 and REFN0 inputs
     2 - External on AIN0/REFP1 and AIN3/REFN1 inputs
     3 - Use analog supply as reference
  */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x03)
  {
    value = 0x00;
  }
  value = value << 6; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_VOLTAGE_REF, CONFIG_REG2_ADDRESS);
}

void ads1120::setFIR(quint8 value)
{
  /* Controls the FIR filter on the ADC.
     0 - No 50 or 60 Hz rejection
     1 - Both 50 and 60 Hz rejection
     2 - 50 Hz rejection
     3 - 60 Hz rejection
  */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x03)
  {
    value = 0x00;
  }

  value = value << 4; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_FIR_CONF, CONFIG_REG2_ADDRESS);
}

void ads1120::setPowerSwitch(uint8_t value)
{
  /* Configures behavior of low-side switch between AIN3/REFN1 and AVSS.
     0 - Always open
     1 - Automatically closes when START/SYNC command is sent and opens when
         POWERDOWN command is issues.
  */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x01) {
    value = 0x00;
  }
  value = value << 3; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_PWR_SWITCH, CONFIG_REG2_ADDRESS);
}

void ads1120::setIDACcurrent(quint8 value)
{
  /* Set current for both IDAC1 and IDAC2 excitation sources.
     0 - Off
     1 - 10 uA
     2 - 50 uA
     3 - 100 uA
     4 - 250 uA
     5 - 500 uA
     6 - 1000 uA
     7 - 1500 uA
  */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x07) {
    value = 0x00;
  }
  writeRegisterMasked(value, REG_MASK_IDAC_CURRENT, CONFIG_REG2_ADDRESS);
}

void ads1120::setIDAC1routing(quint8 value)
{
  /* Selects where IDAC1 is routed to.
     0 - Disabled
     1 - AIN0/REFP1
     2 - AIN1
     3 - AIN2
     4 - AIN3/REFN1
     5 - REFP0
     6 - REFN0
  */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x06) {
    value = 0x00;
  }
  value = value << 5; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_IDAC1_ROUTING, CONFIG_REG3_ADDRESS);
}

void ads1120::setIDAC2routing(quint8 value)
{
  /* Selects where IDAC2 is routed to.
     0 - Disabled
     1 - AIN0/REFP1
     2 - AIN1
     3 - AIN2
     4 - AIN3/REFN1
     5 - REFP0
     6 - REFN0
  */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x06) {
    value = 0x00;
  }
  value = value << 2; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_IDAC2_ROUTING, CONFIG_REG3_ADDRESS);
}

void ads1120::setDRDYmode(quint8 value)
{
  /* Controls the behavior of the DOUT/DRDY pin when new data are ready.
     0 - Only the dedicated DRDY pin is used
     1 - Data ready indicated on DOUT/DRDY and DRDY
 */
  // Make sure the value is in the valid range. Otherwise set to 0x00
  if (value > 0x01)
  {
    value = 0x00;
  }
  value = value << 1; // Shift to match with mask
  writeRegisterMasked(value, REG_MASK_DRDY_MODE, CONFIG_REG3_ADDRESS);
}

void ads1120::reset()
{
  sendCommand(CMD_RESET);
}

void ads1120::startSync()
{
  sendCommand(CMD_START_SYNC);
}

void ads1120::powerDown()
{
  sendCommand(CMD_PWRDWN);
}

void ads1120::rdata()
{
  sendCommand(CMD_RDATA);
}
