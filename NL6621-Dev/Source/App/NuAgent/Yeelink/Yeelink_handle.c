#include "common.h"
#include <stdlib.h>

SENSOR_DATA   sensordata;
XPG_GLOBALVAR g_Yeelink_GlobalVar; //Yeelink全局参数
extern char * send_buf;
extern unsigned char TCPClient_reset_flag;


void Yeelink_initGPIO(void)
{
    //GPIO09
	BSP_GPIOPinMux(USER_GPIO_IDX_LED);		    /* led indicator */
	BSP_GPIOSetDir(USER_GPIO_IDX_LED, 1);		/* output */
	BSP_GPIOSetValue(USER_GPIO_IDX_LED, 1);	    /* low level */

	//GPIO30
	BSP_GPIOPinMux(USER_GPIO_IDX_LED3);		    /* led indicator */
	BSP_GPIOSetDir(USER_GPIO_IDX_LED3, 1);		/* output */
	BSP_GPIOSetValue(USER_GPIO_IDX_LED3, 1);	    /* low level */
	

	/* DirectConfig gpio is valied when set to low level */
	BSP_GPIOPinMux(USER_GPIO_DIRECTCONFIG);  
	BSP_GPIOSetDir(USER_GPIO_DIRECTCONFIG, GPIO_DIRECTION_INPUT); 
}

//初始化yeelink参数。
void Yeelink_Global_Status_Init(void)
{
	/* Initialize Light resource */
	g_Yeelink_GlobalVar.http_sockettype = 0;

	g_Yeelink_GlobalVar.led_State = LED_OFF;
}

//yeelink初始化，GPIO,参数读取等等
void Yeelink_Init(void)
{
	Yeelink_initGPIO();

    Yeelink_Global_Status_Init();
 
    return;
}

char* yeelink_post(const char *device_id,const char *sensors_id, int value)
{
    char str_tmp[128] = {0};
    
    // Http内容，表单内容
    char http_content[32] = {0};


    memset(send_buf,0,CLOUD_SEND_BUFFER_SIZE);

    sprintf(str_tmp,"/v1.0/device/%s/sensor/%s/datapoints",device_id,sensors_id);
    // 确定HTTP表单提交内容 {"value":20}

    sprintf( http_content , "{\"value\":%d}" , (int)value);	


    // 确定 HTTP请求首部
    // 例如POST /v1.0/device/98d19569e0474e9abf6f075b8b5876b9/1/1/datapoints/add HTTP/1.1\r\n
    sprintf( send_buf , "POST %s HTTP/1.1\r\n",str_tmp);
    // 增加属性 例如 Host: api.machtalk.net\r\n
    sprintf( str_tmp , "Host:%s\r\n" , YEELINK_SERVER);
    strcat( send_buf , str_tmp);

    // 增加密码 例如 APIKey: d8a605daa5f4c8a3ad086151686dce64
    sprintf( str_tmp , "U-ApiKey:%s\r\n" , YEELINK_APIKEY);//自己的APIKey
    strcat( send_buf , str_tmp);
    //
    strcat( send_buf , "Accept: */*\r\n");
    // 增加提交表单内容的长度 例如 Content-Length:12\r\n
    sprintf( str_tmp , "Content-Length:%d\r\n" ,strlen(http_content) );
    strcat( send_buf , str_tmp);
    // 增加表单编码格式 Content-Type:application/x-www-form-urlencoded\r\n
    strcat( send_buf , "Content-Type: application/x-www-form-urlencoded\r\n");
    strcat( send_buf , "Connection: close\r\n");
    // HTTP首部和HTTP内容 分隔部分
    strcat( send_buf , "\r\n");
    // HTTP负载内容
    strcat( send_buf , http_content);

    return send_buf;
}



