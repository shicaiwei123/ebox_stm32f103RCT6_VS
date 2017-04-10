# ebox_stm32f103RCT6_VS
为stm32f103RCT6配置好的ebox，visual studio项目，方便以后使用

vs工程和移植注意事项在ebox_stm32f103RCT6_VS/ebox_stm32f103RCT6_VS/ebox_stm32f103RCT6_VS文件夹下

## 包含驱动
### 逻辑器件
- 74HC595：移位寄存器
### 储存
- E2PROM
    - AT24C02
- Flash
    - AT45DB
    - w25x16
- RAM
    - FM25V
- SD/mmc
    - mmc_sd
### 输入设备
- 按键
    - Button
    - key_dm
- 矩阵键盘
    - TM1638：带键盘扫描接口的LED驱动控制专用电路
### 输出设备
- 显示屏
    - FT800
    - lcd_1.8
    - LCD1602
    - Nokia5110
    - OLED
    - OLED_ssd1306
- LED
    - colorled
    - LED
    - led_segment：数码管，使用74hc595
    - TM1638：带键盘扫描接口的LED驱动控制专用电路
    - ws2812：内置IC串行RGBLED
- OSD
    - Max7456：单通道、单色随屏显示（OSD）发生器，内置EEPROM
- GPIO
    - parallel_gpio：8位并行输出，可以组合任意端口
### 传感器
- 湿度传感器
    - dht11
- 温度传感器
    - ds18b20
- 尘埃传感器
    - GP2Y1051
- 红外遥控
    - ir_decoder
    - ir_encoder
- 陀螺仪
    - MPU6050
    - L3G4200D
- 电子罗盘
    - lsm303dhlc
- ADC
    - LTC1446：12位逐次逼近式工作的A/D转换器
- 距离传感器
    - ultrasonic_wave：超声波
### 传输
- 2.4G无线
    - NRF24L01
- 数传
    - si4432：低于1GHz频段的无线数传芯片
- 以太网
    - w5500：全硬件TCP/IP嵌入式以太网控制器
### RTC（实时时钟）
- ds3231：i2c
### 正交编码器
- encoder