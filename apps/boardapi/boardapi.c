#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <public.h>
#include "boardapi.h"

/********************************************************************************************
*	函数名称:boardapi_checkCpuEndian
*	函数功能:判断处理器的字节序是大端还是小端
*	return true: little-endian, return false: big-endian
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_checkCpuEndian(void)
{
	union w
	{
		int a;
		char b;
	}c;
	c.a = 1;
	return (1 == c.b);
}

/********************************************************************************************
*	函数名称:boardapi_getMacAddress
*	函数功能:获取CBAT的MAC地址
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getMacAddress(void)
{
	int fd = 0;
	static char sMAC[32] = {0};
	struct ifreq req;

	strcpy(req.ifr_name, "eth0");
	
	if( ( fd = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		perror("boardapi_getMacAddress socket error !\n");     
		strcpy(sMAC, "30:71:B2:00:00:00");
	}
	else if( ioctl(fd, SIOCGIFHWADDR, &req) < 0 )
	{
		perror("boardapi_getMacAddress ioctl error !\n");
		strcpy(sMAC, "30:71:B2:00:00:00");
	}
	else
	{
		sprintf(sMAC, "%02X:%02X:%02X:%02X:%02X:%02X", 
			req.ifr_hwaddr.sa_data[0], req.ifr_hwaddr.sa_data[1], req.ifr_hwaddr.sa_data[2], 
			req.ifr_hwaddr.sa_data[3], req.ifr_hwaddr.sa_data[4], req.ifr_hwaddr.sa_data[5]);
	}
	close(fd);
	return sMAC;
}

/********************************************************************************************
*	函数名称:boardapi_macs2b
*	函数功能:将字符串形式的MAC地址转换为6位二进制格式
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_macs2b(const char *str, uint8_t *bin)
{
	int result, i;
	unsigned int mac[6];
	
	if((str == NULL) || (bin == NULL))
	{
		return CMM_FAILED;
	}
	
	if( strlen(str) == 0 )
	{
		for(i = 0; i < 6; i++)
		{
			bin[i] = 0;
		}
		return CMM_SUCCESS;
	}

	if( 6 == sscanf(str,"%2X:%2X:%2X:%2X:%2X:%2X", mac + 0, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5) )
	{
		for(i = 0; i < 6; i++)
		{
			bin[i] = mac[i];
		}
		return CMM_SUCCESS;
	}
	else if( 6 == sscanf(str,"%2X-%2X-%2X-%2X-%2X-%2X", mac + 0, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5) )
	{
		for(i = 0; i < 6; i++)
		{
			bin[i] = mac[i];
		}
		return CMM_SUCCESS;
	}
	else if( 6 == sscanf(str,"%2X%2X%2X%2X%2X%2X", mac + 0, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5) )
	{
		for(i = 0; i < 6; i++)
		{
			bin[i] = mac[i];
		}
		return CMM_SUCCESS;
	}
	else
	{
		return CMM_FAILED;
	}	
}

/********************************************************************************************
*	函数名称:boardapi_getDeviceModelStr
*	函数功能:获取字符串表示的设备型号
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getDeviceModelStr(uint32_t model)
{
	switch(model)
	{
		case WEC_3501I_X7:
		{
			return "WEC-3501I X7";
		}
		case WEC_3501I_E31:
		{
			return "WEC-3501I E31";
		}
		case WEC_3501I_C22:
		{
			return "WEC-3501I C22";
		}
		case WEC_3501I_S220:
		{
			return "WEC-3501I S220";
		}
		case WEC_3501I_S60:
		{
			return "WEC-3501I S60";
		}
		case WEC_3501I_Q31:
		{
			return "WEC-3501I Q31";
		}
		case WEC_3601I:
		{
			return "WEC-3601I";
		}
		case WEC_3602I:
		{
			return "WEC-3602I";
		}
		case WEC_3604I:
		{
			return "WEC-3604I";
		}
		case WEC_3702I:
		{
			return "WEC-3702I L2";
		}
		case WEC_3703I:
		{
			return "WEC-3703I L3";
		}
		case WEC_3704I:
		{
			return "WEC-3704I L4";
		}
		case WEC_602:
		{
			return "WEC-3702I C2";
		}
		case WEC_604:
		{
			return "WEC-3702I C4";
		}
		case WEC9720EK_C22:
		{
			//return "WEC9720EK C22";
			return "EOC-MO350-2G";
		}
		case WEC9720EK_E31:
		{
			return "WEC9720EK E31";
		}
		case WEC9720EK_Q31:
		{
			return "WEC9720EK Q31";
		}
		case WEC9720EK_S220:
		{
			return "WEC9720EK S220";
		}
		case WEC9720EK_SD220:
		{
			return "WEC9720EK SD220";
		}
		case WEC701_C2:
		{
			return "WEC701 C2";
		}
		case WEC701_C4:
		{
			//return "WEC701 C4";
			return "EOC-S100-4F";
		}
		case WEC_3501I_XD25:
		{
			return "WEC-3501I XD25";
		}
		case WEC9720EK_XD25:
		{
			return "WEC9720EK XD25";
		}
		case WR1004JL:
		{
			return "WR1004JL";
		}
		case WR1004SJL:
		{
			return "WR1004SJL";
		}
		default:
		{
			return "UNKNOWN";
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_getCnuHfid
*	函数功能:根据设备型号获取烧录在PIB中存储的HFID
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
const char *boardapi_getCnuHfid(uint32_t devType)
{
	const char *user_HFID = NULL;
	
	switch(devType)
	{
		case WEC_3602I:
			user_HFID = "WEC-3602I";
			break;
		case WEC_3702I:
			user_HFID = "WEC-3702I";
			break;
		case WEC_3703I:
			user_HFID = "WEC-3703I";
			break;
		case WEC_602:
			user_HFID = "WEC-602";
			break;
		case WEC_604:
			user_HFID = "WEC-604";
			break;
		case WEC701_C2:
			user_HFID = "WEC701-C2";
			break;
		case WEC701_C4:
			user_HFID = "WEC701-C4";
			break;
		default :
			user_HFID = "Intellon Enabled Product";
			break;
	}
	return user_HFID;
}

/********************************************************************************************
*	函数名称:boardapi_isCnuSupported
*	函数功能:根据输入的设备型号判断系统是否支持该设备
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_isCnuSupported(uint32_t DevType)
{
	switch(DevType)
	{
		case WEC_3702I:		/* WEC-3702I L2 */
		case WEC_3703I:		/* WEC-3702I L3 */
		case WEC_602:		/* WEC-3702I C2 */
		case WEC_604:		/* WEC-3702I C4 */
		case WEC701_C2:		/* WEC701 C2 */
		case WEC701_C4:		/* WEC701 C4 */
		{
			return BOOL_TRUE;
		}		
		default:
			return BOOL_FALSE;
	}
}

