#include <stdio.h>
#include <string.h>
#include <dbsapi.h>
#include "http2dbs.h"
#include <boardapi.h>

#if 0
/***********************************************************/
/*代替数据库用的临时存储变量，调试阶段*/
/*过后需要全部取消，用数据库操作来替代*/
/***********************************************************/
static char sg_wecIpaddr[16] = "192.168.223.1";
static char sg_wecNetmask[16] = "255.255.255.0";
static char sg_wecDefaultGw[16] = "0.0.0.0";
static int sg_wecMgmtVlanSts = 0;
static int sg_wecMgmtVlanId = 1;
static char sg_snmpRoCommunity[64] = "public";
static char sg_snmpRwCommunity[64] = "private";
static char sg_snmpTrapIpaddr[16] = "192.168.223.254";
static int sg_snmpTrapDport = 162;

static int sg_devSerials = 1;
static int sg_devModel = 1;
static int sg_eocType = 1;
static int sg_cltNumber = 2;
static int sg_cnuStations = 128;
static int sg_wlistStatus = 1;
static int sg_wdtStatus = 1;
static int sg_flashSize = 8;
static int sg_sdramSize = 32;
static char sg_hwVersion[64] = "Atmel AT91SAM9G20-EK";
static char sg_bootVersion[64] = "V1.2.0";
static char sg_kernelVersion[64] = "Linux version 2.6.27";
static char sg_appVersion[64] = "V1.6.4.0-CR14";
static char sg_manufactory[128] = "Hangzhou Prevail Optoelectronic Equipment Co.,LTD";

/***********************************************************/
/***********************************************************/
#endif

int http2dbs_getCnuIndexByMacaddress(char *mac, stCnuNode *index)
{
	return dbsSelectCnuIndexByMacAddress(mac, index);
}

int http2dbs_getProfile(uint16_t id, st_dbsProfile * profile)
{
	return dbsGetProfile(id, profile);
}

int http2dbs_setProfile(uint16_t id, st_dbsProfile * profile)
{
	return dbsUpdateProfile(id, profile);
}

int http2dbs_getCnu(uint16_t id, st_dbsCnu * cnu)
{
	return dbsGetCnu(id, cnu);
}

int http2dbs_doCltAgTimeSettings(PWEB_NTWK_VAR pWebVar)
{
	int flag = 0;
	st_dbsCltConf row;

	if( CMM_SUCCESS != dbsGetCltconf(pWebVar->cltid, &row) )
	{
		return CMM_FAILED;
	}

	if( row.col_loagTime != pWebVar->col_loagTime )
	{
		row.col_loagTime = pWebVar->col_loagTime;
		flag++;
	}
	if( row.col_reagTime != pWebVar->col_reagTime )
	{
		row.col_reagTime = pWebVar->col_reagTime;
		flag++;
	}
	
	if( flag )
	{
		return dbsUpdateCltconf(pWebVar->cltid, &row);
	}
	else
	{
		return CMM_SUCCESS;
	}
}

int http2dbs_doCltDecapSettings(PWEB_NTWK_VAR pWebVar)
{
	int flag = 0;
	st_dbsCltConf row;

	if( CMM_SUCCESS != dbsGetCltconf(pWebVar->cltid, &row) )
	{
		return CMM_FAILED;
	}

	if( row.col_igmpPri != pWebVar->col_igmpPri )
	{
		row.col_igmpPri = pWebVar->col_igmpPri;
		flag++;
	}
	if( row.col_unicastPri != pWebVar->col_unicastPri )
	{
		row.col_unicastPri = pWebVar->col_unicastPri;
		flag++;
	}
	if( row.col_avsPri != pWebVar->col_avsPri )
	{
		row.col_avsPri = pWebVar->col_avsPri;
		flag++;
	}
	if( row.col_mcastPri != pWebVar->col_mcastPri )
	{
		row.col_mcastPri = pWebVar->col_mcastPri;
		flag++;
	}
	
	if( flag )
	{
		return dbsUpdateCltconf(pWebVar->cltid, &row);
	}
	else
	{
		return CMM_SUCCESS;
	}
}

