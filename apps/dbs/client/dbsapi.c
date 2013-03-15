#include <assert.h>
#include <dbsapi.h>
#include <sqlite3.h>

static T_UDP_SK_INFO dbsapiFd;
static uint16_t dbsSrcMod = 0;
static uint8_t gBuf_dbsApi[MAX_UDP_SIZE] = {0};

int __dbsCommunicate(uint8_t *buf, uint32_t len)
{
	T_UDP_SK_INFO *sk = &dbsapiFd;
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;
	uint16_t msgType = req->HEADER.usMsgType;
	
	fd_set fdsr;
	int maxsock;
	struct timeval tv;
	
	int ret = 0;
	int sendn = 0;
	
	T_DB_MSG_HEADER_ACK *r = NULL;
	struct sockaddr_in from;
	
	int FromAddrSize = 0;
	int rev_len = 0;

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		fprintf(stderr, "ERROR: MOD[%d] dbsCommunicate->sendto !\n", dbsSrcMod);
		return -1;
	}

	while(1)
	{
		// initialize file descriptor set
		FD_ZERO(&fdsr);
		FD_SET(sk->sk, &fdsr);

		// timeout setting
		tv.tv_sec = 10;
		tv.tv_usec = 0;

		maxsock = sk->sk;
		ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
		if( ret <= 0 )
		{
			fprintf(stderr, "ERROR: MOD[%d] dbsCommunicate->select[msgType=%d] !\n", dbsSrcMod, msgType);
			return -1;
		}
		
		// check whether a new connection comes
		if (FD_ISSET(sk->sk, &fdsr))
		{
			FromAddrSize = sizeof(from);
			rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);

			if ( -1 == rev_len )
			{
				fprintf(stderr, "ERROR: MOD[%d] dbsCommunicate->recvfrom !\n", dbsSrcMod);
				return -1;
			}
			else
			{
				r = (T_DB_MSG_HEADER_ACK *)buf;
				if( msgType != r->usMsgType )
				{
					fprintf(stderr, "WARNNING: MOD[%d] dbsCommunicate: msgType[%d!=%d], [continue] !\n", 
						dbsSrcMod, r->usMsgType, msgType);
					continue;
				}
				else if( MID_DBS != r->usSrcMID )
				{
					fprintf(stderr, "ERROR: MOD[%d] dbsCommunicate: usSrcMID[%d!=MID_DBS], [continue] !\n", 
						dbsSrcMod, r->usSrcMID);
					continue;
				}
				else if( r->result )
				{
					if( msgType == DB_GET_SYSLOG )
					{
						return r->result;
					}
					else if( msgType == DB_GET_OPTLOG )
					{
						return r->result;
					}
					else if( msgType == DB_GET_ALARMLOG )
					{
						return r->result;
					}
					else
					{
						fprintf(stderr, "\r\n  ERROR: MOD[%d] dbsCommunicate: result [%d] !\n", dbsSrcMod, r->result);
						return r->result;
					}
				}
				else
				{
					return r->result;
				}
			}			
		}
		else
		{
			fprintf(stderr, "ERROR: MOD[%d] dbsCommunicate->FD_ISSET !\n", dbsSrcMod);
			return -1;
		}
	}
}

BOOLEAN __isDbsModuleOK(uint16_t mid)
{
	//uint8_t buf[MAX_UDP_SIZE] = {0};
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_mid = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;
	uint16_t *ack_ms = (uint16_t *)(ack->BUF);	
	
	if( mid > MAX_MODULE_NUMS )
	{
		fprintf(stderr, "WARNING: dbsRegisterModule mid[%d] is out of range !\n", mid);
		return BOOL_FALSE;
	}	

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_MODULE_STS;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	
	*req_mid = mid;
	
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( CMM_SUCCESS == __dbsCommunicate(buf, len) )
	{
		return (*ack_ms)?BOOL_TRUE:BOOL_FALSE;
	}
	else
	{
		return BOOL_FALSE;
	}	
}

int __dbsRegisterModule(void)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_mid = (uint16_t *)(req->BUF);	

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_REGISTER_MODULE;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	*req_mid = dbsSrcMod;
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);
}

int __dbsDestroyModule(void)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_mid = (uint16_t *)(req->BUF);	

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_MODULE;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	*req_mid = dbsSrcMod;
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);
}

