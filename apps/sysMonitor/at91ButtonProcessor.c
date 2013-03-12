#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <public.h>
#include "sm2dbsMutex.h"
#include "systemStsControl.h"
#include "sysMonitor2cmm.h"
#include "sysMonitor.h"
#include "at91ButtonProcessor.h"

static int at91btns_fd = 0;

void at91btnsSyncHandler(int args)
{
#if 0
	int tmp = 0;
	/* 通过系统调用获取系统状态*/
	if( ioctl(at91btns_fd, GET_KEY_STS_CMD, &tmp) >= 0 )
	{
		set_systemStatus(tmp);
		printf("-->at91btnsSyncHandler : set_systemStatus(%d)\n", get_systemStatus());
	}
#endif
}

int init_at91btns(void)
{
	int oflags;

	at91btns_fd =  open( "/dev/at91btns", O_RDWR );
	if( at91btns_fd > 0 )
	{
		signal( SIGIO, &at91btnsSyncHandler );
		fcntl( at91btns_fd, F_SETOWN, getpid() );
		oflags = fcntl( at91btns_fd, F_GETFL );
		fcntl( at91btns_fd, F_SETFL, oflags|FASYNC );	
		return CMM_SUCCESS;
	}
	else
	{
		printf("init_at91btns failed : cannot open device driver\n");
		return CMM_FAILED;
	}
}

void *at91ButtonProcessor(void)
{
	int ret;
	int key_event = 0;
	fd_set rds;
	T_UDP_SK_INFO myCmmSk;

	if( CMM_SUCCESS != init_at91btns() )
	{
		fprintf(stderr, "at91buttonProcessor init_at91btns failed\n");
		dbs_mutex_sys_log(DBS_LOG_ERR, "at91buttonProcessor init_at91btns failed");
		return (void *)0;
	}
	else if( CMM_SUCCESS != sysMonitor2cmm_init(&myCmmSk) )
	{
		fprintf(stderr, "at91buttonProcessor sysMonitor2cmm_init failed\n");
		dbs_mutex_sys_log(DBS_LOG_ERR, "at91buttonProcessor sysMonitor2cmm_init failed");
		return (void *)0;
	}

	//printf("sysMonitor->at91buttonProcessor start\n");
	fprintf(stderr, "Starting thread at91ButtonProcessor	......	[OK]\n");
	dbs_mutex_sys_log(DBS_LOG_INFO, "starting thread at91buttonProcessor success");

	for (;;)
	{
		FD_ZERO(&rds);
		FD_SET(at91btns_fd, &rds);

		ret = select(at91btns_fd + 1, &rds, NULL, NULL, NULL);
		if (ret < 0) {
			perror("APP: select error\n");
			continue;
		}
		else if (ret == 0) {
			/*
			 * Actually it never come here, since we
			 * don't set timeout value in select()
			 */
			perror("APP: timeout\n");
			continue;
		}
		else if (FD_ISSET(at91btns_fd, &rds))
		{
			ret = read(at91btns_fd, &key_event, sizeof(key_event));
			if (ret != sizeof(key_event))
			{
				if (errno != EAGAIN)
					perror("APP: read key");
				continue;
			}
			else
			{
				/* 在这里添加按键事件处理逻辑*/
				//printf("\nat91buttonProcessor: key_event = %d\n", key_event);
				switch(key_event)
				{
					case 1:
					{
						/* reset system */
						set_systemStatus(SYSLED_STS_RESET);
						printf("\nat91buttonProcessor key_event: reset system.\n");
						if( CMM_SUCCESS != sysMonitor2cmm_sysReboot(&myCmmSk) )
						{
							perror("at91buttonProcessor->sysMonitor2cmm_sysReboot failed!\n");
						}
						break;
					}
					case 2:
					{
						/* restore default */
						set_systemStatus(SYSLED_STS_RESET);
						printf("\nat91buttonProcessor key_event: restore default settings.\n");
						if( CMM_SUCCESS != sysMonitor2cmm_restoreDefault(&myCmmSk) )
						{
							perror("at91buttonProcessor->sysMonitor2cmm_restoreDefault failed!\n");
						}
						break;
					}
					case 3:
					{
						/* start safe mode *//* do not support this case yet */
						set_systemStatus(SYSLED_STS_NORMAL);
						printf("\nat91buttonProcessor key_event: start safe mode.\n");
						break;
					}
					default:
					{
						/* do nothing */
						set_systemStatus(SYSLED_STS_NORMAL);
						printf("\nat91buttonProcessor key_event: nothing to do.\n");
						break;
					}
				}
				continue;
			}
		}
	}
	
	sysMonitor2cmm_destroy(&myCmmSk);
	
	return (void *)0;
}