int http2dbs_doCltQosSettings(PWEB_NTWK_VAR pWebVar)
{
	//int flag = 0;
	st_dbsCltConf row;

	if( CMM_SUCCESS != dbsGetCltconf(pWebVar->cltid, &row) )
	{
		return CMM_FAILED;
	}

	/* 禁用QoS */
	if( 0 == pWebVar->col_tbaPriSts )
	{
		row.col_tbaPriSts = 0;
		
		row.col_cosPriSts = 1;
		row.col_cos0pri = 1;
		row.col_cos1pri = 0;
		row.col_cos2pri = 0;
		row.col_cos3pri = 1;
		row.col_cos4pri = 2;
		row.col_cos5pri = 2;
		row.col_cos6pri = 3;
		row.col_cos7pri = 3;

		row.col_tosPriSts = 0;
		row.col_tos0pri = 1;
		row.col_tos1pri = 0;
		row.col_tos2pri = 0;
		row.col_tos3pri = 1;
		row.col_tos4pri = 2;
		row.col_tos5pri = 2;
		row.col_tos6pri = 3;
		row.col_tos7pri = 3;
	}
	else
	{
		if( 1 == pWebVar->col_cosPriSts )
		{
			row.col_tbaPriSts = 1;
		
			row.col_cosPriSts = 1;
			row.col_cos0pri = pWebVar->col_cos0pri;
			row.col_cos1pri = pWebVar->col_cos1pri;
			row.col_cos2pri = pWebVar->col_cos2pri;
			row.col_cos3pri = pWebVar->col_cos3pri;
			row.col_cos4pri = pWebVar->col_cos4pri;
			row.col_cos5pri = pWebVar->col_cos5pri;
			row.col_cos6pri = pWebVar->col_cos6pri;
			row.col_cos7pri = pWebVar->col_cos7pri;

			row.col_tosPriSts = 0;
			row.col_tos0pri = 1;
			row.col_tos1pri = 0;
			row.col_tos2pri = 0;
			row.col_tos3pri = 1;
			row.col_tos4pri = 2;
			row.col_tos5pri = 2;
			row.col_tos6pri = 3;
			row.col_tos7pri = 3;
		}
		else if( 1 == pWebVar->col_tosPriSts )
		{
			row.col_tbaPriSts = 1;

			row.col_cosPriSts = 0;
			row.col_cos0pri = 1;
			row.col_cos1pri = 0;
			row.col_cos2pri = 0;
			row.col_cos3pri = 1;
			row.col_cos4pri = 2;
			row.col_cos5pri = 2;
			row.col_cos6pri = 3;
			row.col_cos7pri = 3;
		
			row.col_tosPriSts = 1;
			row.col_tos0pri = pWebVar->col_tos0pri;
			row.col_tos1pri = pWebVar->col_tos1pri;
			row.col_tos2pri = pWebVar->col_tos2pri;
			row.col_tos3pri = pWebVar->col_tos3pri;
			row.col_tos4pri = pWebVar->col_tos4pri;
			row.col_tos5pri = pWebVar->col_tos5pri;
			row.col_tos6pri = pWebVar->col_tos6pri;
			row.col_tos7pri = pWebVar->col_tos7pri;			
		}
		else
		{
			row.col_tbaPriSts = 0;
		
			row.col_cosPriSts = 1;
			row.col_cos0pri = 1;
			row.col_cos1pri = 0;
			row.col_cos2pri = 0;
			row.col_cos3pri = 1;
			row.col_cos4pri = 2;
			row.col_cos5pri = 2;
			row.col_cos6pri = 3;
			row.col_cos7pri = 3;

			row.col_tosPriSts = 0;
			row.col_tos0pri = 1;
			row.col_tos1pri = 0;
			row.col_tos2pri = 0;
			row.col_tos3pri = 1;
			row.col_tos4pri = 2;
			row.col_tos5pri = 2;
			row.col_tos6pri = 3;
			row.col_tos7pri = 3;
		}
	}

	return dbsUpdateCltconf(pWebVar->cltid, &row);
}

int http2dbs_getWebAdminPwd(char *varValue)
{
	st_dbsCliRole row;

	if( CMM_SUCCESS != dbsGetCliRole(4, &row) )
	{
		sprintf(varValue, "%s", "admin");
	}
	else
	{
		sprintf(varValue, "%s", row.col_pwd);		
	}
	return CMM_SUCCESS;
}

int http2dbs_setWebAdminPwd(PWEB_NTWK_VAR pWebVar)
{
	st_dbsCliRole row;

	row.id = 4;
	strcpy((char *)row.col_user, "admin");
	strcpy((char *)row.col_pwd, (const char *)(pWebVar->sysPassword));
	return dbsUpdateCliRole(4, &row);
}

int http2dbs_getDevSerials(char *varValue)
{
	strcpy(varValue, "WECxx EoC CBAT");
	return CMM_SUCCESS;
}

int http2dbs_getDevModel(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(1, &row) )
	{
		return CMM_FAILED;
	}

	strcpy(varValue, boardapi_getDeviceModelStr(row.col_model));
	
	return CMM_SUCCESS;
}

int http2dbs_getEocType(char *varValue)
{
	strcpy(varValue, "AR7410");
	return CMM_SUCCESS;
}

int http2dbs_getCltNumber(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_maxclt);
		return CMM_SUCCESS;
	}
}