int dbsSelectCnuIndexByMacAddress(char *mac, stCnuNode *index)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint8_t *req_cell = (uint8_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	stCnuNode *ack_cell = (stCnuNode *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_SELECT_CNU_INDEX_BY_MAC;
	req->HEADER.ulBodyLength = strlen(mac);

	memcpy(req_cell, mac, req->HEADER.ulBodyLength);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(index, ack_cell, sizeof(stCnuNode));	
	}
	return ack->HEADER.result;
}

int dbsGetInteger(DB_INTEGER_V *v)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;

	assert( NULL != v );
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	DB_COL_INFO *ci = (DB_COL_INFO *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;
	DB_COL_VAR *ack_cell = (DB_COL_VAR *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_COL_VALUE;
	req->HEADER.ulBodyLength = sizeof(DB_COL_INFO);

	ci->tbl = v->ci.tbl;
	ci->row = v->ci.row;
	ci->col = v->ci.col;
	ci->colType = SQLITE_INTEGER;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( 0 == __dbsCommunicate(buf, len) )
	{
		v->ci.colType = ack_cell->ci.colType;
		
		if( SQLITE_NULL == v->ci.colType )
		{
			v->len = 0;
			v->integer = 0;
		}
		else
		{
			v->len = ack_cell->len;
			v->integer = *(uint32_t *)(ack_cell->data);
		}
	}
	return ack->HEADER.result;
}

int dbsGetText(DB_TEXT_V *v)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	DB_COL_INFO *ci = (DB_COL_INFO *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	DB_COL_VAR *ack_cell = (DB_COL_VAR *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_COL_VALUE;
	req->HEADER.ulBodyLength = sizeof(DB_COL_INFO);	

	ci->tbl = v->ci.tbl;
	ci->row = v->ci.row;
	ci->col = v->ci.col;
	ci->colType = SQLITE3_TEXT;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		v->ci.colType = ack_cell->ci.colType;
		if( SQLITE_NULL == v->ci.colType )
		{
			v->len = 0;
			v->text[0] = '\0';
		}
		else
		{
			if( ack_cell->len >= DBS_COL_MAX_LEN )
			{
				v->len = 0;
				v->text[0] = '\0';
				return CMM_FAILED;
			}
			else
			{
				v->len = ack_cell->len;
				strncpy(v->text, ack_cell->data, ack_cell->len);
				v->text[v->len] = '\0';
			}
		}		
	}
	return ack->HEADER.result;
}

int dbsGetCliRole(uint16_t id, st_dbsCliRole *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsCliRole *ack_cell = (st_dbsCliRole *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_CLI_ROLE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsCliRole));	
	}
	return ack->HEADER.result;
}

int dbsGetClt(uint16_t id, st_dbsClt *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsClt *ack_cell = (st_dbsClt *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_CLT;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsClt));	
	}
	return ack->HEADER.result;
}

int dbsGetCltconf(uint16_t id, st_dbsCltConf *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsCltConf *ack_cell = (st_dbsCltConf *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_CLTCONF;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsCltConf));	
	}
	return ack->HEADER.result;
}

int dbsGetCnu(uint16_t id, st_dbsCnu *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsCnu *ack_cell = (st_dbsCnu *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_CNU;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsCnu));	
	}
	return ack->HEADER.result;
}

int dbsGetDepro(uint16_t id, st_dbsCnuDefaultProfile *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsCnuDefaultProfile *ack_cell = (st_dbsCnuDefaultProfile *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_DEPRO;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsCnuDefaultProfile));	
	}
	return ack->HEADER.result;
}

int dbsGetNetwork(uint16_t id, st_dbsNetwork *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsNetwork *ack_cell = (st_dbsNetwork *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_NETWORK;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsNetwork));	
	}
	return ack->HEADER.result;
}

int dbsGetProfile(uint16_t id, st_dbsProfile *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsProfile *ack_cell = (st_dbsProfile *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsProfile));	
	}
	return ack->HEADER.result;
}

int dbsGetSnmp(uint16_t id, st_dbsSnmp *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsSnmp *ack_cell = (st_dbsSnmp *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_SNMP;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsSnmp));	
	}
	return ack->HEADER.result;
}

int dbsGetSwmgmt(uint16_t id, st_dbsSwmgmt *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsSwmgmt *ack_cell = (st_dbsSwmgmt *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_SWMGMT;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsSwmgmt));	
	}
	return ack->HEADER.result;
}

int dbsGetSysinfo(uint16_t id, st_dbsSysinfo *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsSysinfo *ack_cell = (st_dbsSysinfo *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ROW_SYSINFO;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(row, ack_cell, sizeof(st_dbsSysinfo));	
	}
	return ack->HEADER.result;
}