/********************************************************************************************
*	函数名称:boardapi_getModNameStr
*	函数功能:根据模块ID获取字符串表示的模块名称
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getModNameStr(uint16_t mid)
{
	switch(mid)
	{
		case MID_SNMP:
		{
			return "MID_SNMP";
		}
		case MID_CLI:
		{
			return "MID_CLI";
		}
		case MID_HTTP:
		{
			return "MID_HTTP";
		}
		case MID_CMM:
		{
			return "MID_CMM";
		}
		case MID_ALARM:
		{
			return "MID_ALARM";
		}
		case MID_LLOG:
		{
			return "MID_LLOG";
		}
		case MID_DBA:
		{
			return "MID_DBA";
		}
		case MID_REGISTER:
		{
			return "MID_REG";
		}
		case MID_MMEAD:
		{
			return "MID_MMEAD";
		}
		case MID_SQL:
		{
			return "MID_SQL";
		}
		case MID_TEMPLATE:
		{
			return "MID_TM";
		}
		case MID_DBS:
		{
			return "MID_DBS";
		}
		case MID_DBS_TESTER:
		{
			return "DBS_TESTER";
		}
		case MID_SYSMONITOR:
		{
			return "MID_MON";
		}
		case MID_TM_TESTER:
		{
			return "TM_TESTER";
		}
		case MID_SYSEVENT:
		{
			return "MID_EVENT";
		}
		case MID_DSDT_TESTER:
		{
			return "DSDT_TESTER";
		}
		case MID_AT91BTN:
		{
			return "T_AT91BTN";
		}
		case MID_ATM:
		{
			return "T_ATM";
		}
		case MID_SYSINDI:
		{
			return "T_SYSINDI";
		}
		case MID_SYSLED:
		{
			return "T_SYSLED";
		}
		case MID_WDTIMER:
		{
			return "T_WDT";
		}		
		default:
		{
			return "MID_OTHER";
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_mapDevModel
*	函数功能:将CBAT中定义的设备型号印射为NMS定义的设备型号
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_mapDevModel(int model)
{
	switch(model)
	{
		case WEC_3501I_X7:
		{
			return 1;
		}
		case WEC_3501I_E31:
		{
			return 2;
		}
		case WEC_3501I_Q31:
		{
			return 3;
		}
		case WEC_3501I_C22:
		{
			return 4;
		}
		case WEC_3501I_S220:
		{
			return 5;
		}
		case WEC_3501I_S60:
		{
			return 6;
		}
		case WEC_3702I:
		{
			return 7;
		}
		case WEC_3703I:
		{
			return 8;
		}
		case WEC_602:
		{
			return 9;
		}
		case WEC_604:
		{
			return 10;
		}
		case WEC_3801I:
		{
			return 11;
		}
		case WEC_3501I_XD25:
		{
			return 12;
		}
		case WEC9720EK_C22:
		{
			return 20;
		}
		case WEC9720EK_E31:
		{
			return 21;
		}
		case WEC9720EK_Q31:
		{
			return 22;
		}
		case WEC9720EK_S220:
		{
			return 23;
		}
		case WEC9720EK_SD220:
		{
			return 24;
		}
		case WEC9720EK_XD25:
		{
			return 25;
		}
		case WR1004JL:
		{
			return 26;
		}
		case WR1004SJL:
		{
			return 27;
		}
		case WEC701_M0:
		{
			return 36;
		}
		case WEC701_C2:
		{
			return 40;
		}
		case WEC701_C4:
		{
			return 41;
		}
		default:
		{
			return 256;
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_umapDevModel
*	函数功能:将NMS定义的设备型号印射为CBAT中定义的设备型号
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_umapDevModel(int model)
{
	switch(model)
	{
		case 1:
		{
			return WEC_3501I_X7;
		}
		case 2:
		{
			return WEC_3501I_E31;
		}
		case 3:
		{
			return WEC_3501I_Q31;
		}
		case 4:
		{
			return WEC_3501I_C22;
		}
		case 5:
		{
			return WEC_3501I_S220;
		}
		case 6:
		{
			return WEC_3501I_S60;
		}
		case 7:
		{
			return WEC_3702I;
		}
		case 8:
		{
			return WEC_3703I;
		}
		case 9:
		{
			return WEC_602;
		}
		case 10:
		{
			return WEC_604;
		}
		case 11:
		{
			return WEC_3801I;
		}
		case 12:
		{
			return WEC_3501I_XD25;
		}
		case 20:
		{
			return WEC9720EK_C22;
		}
		case 21:
		{
			return WEC9720EK_E31;
		}
		case 22:
		{
			return WEC9720EK_Q31;
		}
		case 23:
		{
			return WEC9720EK_S220;
		}
		case 24:
		{
			return WEC9720EK_SD220;
		}
		case 25:
		{
			return WEC9720EK_XD25;
		}
		case 26:
		{
			return WR1004JL;
		}
		case 27:
		{
			return WR1004SJL;
		}
		case 36:
		{
			return WEC701_M0;
		}
		case 40:
		{
			return WEC701_C2;
		}
		case 41:
		{
			return WEC701_C4;
		}
		default:
		{
			return WEC_INVALID;
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_getAlarmTypeStr
*	函数功能:获取字符串表示的告警类型
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getAlarmTypeStr(uint16_t alarmType)
{
	switch(alarmType)
	{
		case ALARM_STS_NOMINAL:
		{
			return "aasNominal";
		}
		case ALARM_STS_HIHI:
		{
			return "aasHIHI";
		}
		case ALARM_STS_HI:
		{
			return "aasHI";
		}
		case ALARM_STS_LO:
		{
			return "aasLO";
		}
		case ALARM_STS_LOLO:
		{
			return "aasLOLO";
		}
		case ALARM_STS_MAJOR:
		{
			return "MAJOR";
		}
		case ALARM_STS_MINOR:
		{
			return "MINOR";
		}		
		default:
		{
			return "aasNominal";
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_getAlarmLevelByCode
*	函数功能:根据告警码获取该告警的等级
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_getAlarmLevelByCode(uint32_t alarmCode)
{
	switch(alarmCode)
	{
		/*Emergency*/
		case 200903:		/* 环境温度告警*/
		case 200906:		/* 噪声过高告警*/
		case 200907:		/* 链路衰减告警*/
		case 200908:		/* 物理层速率告警*/				
		case 200923:		/* can not find clt */
		{
			return DBS_LOG_EMERG;
		}
		/*DBS_LOG_ALERT*/
		case 200904:		/* CBAT管理CPU负载过高告警以及恢复*/
		case 200905:		/* CBAT内存利用率过高告警*/
		case 200921:		/* CBAT down */
		{
			return DBS_LOG_ALERT;
		}
		/*DBS_LOG_CRIT*/
		case 200915:		/* auto-config pib */
		case 200916:		/* auto-config mod */
		{
			return DBS_LOG_CRIT;
		}
		/*DBS_LOG_ERR*/
		case 200910:		/* 自动配置失败事件*/
		case 200911:		/* INDEX重复告警以及恢复*/
		{
			return DBS_LOG_ERR;
		}
		/*DBS_LOG_WARNING*/
		case 200912:		/* 非法CNU注册告警*/
		case 200913:		/* CNU用户数量超限告警*/
		case 200917:		/* abort auto config */
		case 200922:		/* clt heartbeat loss */
		{
			return DBS_LOG_WARNING;
		}
		/*DBS_LOG_NOTICE*/
		case 200909:		/*  升级状态告警标识OID */
		case 200914:		/* block cnu */
		case 200918:		/* kick off cnu */
		case 200919:		/* force re-registration */
		case 200920:		/* CBAT ColdStart */
		{
			return DBS_LOG_NOTICE;
		}
		/*DBS_LOG_INFO*/
		case 200901:		/* CLT上下线*/
		case 200902:		/* CNU上下线*/
		case 200000:		/* 心跳TRAP-CBAT状态*/
		case 200001:		/* 心跳TRAP-CNU状态*/
		{
			return DBS_LOG_INFO;
		}
		/*DBS_LOG_DEBUG*/
		default:
		{
			return DBS_LOG_DEBUG;
		}
	}
}

/********************************************************************************************
*	函数名称:boardapi_setMTParameters
*	函数功能:烧录NVM参数的接口函数
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_setMTParameters(stMTmsgInfo *para)
{
	char mac[6];

	/* parse mac address */
	if( CMM_SUCCESS != boardapi_macs2b(para->mac, mac) )
	{
		printf("	ERROR: boardapi_setMTParameters mac address invalid\n");
		return CMM_FAILED;
	}
	else
	{
		sprintf(para->mac, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		return nvm_set_mt_parameters(para);
	}	
} 
