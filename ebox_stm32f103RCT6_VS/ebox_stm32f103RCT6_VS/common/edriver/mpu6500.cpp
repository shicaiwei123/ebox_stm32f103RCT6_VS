#include "mpu6500.h"
#include "my_math.h"

const uint16_t MPU6500::gyroFsrList[] = { 250,500,1000,2000 };
const uint8_t MPU6500::accelFsrList[] = { 2,4,8,16 };
const uint8_t MPU6500::mpuIDList[] = { MPU6500_ID,MPU6555_ID };

MPU6500::MPU6500(I2c* i2c, MPU6500_Model_Typedef model, uint8_t address /*= address*/) :
	I2cIC(i2c),
	address(address)
{
	mpuID = mpuIDList[model];
}

void MPU6500::begin(uint32_t speed /*= 400000*/, uint16_t sampleRate /*= 1000*/,
	MPU6500_Gyro_Full_Scale_Typedef gyroFsrReg/* = MPU6500_Gyro_Full_Scale_2000dps*/,
	MPU6500_Accel_Full_Scale_Typedef accelFsrReg/* = MPU6500_Accel_Full_Scale_8g*/,
	MPU6500_DLPF_Bandwidth_Typedef bwDLPF /*= MPU6500_DLPF_Bandwidth_41Hz*/,
	MPU6500_A_DLPF_Bandwidth_Typedef bwADLPF /*= MPU6500_A_DLPF_Bandwidth_44_8Hz*/)
{
	I2cIC::begin(speed);

	//初始化
	writeByte(address, MPU6500_PWR_MGMT1 , 0X80);//复位MPU6500
	delay_ms(100);  //延时100ms
	writeByte(address, MPU6500_PWR_MGMT1 , 0X00);//唤醒MPU6500

	//配置陀螺仪和加速度计
	setGyroFsr(gyroFsrReg);
	setAccelFsr(accelFsrReg);

	writeByte(address, MPU6500_INT_EN , 0X00);   //关闭所有中断
	writeByte(address, MPU6500_USER_CTRL , 0X00);//I2C主模式关闭
	writeByte(address, MPU6500_FIFO_EN , 0X00);	//关闭FIFO
	writeByte(address, MPU6500_INTBP_CFG , 0X82);//INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
	u8 id = readByte(address, MPU6500_DEVICE_ID );  //读取MPU6500的ID
	if (id == mpuID) //器件ID正确
	{
		writeByte(address, MPU6500_PWR_MGMT1 , 0X01);  	//设置CLKSEL,PLL X轴为参考
		writeByte(address, MPU6500_PWR_MGMT2 , 0X00);  	//加速度与陀螺仪都工作
		setSampleRate(sampleRate);						       	//设置采样率
	}
}

void MPU6500::setGyroFsr(MPU6500_Gyro_Full_Scale_Typedef fsr)
{
	this->gyroFsr = this->gyroFsrList[fsr];
	uint8_t c = readByte(address, MPU6500_GYRO_CFG);
	replaceBits<uint8_t>(c, fsr, 4, 3);//MPU6500_GYRO_CFG[4:3]，GYRO_FS_SEL
	writeByte(address, MPU6500_GYRO_CFG, c);//设置陀螺仪满量程范围
}

void MPU6500::setAccelFsr(MPU6500_Accel_Full_Scale_Typedef fsr)
{
	this->accelFsr = this->accelFsrList[fsr];
	uint8_t c = readByte(address, MPU6500_ACCEL_CFG);
	replaceBits<uint8_t>(c, fsr, 4, 3);//MPU6500_ACCEL_CFG[4:3]，ACCEL_FS_SEL
	writeByte(address, MPU6500_ACCEL_CFG, c);//设置加速度传感器满量程范围
}

void MPU6500::setBothLPF(MPU6500_DLPF_Bandwidth_Typedef bwDLPF, MPU6500_A_DLPF_Bandwidth_Typedef bwADLPF)
{
	setDLPF(bwDLPF);
	setADLPF(bwADLPF);
}


