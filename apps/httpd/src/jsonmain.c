#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#include "json.h"
//#include "parse_flags.h"
#include "cgimain.h"
#include "jsonmain.h"
#include "http2dbs.h"
#include "http2cmm.h"
#include <public.h>
#include <boardapi.h>

#define JSON_STRING_SIZE	1024

typedef struct
{
	int devType;				/* devType defined by NMS */
	char macAddr[32];		/* mac address string */
	int cnuPermit;			/* snmp true(1) or false(2) value */
	int cnuVlanSts;			/* snmp true(1) or false(2) value */
	int cnuEth1Vid;			/* 1~4094 */
	int cnuEth2Vid;			/* 1~4094 */
	int cnuEth3Vid;			/* 1~4094 */
	int cnuEth4Vid;			/* 1~4094 */
	int cnuTxRateLimitSts;	/* snmp true(1) or false(2) value */
	int cnuCpuPortTxRate;	/* times of 32Kb(0Kb~100*1024Kb). No rate limiting if rate is 0, 1 means 32Kb, and so on */
	int cnuEth1TxRate;		/* times of 32Kb(0Kb~100*1024Kb). No rate limiting if rate is 0 */
	int cnuEth2TxRate;		/* times of 32Kb(0Kb~100*1024Kb). No rate limiting if rate is 0 */
	int cnuEth3TxRate;		/* times of 32Kb(0Kb~100*1024Kb). No rate limiting if rate is 0 */
	int cnuEth4TxRate;		/* times of 32Kb(0Kb~100*1024Kb). No rate limiting if rate is 0 */
	int cnuRxRateLimitSts;	/* snmp true(1) or false(2) value */
	int cnuCpuPortRxRate;	/* times of 32Kb(0Kb~100*1024Kb). No rate limiting if rate is 0 */
	int cnuEth1RxRate;		/* times of 32Kb(0Kb~100*1024Kb). No rate limiting if rate is 0 */
	int cnuEth2RxRate;		/* times of 32Kb(0Kb~100*1024Kb). No rate limiting if rate is 0 */
	int cnuEth3RxRate;		/* times of 32Kb(0Kb~100*1024Kb). No rate limiting if rate is 0 */
	int cnuEth4RxRate;		/* times of 32Kb(0Kb~100*1024Kb). No rate limiting if rate is 0 */
	
}
JSON_VAR, *PJSON_VAR;

JSON_VAR glbJsonVar;

CGI_ITEM jsonSetTable[] = 
{
	//{ "type",			(void *)(&(glbJsonVar.devType)),			CGI_TYPE_NUM },
	{ "mac",			(void *)(  (glbJsonVar.macAddr)),			CGI_TYPE_STR },
	{ "permit",		(void *)(&(glbJsonVar.cnuPermit)),			CGI_TYPE_NUM },
	{ "vlanen",		(void *)(&(glbJsonVar.cnuVlanSts)),			CGI_TYPE_NUM },
	{ "vlan0id",		(void *)(&(glbJsonVar.cnuEth1Vid)),			CGI_TYPE_NUM },
	{ "vlan1id",		(void *)(&(glbJsonVar.cnuEth2Vid)),			CGI_TYPE_NUM },
	{ "vlan2id",		(void *)(&(glbJsonVar.cnuEth3Vid)),			CGI_TYPE_NUM },
	{ "vlan3id",		(void *)(&(glbJsonVar.cnuEth4Vid)),			CGI_TYPE_NUM },
	{ "txlimitsts",		(void *)(&(glbJsonVar.cnuTxRateLimitSts)),	CGI_TYPE_NUM },
	{ "cpuporttxrate",	(void *)(&(glbJsonVar.cnuCpuPortTxRate)),	CGI_TYPE_NUM },
	{ "port0txrate",	(void *)(&(glbJsonVar.cnuEth1TxRate)),		CGI_TYPE_NUM },
	{ "port1txrate",	(void *)(&(glbJsonVar.cnuEth2TxRate)),		CGI_TYPE_NUM },
	{ "port2txrate",	(void *)(&(glbJsonVar.cnuEth3TxRate)),		CGI_TYPE_NUM },
	{ "port3txrate",	(void *)(&(glbJsonVar.cnuEth4TxRate)),		CGI_TYPE_NUM },

	{ "rxlimitsts",		(void *)(&(glbJsonVar.cnuRxRateLimitSts)),	CGI_TYPE_NUM },
	{ "cpuportrxrate",	(void *)(&(glbJsonVar.cnuCpuPortRxRate)),	CGI_TYPE_NUM },
	{ "port0rxrate",	(void *)(&(glbJsonVar.cnuEth1RxRate)),		CGI_TYPE_NUM },
	{ "port1rxrate",	(void *)(&(glbJsonVar.cnuEth2RxRate)),		CGI_TYPE_NUM },
	{ "port2rxrate",	(void *)(&(glbJsonVar.cnuEth3RxRate)),		CGI_TYPE_NUM },
	{ "port3rxrate",	(void *)(&(glbJsonVar.cnuEth4RxRate)),		CGI_TYPE_NUM },

	{ NULL, NULL, CGI_TYPE_NONE }
};