int dbsUpdateInteger(DB_INTEGER_V *v)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;	

	assert( NULL != v );
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	DB_COL_VAR *req_cell = (DB_COL_VAR *)(req->BUF);

	req_cell->ci.tbl = v->ci.tbl;
	req_cell->ci.row = v->ci.row;
	req_cell->ci.col = v->ci.col;
	req_cell->ci.colType = SQLITE_INTEGER;
	req_cell->len = sizeof(uint32_t);
	*(uint32_t *)(req_cell->data) = v->integer;	

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_COL_VALUE;
	req->HEADER.ulBodyLength = sizeof(DB_COL_VAR) + req_cell->len;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;	

	return __dbsCommunicate(buf, len);
}

int dbsSetDsdtRgmiiDelay(st_dsdtRgmiiTimingDelay *dsdtRgmiiTimingDelay)
{
	DB_INTEGER_V iv;
	
	iv.ci.tbl = DBS_SYS_TBL_ID_SYSINFO;
	iv.ci.row = 1;
	iv.ci.col = DBS_SYS_TBL_SYSINFO_COL_ID_P6RXD;
	iv.ci.colType = SQLITE_INTEGER;
	iv.len = sizeof(uint32_t);
	iv.integer = dsdtRgmiiTimingDelay->rxdelay;
	if( CMM_SUCCESS != dbsUpdateInteger(&iv) )
	{
		return CMM_FAILED;
	}

	iv.ci.tbl = DBS_SYS_TBL_ID_SYSINFO;
	iv.ci.row = 1;
	iv.ci.col = DBS_SYS_TBL_SYSINFO_COL_ID_P6TXD;
	iv.ci.colType = SQLITE_INTEGER;
	iv.len = sizeof(uint32_t);
	iv.integer = dsdtRgmiiTimingDelay->txdelay;
	return dbsUpdateInteger(&iv);
}

int dbsUpdateText(DB_TEXT_V *v)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	DB_COL_VAR *req_cell = (DB_COL_VAR *)(req->BUF);
	
	if( strlen(v->text) >= DBS_COL_MAX_LEN )
	{
		return CMM_FAILED;
	}

	req_cell->ci.tbl = v->ci.tbl;
	req_cell->ci.row = v->ci.row;
	req_cell->ci.col = v->ci.col;
	req_cell->ci.colType = SQLITE3_TEXT;
	req_cell->len = strlen(v->text);
	strcpy(req_cell->data, v->text);	

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_COL_VALUE;
	req->HEADER.ulBodyLength = sizeof(DB_COL_VAR) + req_cell->len;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __dbsCommunicate(buf, len);
}


