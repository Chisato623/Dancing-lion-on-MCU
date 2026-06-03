# 这是一个基于单片机MSPM0G3507的醒狮控制程序，初步实现功能为：
- 语音识别功能
- 舵机控制醒狮前进后退，跳舞趴下

根目录下的motor和voice文件夹中是初版代码，根目录则将两个模块合并并整理了一下引脚

## 使用方法：
1. 在m0sdk的driverlib地址中，例如：E:\mspm0_sdk_1_30_00_03\examples\nortos\LP_MSPM0G3507\driverlib，新建一个文件夹，将该项目解压即可，如果不在在此地址解压，则可能由于缺少头文件报错

## 引脚分配表

| 引脚 | 功能 | 外设实例 | 说明 |
|------|------|----------|------|
| **PA0** | UART TX | `UART_0` (UART0) | 串口发送 (MFCLK) |
| **PA1** | UART RX | `UART_0` (UART0) | 串口接收 |
| **PA6** | GPIO (输出) | `GPIO` | LCD CS 片选 |
| **PA12** | GPIO (输出, 初始高) | `GPIO` | LED 指示灯 |
| **PA15** | PWM CCP2 | `PWM_MOTOR2` (TIMA0) | 电机2 CH2 |
| **PA17** | PWM CCP3 | `PWM_MOTOR2` (TIMA0) | 电机2 CH3 |
| **PA19** | SWDIO | DEBUGSS | 调试数据 |
| **PA20** | SWCLK | DEBUGSS | 调试时钟 |
| **PA21** | PWM CCP0 | `PWM_MOTOR2` (TIMA0) | 电机2 CH0 |
| **PA22** | PWM CCP1 | `PWM_MOTOR2` (TIMA0) | 电机2 CH1 |
| **PA24** | GPIO (输出) | `GPIO` | LCD DC 数据/命令选择 |
| **PA25** | GPIO (输出) | `GPIO` | LCD RES 复位 |
| **PA26** | GPIO (输出) | `GPIO` | LCD SDA SPI数据 (软件模拟) |
| **PA27** | GPIO (输出) | `GPIO` | LCD SCL SPI时钟 (软件模拟) |
| **PA28** | I2C SDA | `I2C_0` (I2C0) | 语音模块 数据线 |
| **PA31** | I2C SCL | `I2C_0` (I2C0) | 语音模块 时钟线 |
| **PB7** | GPIO (输出) | `IN4` | 方向控制 IN4 |
| **PB19** | GPIO (输出) | `IN2` | 方向控制 IN2 |
| **PB20** | PWM CCP0 | `PWM_MOTOR1` (TIMG12) | 电机1 CH0 |

## 当前已实现功能：
- ✅ 语音识别控制
- ✅ 舵机控制（前进、后退、跳舞、趴下）
- ✅ LCD 屏幕显示（欢迎画面 + 表情绘制）

## 后续开发目标：实现播放音乐等功能
