/*
 * =====================================================================================
 *
 *       Filename:  cloud.c
 *
 *    Description:  cloud network 
 *
 *        Version:  0.0.1
 *        Created:  2015/7/1 14:12:55
 *       Revision:  none
 *
 *         Author:  Lin Hui (Link), linhui.568@163.com
 *   Organization:  Nufront
 *
 *--------------------------------------------------------------------------------------          
 *       ChangLog:
 *        version    Author      Date          Purpose
 *        0.0.1      Lin Hui    2015/7/1      
 *
 * =====================================================================================
 */
#include "common.h"
#include "cloud.h"

char * recv_buf = NULL;
char * send_buf = NULL;
extern unsigned char TCPClient_reset_flag;
extern XPG_GLOBALVAR g_Yeelink_GlobalVar; //Yeelink全局参数


void recv_data_process(char * buffer, int len)
{
    int presult;

    //判断是否收到HTTP OK
	presult = Search_str(buffer , "200 OK\r\n");
    if( presult != NULL ){
	 	presult = Search_str(buffer , "\"value\":");

		if(presult != NULL )
	    {
			if(buffer[presult] == '0'){
			   	 LED3_OFF();
			   	 g_Yeelink_GlobalVar.led_State = LED_OFF;
			}
			else if(buffer[presult] == '1') {
			     LED3_ON();  
				 g_Yeelink_GlobalVar.led_State = LED_ON;
			}		
		}
    }else{
        Yeelink_Printf(GAGENT_WARNING, "Http Response Error\r\n");
        Yeelink_Printf(GAGENT_WARNING, "%s",buffer);
    }
	      	    
    //判断是否收到HTTP OK
    TCPClient_reset_flag = 0xa5; //关闭链接  
}



void Agent_cloud_process(void)
{
	int ret;

	/* 主进程用于接收云端服务器的数据 */
	while (1)
	{
	    ret = Socket_TCPClientRecvData(recv_buf, CLOUD_RECV_BUFFER_SIZE);
		if (ret > 0) {
	     	recv_buf[ret] = '\0';
            recv_data_process(recv_buf, ret);
		} 
		if(TCPClient_reset_flag == 0xa5 || ret < 0 ){
		    Socket_TCPClientClose();
			break;
	    }		
	}
}

void Agent_cloud_init(void)
{
	/* 分配接收数据和发送数据的内存块 */
	recv_buf = malloc(CLOUD_RECV_BUFFER_SIZE);
	send_buf = malloc(CLOUD_SEND_BUFFER_SIZE);
}



