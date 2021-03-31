# 快速设计一款智能风扇（MCU接入）

## 概况 

科技在不断进步，越来越多的智能家居产品和服务进入到人们的日常生活中。智能风扇是一款常见的智能设备，用户可以使用手机 App 轻松控制，通过远程控制开关、风速、场景联动等来轻松创造出温暖、放松、舒适的室内空间。

该功能板采用FU6832s作为主控芯片，FU6832 系列是一款集成电机控制引擎(ME)和 8051 内核的高性能电机驱动专用芯片，ME 集成FOC、MDU、LPF、PI、SVPWM/SPWM 等诸多硬件模块，可硬件自动完成电机 FOC/BLDC运算控制; 8051 内核用于参数配置和日常事务处理，双核并行工作实现各种高性能电机控制。其中8051 内核大部分指令周期为 1T 或 2T，芯片内部集成有高速运算放大器、比较器、Pre-driver、高速 ADC、高速乘/除法器、CRC、SPI、I2C、UART、LIN、多种 TIMER、PWM 等功能，内置高压LDO，适用于 BLDC/PMSM 电机的方波、SVPWM/SPWM、FOC 驱动控制。

FU6832内部具有全面保护，包括过压保护，欠压保护， 过流保护，FO保护，堵转保护，缺相保护，过温保护，过功率保护，运放偏置电压异常保护。可根据需要选择使能对应的保护，再根据实际情况微调即可。

只需简单的外围处理便可实现高效率的FOC电机驱动。FU6832的有感启动无感运行FOC驱动主要应用在各类低压风机上，典型应用如落地扇、空气净化器等。

基于涂鸦智能平台和三明治开发板，您可以快速地开发一款智能的，安全性强的风扇产品的原型。



## BLDC功能板须知

**注意：BLDC功能板是支持串口，按键和PWM控制，但是因为MCU控制板和WIFI通信板通过右下角串口通信，为了减少对BLDC控制的影响，该功能板默认是只支持PWM控制的。**

BLDC板的采样频率是12M，通过PWM占空比的大小来控制电机转速的，输入PWM占空比越大转速越快，在这里我们约定，PWM占空比越大转速越快，Demo中输出频率为1000HZ。



**开机PWM占空比：0.08，大于该占空比时开机**

**关机PWM占空比：0.06，小于该占空比关机（停机占空比不要设置为改变转向占空比区间内）   **

**改变转向的PWM占空比：0.01~0.025，处于该占空比则停机改变方向   **

**PWM输出极性(Polarity)为低（LOW）**

PWM配置示例(主频16M)：