int dbsUpdateCliRole(uint16_t id, st_dbsCliRole *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	st_dbsCliRole *req_cell = (st_dbsCliRole *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_CLI_ROLE;
	req->HEADER.ulBodyLength = sizeof(st_dbsCliRole);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		dbs_sys_log(DBS_LOG_ALERT, "dbsUpdateCliRole: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsCliRole));
		req_cell->id = id;	
		return __dbsCommunicate(buf, len);
	}
}

int dbsUpdateClt(uint16_t id, st_dbsClt *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	st_dbsClt *req_cell = (st_dbsClt *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_CLT;
	req->HEADER.ulBodyLength = sizeof(st_dbsClt);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		dbs_sys_log(DBS_LOG_ALERT, "dbsUpdateClt: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsClt));
		req_cell->id = id;	
		return __dbsCommunicate(buf, len);
	}
}

int dbsUpdateCltconf(uint16_t id, st_dbsCltConf *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	st_dbsCltConf *req_cell = (st_dbsCltConf *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_CLTCONF;
	req->HEADER.ulBodyLength = sizeof(st_dbsCltConf);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		dbs_sys_log(DBS_LOG_ALERT, "dbsUpdateCltconf: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsCltConf));
		req_cell->id = id;	
		return __dbsCommunicate(buf, len);
	}
}

int dbsUpdateCnu(uint16_t id, st_dbsCnu *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	st_dbsCnu *req_cell = (st_dbsCnu *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_CNU;
	req->HEADER.ulBodyLength = sizeof(st_dbsCnu);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		dbs_sys_log(DBS_LOG_ALERT, "dbsUpdateCnu: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsCnu));
		req_cell->id = id;	
		return __dbsCommunicate(buf, len);
	}
}

int dbsUpdateDepro(uint16_t id, st_dbsCnuDefaultProfile *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	st_dbsCnuDefaultProfile *req_cell = (st_dbsCnuDefaultProfile *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_DEPRO;
	req->HEADER.ulBodyLength = sizeof(st_dbsCnuDefaultProfile);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		dbs_sys_log(DBS_LOG_ALERT, "dbsUpdateDepro: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsCnuDefaultProfile));
		req_cell->id = id;	
		return __dbsCommunicate(buf, len);
	}
}

int dbsUpdateNetwork(uint16_t id, st_dbsNetwork *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	st_dbsNetwork *req_cell = (st_dbsNetwork *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_NETWORK;
	req->HEADER.ulBodyLength = sizeof(st_dbsNetwork);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		dbs_sys_log(DBS_LOG_ALERT, "dbsUpdateNetwork: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsNetwork));
		req_cell->id = id;	
		return __dbsCommunicate(buf, len);
	}
}

int dbsUpdateProfile(uint16_t id, st_dbsProfile *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	st_dbsProfile *req_cell = (st_dbsProfile *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_PROFILE;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		dbs_sys_log(DBS_LOG_ALERT, "dbsUpdateProfile: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsProfile));
		req_cell->id = id;	
		return __dbsCommunicate(buf, len);
	}
}

int dbsUpdateSnmp(uint16_t id, st_dbsSnmp *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	st_dbsSnmp *req_cell = (st_dbsSnmp *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_SNMP;
	req->HEADER.ulBodyLength = sizeof(st_dbsSnmp);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		dbs_sys_log(DBS_LOG_ALERT, "dbsUpdateSnmp: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsSnmp));
		req_cell->id = id;	
		return __dbsCommunicate(buf, len);
	}
}

int dbsUpdateSwmgmt(uint16_t id, st_dbsSwmgmt *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	st_dbsSwmgmt *req_cell = (st_dbsSwmgmt *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_SWMGMT;
	req->HEADER.ulBodyLength = sizeof(st_dbsSwmgmt);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		dbs_sys_log(DBS_LOG_ALERT, "dbsUpdateSwmgmt: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsSwmgmt));
		req_cell->id = id;	
		return __dbsCommunicate(buf, len);
	}
}

int dbsUpdateSysinfo(uint16_t id, st_dbsSysinfo *row)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	st_dbsSysinfo *req_cell = (st_dbsSysinfo *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_UPDATE_ROW_SYSINFO;
	req->HEADER.ulBodyLength = sizeof(st_dbsSysinfo);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		dbs_sys_log(DBS_LOG_ALERT, "dbsUpdateSysinfo: CMM_BUFFER_OVERFLOW");
		return CMM_BUFFER_OVERFLOW;
	}
	else
	{
		memcpy(req_cell, row, sizeof(st_dbsSysinfo));
		req_cell->id = id;	
		return __dbsCommunicate(buf, len);
	}
}

int dbsDestroyRowClt(uint16_t id)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_ROW_TBL_CLT;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);	
}

int dbsDestroyRowCltconf(uint16_t id)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_ROW_TBL_CLTCONF;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);	
}

int dbsDestroyRowCnu(uint16_t id)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_ROW_TBL_CNU;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);	
}

int dbsDestroyRowProfile(uint16_t id)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_ROW_TBL_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);	
}

int dbsCreateSuProfileForCnu(uint16_t id)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_SU_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);	
}

int dbsCreateDewlProfileForCnu(uint16_t id)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_WL_DE_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);	
}

int dbsCreateDeblProfileForCnu(uint16_t id)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_BL_DE_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);	
}

int dbsCreateSuProfileForWec701Cnu(uint16_t id)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_SU2_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);	
}

int dbsCreateDewlProfileForWec701Cnu(uint16_t id)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_WL_DE2_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);	
}

int dbsCreateDeblProfileForWec701Cnu(uint16_t id)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_CREATE_BL_DE2_PROFILE;
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*req_cell = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);
}

int dbsLogCount(uint16_t tbl, uint32_t *n)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;

	assert( NULL != n );

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *v = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_LOG_COUNT;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);

	*v = tbl;
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	*n = 0;
	
	if( 0 == __dbsCommunicate(buf, len) )
	{
		*n = *(uint32_t *)(ack->BUF);
	}
	return ack->HEADER.result;
}

