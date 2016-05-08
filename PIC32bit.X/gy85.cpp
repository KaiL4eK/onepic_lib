#include "gy85.h"

#define ADXL345_DEVICE (0x53)    // ADXL345 device address
#define ADXL345_TO_READ (6)      // num of bytes we are going to read each time (two bytes for each axis)

ADXL345::ADXL345() {
	status = ADXL345_OK;
	error_code = ADXL345_NO_ERROR;
	
	gains[0] = 0.00376390;
	gains[1] = 0.00376009;
	gains[2] = 0.00349265;
}

void ADXL345::powerOn() {
	//Turning on the ADXL345
	writeTo(ADXL345_POWER_CTL, 0);      
	writeTo(ADXL345_POWER_CTL, 16);
	writeTo(ADXL345_POWER_CTL, 8);
}

// Reads the acceleration into three variable x, y and z
void ADXL345::readAccel(int16_t *xyz){
	readAccel(xyz, xyz + 1, xyz + 2);
}

void ADXL345::readAccel(int16_t *x, int16_t *y, int16_t *z) {
	readFrom(ADXL345_DATAX0, ADXL345_TO_READ, _buff); //read the acceleration data from the ADXL345
	
	// each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
	// thus we are converting both bytes in to one int
	*x = (((int)_buff[1]) << 8) | _buff[0];   
	*y = (((int)_buff[3]) << 8) | _buff[2];
	*z = (((int)_buff[5]) << 8) | _buff[4];
}

void ADXL345::get_Gxyz(double *xyz){
	int i;
	int16_t xyz_int[3];
	readAccel(xyz_int);
	for(i=0; i<3; i++){
		xyz[i] = xyz_int[i] * gains[i];
	}
}
// Writes val to address register on device
void ADXL345::writeTo(uint8_t address, uint8_t val) {
    i2c_write_byte_eeprom( ADXL345_DEVICE, address, val );
}

// Reads num bytes starting from address register on device in to _buff array
void ADXL345::readFrom(uint8_t address, int num, uint8_t _buff[]) {
    i2c_read_bytes_eeprom( ADXL345_DEVICE, address, _buff, num );
}

// Gets the range setting and return it into rangeSetting
// it can be 2, 4, 8 or 16
void ADXL345::getRangeSetting ( uint8_t* rangeSetting ) 
{
	*rangeSetting = i2c_read_byte_eeprom( ADXL345_DEVICE, ADXL345_DATA_FORMAT ) & 0b11;
}

// Sets the range setting, possible values are: 2, 4, 8, 16
void ADXL345::setRangeSetting ( uint8_t val ) 
{
	uint8_t range;
	switch (val) {
		case 2:  
			range = 0b00; 
			break;
		case 4:  
			range = 0b01; 
			break;
		case 8:  
			range = 0b10; 
			break;
		case 16: 
			range = 0b11; 
			break;
		default: 
			range = 0b00;
	}
    i2c_write_bits_eeprom( ADXL345_DEVICE, ADXL345_DATA_FORMAT, 1, 2, range );
}

ITG3200::ITG3200() {
  setOffsets(0,0,0);
  setScaleFactor(1.0, 1.0, 1.0, false);  // true to change readGyro output to radians
  //Wire.begin();       //Normally this code is called from setup() at user code
                        //but some people reported that joining I2C bus earlier
                        //apparently solved problems with master/slave conditions.
                        //Uncomment if needed.
}

void ITG3200::init(unsigned int  address) {
  // Uncomment or change your default ITG3200 initialization
  
  // fast sample rate - divisor = 0 filter = 0 clocksrc = 0, 1, 2, or 3  (raw values)
  init(address, NOSRDIVIDER, RANGE2000, BW256_SR8, PLL_XGYRO_REF, true, true);
  
  // slow sample rate - divisor = 0  filter = 1,2,3,4,5, or 6  clocksrc = 0, 1, 2, or 3  (raw values)
  //init(NOSRDIVIDER, RANGE2000, BW010_SR1, INTERNALOSC, true, true);
  
  // fast sample rate 32Khz external clock - divisor = 0  filter = 0  clocksrc = 4  (raw values)
  //init(NOSRDIVIDER, RANGE2000, BW256_SR8, PLL_EXTERNAL32, true, true);
  
  // slow sample rate 32Khz external clock - divisor = 0  filter = 1,2,3,4,5, or 6  clocksrc = 4  (raw values)
  //init(NOSRDIVIDER, RANGE2000, BW010_SR1, PLL_EXTERNAL32, true, true);
}

void ITG3200::init(unsigned int address, uint8_t _SRateDiv, uint8_t _Range, uint8_t _filterBW, uint8_t _ClockSrc, bool _ITGReady, bool _INTRawDataReady) {
  _dev_address = address;
  setSampleRateDiv(_SRateDiv);
  setFSRange(_Range);
  setFilterBW(_filterBW);
  setClockSource(_ClockSrc);
  setITGReady(_ITGReady);
  setRawDataReady(_INTRawDataReady);  
  delay_ms(GYROSTART_UP_DELAY);  // startup 
}