char* yeelink_get(const char *device_id,const char *sensors_id,char *value)
{
    char str_tmp[128] = {0};

    memset(send_buf,0,CLOUD_SEND_BUFFER_SIZE);

    sprintf(str_tmp,"/v1.0/device/%s/sensor/%s/datapoints",device_id,sensors_id);
    // 确定 HTTP请求首部
    // 例如POST /v1.0/device/98d19569e0474e9abf6f075b8b5876b9/1/1/datapoints/add HTTP/1.1\r\n
    sprintf( send_buf , "GET %s HTTP/1.1\r\n",str_tmp);
    // 增加属性 例如 Host: api.machtalk.net\r\n
    sprintf( str_tmp , "Host:%s\r\n" , YEELINK_SERVER);
    strcat( send_buf , str_tmp);
 
    // 增加密码 例如 APIKey: "af735029b2e431481dd0e6ef5bee6a27"
    sprintf( str_tmp , "U-ApiKey:%s\r\n" , YEELINK_APIKEY);//需要替换为自己的APIKey
    strcat( send_buf , str_tmp);
    //
    strcat( send_buf , "Accept: */*\r\n");
    // 增加表单编码格式 Content-Type:application/x-www-form-urlencoded\r\n
    strcat( send_buf , "Content-Type: application/x-www-form-urlencoded\r\n");
    strcat( send_buf , "Connection: close\r\n");
    // HTTP首部和HTTP内容 分隔部分
    strcat( send_buf , "\r\n");

    return send_buf;
}


VOID Yeelink_Run(VOID *arg)
{
   int ret;
   char *buffer = NULL;

   while(1){
	   if(TCPClient_reset_flag == 1)
	   {  
 	       //得到各个传感器数值
	   	   GetSensorData();

	       //粉尘浓度    376377
		   buffer = yeelink_post(AirTest_ID,Dust_ID,sensordata.Dust);
           ret = Socket_TCPClientSendData(buffer,strlen(buffer));

	       if (ret <= 0) {
	          Yeelink_Printf(GAGENT_WARNING,"Dust=>Send failed(ret:%d).\n", ret);
	       } else {
		      TCPClient_reset_flag = 2;
	          Yeelink_Printf(GAGENT_WARNING,"Dust=>Send success.\n");
	       }	
		   while(TCPClient_reset_flag != 1)
		      OSTimeDly(100);


		   //室内温度    376378
		   buffer = yeelink_post(AirTest_ID,Temp_ID,sensordata.Temperature);
           ret = Socket_TCPClientSendData(buffer,strlen(buffer));
	       if (ret <= 0) {
	          Yeelink_Printf(GAGENT_WARNING,"Temperature=>Send data failed(ret:%d).\n", ret);
	       } else {
		   	  TCPClient_reset_flag = 2;
	          Yeelink_Printf(GAGENT_WARNING,"Temperature=>Send data success.\n");
	       }		
		   while(TCPClient_reset_flag != 1)
		      OSTimeDly(10);
		   OSTimeDly(100);	
	  
		  //甲醛浓度    376382
		   buffer = yeelink_post(AirTest_ID,Methanal_ID,sensordata.Methanal);
           ret = Socket_TCPClientSendData(buffer,strlen(buffer));
	       if (ret <= 0) {
	          Yeelink_Printf(GAGENT_WARNING,"Methanal=>Send data failed(ret:%d).\n", ret);
	       } else {	
		   	  TCPClient_reset_flag = 2;
	          Yeelink_Printf(GAGENT_WARNING,"Methanal=>Send data success.\n");
	       }		
		   while(TCPClient_reset_flag != 1)
		      OSTimeDly(100);

		  //室内湿度    376383
		   buffer = yeelink_post(AirTest_ID,Humidity_ID,(int)sensordata.Humidity);
           ret = Socket_TCPClientSendData(buffer,strlen(buffer));
	       if (ret <= 0) {
	          Yeelink_Printf(GAGENT_WARNING,"Humidity=>Send data failed(ret:%d).\n", ret);
	       } else {
			  TCPClient_reset_flag = 2;
	          Yeelink_Printf(GAGENT_WARNING,"Humidity=>Send data success.\n");
	       }		
		   while(TCPClient_reset_flag != 1)
		      OSTimeDly(100);

		  //空气质量指数 376376
		   buffer = yeelink_post(AirTest_ID,AQI_ID,sensordata.AQI);
           ret = Socket_TCPClientSendData(buffer,strlen(buffer));
	       if (ret <= 0) {
	          Yeelink_Printf(GAGENT_WARNING,"AQI=>Send data failed(ret:%d).\n", ret);
	       } else {
		      TCPClient_reset_flag = 2;
	          Yeelink_Printf(GAGENT_WARNING,"AQI=>Send data success.\n");
	       }		
		   while(TCPClient_reset_flag != 1)
		      OSTimeDly(100);
	   }
  	   OSTimeDly(100); 	   
   }
}