void MPU6500::setDLPF(MPU6500_DLPF_Bandwidth_Typedef bwDLPF)
{
	uint8_t c = readByte(address, MPU6500_CFG);
	replaceBits<uint8_t>(c, bwDLPF, 2, 0);//MPU6500_CFG[2:0]，DLPF_CFG
	writeByte(address, MPU6500_CFG, c);//设置数字低通滤波器

	c = readByte(address, MPU6500_GYRO_CFG);
	replaceBits<uint8_t>(c, 0, 1, 0);//MPU6500_GYRO_CFG[1:0]，Fchoice_b
	writeByte(address, MPU6500_GYRO_CFG, c);//设置GYRO数字滤波器
}

void MPU6500::setADLPF(MPU6500_A_DLPF_Bandwidth_Typedef bwADLPF)
{
	uint8_t c = readByte(address, MPU6500_ACCEL_CFG);
	replaceBits<uint8_t>(c, 0, 3, 3);//MPU6500_ACCEL_CFG[3]，accel_fchoice_b
	replaceBits<uint8_t>(c, bwADLPF, 2, 0);//MPU6500_ACCEL_CFG[3]，A_DLPFCFG
	writeByte(address, MPU6500_ACCEL_CFG, c);//设置ACCEL数字滤波器
}

void MPU6500::setSampleRate(u16 sampleRate)
{
	u8 data;
	limit<u16>(sampleRate, 4, 1000);
	this->sampleRate = sampleRate;
	data = 1000 / sampleRate - 1;
	writeByte(address, MPU6500_SAMPLE_RATE, data); //This is the update rate of sensor register.
}

void MPU6500::setBypass()
{
	uint8_t c = readByte(address, MPU6500_INTBP_CFG);
	replaceBits<uint8_t>(c, 1, 1, 1);//MPU6500_INTBP_CFG[1]，BYPASS_EN
	writeByte(address, MPU6500_INTBP_CFG, 0X82);//INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
}

float MPU6500::getTemperature(void)
{
	u8 buf[2];
	short raw;
	float temp;
	readBytes(address, MPU6500_TEMP_OUTH , 2, buf);
	raw = ((u16)buf[0] << 8) | buf[1];
	temp = 21 + ((double)raw) / 333.87;
	return temp;
}

void MPU6500::getGyroscope(short *gx, short *gy, short *gz)
{
	u8 buf[6];
	readBytes(address, MPU6500_GYRO_XOUTH , 6, buf);
	*gx = ((u16)buf[0] << 8) | buf[1];
	*gy = ((u16)buf[2] << 8) | buf[3];
	*gz = ((u16)buf[4] << 8) | buf[5];
}

void MPU6500::getGyroscope(float *gx, float *gy, float *gz)
{
	short x, y, z;
	getGyroscope(&x, &y, &z);
	//16.4 = 2^16/4000 lsb °/s     1/16.4=0.061     0.0174 = 3.14/180
	//陀螺仪数据从ADC转化为弧度每秒(这里需要减去偏移值)
	*gx = (double)x * gyroFsr * 5.31005859375e-7;  //2 / 65536 * 0.0174
	*gy = (double)y * gyroFsr * 5.31005859375e-7;
	*gz = (double)z * gyroFsr * 5.31005859375e-7;	//读出值减去基准值乘以单位，计算陀螺仪角速度
}

void MPU6500::getAccelerometer(short *ax, short *ay, short *az)
{
	u8 buf[6];
	readBytes(address, MPU6500_ACCEL_XOUTH, 6, buf);
	*ax = ((u16)buf[0] << 8) | buf[1];
	*ay = ((u16)buf[2] << 8) | buf[3];
	*az = ((u16)buf[4] << 8) | buf[5];
}

void MPU6500::getAccelerometer(float *ax, float *ay, float *az)
{
	short x, y, z;
	getAccelerometer(&x, &y, &z);
	//+-8g,2^16/16=4096lsb/g--0.244mg/lsb
	//此处0.0098是：(9.8m/s^2)/1000,乘以mg得m/s^2
	*ax = (double)x * accelFsr * 2.99072265625e-4;  //2 * 9.8 / 65536
	*ay = (double)y * accelFsr * 2.99072265625e-4;
	*az = (double)z * accelFsr * 2.99072265625e-4;
}

u16 MPU6500::getSampleRate()
{
	return sampleRate;
}