uint8_t ITG3200::getDevAddr() {
  /*readmem(WHO_AM_I, 1, &_buff[0]); 
  return _buff[0];  */
  return _dev_address;
}

void ITG3200::setDevAddr(unsigned int  _addr) {
  writemem(WHO_AM_I, _addr); 
  _dev_address = _addr;
}

uint8_t ITG3200::getSampleRateDiv() {
  readmem(SMPLRT_DIV, 1, &_buff[0]);
  return _buff[0];
}

void ITG3200::setSampleRateDiv(uint8_t _SampleRate) {
  writemem(SMPLRT_DIV, _SampleRate);
}

uint8_t ITG3200::getFSRange() {
  readmem(DLPF_FS, 1, &_buff[0]);
  return ((_buff[0] & DLPFFS_FS_SEL) >> 3);
}

void ITG3200::setFSRange(uint8_t _Range) {
  readmem(DLPF_FS, 1, &_buff[0]);   
  writemem(DLPF_FS, ((_buff[0] & ~DLPFFS_FS_SEL) | (_Range << 3)) ); 
}

uint8_t ITG3200::getFilterBW() {  
  readmem(DLPF_FS, 1, &_buff[0]);
  return (_buff[0] & DLPFFS_DLPF_CFG); 
}

void ITG3200::setFilterBW(uint8_t _BW) {   
  readmem(DLPF_FS, 1, &_buff[0]);
  writemem(DLPF_FS, ((_buff[0] & ~DLPFFS_DLPF_CFG) | _BW)); 
}

bool ITG3200::isINTActiveOnLow() {  
  readmem(INT_CFG, 1, &_buff[0]);
  return ((_buff[0] & INTCFG_ACTL) >> 7);
}

void ITG3200::setINTLogiclvl(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_ACTL) | (_State << 7))); 
}

bool ITG3200::isINTOpenDrain() {  
  readmem(INT_CFG, 1, &_buff[0]);
  return ((_buff[0] & INTCFG_OPEN) >> 6);
}

void ITG3200::setINTDriveType(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_OPEN) | _State << 6)); 
}

bool ITG3200::isLatchUntilCleared() {    
  readmem(INT_CFG, 1, &_buff[0]);
  return ((_buff[0] & INTCFG_LATCH_INT_EN) >> 5);
}

void ITG3200::setLatchMode(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_LATCH_INT_EN) | _State << 5)); 
}

bool ITG3200::isAnyRegClrMode() {    
  readmem(INT_CFG, 1, &_buff[0]);
  return ((_buff[0] & INTCFG_INT_ANYRD_2CLEAR) >> 4);
}

void ITG3200::setLatchClearMode(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_INT_ANYRD_2CLEAR) | _State << 4)); 
}

bool ITG3200::isITGReadyOn() {   
  readmem(INT_CFG, 1, &_buff[0]);
  return ((_buff[0] & INTCFG_ITG_RDY_EN) >> 2);
}

void ITG3200::setITGReady(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_ITG_RDY_EN) | _State << 2)); 
}

bool ITG3200::isRawDataReadyOn() {
  readmem(INT_CFG, 1, &_buff[0]);
  return (_buff[0] & INTCFG_RAW_RDY_EN);
}

void ITG3200::setRawDataReady(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_RAW_RDY_EN) | _State)); 
}

bool ITG3200::isITGReady() {
  readmem(INT_STATUS, 1, &_buff[0]);
  return ((_buff[0] & INTSTATUS_ITG_RDY) >> 2);
}

bool ITG3200::isRawDataReady() {
  readmem(INT_STATUS, 1, &_buff[0]);
  return (_buff[0] & INTSTATUS_RAW_DATA_RDY);
}

void ITG3200::readTemp(float *_Temp) {
  readmem(TEMP_OUT,2,_buff);
  *_Temp = 35 + ((_buff[0] << 8 | _buff[1]) + 13200) / 280.0;    // F=C*9/5+32
}

void ITG3200::readGyroRaw( int16_t *_GyroX, int16_t *_GyroY, int16_t *_GyroZ){
  readmem(GYRO_XOUT, 6, _buff);
  *_GyroX = _buff[0] << 8 | _buff[1];
  *_GyroY = _buff[2] << 8 | _buff[3]; 
  *_GyroZ = _buff[4] << 8 | _buff[5];
}

void ITG3200::readGyroRaw( int16_t *_GyroXYZ){
  readGyroRaw(_GyroXYZ, _GyroXYZ+1, _GyroXYZ+2);
}

void ITG3200::setScaleFactor(float _Xcoeff, float _Ycoeff, float _Zcoeff, bool _Radians) { 
  scalefactor[0] = 14.375 * _Xcoeff;   
  scalefactor[1] = 14.375 * _Ycoeff;
  scalefactor[2] = 14.375 * _Zcoeff;    
    
  if (_Radians){
    scalefactor[0] /= 0.0174532925;//0.0174532925 = PI/180
    scalefactor[1] /= 0.0174532925;
    scalefactor[2] /= 0.0174532925;
  }
}

