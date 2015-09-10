#ifndef __YEELINK_HANDLE_H_
#define __YEELINK_HANDLE_H_




//域名
#define YEELINK_SERVER      "api.yeelink.net"

//API KEY
#define YEELINK_APIKEY      "af735029b2e431481dd0e6ef5bee6a27"

//设备名称
#define Control_LED         "196272"
    //设备下传感器ID号
    #define KEY_ID          "214772"
	#define LED2_ID         "241611"

//空气质量监测仪
#define AirTest_ID      "339895"
   //空气质量指数
   #define  AQI_ID      "376376"
   //粉尘浓度
   #define  Dust_ID     "376377"
   //空气湿度
   #define  Humidity_ID "376378"
   //甲醛浓度
   #define  Methanal_ID  "376382"
   //空气温度
   #define  Temp_ID      "376383"

typedef struct{
   float C_LOW,C_HIGH;
   int   I_LOW,I_HIGH;
}AQI_DATA;

typedef struct{
    int Humidity;
	int Methanal;
	int Temperature;
	int Dust;
	int AQI;
}SENSOR_DATA;



typedef struct  _XPG_GLOBALVAR
{

    char http_sockettype; //yeelink链接类型

	char a_mac[13]; //MAC地址

#define LED_ON   1
#define LED_OFF  0
	char led_State;//LED状态

    
}XPG_GLOBALVAR;



//LED ON
#define LED2_ON()  BSP_GPIOSetValue(USER_GPIO_IDX_LED, GPIO_LOW_LEVEL)
//LED OFF
#define LED2_OFF() BSP_GPIOSetValue(USER_GPIO_IDX_LED, GPIO_HIGH_LEVEL)

//LED ON
#define LED3_ON()  BSP_GPIOSetValue(USER_GPIO_IDX_LED3, GPIO_LOW_LEVEL)
//LED OFF
#define LED3_OFF() BSP_GPIOSetValue(USER_GPIO_IDX_LED3, GPIO_HIGH_LEVEL)

//Launch directconfig mode
#define KEY2_IN()  BSP_GPIOGetValue(USER_GPIO_DIRECTCONFIG)


/* zhou mask for debug */
#if 0
#define Yeelink_Printf(level, fmt, args...)
#else
#define Yeelink_Printf(level, fmt, args...)  printf(fmt, ##args);printf("\r\n")
#endif



extern void Yeelink_Init(void);
extern VOID Yeelink_Run(VOID *arg);
int GetSensorData(void);


#endif