int http2dbs_getCnuStations(char *varValue)
{
	sprintf(varValue, "%d", MAX_CNU_AMOUNT_LIMIT);
	return 0;
}

int http2dbs_getWlistStatus(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_wlctl);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWdtStatus(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_wdt);
		return CMM_SUCCESS;
	}
}

int http2dbs_getFlashSize(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_flashsize);
		return CMM_SUCCESS;
	}
}

int http2dbs_getSdramSize(char *varValue)
{	
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_ramsize);
		return CMM_SUCCESS;
	}
}

int http2dbs_getHwVersion(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_hwver);
		return CMM_SUCCESS;
	}
}

int http2dbs_getBootVersion(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_bver);
		return CMM_SUCCESS;
	}
}

int http2dbs_getKernelVersion(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_kver);
		return CMM_SUCCESS;
	}
}

int http2dbs_getAppVersion(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_appver);
		return CMM_SUCCESS;
	}
}

int http2dbs_getManufactory(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_mfinfo);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWecIpaddr(char *varValue)
{
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_ip);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWecNetmask(char *varValue)
{
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_netmask);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWecDefaultGw(char *varValue)
{	
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_gw);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWecMgmtVlanSts(char *varValue)
{
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_mvlan_sts);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWecMgmtVlanId(char *varValue)
{
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_mvlan_id);
		return CMM_SUCCESS;
	}
}

int http2dbs_getSnmpRoCommunity(char *varValue)
{
	st_dbsSnmp row;

	if( CMM_SUCCESS != dbsGetSnmp(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_rdcom);
		return CMM_SUCCESS;
	}
}

int http2dbs_getSnmpRwCommunity(char *varValue)
{
	st_dbsSnmp row;

	if( CMM_SUCCESS != dbsGetSnmp(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_wrcom);
		return CMM_SUCCESS;
	}
}

int http2dbs_getSnmpTrapIpaddr(char *varValue)
{
	st_dbsSnmp row;

	if( CMM_SUCCESS != dbsGetSnmp(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_sina);
		return CMM_SUCCESS;
	}
}

int http2dbs_getSnmpTrapDport(char *varValue)
{
	st_dbsSnmp row;

	if( CMM_SUCCESS != dbsGetSnmp(1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_tpa);
		return CMM_SUCCESS;
	}
}

int http2dbs_setWecNetworkConfig(PWEB_NTWK_VAR pWebVar)
{
	int flag = 0;
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(1, &row) )
	{
		return CMM_FAILED;
	}

	if( strcmp((const char *)row.col_ip, (const char *)pWebVar->wecIpaddr) != 0 )
	{
		strcpy((char *)row.col_ip, (const char *)pWebVar->wecIpaddr);
		flag++;
	}
	if( strcmp((const char *)row.col_netmask, (const char *)pWebVar->wecNetmask) != 0 )
	{
		strcpy((char *)row.col_netmask, (const char *)pWebVar->wecNetmask);
		flag++;
	}
	if( strcmp((const char *)row.col_gw, (const char *)pWebVar->wecDefaultGw) != 0 )
	{
		strcpy((char *)row.col_gw, (const char *)pWebVar->wecDefaultGw);
		flag++;
	}
	if( row.col_mvlan_sts != pWebVar->wecMgmtVlanSts )
	{
		row.col_mvlan_sts = pWebVar->wecMgmtVlanSts;
		flag++;
	}
	if( row.col_mvlan_id != pWebVar->wecMgmtVlanId )
	{
		row.col_mvlan_id = pWebVar->wecMgmtVlanId;
		flag++;
	}
	if( flag )
	{
		return dbsUpdateNetwork(1, &row);
	}
	else
	{
		return CMM_SUCCESS;
	}
}

int http2dbs_setSnmpConfig(PWEB_NTWK_VAR pWebVar)
{
	int flag = 0;
	st_dbsSnmp row;

	if( CMM_SUCCESS != dbsGetSnmp(1, &row) )
	{
		return CMM_FAILED;
	}
	
	if( strcmp((const char *)row.col_rdcom, (const char *)pWebVar->snmpRoCommunity) != 0 )
	{
		strcpy((char *)row.col_rdcom, (const char *)pWebVar->snmpRoCommunity);
		flag++;
	}
	if( strcmp((const char *)row.col_wrcom, (const char *)pWebVar->snmpRwCommunity) != 0 )
	{
		strcpy((char *)row.col_wrcom, (const char *)pWebVar->snmpRwCommunity);
		flag++;
	}
	if( strcmp((const char *)row.col_sina, (const char *)pWebVar->snmpTrapIpaddr) != 0 )
	{
		strcpy((char *)row.col_sina, (const char *)pWebVar->snmpTrapIpaddr);
		flag++;
	}
	if( row.col_tpa != pWebVar->snmpTrapDport )
	{
		row.col_tpa = pWebVar->snmpTrapDport;
		flag++;
	}
	if( flag )
	{
		return dbsUpdateSnmp(1, &row);
	}
	else
	{
		return CMM_SUCCESS;
	}
}

