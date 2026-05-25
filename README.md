# 这是一个基于单片机MSPM0G3507的醒狮控制程序，初步实现功能为：
- 语音识别功能
- 舵机控制醒狮前进后退，跳舞趴下

根目录下的motor和voice文件夹中是初版代码，根目录则将两个模块合并并整理了一下引脚

## 引脚分配表

| 引脚 | 功能 | 外设实例 | 说明 |
|------|------|----------|------|
| **PA12** | GPIO (输出, 初始高) | `GPIO` | LED 指示灯 |
| **PA0** | I2C SDA | `I2C_0` (I2C0) | OLED 数据线 |
| **PA1** | I2C SCL | `I2C_0` (I2C0) | OLED 时钟线 |
| **PA10** | UART TX | `UART_0` (UART0) | 串口发送 (MFCLK) |
| **PA11** | UART RX | `UART_0` (UART0) | 串口接收 |
| **PA15** | PWM CCP2 | `PWM_MOTOR2` (TIMA0) | 电机2 CH2 |
| **PA17** | PWM CCP3 | `PWM_MOTOR2` (TIMA0) | 电机2 CH3 |
| **PA21** | PWM CCP0 | `PWM_MOTOR2` (TIMA0) | 电机2 CH0 |
| **PA22** | PWM CCP1 | `PWM_MOTOR2` (TIMA0) | 电机2 CH1 |
| **PB7** | GPIO (输出) | `IN4` | 方向控制 IN4 |
| **PB19** | GPIO (输出) | `IN2` | 方向控制 IN2 |
| **PB20** | PWM CCP0 | `PWM_MOTOR1` (TIMG12) | 电机1 CH0 |
| **PA19** | SWDIO | DEBUGSS | 调试数据 |
| **PA20** | SWCLK | DEBUGSS | 调试时钟 |

## 后续开发目标：实现播放音乐，OLED屏幕显示等功能