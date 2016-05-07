#ifndef GY85_H_
#define GY85_H_

#include "core.h"

#define ITG3200_ADDR_AD0_HIGH  0x69   //AD0=1 0x69 I2C address when AD0 is connected to HIGH (VCC) - default for sparkfun breakout
#define ITG3200_ADDR_AD0_LOW   0x68   //AD0=0 0x68 I2C address when AD0 is connected to LOW (GND)
// "The LSB bit of the 7 bit address is determined by the logic level on pin 9. 
// This allows two ITG-3200 devices to be connected to the same I2C bus.
// One device should have pin9 (or bit0) LOW and the other should be HIGH." source: ITG3200 datasheet
// Note that pin9 (AD0 - I2C Slave Address LSB) may not be available on some breakout boards so check 
// the schematics of your breakout board for the correct address to use.

#define GYROSTART_UP_DELAY  70    // 50ms from gyro startup + 20ms register r/w startup

/* ---- Registers ---- */
#define WHO_AM_I           0x00  // RW   SETUP: I2C address   
#define SMPLRT_DIV         0x15  // RW   SETUP: Sample Rate Divider       
#define DLPF_FS            0x16  // RW   SETUP: Digital Low Pass Filter/ Full Scale range
#define INT_CFG            0x17  // RW   Interrupt: Configuration
#define INT_STATUS         0x1A  // R	Interrupt: Status
#define TEMP_OUT           0x1B  // R	SENSOR: Temperature 2bytes
#define GYRO_XOUT          0x1D  // R	SENSOR: Gyro X 2bytes  
#define GYRO_YOUT          0x1F  // R	SENSOR: Gyro Y 2bytes
#define GYRO_ZOUT          0x21  // R	SENSOR: Gyro Z 2bytes
#define PWR_MGM            0x3E  // RW	Power Management

/* ---- bit maps ---- */
#define DLPFFS_FS_SEL             0x18  // 00011000
#define DLPFFS_DLPF_CFG           0x07  // 00000111
#define INTCFG_ACTL               0x80  // 10000000
#define INTCFG_OPEN               0x40  // 01000000
#define INTCFG_LATCH_INT_EN       0x20  // 00100000
#define INTCFG_INT_ANYRD_2CLEAR   0x10  // 00010000
#define INTCFG_ITG_RDY_EN         0x04  // 00000100
#define INTCFG_RAW_RDY_EN         0x01  // 00000001
#define INTSTATUS_ITG_RDY         0x04  // 00000100
#define INTSTATUS_RAW_DATA_RDY    0x01  // 00000001
#define PWRMGM_HRESET             0x80  // 10000000
#define PWRMGM_SLEEP              0x40  // 01000000
#define PWRMGM_STBY_XG            0x20  // 00100000
#define PWRMGM_STBY_YG            0x10  // 00010000
#define PWRMGM_STBY_ZG            0x08  // 00001000
#define PWRMGM_CLK_SEL            0x07  // 00000111

/************************************/
/*    REGISTERS PARAMETERS    */
/************************************/
// Sample Rate Divider
#define NOSRDIVIDER         0 // default    FsampleHz=SampleRateHz/(divider+1)
// Gyro Full Scale Range
#define RANGE2000           3   // default
// Digital Low Pass Filter BandWidth and SampleRate
#define BW256_SR8           0   // default    256Khz BW and 8Khz SR
#define BW188_SR1           1
#define BW098_SR1           2
#define BW042_SR1           3
#define BW020_SR1           4
#define BW010_SR1           5
#define BW005_SR1           6
// Interrupt Active logic lvl
#define ACTIVE_ONHIGH       0 // default
#define ACTIVE_ONLOW        1
// Interrupt drive type
#define PUSH_PULL           0 // default
#define OPEN_DRAIN          1
// Interrupt Latch mode
#define PULSE_50US          0 // default
#define UNTIL_INT_CLEARED   1
// Interrupt Latch clear method
#define READ_STATUSREG      0 // default
#define READ_ANYREG         1
// Power management
#define NORMAL              0 // default
#define STANDBY             1
// Clock Source - user parameters
#define INTERNALOSC         0   // default
#define PLL_XGYRO_REF       1
#define PLL_YGYRO_REF       2
#define PLL_ZGYRO_REF       3
#define PLL_EXTERNAL32      4   // 32.768 kHz
#define PLL_EXTERNAL19      5   // 19.2 Mhz

typedef uint8_t byte;

class ITG3200 {

public:
  float scalefactor[3];    // Scale Factor for gain and polarity
  int offsets[3];

  ITG3200();
  
  // Gyro initialization
  void init(unsigned int address);
  void init(unsigned int address, byte _SRateDiv, byte _Range, byte _filterBW, byte _ClockSrc, bool _ITGReady, bool _INTRawDataReady);      
    