int http2dbs_setWebAdminPasswd(char *varValue)
{
	return CMM_FAILED;
}

int http2dbs_setCliAdminPasswd(char *varValue)
{
	st_dbsCliRole row;

	row.id = 1;
	strcpy((char *)row.col_user, "admin");
	strcpy((char *)row.col_pwd, (const char *)varValue);
	return dbsUpdateCliRole(1, &row);
}

int http2dbs_setCliOptPasswd(char *varValue)
{
	st_dbsCliRole row;

	row.id = 2;
	strcpy((char *)row.col_user, "operator");
	strcpy((char *)row.col_pwd, (const char *)varValue);
	return dbsUpdateCliRole(2, &row);
}

int http2dbs_setCliUserPasswd(char *varValue)
{
	st_dbsCliRole row;

	row.id = 3;
	strcpy((char *)row.col_user, "user");
	strcpy((char *)row.col_pwd, (const char *)varValue);
	return dbsUpdateCliRole(3, &row);
}

int http2dbs_getFtpIpaddr(char *varValue)
{
	st_dbsSwmgmt row;

	if( 0 != dbsGetSwmgmt(1, &row) )
	{
		return CMM_FAILED;
	}	
	strcpy(varValue, (const char *)row.col_ip);
	return CMM_SUCCESS;
}

int http2dbs_getFtpPort(char *varValue)
{
	st_dbsSwmgmt row;

	if( 0 != dbsGetSwmgmt(1, &row) )
	{
		return CMM_FAILED;
	}	
	sprintf(varValue, "%d", row.col_port);
	return CMM_SUCCESS;
}

int http2dbs_getFtpUser(char *varValue)
{
	st_dbsSwmgmt row;

	if( 0 != dbsGetSwmgmt(1, &row) )
	{
		return CMM_FAILED;
	}	
	strcpy(varValue, (const char *)row.col_user);
	return CMM_SUCCESS;
}

int http2dbs_getFtpPasswd(char *varValue)
{
	st_dbsSwmgmt row;

	if( 0 != dbsGetSwmgmt(1, &row) )
	{
		return CMM_FAILED;
	}	
	strcpy(varValue, (const char *)row.col_pwd);
	return CMM_SUCCESS;
}

int http2dbs_getFtpFilePath(char *varValue)
{
	st_dbsSwmgmt row;

	if( 0 != dbsGetSwmgmt(1, &row) )
	{
		return CMM_FAILED;
	}	
	strcpy(varValue, (const char *)row.col_path);
	return CMM_SUCCESS;
}

int http2dbs_getOptlog(int id, st_dbsOptlog *plog)
{
	return dbsGetOptlog(id, plog);
}

int http2dbs_writeOptlog(int status, char *msg)
{
	//int i = 0;
	time_t b_time;
	time(&b_time);
	st_dbsOptlog log;

	log.time = b_time;
	log.who = MID_HTTP;		/* MID_HTTP */
	strcpy((char *)log.cmd, (const char *)msg);
	log.level = DBS_LOG_INFO;	/* DBS_LOG_INFO */
	log.result = status?CMM_FAILED:CMM_SUCCESS;

	return dbs_opt_log(&log);
}

int http2dbs_getSyslog(uint32_t row, st_dbsSyslog *log)
{
	return dbsGetSyslog(row, log);
}

int http2dbs_writeSyslog(uint32_t priority, const char *log)
{
	return dbs_sys_log(priority, log);
}

int http2dbs_getAlarmlog(uint32_t row, st_dbsAlarmlog *log)
{
	return dbsGetAlarmlog(row, log);
}

int http2dbs_writeAlarmlog(st_dbsAlarmlog *log)
{
	return dbs_alarm_log(log);
}

int http2dbs_getSwmgmt(int id, st_dbsSwmgmt *pRow)
{
	return dbsGetSwmgmt(id, pRow);
}

int http2dbs_saveConfig(void)
{
	return dbsFflush();
}

int http2dbs_destroy(void)
{
	dbs_sys_log(DBS_LOG_INFO, "module httpd exit");
	dbsClose();
	return CMM_SUCCESS;
}

int http2dbs_init(void)
{
	if( 0 != dbsOpen(MID_HTTP) )
	{
		return CMM_FAILED;
	}
	else
	{
		dbs_sys_log(DBS_LOG_INFO, "starting module httpd success");
		printf("Starting module httpd		......		[OK]\n");
	}
	return CMM_SUCCESS;
} 

