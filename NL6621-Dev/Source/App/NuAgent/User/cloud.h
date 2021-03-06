/*
 * =====================================================================================
 *
 *       Filename:  cloud.h
 *
 *    Description:  Head file of NuAgent cloud network
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
#ifndef __CLOUD_H__
#define __CLOUD_H__

#define CLOUD_RECV_BUFFER_SIZE		(1024)
#define CLOUD_SEND_BUFFER_SIZE		(512)

void Agent_cloud_process(void);
void Agent_cloud_init(void);

#endif