int dbsGetSyslog(uint32_t row, st_dbsSyslog *log)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint32_t *req_data = (uint32_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;
	st_dbsSyslog *ack_data = (st_dbsSyslog *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_SYSLOG;	
	req->HEADER.ulBodyLength = sizeof(uint32_t);

	*req_data = row;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(log, ack_data, sizeof(st_dbsSyslog));
	}
	return ack->HEADER.result;
}

int dbsGetOptlog(uint32_t row, st_dbsOptlog *log)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint32_t *req_data = (uint32_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;
	st_dbsOptlog *ack_data = (st_dbsOptlog *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_OPTLOG;	
	req->HEADER.ulBodyLength = sizeof(uint32_t);

	*req_data = row;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(log, ack_data, sizeof(st_dbsOptlog));
	}
	return ack->HEADER.result;
}

int dbsGetAlarmlog(uint32_t row, st_dbsAlarmlog *log)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint32_t *req_data = (uint32_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;
	st_dbsAlarmlog *ack_data = (st_dbsAlarmlog *)(ack->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_GET_ALARMLOG;	
	req->HEADER.ulBodyLength = sizeof(uint32_t);

	*req_data = row;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( 0 == __dbsCommunicate(buf, len) )
	{
		memcpy(log, ack_data, sizeof(st_dbsAlarmlog));
	}
	return ack->HEADER.result;
}

int dbs_sys_log(uint32_t priority, const char *message)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	time_t b_time;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	st_dbsSyslog *pLog = (st_dbsSyslog *)(req->BUF);

	assert( NULL != message );

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_WRITE_SYSLOG;	
	req->HEADER.ulBodyLength = sizeof(st_dbsSyslog);

	time(&b_time);
	pLog->time = b_time;
	pLog->who = dbsSrcMod;
	pLog->level = priority;
	
	if( strlen(message) < DBS_COL_MAX_LEN )
	{
		strcpy(pLog->log, message);
	}
	else
	{
		pLog->level = DBS_LOG_ERR;
		sprintf(pLog->log, "module write syslog length[%d]", strlen(message));
	}	

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __dbsCommunicate(buf, len);
}

int dbs_opt_log(st_dbsOptlog *log)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;		
	st_dbsOptlog *pLog = (st_dbsOptlog *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_WRITE_OPTLOG;	
	req->HEADER.ulBodyLength = sizeof(st_dbsOptlog);

	memcpy(pLog, log, req->HEADER.ulBodyLength);
	
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);
}

int dbs_alarm_log(st_dbsAlarmlog *log)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;		
	st_dbsAlarmlog *pLog = (st_dbsAlarmlog *)(req->BUF);

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_WRITE_ALARMLOG;	
	req->HEADER.ulBodyLength = sizeof(st_dbsAlarmlog);

	memcpy(pLog, log, req->HEADER.ulBodyLength);
	
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);
}

int dbsRegisterModuleById(uint16_t mid)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_mid = (uint16_t *)(req->BUF);

	if( mid > MAX_MODULE_NUMS )
	{
		return -1;
	}

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_REGISTER_MODULE;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	*req_mid = mid;
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);
}

int dbsDestroyModuleById(uint16_t mid)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;	
	uint16_t *req_mid = (uint16_t *)(req->BUF);	

	if( mid > MAX_MODULE_NUMS )
	{
		return -1;
	}

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_DESTROY_MODULE;	
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	*req_mid = mid;
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __dbsCommunicate(buf, len);
}

void dbsWaitModule(uint32_t MF)
{	
	int i = 0;
	BOOLEAN ms = BOOL_FALSE;

	for( i=0; i<MAX_MODULE_NUMS; i++ )
	{
		if( MF&(1<<i) )
		{
			do
			{				
				ms = __isDbsModuleOK(i+1);
				if( BOOL_FALSE == ms )
				{
					//fprintf(stderr, "WARNING: MID[%d] dbsWaitModule MID[%d] Failed, Continue !\n", dbsSrcMod, i+1);
					usleep(200000);
				}
				else
				{
					//fprintf(stderr, "INFO: MID[%d] dbsWaitModule MID[%d] Success !\n", dbsSrcMod, i+1);
					usleep(2000);
				}
			}
			while( BOOL_FALSE == ms );
		}
	}
	
}

BOOLEAN dbsGetModuleStatus(uint16_t mid)
{
	return __isDbsModuleOK(mid);
}

