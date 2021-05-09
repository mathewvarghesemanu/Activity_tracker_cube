#include <Wire.h>
#include <math.h>

struct gyro_readings_struct{
  /*
  Structure to store roll and pitch values to be used in other functions
  */
  double roll;
  double pitch;
};
gyro_readings_struct gyro_readings;

// MPU6050 Slave Device Address
const uint8_t MPU6050SlaveAddress = 0x68;

// Select SDA and SCL pins for I2C communication 
const uint8_t scl = D6;
const uint8_t sda = D7;

// sensitivity scale factor respective to full scale setting provided in datasheet 
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;
double roll,pitch,rollF,pitchF=0;
// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;

int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

void MPU6050_Init(){

  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}

void initialize_gyro(){
  /*
  Function to initialise the gyro. To be called from inside the Setup function
  */
  Wire.begin(sda, scl);
  MPU6050_Init();
}


void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  // Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  // GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  // GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  // GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}



void read_gyro(){
  /*
  Function to read the gyro values and assignment to the global structure for gyro
  */
  double Ax, Ay, Az, T, Gx, Gy, Gz;
  for (int i=0;i<100;i++){
    Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
    Ax = (double)AccelX/AccelScaleFactor;
    Ay = (double)AccelY/AccelScaleFactor;
    Az = (double)AccelZ/AccelScaleFactor;
    if (Ax!=0 &&Ay!=0 &&Az!=0)
    {
      roll = atan(Ay / sqrt(pow(Ax, 2) + pow(Az, 2))) * 180 / PI;
      pitch = atan(-1 * Ax / sqrt(pow(Ay, 2) + pow(Az, 2))) * 180 / PI;
    
    // Low-pass filter
      rollF = 0.94 * rollF + 0.06 * roll;
      pitchF = 0.94 * pitchF + 0.06 * pitch;

      gyro_readings.roll=rollF;
      gyro_readings.pitch=pitchF;
      
      // Serial.print(rollF);
      // Serial.print("/");
      // Serial.println(pitchF);
    }
  }
  delay(400);
}