void jsonSetVar(char *varName, char *varValue)
{
	int i = 0;

	for ( ; jsonSetTable[i].variable != NULL; i++ )
	{
		if ( strcmp(varName, jsonSetTable[i].variable) == 0 )
		{
			break;
		}
	}			

	if ( jsonSetTable[i].variable != NULL )
	{
		switch ( jsonSetTable[i].type )
		{
			case CGI_TYPE_STR:
			{
				strcpy((char *)jsonSetTable[i].value, varValue);
				break;
			}
			case CGI_TYPE_NUM:
			{
				*((int *)jsonSetTable[i].value) = atoi(varValue);
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

void jsonParseSet(char *jsonString)
{
	char name[32] = {0};
	char value[64] = {0};
	json_object *my_object;
	my_object = json_tokener_parse(jsonString);
	
	json_object_object_foreach(my_object, key, val)
	{
		strcpy(name, key);
		strcpy(value, json_object_get_string(val));
		//printf("\t%s: %s\n", name, value);
		cgiUrlDecode(value);
		jsonSetVar(name, value);		
	}	
	//printf("my_object.to_string()=%s\n", json_object_to_json_string(my_object));
	json_object_put(my_object);
}

static void jsonSendAck( FILE * fs, int status, const char* jsonString )
{
	time_t now;
	char timebuf[100];

	if( status == 0 )
	{
		(void) fprintf( fs, "%s %d %s\r\n", "HTTP/1.1", 200, "Ok" );
	}
	else
	{
		(void) fprintf( fs, "%s %d %s\r\n", "HTTP/1.1", 400, "Bad Request" );
	}
	
	(void) fprintf( fs, "Server: %s\r\n", "micro_httpd" );
	(void) fprintf( fs,"Cache-Control: no-cache\r\n") ;
	now = time( (time_t*) 0 );
	(void) strftime( timebuf, sizeof(timebuf), "%a, %d %b %Y %H:%M:%S GMT", gmtime( &now ) );
	(void) fprintf( fs, "Date: %s\r\n", timebuf );
	(void) fprintf( fs, "Content-Type: %s\r\n", "text/json" );
	(void) fprintf( fs, "Connection: close\r\n" );
	(void) fprintf( fs, "\r\n" );
	if( NULL != jsonString )
	{
		(void) fprintf( fs, jsonString );
	}
}

/* check input parameters , return 0: success; else failed */
int jsonSetCnuCheckInput(void)
{
	char bmac[6] = {0};
	uint8_t MA[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
	uint8_t MB[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	
	/* check if cnu mac address is valid*/
	if( CMM_SUCCESS != boardapi_macs2b(glbJsonVar.macAddr, bmac) )
		return CMM_FAILED;
	else if( memcmp(bmac, MA, 6) == 0 )
		return CMM_FAILED;
	else if( memcmp(bmac, MB, 6) == 0 )
		return CMM_FAILED;
	else
		sprintf(glbJsonVar.macAddr, "%02X:%02X:%02X:%02X:%02X:%02X", 
			bmac[0], bmac[1], bmac[2], bmac[3], bmac[4], bmac[5]
		);

	/* check port vlan id if vlan enable */
	if( 1 == glbJsonVar.cnuVlanSts )
	{
		if( (glbJsonVar.cnuEth1Vid < 1) || (glbJsonVar.cnuEth1Vid > 4094 ))
			return CMM_FAILED;
		if( (glbJsonVar.cnuEth2Vid < 1) || (glbJsonVar.cnuEth2Vid > 4094 ))
			return CMM_FAILED;
		if( (glbJsonVar.cnuEth3Vid < 1) || (glbJsonVar.cnuEth3Vid > 4094 ))
			return CMM_FAILED;
		if( (glbJsonVar.cnuEth4Vid < 1) || (glbJsonVar.cnuEth4Vid > 4094 ))
			return CMM_FAILED;
	}

	/* check tx rate limiting parameters if cnuTxRateLimitSts enable */
	if( 1 == glbJsonVar.cnuTxRateLimitSts )
	{
		if( (glbJsonVar.cnuCpuPortTxRate < 0) || (glbJsonVar.cnuCpuPortTxRate > 3200 ))
			return CMM_FAILED;
		if( (glbJsonVar.cnuEth1TxRate < 0) || (glbJsonVar.cnuEth1TxRate > 3200 ))
			return CMM_FAILED;
		if( (glbJsonVar.cnuEth2TxRate < 0) || (glbJsonVar.cnuEth2TxRate > 3200 ))
			return CMM_FAILED;
		if( (glbJsonVar.cnuEth3TxRate < 0) || (glbJsonVar.cnuEth3TxRate > 3200 ))
			return CMM_FAILED;
		if( (glbJsonVar.cnuEth4TxRate < 0) || (glbJsonVar.cnuEth4TxRate > 3200 ))
			return CMM_FAILED;
	}

	/* check rx rate limiting parameters if cnuRxRateLimitSts enable */
	if( 1 == glbJsonVar.cnuRxRateLimitSts )
	{
		if( (glbJsonVar.cnuCpuPortRxRate < 0) || (glbJsonVar.cnuCpuPortRxRate > 3200 ))
			return CMM_FAILED;
		if( (glbJsonVar.cnuEth1RxRate < 0) || (glbJsonVar.cnuEth1RxRate > 3200 ))
			return CMM_FAILED;
		if( (glbJsonVar.cnuEth2RxRate < 0) || (glbJsonVar.cnuEth2RxRate > 3200 ))
			return CMM_FAILED;
		if( (glbJsonVar.cnuEth3RxRate < 0) || (glbJsonVar.cnuEth3RxRate > 3200 ))
			return CMM_FAILED;
		if( (glbJsonVar.cnuEth4RxRate < 0) || (glbJsonVar.cnuEth4RxRate > 3200 ))
			return CMM_FAILED;
	}
	
	return CMM_SUCCESS;
}

/* prepare setting parameters to profile, return 0: success; else failed */
int jsonSetCnuPrepare(st_dbsProfile * profile)
{
	/* over write vlan settings */
	profile->col_vlanSts = (1==glbJsonVar.cnuVlanSts)?1:0;
	if(profile->col_vlanSts)
	{
		profile->col_eth1vid = glbJsonVar.cnuEth1Vid;
		profile->col_eth2vid = glbJsonVar.cnuEth2Vid;
		profile->col_eth3vid = glbJsonVar.cnuEth3Vid;
		profile->col_eth4vid = glbJsonVar.cnuEth4Vid;
	}
	else
	{
		profile->col_eth1vid = 1;
		profile->col_eth2vid = 1;
		profile->col_eth3vid = 1;
		profile->col_eth4vid = 1;
	}

	/* over write rate limit settings*/
	profile->col_rxLimitSts = (1==glbJsonVar.cnuRxRateLimitSts)?1:0;
	if(profile->col_rxLimitSts)
	{
		profile->col_cpuPortRxRate = glbJsonVar.cnuCpuPortRxRate*32;
		profile->col_eth1rx = glbJsonVar.cnuEth1RxRate*32;
		profile->col_eth2rx = glbJsonVar.cnuEth2RxRate*32;
		profile->col_eth3rx = glbJsonVar.cnuEth3RxRate*32;
		profile->col_eth4rx = glbJsonVar.cnuEth4RxRate*32;
	}
	else
	{
		profile->col_cpuPortRxRate = 0;
		profile->col_eth1rx = 0;
		profile->col_eth2rx = 0;
		profile->col_eth3rx = 0;
		profile->col_eth4rx = 0;
	}
	profile->col_txLimitSts = (1==glbJsonVar.cnuTxRateLimitSts)?1:0;
	if(profile->col_txLimitSts)
	{
		profile->col_cpuPortTxRate = glbJsonVar.cnuCpuPortTxRate*32;
		profile->col_eth1tx = glbJsonVar.cnuEth1TxRate*32;
		profile->col_eth2tx = glbJsonVar.cnuEth2TxRate*32;
		profile->col_eth3tx = glbJsonVar.cnuEth3TxRate*32;
		profile->col_eth4tx = glbJsonVar.cnuEth4TxRate*32;
	}
	else
	{
		profile->col_cpuPortTxRate = 0;
		profile->col_eth1tx = 0;
		profile->col_eth2tx = 0;
		profile->col_eth3tx = 0;
		profile->col_eth4tx = 0;
	}
	
	return CMM_SUCCESS;
}

int jsonSetCnuProfile(FILE * fs)
{
	int ret = CMM_SUCCESS;
	char strlog[128] = {0};
	stCnuNode iNode;
	st_dbsProfile myProfile;
	json_object *my_object;

	/* for debug */
	//printf("\n-->call jsonSetCnuProfile()\n");
	
	/* process json set request here */
	/* 1. check input */
	ret = jsonSetCnuCheckInput();
	if( CMM_SUCCESS != ret ) goto json_ack;

	/* 2. get cnu index by input mac address */
	ret = http2dbs_getCnuIndexByMacaddress(glbJsonVar.macAddr, &iNode);
	if( CMM_SUCCESS != ret )
	{	
		/* system error */
		goto json_ack;
	}
	else if( 0 == iNode.cnu )
	{
		/* can not select this cnu */
		ret = CMM_FAILED;
		goto json_ack;
	}

	/* 3. get profile by id */
	ret = http2dbs_getProfile(iNode.cnu, &myProfile);
	if( CMM_SUCCESS != ret ) goto json_ack;

	/* 4. set parameters from glbJsonVar to myProfile */
	ret = jsonSetCnuPrepare(&myProfile);
	if( CMM_SUCCESS != ret ) goto json_ack;

	/* 5. set profile to databases */
	ret = http2dbs_setProfile(iNode.cnu, &myProfile);
	if( CMM_SUCCESS != ret ) goto json_ack;

	/* 6. permit/undo-permit cnu */
	if( 1 == glbJsonVar.cnuPermit )
	{
		ret = http2cmm_permitCnu(iNode.cnu);
		if( CMM_SUCCESS != ret ) goto json_ack;
	}
	else
	{
		ret = http2cmm_undoPermitCnu(iNode.cnu);
		if( CMM_SUCCESS != ret ) goto json_ack;
	}

	/* 7. reload profile for cnu */
	ret = http2cmm_reloadCnu(iNode.cnu);
	if( CMM_SUCCESS != ret ) goto json_ack;
	

json_ack:
	/* send ack to nms */
	my_object = json_object_new_object();
	json_object_object_add(my_object, "status", json_object_new_int(ret?1:0));
	jsonSendAck(fs, ret, json_object_to_json_string(my_object));
	/* free json object !!! */
	json_object_put(my_object);

	/* write opt-log here */
	sprintf(strlog, "json set profile to cnu/%d/%d", iNode.clt, iNode.cnu);
	http2dbs_writeOptlog(ret, strlog);
	
	return CMM_SUCCESS;
}

/* check input parameters , return 0: success; else failed */
int jsonGetCnuCheckInput(void)
{
	char bmac[6] = {0};
	uint8_t MA[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
	uint8_t MB[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	
	/* check if cnu mac address is valid*/
	if( CMM_SUCCESS != boardapi_macs2b(glbJsonVar.macAddr, bmac) )
		return CMM_FAILED;
	else if( memcmp(bmac, MA, 6) == 0 )
		return CMM_FAILED;
	else if( memcmp(bmac, MB, 6) == 0 )
		return CMM_FAILED;
	else
		sprintf(glbJsonVar.macAddr, "%02X:%02X:%02X:%02X:%02X:%02X", 
			bmac[0], bmac[1], bmac[2], bmac[3], bmac[4], bmac[5]
		);	
	
	return CMM_SUCCESS;
}

/* prepare setting parameters to profile, return 0: success; else failed */
int jsonGetCnuPrepare(st_dbsProfile * profile, st_dbsCnu * cnu)
{
	/* over write vlan settings */
	glbJsonVar.cnuVlanSts = profile->col_vlanSts?1:2;
	if(profile->col_vlanSts)
	{
		glbJsonVar.cnuEth1Vid = profile->col_eth1vid;
		glbJsonVar.cnuEth2Vid = profile->col_eth2vid;
		glbJsonVar.cnuEth3Vid = profile->col_eth3vid;
		glbJsonVar.cnuEth4Vid = profile->col_eth4vid;
	}
	else
	{
		glbJsonVar.cnuEth1Vid = 1;
		glbJsonVar.cnuEth2Vid = 1;
		glbJsonVar.cnuEth3Vid = 1;
		glbJsonVar.cnuEth4Vid = 1;
	}

	/* over write rate limit settings*/
	glbJsonVar.cnuRxRateLimitSts = profile->col_rxLimitSts?1:2;
	if(profile->col_rxLimitSts)
	{
		glbJsonVar.cnuCpuPortRxRate = profile->col_cpuPortRxRate/32;
		glbJsonVar.cnuEth1RxRate = profile->col_eth1rx/32;
		glbJsonVar.cnuEth2RxRate = profile->col_eth2rx/32;
		glbJsonVar.cnuEth3RxRate = profile->col_eth3rx/32;
		glbJsonVar.cnuEth4RxRate = profile->col_eth4rx/32;
	}
	else
	{
		glbJsonVar.cnuCpuPortRxRate = 0;
		glbJsonVar.cnuEth1RxRate = 0;
		glbJsonVar.cnuEth2RxRate = 0;
		glbJsonVar.cnuEth3RxRate = 0;
		glbJsonVar.cnuEth4RxRate = 0;
	}
	glbJsonVar.cnuTxRateLimitSts = profile->col_txLimitSts?1:2;
	if(profile->col_txLimitSts)
	{
		glbJsonVar.cnuCpuPortTxRate = profile->col_cpuPortTxRate/32;
		glbJsonVar.cnuEth1TxRate = profile->col_eth1tx/32;
		glbJsonVar.cnuEth2TxRate = profile->col_eth2tx/32;
		glbJsonVar.cnuEth3TxRate = profile->col_eth3tx/32;
		glbJsonVar.cnuEth4TxRate = profile->col_eth4tx/32;
	}
	else
	{
		glbJsonVar.cnuCpuPortTxRate = 0;
		glbJsonVar.cnuEth1TxRate = 0;
		glbJsonVar.cnuEth2TxRate = 0;
		glbJsonVar.cnuEth3TxRate = 0;
		glbJsonVar.cnuEth4TxRate = 0;
	}

	/* cnu permit */
	if( 0 == cnu->col_row_sts )
		glbJsonVar.cnuPermit = 2;
	else
		glbJsonVar.cnuPermit = cnu->col_auth?1:2;
	
	return CMM_SUCCESS;
}

int jsonGetCnuProfile(FILE * fs)
{	
	int ret = CMM_SUCCESS;
	int i = 0;
	char strlog[128] = {0};
	stCnuNode iNode;
	st_dbsProfile myProfile;
	st_dbsCnu cnu;
	json_object *my_object;

	/* for debug */
	//printf("\n-->call jsonGetCnuProfile()\n");
	
	/* process json get request here */	
	/* 1. check input */
	ret = jsonGetCnuCheckInput();
	if( CMM_SUCCESS != ret ) goto json_out;

	/* 2. get cnu index by input mac address */
	ret = http2dbs_getCnuIndexByMacaddress(glbJsonVar.macAddr, &iNode);
	if( CMM_SUCCESS != ret )
	{	
		/* system error */
		goto json_out;
	}
	else if( 0 == iNode.cnu )
	{
		/* can not select this cnu */
		ret = CMM_FAILED;
		goto json_out;
	}

	/* 3. get profile by id */
	ret = http2dbs_getProfile(iNode.cnu, &myProfile);
	if( CMM_SUCCESS != ret ) goto json_out;

	/* 4. get cnu permit status */
	ret = http2dbs_getCnu(iNode.cnu, &cnu);
	if( CMM_SUCCESS != ret ) goto json_out;
	
	/* 5. get parameters from myProfile and cnu to glbJsonVar */
	ret = jsonGetCnuPrepare(&myProfile, &cnu);
	if( CMM_SUCCESS != ret ) goto json_out;

json_out:	
	/* send ack to NMS */
	my_object = json_object_new_object();
	json_object_object_add(my_object, "status", json_object_new_int(ret?1:0));
	if( CMM_SUCCESS == ret )
	{
		for ( i = 0; jsonSetTable[i].variable != NULL; i++ )
		{
			switch ( jsonSetTable[i].type )
			{
				case CGI_TYPE_STR:
				{
					json_object_object_add(my_object, jsonSetTable[i].variable, json_object_new_string(jsonSetTable[i].value));
					break;
				}
				case CGI_TYPE_NUM:
				{
					json_object_object_add(my_object, jsonSetTable[i].variable, json_object_new_int(*((int *)jsonSetTable[i].value)));
					break;
				}
			}
		}		
	}
	jsonSendAck(fs, ret, json_object_to_json_string(my_object));
	json_object_put(my_object);

	/* write opt-log here */
	sprintf(strlog, "json get profile from cnu/%d/%d", iNode.clt, iNode.cnu);
	http2dbs_writeOptlog(ret, strlog);
	
	return CMM_SUCCESS;
}


/******************************************************************************************
curl -X "POST"  -H "Application/json" http://192.168.1.150/getcnu.json -d "{'mac':'30:71:B2:00:02:1E'}"
*******************************************************************************************/
int do_json(char *path, FILE *fs, int jstrLen)
{
	char jsonString[JSON_STRING_SIZE] = {0};

	/* get post json string */
	fgets(jsonString, JSON_STRING_SIZE, fs);
	
	/* for debug */
	//printf("\n\n-->call do_json()\n");
	//printf("jstrLen		= %d\n", jstrLen);
	//printf("jsonString	= %s\n", jsonString);
	
	/* parse jason string and save in glbJsonVar */	
	jsonParseSet(jsonString);

	/* process json request */
	if ( strcmp( path, "/setcnu.json" ) == 0 )
	{
		return jsonSetCnuProfile(fs);
	}
	else if ( strcmp( path, "/getcnu.json" ) == 0 )
	{
		return jsonGetCnuProfile(fs);
	}
	else
	{
		jsonSendAck(fs, -1, NULL );
		return 0;
	}
}