int dbsMsgDebug(uint32_t status)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = (status?DB_MSG_DEBUG_ENABLE:DB_MSG_DEBUG_DISABLE);
	req->HEADER.ulBodyLength = 0;

	len = sizeof(req->HEADER);
	
	return __dbsCommunicate(buf, len);
}

int dbsSQLDebug(uint32_t status)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = (status?DB_SQL_DEBUG_ENABLE:DB_SQL_DEBUG_DISABLE);
	req->HEADER.ulBodyLength = 0;

	len = sizeof(req->HEADER);
	
	return __dbsCommunicate(buf, len);
}

int dbsFflush(void)
{
	uint8_t *buf = gBuf_dbsApi;
	bzero(buf, MAX_UDP_SIZE);
	uint32_t len = 0;	
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)buf;		

	req->HEADER.usSrcMID = dbsSrcMod;
	req->HEADER.usDstMID = MID_DBS;
	req->HEADER.usMsgType = DB_ONLINE_BACKUP;
	req->HEADER.ulBodyLength = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __dbsCommunicate(buf, len);
}

int dbsNoWaitOpen(uint16_t srcMod)
{
	BOOLEAN ms = BOOL_FALSE;
	T_UDP_SK_INFO *sk = &dbsapiFd;

	if( sk->sk > 0 )
	{
		fprintf(stderr, "WARNING: dbsNoWaitOpen has been opened already !\n");
		return -1;
	}
	
	/*创建外部SOCKET  接口*/
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		fprintf(stderr, "ERROR: dbsOpen->socket !\n");
		return -1;
	}
	
	sk->skaddr.sin_family = PF_INET;
	sk->skaddr.sin_port = htons(DBS_LISTEN_PORT);
	sk->skaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	dbsSrcMod = srcMod;

	do
	{
		ms = __isDbsModuleOK(MID_DBS);
		if( BOOL_FALSE == ms )
		{
			usleep(300000);
		}
	}
	while( BOOL_FALSE == ms );

	if( 0 == __dbsRegisterModule() )
	{
		//fprintf(stderr, "INFO: MOD[%d] dbsOpen success !\n", dbsSrcMod);
		return CMM_SUCCESS;
	}
	else
	{
		fprintf(stderr, "ERROR: MOD[%d] dbsOpen->__dbsRegisterModule failed !\n", dbsSrcMod);
		return CMM_FAILED;
	}
}

int dbsOpen(uint16_t srcMod)
{
	BOOLEAN ms = BOOL_FALSE;
	T_UDP_SK_INFO *sk = &dbsapiFd;

	if( sk->sk > 0 )
	{
		fprintf(stderr, "WARNING: dbsOpen has been opened already !\n");
		return -1;
	}
	
	/*创建外部SOCKET  接口*/
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		fprintf(stderr, "ERROR: dbsOpen->socket !\n");
		return -1;
	}
	
	sk->skaddr.sin_family = PF_INET;
	sk->skaddr.sin_port = htons(DBS_LISTEN_PORT);
	sk->skaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	dbsSrcMod = srcMod;

	/* wait for dbs starting */
	sleep(3);
	do
	{
		ms = __isDbsModuleOK(MID_DBS);
		if( BOOL_FALSE == ms )
		{
			usleep(300000);
		}
	}
	while( BOOL_FALSE == ms );

	if( 0 == __dbsRegisterModule() )
	{
		//fprintf(stderr, "INFO: MOD[%d] dbsOpen success !\n", dbsSrcMod);
		return CMM_SUCCESS;
	}
	else
	{
		fprintf(stderr, "ERROR: MOD[%d] dbsOpen->__dbsRegisterModule failed !\n", dbsSrcMod);
		return CMM_FAILED;
	}
}

int dbsClose(void)
{
	T_UDP_SK_INFO *sk = &dbsapiFd;
	
	if( sk->sk != 0)
	{
		if( 0 != __dbsDestroyModule() )
		{
			fprintf(stderr, "ERROR: MOD[%d] dbsClose->__dbsDestroyModule failed !\n", dbsSrcMod);
			return CMM_FAILED;
		}
		else
		{
			//fprintf(stderr, "INFO: MOD[%d] dbsClose dbs success !\n", dbsSrcMod);
			close(sk->sk);
			sk->sk = 0;
			dbsSrcMod = 0;
			return CMM_SUCCESS;
		}		
	}
	else
	{
		fprintf(stderr, "WARNING: MOD[%d] dbsClose->close !\n", dbsSrcMod);
		return CMM_FAILED;
	}
}