void ITG3200::setOffsets(int _Xoffset, int _Yoffset, int _Zoffset) {
  offsets[0] = _Xoffset;
  offsets[1] = _Yoffset;
  offsets[2] = _Zoffset;
}

void ITG3200::zeroCalibrate(unsigned int totSamples, unsigned int sampleDelayMS) {
  float tmpOffsets[] = {0,0,0};
  int16_t xyz[3];

  for (int i = 0;i < totSamples;i++){
    delay_ms(sampleDelayMS);
    readGyroRaw(xyz);
    tmpOffsets[0] += xyz[0];
    tmpOffsets[1] += xyz[1];
    tmpOffsets[2] += xyz[2];
  }
  setOffsets(-tmpOffsets[0] / totSamples + 0.5, -tmpOffsets[1] / totSamples + 0.5, -tmpOffsets[2] / totSamples + 0.5);
}

void ITG3200::readGyroRawCal(int16_t *_GyroX, int16_t *_GyroY, int16_t *_GyroZ) { 
  readGyroRaw(_GyroX, _GyroY, _GyroZ); 
  *_GyroX += offsets[0]; 
  *_GyroY += offsets[1]; 
  *_GyroZ += offsets[2]; 
} 

void ITG3200::readGyroRawCal(int16_t *_GyroXYZ) { 
  readGyroRawCal(_GyroXYZ, _GyroXYZ+1, _GyroXYZ+2); 
} 

void ITG3200::readGyro(float *_GyroX, float *_GyroY, float *_GyroZ){
  int16_t x, y, z; 
  readGyroRawCal(&x, &y, &z); // x,y,z will contain calibrated integer values from the sensor 
  *_GyroX =  x / scalefactor[0]; 
  *_GyroY =  y / scalefactor[1]; 
  *_GyroZ =  z / scalefactor[2];     
} 

void ITG3200::readGyro(float *_GyroXYZ){
  readGyro(_GyroXYZ, _GyroXYZ+1, _GyroXYZ+2);
}

void ITG3200::reset() {     
  writemem(PWR_MGM, PWRMGM_HRESET); 
  delay_ms(GYROSTART_UP_DELAY); //gyro startup 
}

bool ITG3200::isLowPower() {   
  readmem(PWR_MGM, 1, &_buff[0]);
  return (_buff[0] & PWRMGM_SLEEP) >> 6;
}
  
void ITG3200::setPowerMode(bool _State) {
  readmem(PWR_MGM, 1, &_buff[0]);
  writemem(PWR_MGM, ((_buff[0] & ~PWRMGM_SLEEP) | _State << 6));  
}

bool ITG3200::isXgyroStandby() {
  readmem(PWR_MGM, 1, &_buff[0]);
  return (_buff[0] & PWRMGM_STBY_XG) >> 5;
}

bool ITG3200::isYgyroStandby() {
  readmem(PWR_MGM, 1, &_buff[0]);
  return (_buff[0] & PWRMGM_STBY_YG) >> 4;
}

bool ITG3200::isZgyroStandby() {
  readmem(PWR_MGM, 1, &_buff[0]);
  return (_buff[0] & PWRMGM_STBY_ZG) >> 3;
}

void ITG3200::setXgyroStandby(bool _Status) {
  readmem(PWR_MGM, 1, &_buff[0]);
  writemem(PWR_MGM, ((_buff[0] & PWRMGM_STBY_XG) | _Status << 5));
}

void ITG3200::setYgyroStandby(bool _Status) {
  readmem(PWR_MGM, 1, &_buff[0]);
  writemem(PWR_MGM, ((_buff[0] & PWRMGM_STBY_YG) | _Status << 4));
}

void ITG3200::setZgyroStandby(bool _Status) {
  readmem(PWR_MGM, 1, &_buff[0]);
  writemem(PWR_MGM, ((_buff[0] & PWRMGM_STBY_ZG) | _Status << 3));
}

uint8_t ITG3200::getClockSource() {  
  readmem(PWR_MGM, 1, &_buff[0]);
  return (_buff[0] & PWRMGM_CLK_SEL);
}

void ITG3200::setClockSource(uint8_t _CLKsource) {   
  readmem(PWR_MGM, 1, &_buff[0]);
  writemem(PWR_MGM, ((_buff[0] & ~PWRMGM_CLK_SEL) | _CLKsource)); 
}

void ITG3200::writemem(uint8_t _addr, uint8_t _val) {
    i2c_write_byte_eeprom( _dev_address, _addr, _val );
}

void ITG3200::readmem(uint8_t _addr, uint8_t _nbytes, uint8_t __buff[]) {
    i2c_read_bytes_eeprom( _dev_address, _addr, __buff, _nbytes );
}
