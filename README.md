#LuaPad
基于STM32的Lua脚本运行器，目前还在做裸机调试。下一步是移植FreeRTOS操作系统和Lua解释器。

已经测试成功的功能模块有：SD卡，串口，ADC，PWM，I2C，SPI，板载LED，MPU6050

 这个版本程序实现的功能是，按住按键开机，则会一直记录MPU6050数据到SD卡的IMU.txt文件里，频率大概是2000Hz，直到松手。将SD卡插到电脑就可以看到数据了。