  // Who Am I
  byte getDevAddr();
  void setDevAddr(unsigned int _addr);
  // Sample Rate Divider
  byte getSampleRateDiv();          
  void setSampleRateDiv(byte _SampleRate);
  // Digital Low Pass Filter BandWidth and SampleRate 
  byte getFSRange();
  void setFSRange(byte _Range); // RANGE2000
  byte getFilterBW(); 
  void setFilterBW(byte _BW); // see register parameters above
  // Interrupt Configuration
  bool isINTActiveOnLow();
  void setINTLogiclvl(bool _State); //ACTIVE_ONHIGH, ACTIVE_ONLOW
  // Interrupt drive type
  bool isINTOpenDrain();
  void setINTDriveType(bool _State); //OPEN_DRAIN, PUSH_PULL
  // Interrupt Latch mode
  bool isLatchUntilCleared();
  void setLatchMode(bool _State); //UNTIL_INT_CLEARED, PULSE_50US
  // Interrupt Latch clear method
  bool isAnyRegClrMode();
  void setLatchClearMode(bool _State); //READ_ANYREG, READ_STATUSREG
  // INT pin triggers
  bool isITGReadyOn();          
  void setITGReady(bool _State);
  bool isRawDataReadyOn();
  void setRawDataReady(bool _State);      
  // Trigger Status
  bool isITGReady();
  bool isRawDataReady();
  // Gyro Sensors
  void readTemp(float *_Temp);  
  void readGyroRaw( int *_GyroX, int *_GyroY, int *_GyroZ); // uncalibrated raw values
  void readGyroRaw( int *_GyroXYZ); // uncalibrated raw values
  void setScaleFactor(float _Xcoeff, float _Ycoeff, float _Zcoeff, bool _Radians);  // negative ciefficient = Reversed
  void setOffsets(int _Xoffset, int _Yoffset, int _Zoffset);
  void zeroCalibrate(unsigned int totSamples, unsigned int sampleDelayMS);	// assuming gyroscope is stationary (updates XYZ offsets)
  void readGyroRawCal(int *_GyroX, int *_GyroY, int *_GyroZ); // raw value with offset
  void readGyroRawCal(int *_GyroXYZ); // raw value with offset
  void readGyro(float *_GyroX, float *_GyroY, float *_GyroZ); // deg/sec calibrated & ScaleFactor 
  void readGyro(float *_GyroXYZ); // deg/sec calibrated & ScaleFactor  
  // Power management
  void reset(); // after reset all registers have default values
  bool isLowPower();
  void setPowerMode(bool _State); // NORMAL, STANDBY
  bool isXgyroStandby();            
  bool isYgyroStandby();
  bool isZgyroStandby();
  void setXgyroStandby(bool _Status); // NORMAL, STANDBY
  void setYgyroStandby(bool _Status);
  void setZgyroStandby(bool _Status);
  byte getClockSource();
  void setClockSource(byte _CLKsource); // see register parameters above
  
  void writemem(uint8_t _addr, uint8_t _val);
  void readmem(uint8_t _addr, uint8_t _nbytes, uint8_t __buff[]);
  
private:

  uint8_t _dev_address;
  uint8_t _buff[6];      
};

/* ------- Register names ------- */
#define ADXL345_DEVID 0x00
#define ADXL345_RESERVED1 0x01
#define ADXL345_THRESH_TAP 0x1d
#define ADXL345_OFSX 0x1e
#define ADXL345_OFSY 0x1f
#define ADXL345_OFSZ 0x20
#define ADXL345_DUR 0x21
#define ADXL345_LATENT 0x22
#define ADXL345_WINDOW 0x23
#define ADXL345_THRESH_ACT 0x24
#define ADXL345_THRESH_INACT 0x25
#define ADXL345_TIME_INACT 0x26
#define ADXL345_ACT_INACT_CTL 0x27
#define ADXL345_THRESH_FF 0x28
#define ADXL345_TIME_FF 0x29
#define ADXL345_TAP_AXES 0x2a
#define ADXL345_ACT_TAP_STATUS 0x2b
#define ADXL345_BW_RATE 0x2c
#define ADXL345_POWER_CTL 0x2d
#define ADXL345_INT_ENABLE 0x2e
#define ADXL345_INT_MAP 0x2f
#define ADXL345_INT_SOURCE 0x30
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0 0x32
#define ADXL345_DATAX1 0x33
#define ADXL345_DATAY0 0x34
#define ADXL345_DATAY1 0x35
#define ADXL345_DATAZ0 0x36
#define ADXL345_DATAZ1 0x37
#define ADXL345_FIFO_CTL 0x38
#define ADXL345_FIFO_STATUS 0x39

#define ADXL345_BW_1600 0xF // 1111
#define ADXL345_BW_800  0xE // 1110
#define ADXL345_BW_400  0xD // 1101  
#define ADXL345_BW_200  0xC // 1100
#define ADXL345_BW_100  0xB // 1011  
#define ADXL345_BW_50   0xA // 1010 
#define ADXL345_BW_25   0x9 // 1001 
#define ADXL345_BW_12   0x8 // 1000 
#define ADXL345_BW_6    0x7 // 0111
#define ADXL345_BW_3    0x6 // 0110


/* 
 Interrupt PINs
 INT1: 0
 INT2: 1
 */
#define ADXL345_INT1_PIN 0x00
#define ADXL345_INT2_PIN 0x01

/* 
 Interrupt bit position
 */
