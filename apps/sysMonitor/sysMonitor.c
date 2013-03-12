#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <public.h>
#include <boardapi.h>

#include "sm2dbsMutex.h"
#include "sysMonitor.h"
#include "at91ButtonProcessor.h"
#include "sysindiProcessor.h"
#include "sysledProcessor.h"
#include "wdtProcessor.h"
#include "atmProcessor.h"

/********************************************************************************************
*	函数名称:sysMonitorSignalHandler
*	函数功能:模块异常处理句柄函数
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void sysMonitorSignalHandler(int n)
{
	printf("sysMonitorSignalHandler : module sysMonitor exit !\n");
	dbs_mutex_sys_log(DBS_LOG_INFO, "sysMonitorSignalHandler : module sysMonitor exit");
	destroy_systemStatusLock();
	destroy_sm2dbs();
	exit(0);
}

int main(void)
{	
	int ret = 0;
	int fd = 0;
#ifdef __AT30TK175STK__
	pthread_t thread_id[5];
#else
	pthread_t thread_id[4];
#endif		

	/*创建与数据库模块互斥通讯的外部SOCKET接口*/
	if( CMM_SUCCESS != init_sm2dbs(MID_SYSMONITOR) )
	{
		fprintf(stderr,"ERROR: sysMonitor->init_sm2dbs error, exited !\n");
		return CMM_CREATE_SOCKET_ERROR;
	}
	
	/* 初始化g_systemStatus 互斥访问的互斥锁*/
	init_systemStatusLock();

	/* 注册异常退出句柄函数*/
	signal(SIGTERM, sysMonitorSignalHandler);

	/* Waiting for all modus init */
	dbsMutexWaitModule(MF_MMEAD|MF_ALARM|MF_TM|MF_CMM|MF_REGI);
	//dbsWaitModule(MF_ALARM|MF_CMM);	

	/* 按键检测线程*/
	ret = pthread_create( &thread_id[0], NULL, (void *)at91ButtonProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread at91buttonProcessor\n");
		dbs_mutex_sys_log(DBS_LOG_ERR, "Cannot create thread at91buttonProcessor");
		destroy_systemStatusLock();
		destroy_sm2dbs();
		return CMM_FAILED;
	}
	
	/* 系统灯管理线程*/
	ret = pthread_create( &thread_id[1], NULL, (void *)sysledProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread sysledProcessor\n");
		dbs_mutex_sys_log(DBS_LOG_ERR, "Cannot create thread sysledProcessor");
		destroy_systemStatusLock();
		destroy_sm2dbs();
		return CMM_FAILED;
	}

	/* WDT 监管线程*/
	ret = pthread_create( &thread_id[2], NULL, (void *)wdtProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread wdtProcessor\n");
		dbs_mutex_sys_log(DBS_LOG_ERR, "Cannot create thread wdtProcessor");
		destroy_systemStatusLock();
		destroy_sm2dbs();
		return CMM_FAILED;
	}

	/* 外部请求消息响应线程*/
	ret = pthread_create( &thread_id[3], NULL, (void *)sysindiProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread sysindiProcessor\n");
		dbs_mutex_sys_log(DBS_LOG_ERR, "Cannot create thread sysindiProcessor");
		destroy_systemStatusLock();
		destroy_sm2dbs();
		return CMM_FAILED;
	}
	
#ifdef __AT30TK175STK__
	/* 系统环境监控进程*/
	ret = pthread_create( &thread_id[4], NULL, (void *)atmProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread atmProcessor\n");
		dbs_mutex_sys_log(DBS_LOG_ERR, "Cannot create thread atmProcessor");
		destroy_systemStatusLock();
		destroy_sm2dbs();
		return CMM_FAILED;
	}
#endif	

#ifdef __AT30TK175STK__
	pthread_join( thread_id[4], NULL );
#endif
	pthread_join( thread_id[3], NULL );
	pthread_join( thread_id[2], NULL );
	pthread_join( thread_id[0], NULL );
	pthread_join( thread_id[1], NULL );	
	
	printf("module sysMonitor exit !\n");
	destroy_systemStatusLock();
	destroy_sm2dbs();
	return 0;
}