//产生随机数函数
int make_rand(int start, int end)
{
	int value = 0;
	if (end <= start) 
		return -1;
	srand(OSTimeGet());
	value = ((rand() % (end - start + 1)) + start);

	return value;
}

AQI_DATA  aqi_data[7] = {0,     15.4,   0,   50,  //优
                         15.5,  40.4,  51,  100,  //良
						 40.5,  65.4,  101, 150,  //轻度污染
						 65.5,  150.4, 151, 200,  //中度污染
						 150.5, 250.4, 201, 300,  //重度污染
						 250.5, 350.4, 301, 400,  //严重污染
						 350.5, 500.4, 401, 500,  //有毒害     
						 };


//计算AQI值
static int Calculation_AQI(int _dust)
{
	int aqi;
	char i;
	float  dust = (float)_dust;

	for(i=0;i<7;i++)
	{
	   //判断污染浓度范围
	   if(aqi_data[i].C_LOW <= dust && dust <= aqi_data[i].C_HIGH)
	   {
	       aqi = (int)((aqi_data[i].I_HIGH - aqi_data[i].I_LOW)/(aqi_data[i].C_HIGH - aqi_data[i].C_LOW)*(dust - aqi_data[i].C_LOW));

		   aqi += aqi_data[i].I_LOW;
	   
	       return aqi;
	   }
	}

	return 	aqi_data[6].I_HIGH;
}

//得到粉尘浓度
static int GetDustData(void)
{
	int Dust;
	
	Dust = make_rand(5, 500);
	Yeelink_Printf(GAGENT_WARNING, "Dust:%d\n", Dust);
	
	return Dust;
}

//得到室内温度
static int GetTemData(void)
{
    int temp;

	temp = make_rand(-20, 40);
    Yeelink_Printf(GAGENT_WARNING, "Temperature:%d\n", temp);
	
	return temp;
}
//得到室内湿度
static int GetHumData(void)
{
    int Hum;

	Hum = make_rand(0, 100);
	Yeelink_Printf(GAGENT_WARNING, "Humidity:%d\n", Hum);
	
	return Hum;
}


//得到甲醛浓度
static int GetMethanalData(void)
{
    int methanal;

	methanal = make_rand(1, 1000);
    Yeelink_Printf(GAGENT_WARNING, "Methanal:%d\n", methanal);
	
	return methanal;
}

//空气质量AQI
static int GetAQIData(void)
{
    int aqi;

	aqi = Calculation_AQI(sensordata.Dust);
	Yeelink_Printf(GAGENT_WARNING,"AQI:%d\n", aqi);
	
	return aqi;
}

//得到传感器数据
int GetSensorData(void)
{
    sensordata.Humidity = GetHumData();
    sensordata.Dust = GetDustData();
	sensordata.Temperature = GetTemData();
	sensordata.Methanal = GetMethanalData();
	sensordata.AQI = GetAQIData();

    return 0;
}