#define ADXL345_INT_DATA_READY_BIT 0x07
#define ADXL345_INT_SINGLE_TAP_BIT 0x06
#define ADXL345_INT_DOUBLE_TAP_BIT 0x05
#define ADXL345_INT_ACTIVITY_BIT   0x04
#define ADXL345_INT_INACTIVITY_BIT 0x03
#define ADXL345_INT_FREE_FALL_BIT  0x02
#define ADXL345_INT_WATERMARK_BIT  0x01
#define ADXL345_INT_OVERRUNY_BIT   0x00

#define ADXL345_DATA_READY 0x07
#define ADXL345_SINGLE_TAP 0x06
#define ADXL345_DOUBLE_TAP 0x05
#define ADXL345_ACTIVITY   0x04
#define ADXL345_INACTIVITY 0x03
#define ADXL345_FREE_FALL  0x02
#define ADXL345_WATERMARK  0x01
#define ADXL345_OVERRUNY   0x00




#define ADXL345_OK    1 // no error
#define ADXL345_ERROR 0 // indicates error is predent

#define ADXL345_NO_ERROR   0 // initial state
#define ADXL345_READ_ERROR 1 // problem reading accel
#define ADXL345_BAD_ARG    2 // bad method argument


class ADXL345
{
public:
	bool status;           // set when error occurs 
	// see error code for details
	byte error_code;       // Initial state
	double gains[3];        // counts to Gs
	
	ADXL345();
	void powerOn();
	void readAccel(int* xyx);
	void readAccel(int* x, int* y, int* z);
	void get_Gxyz(double *xyz);
	
	void setTapThreshold(int tapThreshold);
	int getTapThreshold();
	void setAxisGains(double *_gains);
	void getAxisGains(double *_gains);
	void setAxisOffset(int x, int y, int z);
	void getAxisOffset(int* x, int* y, int*z);
	void setTapDuration(int tapDuration);
	int getTapDuration();
	void setDoubleTapLatency(int doubleTapLatency);
	int getDoubleTapLatency();
	void setDoubleTapWindow(int doubleTapWindow);
	int getDoubleTapWindow();
	void setActivityThreshold(int activityThreshold);
	int getActivityThreshold();
	void setInactivityThreshold(int inactivityThreshold);
	int getInactivityThreshold();
	void setTimeInactivity(int timeInactivity);
	int getTimeInactivity();
	void setFreeFallThreshold(int freeFallthreshold);
	int getFreeFallThreshold();
	void setFreeFallDuration(int freeFallDuration);
	int getFreeFallDuration();
	
	bool isActivityXEnabled();
	bool isActivityYEnabled();
	bool isActivityZEnabled();
	bool isInactivityXEnabled();
	bool isInactivityYEnabled();
	bool isInactivityZEnabled();
	bool isActivityAc();
	bool isInactivityAc();
	void setActivityAc(bool state);
	void setInactivityAc(bool state);
	
	bool getSuppressBit();
	void setSuppressBit(bool state);
	bool isTapDetectionOnX();
	void setTapDetectionOnX(bool state);
	bool isTapDetectionOnY();
	void setTapDetectionOnY(bool state);
	bool isTapDetectionOnZ();
	void setTapDetectionOnZ(bool state);
	
	void setActivityX(bool state);
	void setActivityY(bool state);
	void setActivityZ(bool state);
	void setInactivityX(bool state);
	void setInactivityY(bool state);
	void setInactivityZ(bool state);
	
	bool isActivitySourceOnX();
	bool isActivitySourceOnY();
	bool isActivitySourceOnZ();
	bool isTapSourceOnX();
	bool isTapSourceOnY();
	bool isTapSourceOnZ();
	bool isAsleep();
	
	bool isLowPower();
	void setLowPower(bool state);
	double getRate();
	void setRate(double rate);
	void set_bw(byte bw_code);
	byte get_bw_code();  
	
	
	bool triggered(byte interrupts, int mask);
	
	
	byte getInterruptSource();
	bool getInterruptSource(byte interruptBit);
	bool getInterruptMapping(byte interruptBit);
	void setInterruptMapping(byte interruptBit, bool interruptPin);
	bool isInterruptEnabled(byte interruptBit);
	void setInterrupt(byte interruptBit, bool state);
	
	void getRangeSetting(byte* rangeSetting);
	void setRangeSetting(int val);
	bool getSelfTestBit();
	void setSelfTestBit(bool selfTestBit);
	bool getSpiBit();
	void setSpiBit(bool spiBit);
	bool getInterruptLevelBit();
	void setInterruptLevelBit(bool interruptLevelBit);
	bool getFullResBit();
	void setFullResBit(bool fullResBit);
	bool getJustifyBit();
	void setJustifyBit(bool justifyBit);
	void printAllRegister();
	
private:
	void writeTo(byte address, byte val);
	void readFrom(byte address, int num, byte buff[]);
	void setRegisterBit(byte regAdress, int bitPos, bool state);
	bool getRegisterBit(byte regAdress, int bitPos);  
	byte _buff[6] ;    //6 bytes buffer for saving data read from the device
};
void print_byte(byte val);

#endif /* GY85_H_ */