![PWM配置](https://images.tuyacn.com/smart/BLDC_DEMO/PWM_cfg.png)



## 物料清单 

涂鸦三明治 Wi-Fi MCU 通信板（WB3S） 

涂鸦三明治BLDC电机驱动功能板 

涂鸦三明治直流供电电源板 

NUCLEO-G071RB 



## 步骤 

### 第1步：方案介绍 

MCU 方案与 SoC 方案不同，传感器和联网模组的驱动代码写在 MCU 中，开发者可以自己开发 MCU 代码，拥有更多的可玩性。因此在开发前，我们需要对方案有个整体了解，方便后续开发。

涂鸦三明治开发板BLDC套件中，BLDC板通过PWM接口接收NUCLEO-G071RB传过来的PWM，BLDC板通过接收到的PWM的占空比的大小对电机进行驱动。MCU 控制板,通过串口 与 Wi-Fi 通信板连接，使用涂鸦智能 App 配网，可以将BLDC板的输出参数状态展现在手机端。MCU 型号为  STM32G071RB ，Demo 教程采用 Keil5 进行编程。



### 第2步：硬件连接

本次使用的涂鸦三明治开发板BLDC套件主要包含：

+ BLDC 功能板，负责通过判断接收到的 PWM 的占空比大小进行对电机的控制 
+ Wi-Fi MCU 通信板，板载涂鸦  WB3S 模组，负责智能化连接。模组已烧录通用固件，MCU 对接涂鸦串口协议，即可使用模组+App+云涂鸦一站式智能化服务。 
+ MCU控制板，采用ST官方 NUCLEO-G071RB 开发板，负责传感数据接收和模组通讯控制。NUCLEO-G071RB 开发板支持 Arduino 接口，本片教程demo我们也将基于 Keil 开发演示。
+ 涂鸦直供电源电源板，因对BLDC功能板需要 12V，5V，3.3V 电源，使用电源板可以减少排线。

将三明治开发板套件电源板，控制板、通讯板、功能板拼接组装，实物效果如下图。

<img src="https://images.tuyacn.com/smart/BLDC_DEMO/dev_connect.jpg" alt="dev_connect" style="zoom:20%;" />



### 第3步：例程下载

软件开发过程主要基于 Keil5 实现 MCU 与传感器和模组协议对接。首先调通 MCU 和模组的通讯，可以实现App配网，MCU 数据传输到App。 



### 第4步：开发实战-通信板调试 

体验了 demo 例程后，接下来我们详细讲解讲解整个demo例程实现的具体步骤，开发者可以根据步骤，快速开发一个属于自己的智能风扇。 

#### 步骤4.1：创建产品

创建工程，下载移植涂鸦 MCU SDK 开发包详细教程：

进入[涂鸦智能IoT平台](https://iot.tuya.com/)，点击创建产品。选择小家电->风扇

![创建产品](https://images.tuyacn.com/smart/BLDC_DEMO/creat_product.png)



选择自定义方案，选择风扇，为你的产品起一个名字，点击创建产品。**MCU 对接时选择自定义方案，才能下载到 MCU 开发资料包。**

![自定义方案](https://images.tuyacn.com/smart/BLDC_DEMO/select_programme.png)



功能根据自身需求选择。

![选择功能](https://images.tuyacn.com/smart/BLDC_DEMO/Func_select.png)



还可以对某一项功能进行编辑，根据个人需求进行编辑，我这里将工作模式修改为只有自然风和睡眠风两种。

![编辑功能1](https://images.tuyacn.com/smart/BLDC_DEMO/Func_select_1.png)



选择自己喜欢的面板，第一次开始调试也可以选择为开发调试面板，便于调试，后面也可以更换面板。



面板选择完后，点击下一步：硬件配置。点击后，将页面拉到最下面，下载开发资料。

![下载开发资料](https://images.tuyacn.com/smart/BLDC_DEMO/download_doc.png)

#### 步骤4.2：硬件测试 

下载到 MCU 开发包后，使用开发包中的涂鸦模组调试助手，开发者可以使用助手模拟 MCU 模式，配合调试模组通讯板，验证模组是否通讯正常，同时也可以熟悉涂鸦串口协议提高对接效率。确定通信板正常可用的，可以跳过此步骤。若调试过程中对协议收发有疑问，也可以使用此助手协助查看正确数据交互格式。 

使用步骤可参考：[涂鸦模组调试助手使用说明链接](https://developer.tuya.com/cn/docs/iot/device-development/access-mode-mcu/access-mode-simmulator/module-debugging-assistant-instruction?id=K9hs0cj3lf0au) 



#### 步骤4.3：移植MCU_SDK

移植调试详细教程：[MCU_SDK移植](https://developer.tuya.com/cn/docs/iot/device-development/access-mode-mcu/wifi-general-solution/software-reference-wifi/overview-of-migrating-tuyas-mcu-sdk?id=K9hhi0xr5vll9) 

我在这里简单的介绍下移植过程和功能实现：

将mcu_sdk中的文件加入工程后，编译根据报错提示，进行修改。

##### 步骤4.3.1：mcu_sdk移植简单介绍 

+ > 如果编译过程中发生错误:` #40: expected an identifier  DISABLE = 0`, 类似这样的错误提示，可以包含头文件` #include "stm32f1xx.h" `来解决。(这里的对应头文件为你的实际芯片型号)

    例如：我这里是G071RB的芯片，这里添加的就是`#include "stm32g0xx.h"`



+ 这里因为是demo的原因就不打开wifi功能测试，把WIFI_TEST_ENABLE宏注释掉。

    ```c
    //#define         WIFI_TEST_ENABLE
    ```

    

+ 完善`uart_transmit_output()`函数

    ![mcu-sdk移植1](https://images.tuyacn.com/smart/BLDC_DEMO/mcu_sdk_transplant_1.png)



+ 完善`uart_receive_input()`函数

![mcu-sdk移植2.1](https://images.tuyacn.com/smart/BLDC_DEMO/mcu_sdk_transplant_2.1.png)

在MCU与WIFI通信板连接的串口的中断服务函数中添加以下代码，注意添加头文件或声明你用到的函数。

![mcu-sdk移植2.2](https://images.tuyacn.com/smart/BLDC_DEMO/mcu_sdk_transplant_2.2.png)



+ `wifi_uart_service()`函数按照`#error`中的提示信息处理即可，处理后注释掉。

    ![mcu-sdk移植3](https://images.tuyacn.com/smart/BLDC_DEMO/mcu_sdk_transplant_3.png) 

    

+ `wifi_protocol_init()`函数按照`#error`中的提示信息处理即可，处理后注释掉。

    ![mcu-sdk移植4](https://images.tuyacn.com/smart/BLDC_DEMO/mcu_sdk_transplant_4.png)

    

+ 接下来便是`all_data_update()`函数，该函数会自动上报系统中所有dp点信息，个人不需调用。 

    定义一个结构体，用来记录电扇的工作状态。

    ```C
    //工作模式
    typedef enum {
    	nature = 0,
    	sleep
    }fan_mode_t;
    
    //正反转
    typedef enum {
    	forward = 0,
    	reverse
    }fan_direction_t;
    
    //电扇工作状态结构体
    typedef struct {
    	_Bool OnOff;
    	fan_mode_t e_fan_mode;
    	unsigned long speed;
    	fan_direction_t e_fan_direction;
    }fan_status_t;
    
    //电扇状态结构体，全局变量
    fan_status_t gs_fan_status = {
    		.OnOff 					= FALSE,
    		.e_fan_mode 			= nature,
    		.speed 					= 10,
    		.e_fan_direction 		= forward
    };
    ```

    

+ 在protocol.c文件中，完善`dp_download_switch_handle()`，`dp_download_mode_handle()`，`dp_download_fan_speed_handle()`和`dp_download_fan_direction_handle()`这四个功能处理函数。

    

    在protocol.c文件的`dp_download_switch_handle()`函数中：

    ```C
    static unsigned char dp_download_switch_handle(const unsigned char value[], unsigned short length)
    {
        //示例:当前DP类型为BOOL
        unsigned char ret;
        //0:关/1:开
        unsigned char switch_1;
        
        switch_1 = mcu_get_dp_download_bool(value,length);
        if(switch_1 == 0) {
            //开关关
    		gs_fan_status.OnOff = FALSE;
        }else {
            //开关开
    		gs_fan_status.OnOff = TRUE;
        }
      
        //处理完DP数据后应有反馈
        ret = mcu_dp_bool_update(DPID_SWITCH,switch_1);
        if(ret == SUCCESS)
            return SUCCESS;
        else
            return ERROR;
    }
    ```

    

    在protocol.c文件的`dp_download_mode_handle()`函数中：

    ```c
    static unsigned char dp_download_mode_handle(const unsigned char value[], unsigned short length)
    {
        //示例:当前DP类型为ENUM
        unsigned char ret;
        unsigned char mode;
        
        mode = mcu_get_dp_download_enum(value,length);
        switch(mode) {
            case 0:
    					gs_fan_status.e_fan_mode = nature;	//自然风模式
            break;
            
            case 1:
    					gs_fan_status.e_fan_mode = sleep;	//睡眠风模式
            break;
            
            default:
    					gs_fan_status.e_fan_mode = nature;
            break;
        }
        
        //处理完DP数据后应有反馈
        ret = mcu_dp_enum_update(DPID_MODE, mode);
        if(ret == SUCCESS)
            return SUCCESS;
        else
            return ERROR;
    }
    ```

    

    在protocol.c文件的`dp_download_fan_speed_handle()`函数中：

    ```c
    static unsigned char dp_download_fan_speed_handle(const unsigned char value[], unsigned short length)
    {
        //示例:当前DP类型为VALUE
        unsigned char ret;
        unsigned long fan_speed;
        
        fan_speed = mcu_get_dp_download_value(value,length);
        /*
        //VALUE类型数据处理
        
        */
        gs_fan_status.speed = fan_speed;	//将下发的速度值给全局变量
        //处理完DP数据后应有反馈
        ret = mcu_dp_value_update(DPID_FAN_SPEED,fan_speed);
        if(ret == SUCCESS)
            return SUCCESS;
        else
            return ERROR;
    }
    ```

    

    在protocol.c文件的`dp_download_fan_direction_handle()`函数中：

    ```c
    static unsigned char dp_download_fan_direction_handle(const unsigned char value[], unsigned short length)
    {
        //示例:当前DP类型为ENUM
        unsigned char ret;
        unsigned char fan_direction;
        
        fan_direction = mcu_get_dp_download_enum(value,length);
        switch(fan_direction) {
            case 0:		//判断当前风向是否为正转，当前风向若不是正转，则改变风向，并将当前状态给全局变量
    					if(gs_fan_status.e_fan_direction != forward) {
    						change_fan_direction();
    						gs_fan_status.e_fan_direction = forward;
    					}
            break;
            
            case 1:		//判断当前风向是否为反转，当前风向若不是反转，则改变风向，并将当前状态给全局变量
    					if(gs_fan_status.e_fan_direction != reverse) {
    						change_fan_direction();
    						gs_fan_status.e_fan_direction = reverse;
    					}
            break;
            
            default:
            break;
        }
        
        //处理完DP数据后应有反馈
        ret = mcu_dp_enum_update(DPID_FAN_DIRECTION, fan_direction);
        if(ret == SUCCESS)
            return SUCCESS;
        else
            return ERROR;
    }
    ```



##### 步骤4.3.2：功能实现简单介绍  

+ 在`main.c`文件中：

    添加头文件`#include "mcu_api.h"`和` #include "wifi.h"`

    定义以下宏和变量：
    
    ```C
    //最小速度时，输出的PWM占空比
    #define MIN_SPEED	10
    
    //最大速度时，输出的PWM占空比
    //最大速度输出的PWM占空比应该为100(建议最大设置为99)，我这里因为演示设置较低
    #define MAX_SPEED	35
    
    //关机输出占空比
    #define	OFF_SPEED	5
    
    //改变风扇转向输出的PWM值，在BLDC开发板中输出PWM在1%~2.5%之间改变电机转向
    #define DIRECTION_CHANGE_PWM 15
    
    //睡眠模式下，风速改变时间
    #define SLEEP_TIME 700
    
    //上一次风扇速度，全局变量
    unsigned long last_fan_speed = 0;
    
    //风速sleep模式下，改变风速计数值和风速改变标志，全局变量
    unsigned long fen_count = SLEEP_TIME;
    _Bool sleep_speed_flag = TRUE;	
    ```



​	启动后，进入`while(1){}`循环前需处理的： 

```C
void setup(void)
{
	//优先输出频率为1000HZ，占空比为5%，使电机处于关机状态
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (OFF_SPEED * 10));
	
	//打开与涂鸦三明治 Wi-Fi MCU 通信板（E3S）通信的UART1接收中断
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	
	//完成wifi协议初始化
	wifi_protocol_init();
}
```

​	在`while(1){}`循环内： 

```C
while (1)
  {
		//wifi串口数据处理服务
		wifi_uart_service();
		
		//进入配网模式，并改变LED灯状态进行提示
		connect_tuya();
		
		if (gs_fan_status.OnOff == TRUE) { //开机
			//判断工作模式
			check_mode();
		} else {
			set_fan_speed(0);
		}
  }
```

​	

​	在`connect_tuya()`函数中：

```c
//该函数主要功能为：当PC3被拉低后，进入配网模式。根据不同联网状态，改变LED灯状态进行提示。
void connect_tuya(void)
{
	//判断PC3是否拉低
	if (HAL_GPIO_ReadPin(WIFI_KEY_GPIO_Port, WIFI_KEY_Pin) == GPIO_PIN_RESET) {
		HAL_Delay(300);
		if (HAL_GPIO_ReadPin(WIFI_KEY_GPIO_Port, WIFI_KEY_Pin) == GPIO_PIN_RESET) {
			mcu_set_wifi_mode(0);
		}
	}
	
	//获取当前连接状态，显示LED提示
	switch(mcu_get_wifi_work_state())
	{
				case SMART_CONFIG_STATE:	//SMART配网模式，快闪
						HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
						HAL_Delay(250);
				break;
				case AP_STATE:	//AP配网模式，快闪
						HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
						HAL_Delay(250);
				break;
				case WIFI_NOT_CONNECTED: //慢闪
						HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
						HAL_Delay(250);
				break;
				case WIFI_CONNECTED://常亮，连接到WIFI
				case WIFI_CONN_CLOUD://常亮，连接到WIFI和云平台
						HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
				break;
				default:
						HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
				break;
	}
}
```

​	

​	在`set_fan_speed()`函数中：

```C
//该函数主要功能为：根据不同转速，输出对应的PWM。PWM频率为1000HZ。
void set_fan_speed(unsigned long speed)
{
	//输入为0，关闭电机
	if (speed == 0) {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (OFF_SPEED * 10));
		last_fan_speed = OFF_SPEED; //将当前转速，记录下来
		return;
	}
	
	//判断输入值是否超出最大，最小值
	if (speed < MIN_SPEED) {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (MIN_SPEED * 10));
		last_fan_speed = MIN_SPEED;
	} else if (speed > MAX_SPEED) {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (MAX_SPEED * 10));
		last_fan_speed = MAX_SPEED;
	} else {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (speed * 10));
		last_fan_speed = speed;
	}
	
	return;
}
```



​	在`check_mode()`函数中：

```C
void check_mode(void) 
{
	if (gs_fan_status.e_fan_mode == sleep) { //进入睡眠模式
		if ((sleep_speed_flag == TRUE) && (fen_count >= SLEEP_TIME)) {
			
			set_fan_speed(gs_fan_status.speed);
			//计数清零，改变风速
			fen_count = 0;
			sleep_speed_flag = FALSE;
		} else if((sleep_speed_flag == FALSE) && (fen_count >= SLEEP_TIME)) {
			
			set_fan_speed(MIN_SPEED);

			//计数清零，改变风速
			fen_count = 0;
			sleep_speed_flag = TRUE;
		}
		
		fen_count++;
		HAL_Delay(10);
	} else {
		if (last_fan_speed != gs_fan_status.speed) { //如果上一次转速和目标转速不一致，改变转速
				set_fan_speed(gs_fan_status.speed);
		}
	}
}
```



## 小结 

基于涂鸦智能平台， 使用三明治开发板，Keil开发环境您可以快速地开发一款智能风扇产品的原型。