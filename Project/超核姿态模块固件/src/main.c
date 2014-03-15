//包含超核库所需的头文件
#include "clock.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "stdio.h"
#include "board.h"

//包含 姿态模块所需的头文件
#include "mpu6050.h"
#include "hmc5883.h"
#include "bmp180.h"
#include "imu.h"
#include "trans.h"
#include "dma.h"

#define I2C_SPEED_URANUS   (400*1000)

static mpu6050_device mpu6050_device1;
static hmc5883_device hmc_device;
static bmp180_device bmp180_device1;
//实现姿态解算的回调并连接回调
static uint32_t imu_get_mag(int16_t * mx, int16_t * my, int16_t *mz)
{
    hmc_device.read_data(&hmc_device, mx, my, mz);
    return 0;
}

static uint32_t imu_get_accel(int16_t * ax, int16_t * ay, int16_t *az)
{
    mpu6050_device1.read_accel(&mpu6050_device1, ax,ay,az);
    return 0;  
}

static uint32_t imu_get_gyro(int16_t * gx, int16_t * gy, int16_t * gz)
{
    mpu6050_device1.read_gyro(&mpu6050_device1, gx,gy,gz);
    return 0;
}

static imu_io_install_t IMU_IOInstallStruct1 = 
{
    .imu_get_accel = imu_get_accel,
    .imu_get_gyro = imu_get_gyro,
    .imu_get_mag = imu_get_mag,
};


uint8_t InitSensor(void)
{
    static int init = 0;
    uint8_t ret = 0;
    if(!init)
    {
        ret += mpu6050_init(&mpu6050_device1, BOARD_I2C_MAP, "mpu6050", I2C_SPEED_URANUS);
        ret += hmc5883_init(&hmc_device, BOARD_I2C_MAP, "hmc5883", I2C_SPEED_URANUS);
        ret += bmp180_init(&bmp180_device1, HW_I2C0, 0x77, I2C_SPEED_URANUS);
        init = 1;
    }
    if(ret)
    {
        return ret;
    }
    return 0;
}


int main(void)
{
    uint32_t i;
    uint8_t ret;
    uint8_t instance;
    uint32_t counter = 0;
    trans_user_data_t send_data;
    uint32_t LED_instanceTable[] = BOARD_LED_GPIO_BASES;
    uint32_t LED_PinTable[] = BOARD_LED_PIN_BASES;
    uint32_t led_num = ARRAY_SIZE(LED_instanceTable);
    //int32_t temperature;
    //int32_t pressure;
    imu_float_euler_angle_t angle;
    imu_raw_data_t raw_data;
    //初始化Delay
    DelayInit();
    // 初始化UART和printf
    instance = UART_QuickInit(BOARD_UART_DEBUG_MAP, 115200);
    printf("UART Init OK!\r\n");
    // 初始化LED
    
    for(i = 0; i < led_num; i++)
    {
        GPIO_QuickInit(LED_instanceTable[i], LED_PinTable[i], kGPIO_Mode_OPP);  
    }
    //初始化传感器
    ret = InitSensor();
    if(ret)
    {
        printf("Sensor init failed! code:%d\r\n", ret);
    }
    //安装IMU 底层驱动函数
    imu_io_install(&IMU_IOInstallStruct1);
    trans_init();
    UART_ITDMAConfig(instance, kUART_DMA_Tx);
    while(1)
    {
        //获取欧拉角 获取原始角度
        imu_get_euler_angle(&angle, &raw_data);
        //printf("%05d %05d %05d\r", (int16_t)angle.imu_pitch, (int16_t)angle.imu_roll, (int16_t)angle.imu_yaw);
        //将数据整合打包 变成上位机协议格式
        send_data.trans_accel[0] = raw_data.ax;
        send_data.trans_accel[1] = raw_data.ay;
        send_data.trans_accel[2] = raw_data.az;
        send_data.trans_gyro[0] = raw_data.gx;
        send_data.trans_gyro[1] = raw_data.gy;
        send_data.trans_gyro[2] = raw_data.gz;
        send_data.trans_mag[0] = raw_data.mx;
        send_data.trans_mag[1] = raw_data.my;
        send_data.trans_mag[2] = raw_data.mz;     
        send_data.trans_pitch = (int16_t)angle.imu_pitch*100;
        send_data.trans_roll = (int16_t)angle.imu_roll*100;
        send_data.trans_yaw = (int16_t)angle.imu_yaw*10;
        //压力采集程序 由于上位机不传送压力，所以先注释掉
        /*
        while(1)
        {
            bmp180_start_conversion(&bmp180_device1, BMP180_T_MEASURE);
            DelayMs(50);
            bmp180_read_temperature(&bmp180_device1, &temperature);
            bmp180_start_conversion(&bmp180_device1, BMP180_P3_MEASURE);
            DelayMs(50);
            bmp180_read_pressure(&bmp180_device1, &pressure);
            printf("T:%05d P:%05d\r\n", temperature, pressure);
        }
        */
        //如果DMA空闲 则 启动发送数据
        if(DMA_IsTransferComplete(HW_DMA_CH1) == 0)
        {
            trans_send_pactket(send_data);
            DelayMs(1);
        }
        //闪LED 说明系统运行
        counter++;
        counter %= 10; 
        if(!counter)
        {
            GPIO_ToggleBit(LED_instanceTable[0], LED_PinTable[0]);
        }
    }
}


#ifdef USE_FULL_ASSERT
void assert_failed(char * file, uint32_t line)
{
	//断言失败检测
	while(1);
}
#endif
