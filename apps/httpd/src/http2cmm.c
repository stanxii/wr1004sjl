#include <stdio.h>
#include <string.h>
#include <wecplatform.h>
#include "syscall.h"
#include "http2cmm.h"
#include <http2dbs.h>
#include <dbsapi.h>
#include <boardapi.h>

static T_UDP_SK_INFO SK_HTTP2CMM;

int __mapSelectIndex2Rate(int index)
{
	switch(index)
	{
		case 0:
		{
			return 0;
		}
		case 1:
		{
			return 128;
		}
		case 2:
		{
			return 256;
		}
		case 3:
		{
			return 512;
		}
		case 4:
		{
			return 1024;
		}
		case 5:
		{
			return 1024+512;
		}
		case 6:
		{
			return 2048;
		}
		case 7:
		{
			return 3*1024;
		}
		case 8:
		{
			return 4*1024;
		}
		case 9:
		{
			return 6*1024;
		}
		case 10:
		{
			return 8*1024;
		}
		default:
		{
			return 0;
		}
	}
}

int __getCnuPortStatus(uint16_t id, uint16_t port)
{
	st_dbsCnu cnu;
	st_dbsProfile profile;

	if( (id < 1)||(id > MAX_CNU_AMOUNT_LIMIT) )
	{
		return 0;
	}

	if( port > 4 )
	{
		return 0;
	}

	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		return 0;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		return 0;
	}

	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		return 0;
	}	
	else if( 0 == profile.col_row_sts )
	{
		return 0;
	}

	if( 0 == profile.col_psctlSts )
	{
		return 1;
	}
	else
	{
		switch(port)
		{
			case 0:
			{
				return profile.col_cpuPortSts?1:0;
			}
			case 1:
			{
				return profile.col_eth1sts?1:0;
			}
			case 2:
			{
				return profile.col_eth2sts?1:0;
			}
			case 3:
			{
				return profile.col_eth3sts?1:0;
			}
			case 4:
			{
				return profile.col_eth4sts?1:0;
			}
			default:
			{
				return 0;
			}
		}
	}
}

int __http2cmm_comm(uint8_t *buf, uint32_t len)
{
	T_UDP_SK_INFO *sk = &SK_HTTP2CMM;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint16_t msgType = req->HEADER.usMsgType;
	T_REQ_Msg_CMM *ack = NULL;

	fd_set fdsr;
	//int maxsock;
	struct timeval tv;	
	int ret = 0;
	int sendn = 0;	
	struct sockaddr_in from;	
	int FromAddrSize = 0;
	int rev_len = 0;

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		dbs_sys_log(dbsdev, DBS_LOG_CRIT, "httpd call __http2cmm_comm sendto failed");
		return CMM_FAILED;
	}

	while(1)
	{
		// initialize file descriptor set
		FD_ZERO(&fdsr);
		FD_SET(sk->sk, &fdsr);

		// timeout setting
		tv.tv_sec = 18;
		tv.tv_usec = 0;

		//检测socket
		ret = select(sk->sk + 1, &fdsr, NULL, NULL, &tv);
		if( ret <= 0 )
		{
			dbs_sys_log(dbsdev, DBS_LOG_CRIT, "httpd call __http2cmm_comm select failed");
			return CMM_FAILED;
		}
		// check whether a new connection comes
		if (FD_ISSET(sk->sk, &fdsr))
		{
			FromAddrSize = sizeof(from);
			rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);
			if ( -1 == rev_len )
			{
				dbs_sys_log(dbsdev, DBS_LOG_CRIT, "httpd call __http2cmm_comm recvfrom failed");
				return CMM_FAILED;
			}
			else
			{
				ack = (T_REQ_Msg_CMM *)buf;
				if( msgType != ack->HEADER.usMsgType )
				{
					fprintf(stderr, "WARNNING: __http2cmm_comm: msgType[%d!=%d], [continue] !\n", 
						ack->HEADER.usMsgType, msgType);
					dbs_sys_log(dbsdev, DBS_LOG_WARNING, "__http2cmm_comm received non-mached msg type");
					continue;
				}
				else if( ack->result != CMM_SUCCESS )
				{	
					dbs_sys_log(dbsdev, DBS_LOG_CRIT, "httpd call __http2cmm_comm recvfrom result error");
					return  ack->result ;
				}
				else
				{
					return ack->result;
				}
			}
		}
		else
		{
			dbs_sys_log(dbsdev, DBS_LOG_CRIT, "httpd call __http2cmm_comm FD_ISSET failed");
			return CMM_FAILED;
		}
	}
}

int http2cmm_rebootClt(int id)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint16_t *index = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLT_RESET;
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	req->HEADER.fragment = 0;

	*index = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __http2cmm_comm(buf, len);
}

int http2cmm_reloadClt(int id)
{
	/* 暂时没有实现*/
	return CMM_SUCCESS;
}


int http2cmm_rebootCnu(int id)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint16_t *index = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_RESET;
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	req->HEADER.fragment = 0;

	*index = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __http2cmm_comm(buf, len);
}

int http2cmm_reloadCnu(int id)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo szNode;

	szNode.clt = 1;
	szNode.cnu = id;

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_SEND_CONFIG;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}

	memcpy(req->BUF, &szNode, req->HEADER.ulBodyLength);

	return __http2cmm_comm(buf, len);
}

int http2cmm_deleteCnu(int id)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo szNode;

	szNode.clt = 1;
	szNode.cnu = id;

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_DELETE_USER;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}

	memcpy(req->BUF, &szNode, req->HEADER.ulBodyLength);

	return __http2cmm_comm(buf, len);
}

int http2cmm_permitCnu(int id)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo szNode;

	szNode.clt = 1;
	szNode.cnu = id;

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_USER_NEW;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}

	memcpy(req->BUF, &szNode, req->HEADER.ulBodyLength);

	return __http2cmm_comm(buf, len);
}

int http2cmm_undoPermitCnu(int id)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo szNode;

	szNode.clt = 1;
	szNode.cnu = id;

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_USER_DEL;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}

	memcpy(req->BUF, &szNode, req->HEADER.ulBodyLength);

	return __http2cmm_comm(buf, len);
}

int http2cmm_getEth1Stat(PWEB_NTWK_VAR pWebVar)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_CMM_PORT_STATS_INFO *ack_data = (T_CMM_PORT_STATS_INFO *)(ack->BUF);

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_PORT_STAT;
	req->HEADER.ulBodyLength = sizeof(uint32_t);
	req->HEADER.fragment = 0;

	*req_data = PORT_ETH1_PORT_ID;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( CMM_SUCCESS == __http2cmm_comm(buf, len) )
	{
		pWebVar->eth1rxbc = ack_data->InBroadcasts;
		pWebVar->eth1rxm = ack_data->InMulticasts;
		pWebVar->eth1rxu = ack_data->InUnicasts;
		pWebVar->eth1rxp = ack_data->rxCtr;
		pWebVar->eth1rxb = ack_data->InGoodOctets;
		pWebVar->eth1txbc = ack_data->OutBroadcasts;
		pWebVar->eth1txm = ack_data->OutMulticasts;
		pWebVar->eth1txu = ack_data->OutUnicasts;
		pWebVar->eth1txp = ack_data->txCtr;
		pWebVar->eth1txb = ack_data->OutGoodOctets;
	}	
	return CMM_SUCCESS;
}

int http2cmm_getEth2Stat(PWEB_NTWK_VAR pWebVar)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_CMM_PORT_STATS_INFO *ack_data = (T_CMM_PORT_STATS_INFO *)(ack->BUF);

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_PORT_STAT;
	req->HEADER.ulBodyLength = sizeof(uint32_t);
	req->HEADER.fragment = 0;

	*req_data = PORT_ETH2_PORT_ID;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( CMM_SUCCESS == __http2cmm_comm(buf, len) )
	{
		pWebVar->eth2rxbc = ack_data->InBroadcasts;
		pWebVar->eth2rxm = ack_data->InMulticasts;
		pWebVar->eth2rxu = ack_data->InUnicasts;
		pWebVar->eth2rxp = ack_data->rxCtr;
		pWebVar->eth2rxb = ack_data->InGoodOctets;
		pWebVar->eth2txbc = ack_data->OutBroadcasts;
		pWebVar->eth2txm = ack_data->OutMulticasts;
		pWebVar->eth2txu = ack_data->OutUnicasts;
		pWebVar->eth2txp = ack_data->txCtr;
		pWebVar->eth2txb = ack_data->OutGoodOctets;
	}	
	return CMM_SUCCESS;
}

int http2cmm_getCable1Stat(PWEB_NTWK_VAR pWebVar)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_CMM_PORT_STATS_INFO *ack_data = (T_CMM_PORT_STATS_INFO *)(ack->BUF);

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_PORT_STAT;
	req->HEADER.ulBodyLength = sizeof(uint32_t);
	req->HEADER.fragment = 0;

	*req_data = PORT_CABLE1_PORT_ID;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( CMM_SUCCESS == __http2cmm_comm(buf, len) )
	{
		pWebVar->eth3rxbc = ack_data->InBroadcasts;
		pWebVar->eth3rxm = ack_data->InMulticasts;
		pWebVar->eth3rxu = ack_data->InUnicasts;
		pWebVar->eth3rxp = ack_data->rxCtr;
		pWebVar->eth3rxb = ack_data->InGoodOctets;
		pWebVar->eth3txbc = ack_data->OutBroadcasts;
		pWebVar->eth3txm = ack_data->OutMulticasts;
		pWebVar->eth3txu = ack_data->OutUnicasts;
		pWebVar->eth3txp = ack_data->txCtr;
		pWebVar->eth3txb = ack_data->OutGoodOctets;
	}	
	return CMM_SUCCESS;
}

int http2cmm_getCable2Stat(PWEB_NTWK_VAR pWebVar)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_CMM_PORT_STATS_INFO *ack_data = (T_CMM_PORT_STATS_INFO *)(ack->BUF);

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_PORT_STAT;
	req->HEADER.ulBodyLength = sizeof(uint32_t);
	req->HEADER.fragment = 0;

	*req_data = PORT_CABLE2_PORT_ID;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( CMM_SUCCESS == __http2cmm_comm(buf, len) )
	{
		pWebVar->eth4rxbc = ack_data->InBroadcasts;
		pWebVar->eth4rxm = ack_data->InMulticasts;
		pWebVar->eth4rxu = ack_data->InUnicasts;
		pWebVar->eth4rxp = ack_data->rxCtr;
		pWebVar->eth4rxb = ack_data->InGoodOctets;
		pWebVar->eth4txbc = ack_data->OutBroadcasts;
		pWebVar->eth4txm = ack_data->OutMulticasts;
		pWebVar->eth4txu = ack_data->OutUnicasts;
		pWebVar->eth4txp = ack_data->txCtr;
		pWebVar->eth4txb = ack_data->OutGoodOctets;
	}	
	return CMM_SUCCESS;
}

int http2cmm_doPortStas(PWEB_NTWK_VAR pWebVar)
{	
	http2cmm_getEth1Stat(pWebVar);
	http2cmm_getEth2Stat(pWebVar);
	http2cmm_getCable1Stat(pWebVar);
	http2cmm_getCable2Stat(pWebVar);
	return CMM_SUCCESS;
}

int http2cmm_clearPortStats(void)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLEAR_PORT_STAT;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __http2cmm_comm(buf, len);
}

int http2cmm_clearPortStas(PWEB_NTWK_VAR pWebVar)
{
	pWebVar->eth1rxbc = 0;
	pWebVar->eth1rxm = 0;
	pWebVar->eth1rxu = 0;
	pWebVar->eth1rxp = 0;
	pWebVar->eth1rxb = 0;
	pWebVar->eth1txbc = 0;
	pWebVar->eth1txm = 0;
	pWebVar->eth1txu = 0;
	pWebVar->eth1txp = 0;
	pWebVar->eth1txb = 0;

	pWebVar->eth2rxbc = 0;
	pWebVar->eth2rxm = 0;
	pWebVar->eth2rxu = 0;
	pWebVar->eth2rxp = 0;
	pWebVar->eth2rxb = 0;
	pWebVar->eth2txbc = 0;
	pWebVar->eth2txm = 0;
	pWebVar->eth2txu = 0;
	pWebVar->eth2txp = 0;
	pWebVar->eth2txb = 0;

	pWebVar->eth3rxbc = 0;
	pWebVar->eth3rxm = 0;
	pWebVar->eth3rxu = 0;
	pWebVar->eth3rxp = 0;
	pWebVar->eth3rxb = 0;
	pWebVar->eth3txbc = 0;
	pWebVar->eth3txm = 0;
	pWebVar->eth3txu = 0;
	pWebVar->eth3txp = 0;
	pWebVar->eth3txb = 0;

	pWebVar->eth4rxbc = 0;
	pWebVar->eth4rxm = 0;
	pWebVar->eth4rxu = 0;
	pWebVar->eth4rxp = 0;
	pWebVar->eth4rxb = 0;
	pWebVar->eth4txbc = 0;
	pWebVar->eth4txm = 0;
	pWebVar->eth4txu = 0;
	pWebVar->eth4txp = 0;
	pWebVar->eth4txb = 0;
	
	return http2cmm_clearPortStats();
}

int http2cmm_doLinkDiag( PWEB_NTWK_VAR pWebVar )
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	T_CMM_LINK_DIAG_INFO *req_data = (T_CMM_LINK_DIAG_INFO *)(req->BUF);

	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_MMEAD_LINK_DIAG_RESULT *ack_data = (T_MMEAD_LINK_DIAG_RESULT *)(ack->BUF);

	strcpy(pWebVar->diagCnuMac, "00:00:00:00:00:00");
	pWebVar->diagCnuModel = 0;
	pWebVar->diagCnuTei = 0;
	strcpy(pWebVar->ccoMac, "00:00:00:00:00:00");
	strcpy(pWebVar->ccoNid, "0");
	pWebVar->ccoSnid = 0;
	pWebVar->ccoTei = 0;
	pWebVar->diagCnuRxRate = 0;
	pWebVar->diagCnuTxRate = 0;
	sprintf(pWebVar->bitCarrier, "0.00");
	pWebVar->diagCnuAtten = 0;
	strcpy(pWebVar->bridgedMac, "00:00:00:00:00:00");
	sprintf(pWebVar->MPDU_ACKD, "0");
	sprintf(pWebVar->MPDU_COLL, "0");
	sprintf(pWebVar->MPDU_FAIL, "0");
	sprintf(pWebVar->PBS_PASS, "0");
	sprintf(pWebVar->PBS_FAIL, "0");

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_DO_LINK_DIAG;	
	req->HEADER.fragment = 0;
	req->HEADER.ulBodyLength = sizeof(T_CMM_LINK_DIAG_INFO);

	if( pWebVar->diagDir == 1 )
	{
		req_data->dir = 0;
	}
	else if( 2 == pWebVar->diagDir )
	{
		req_data->dir = 1;
	}
	else
	{
		pWebVar->diagResult = CMM_FAILED;
		return CMM_FAILED;
	}	
	req_data->clt = pWebVar->cnuid/MAX_CNUS_PER_CLT + 1;
	req_data->cnu = pWebVar->cnuid%MAX_CNUS_PER_CLT;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	
	if( len > MAX_UDP_SIZE )
	{
		pWebVar->diagResult = CMM_FAILED;
		return CMM_FAILED;
	}
	
	pWebVar->diagResult = __http2cmm_comm(buf, len);
	
	if( CMM_SUCCESS == pWebVar->diagResult )
	{
		sprintf(pWebVar->diagCnuMac, "%02X:%02X:%02X:%02X:%02X:%02X", 
			ack_data->mac[0], ack_data->mac[1], ack_data->mac[2], 
			ack_data->mac[3], ack_data->mac[4], ack_data->mac[5]
		);
		pWebVar->diagCnuModel = ack_data->model;
		pWebVar->diagCnuTei = ack_data->tei;
		sprintf(pWebVar->ccoMac, "%02X:%02X:%02X:%02X:%02X:%02X", 
			ack_data->ccoMac[0], ack_data->ccoMac[1], ack_data->ccoMac[2], 
			ack_data->ccoMac[3], ack_data->ccoMac[4], ack_data->ccoMac[5]
		);
		sprintf(pWebVar->ccoNid, "%02X%02X%02X%02X%02X%02X%02X", 
			ack_data->ccoNid[0], ack_data->ccoNid[1], ack_data->ccoNid[2], 
			ack_data->ccoNid[3], ack_data->ccoNid[4], ack_data->ccoNid[5], ack_data->ccoNid[6]
		);
		pWebVar->ccoSnid = ack_data->ccoSnid;
		pWebVar->ccoTei = ack_data->ccoTei;
		pWebVar->diagCnuRxRate = ack_data->rx;
		pWebVar->diagCnuTxRate = ack_data->tx;
		sprintf(pWebVar->bitCarrier, "%.2f", ack_data->bitRate);
		pWebVar->diagCnuAtten = ack_data->attenuation;
		sprintf(pWebVar->bridgedMac, "%02X:%02X:%02X:%02X:%02X:%02X", 
			ack_data->bridgedMac[0], ack_data->bridgedMac[1], ack_data->bridgedMac[2], 
			ack_data->bridgedMac[3], ack_data->bridgedMac[4], ack_data->bridgedMac[5]
		);
		sprintf(pWebVar->MPDU_ACKD, "%lld", ack_data->MPDU_ACKD);
		sprintf(pWebVar->MPDU_COLL, "%lld", ack_data->MPDU_COLL);
		sprintf(pWebVar->MPDU_FAIL, "%lld", ack_data->MPDU_FAIL);
		sprintf(pWebVar->PBS_PASS, "%lld", ack_data->PBS_PASS);
		sprintf(pWebVar->PBS_FAIL, "%lld", ack_data->PBS_FAIL);
	}
		
	return pWebVar->diagResult;
}

int http2cmm_doWListCtrlSettings( PWEB_NTWK_VAR pWebVar )
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = pWebVar->wecWlistStatus?CMM_DO_WLIST_CONTROL:CMM_UNDO_WLIST_CONTROL;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}
	return __http2cmm_comm(buf, len);
}

int http2cmm_doSpeedLimitSettings( PWEB_NTWK_VAR pWebVar )
{
	
	st_dbsProfile profile;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, pWebVar->cnuid,  &profile) )
	{
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		return CMM_FAILED;
	}

	profile.col_rxLimitSts = pWebVar->col_rxLimitSts;
	profile.col_txLimitSts = pWebVar->col_txLimitSts;
	
	if( 0 == profile.col_rxLimitSts )
	{
		profile.col_cpuPortRxRate = 0;
		profile.col_eth1rx = 0;
		profile.col_eth2rx = 0;
		profile.col_eth3rx = 0;
		profile.col_eth4rx = 0;
	}
	else
	{
		profile.col_cpuPortRxRate = __mapSelectIndex2Rate(pWebVar->col_cpuPortRxRate);
		profile.col_eth1rx = __mapSelectIndex2Rate(pWebVar->col_eth1rx);
		profile.col_eth2rx = __mapSelectIndex2Rate(pWebVar->col_eth2rx);
		profile.col_eth3rx = __mapSelectIndex2Rate(pWebVar->col_eth3rx);
		profile.col_eth4rx = __mapSelectIndex2Rate(pWebVar->col_eth4rx);
	}

	if( 0 == profile.col_txLimitSts )
	{
		profile.col_cpuPortTxRate = 0;
		profile.col_eth1tx = 0;
		profile.col_eth2tx = 0;
		profile.col_eth3tx = 0;
		profile.col_eth4tx = 0;
	}
	else
	{
		profile.col_cpuPortTxRate = __mapSelectIndex2Rate(pWebVar->col_cpuPortTxRate);
		profile.col_eth1tx = __mapSelectIndex2Rate(pWebVar->col_eth1tx);
		profile.col_eth2tx = __mapSelectIndex2Rate(pWebVar->col_eth2tx);
		profile.col_eth3tx = __mapSelectIndex2Rate(pWebVar->col_eth3tx);
		profile.col_eth4tx = __mapSelectIndex2Rate(pWebVar->col_eth4tx);
	}
	
	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_FLOWCONTROL;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}

	memcpy(req->BUF, &profile, req->HEADER.ulBodyLength);
	
	return __http2cmm_comm(buf, len);
}

int http2cmm_doShutdownSettings( PWEB_NTWK_VAR pWebVar )
{
	st_dbsProfile profile;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;	

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, pWebVar->cnuid,  &profile) )
	{
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		return CMM_FAILED;
	}
	
	profile.col_psctlSts = 1;

	profile.col_cpuPortSts = __getCnuPortStatus(profile.id, 0);
	profile.col_eth1sts = pWebVar->col_eth1sts;
	profile.col_eth2sts = pWebVar->col_eth2sts;
	profile.col_eth3sts = pWebVar->col_eth3sts;
	profile.col_eth4sts = pWebVar->col_eth4sts;	
	
	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_SHUTDOWN;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}

	memcpy(req->BUF, &profile, req->HEADER.ulBodyLength);
	
	return __http2cmm_comm(buf, len);
}

int http2cmm_doCnuVlanSettings( PWEB_NTWK_VAR pWebVar )
{
	st_dbsProfile profile;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, pWebVar->cnuid,  &profile) )
	{
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		return CMM_FAILED;
	}

	profile.col_vlanSts = pWebVar->col_vlanSts;
	if( profile.col_vlanSts == 0 )
	{
		profile.col_eth1vid = 1;
		profile.col_eth2vid = 1;
		profile.col_eth3vid = 1;
		profile.col_eth4vid = 1;
		pWebVar->col_eth1vid = 1;
		pWebVar->col_eth2vid = 1;
		pWebVar->col_eth3vid = 1;
		pWebVar->col_eth4vid = 1;
	}
	else
	{
		profile.col_eth1vid = pWebVar->col_eth1vid;
		profile.col_eth2vid = pWebVar->col_eth2vid;
		profile.col_eth3vid = pWebVar->col_eth3vid;
		profile.col_eth4vid = pWebVar->col_eth4vid;
	}

	/* 防止端口PVID 被设置为0 */
	if( 0 == profile.col_eth1vid )
	{
		profile.col_eth1vid = 1;
		pWebVar->col_eth1vid = 1;
	}
	if( 0 == profile.col_eth2vid )
	{
		profile.col_eth2vid = 1;
		pWebVar->col_eth2vid = 1;
	}
	if( 0 == profile.col_eth3vid )
	{
		profile.col_eth3vid = 1;
		pWebVar->col_eth3vid = 1;
	}
	if( 0 == profile.col_eth4vid )
	{
		profile.col_eth4vid = 1;
		pWebVar->col_eth4vid = 1;
	}

	/* CHECK VID */
	if( (profile.col_eth1vid < 1) || (profile.col_eth1vid > 4094) )
	{
		return CMM_FAILED;
	}
	if( (profile.col_eth2vid < 1) || (profile.col_eth2vid > 4094) )
	{
		return CMM_FAILED;
	}
	if( (profile.col_eth3vid < 1) || (profile.col_eth3vid > 4094) )
	{
		return CMM_FAILED;
	}
	if( (profile.col_eth4vid < 1) || (profile.col_eth4vid > 4094) )
	{
		return CMM_FAILED;
	}	
	
	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_VLAN_CONFIG;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}

	memcpy(req->BUF, &profile, req->HEADER.ulBodyLength);
	
	return __http2cmm_comm(buf, len);
}

int http2cmm_doSFilterSettings( PWEB_NTWK_VAR pWebVar )
{
	st_dbsProfile profile;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, pWebVar->cnuid,  &profile) )
	{
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		return CMM_FAILED;
	}
	
	profile.col_sfbSts = pWebVar->col_sfbSts;
	profile.col_sfuSts = pWebVar->col_sfuSts;
	profile.col_sfmSts = pWebVar->col_sfmSts;
	if(profile.col_sfbSts|profile.col_sfuSts|profile.col_sfmSts)
	{
		profile.col_sfRate = 1;
	}
	else
	{
		profile.col_sfRate = 0;
	}
	
	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_STROMCONTROL;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}

	memcpy(req->BUF, &profile, req->HEADER.ulBodyLength);
	
	return __http2cmm_comm(buf, len);
}

int http2cmm_doAgTimeSettings( PWEB_NTWK_VAR pWebVar )
{
	st_dbsProfile profile;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, pWebVar->cnuid,  &profile) )
	{
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		return CMM_FAILED;
	}
	
	profile.col_loagTime = pWebVar->col_loagTime;
	profile.col_reagTime = pWebVar->col_reagTime;
	
	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_DO_AGING_TIME_CONFIG;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}

	memcpy(req->BUF, &profile, req->HEADER.ulBodyLength);
	
	return __http2cmm_comm(buf, len);
}

int http2cmm_doMacLimiting( PWEB_NTWK_VAR pWebVar )
{
	st_dbsProfile profile;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, pWebVar->cnuid,  &profile) )
	{
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		return CMM_FAILED;
	}
	
	profile.col_macLimit = pWebVar->col_macLimit;
	
	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_DO_MAC_LIMIT_CONFIG;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}

	memcpy(req->BUF, &profile, req->HEADER.ulBodyLength);
	
	return __http2cmm_comm(buf, len);
}

int http2cmm_createCnu( PWEB_NTWK_VAR pWebVar )
{
	uint8_t bMac[6] = {0};	
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CREATE_CNU;
	req->HEADER.ulBodyLength = 6;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		return CMM_FAILED;
	}
	
	if( 0 != boardapi_macs2b(pWebVar->newCnuMac, bMac) )
	{
		return CMM_FAILED;
	}

	memcpy(req->BUF, bMac, req->HEADER.ulBodyLength);

	return __http2cmm_comm(buf, len);
}

int http2cmm_sysReboot(void)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CBAT_RESET;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __http2cmm_comm(buf, len);
}

int http2cmm_restoreDefault(void)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_RESTORE_DEFAULT;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __http2cmm_comm(buf, len);
}

int http2cmm_getEth1Propety(PWEB_NTWK_VAR pWebVar)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_CMM_PORT_PROPETY_INFO *ack_data = (T_CMM_PORT_PROPETY_INFO *)(ack->BUF);

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_IP175D_PORT_PROPETY;
	req->HEADER.ulBodyLength = sizeof(uint32_t);
	req->HEADER.fragment = 0;

	*req_data = PORT_ETH1_PORT_ID;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( CMM_SUCCESS == __http2cmm_comm(buf, len) )
	{
		pWebVar->eth1speed = ack_data->speed;
		pWebVar->eth1duplex = ack_data->duplex;
		pWebVar->eth1pri = ack_data->pri;
		pWebVar->eth1fc = ack_data->flowControl;
		pWebVar->eth1linksts = ack_data->linkStatus;
		pWebVar->eth1sts = ack_data->portStatus;
	}
	else
	{
		pWebVar->eth1speed = SPEED_AUTO_NEGOTIATION;
		pWebVar->eth1duplex = DUPLEX_AUTO_NEGOTIATION;
		pWebVar->eth1pri = 0;
		pWebVar->eth1fc = BOOL_FALSE;
		pWebVar->eth1linksts = PORT_LINK_DOWN;
		pWebVar->eth1sts = BOOL_TRUE;
	}
	return CMM_SUCCESS;
}

int http2cmm_getEth2Propety(PWEB_NTWK_VAR pWebVar)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_CMM_PORT_PROPETY_INFO *ack_data = (T_CMM_PORT_PROPETY_INFO *)(ack->BUF);

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_IP175D_PORT_PROPETY;
	req->HEADER.ulBodyLength = sizeof(uint32_t);
	req->HEADER.fragment = 0;

	*req_data = PORT_ETH2_PORT_ID;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( CMM_SUCCESS == __http2cmm_comm(buf, len) )
	{
		pWebVar->eth2speed = ack_data->speed;
		pWebVar->eth2duplex = ack_data->duplex;
		pWebVar->eth2pri = ack_data->pri;
		pWebVar->eth2fc = ack_data->flowControl;
		pWebVar->eth2linksts = ack_data->linkStatus;
		pWebVar->eth2sts = ack_data->portStatus;
	}
	else
	{
		pWebVar->eth2speed = SPEED_AUTO_NEGOTIATION;
		pWebVar->eth2duplex = DUPLEX_AUTO_NEGOTIATION;
		pWebVar->eth2pri = 0;
		pWebVar->eth2fc = BOOL_FALSE;
		pWebVar->eth2linksts = PORT_LINK_DOWN;
		pWebVar->eth2sts = BOOL_TRUE;
	}
	return CMM_SUCCESS;
}

int http2cmm_getCable1Propety(PWEB_NTWK_VAR pWebVar)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_CMM_PORT_PROPETY_INFO *ack_data = (T_CMM_PORT_PROPETY_INFO *)(ack->BUF);

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_IP175D_PORT_PROPETY;
	req->HEADER.ulBodyLength = sizeof(uint32_t);
	req->HEADER.fragment = 0;

	*req_data = PORT_CABLE1_PORT_ID;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( CMM_SUCCESS == __http2cmm_comm(buf, len) )
	{
		pWebVar->eth3speed = ack_data->speed;
		pWebVar->eth3duplex = ack_data->duplex;
		pWebVar->eth3pri = ack_data->pri;
		pWebVar->eth3fc = ack_data->flowControl;
		pWebVar->eth3linksts = ack_data->linkStatus;
		pWebVar->eth3sts = ack_data->portStatus;
	}
	else
	{
		pWebVar->eth3speed = SPEED_AUTO_NEGOTIATION;
		pWebVar->eth3duplex = DUPLEX_AUTO_NEGOTIATION;
		pWebVar->eth3pri = 0;
		pWebVar->eth3fc = BOOL_FALSE;
		pWebVar->eth3linksts = PORT_LINK_DOWN;
		pWebVar->eth3sts = BOOL_TRUE;
	}
	return CMM_SUCCESS;
}

int http2cmm_getCable2Propety(PWEB_NTWK_VAR pWebVar)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_CMM_PORT_PROPETY_INFO *ack_data = (T_CMM_PORT_PROPETY_INFO *)(ack->BUF);

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_IP175D_PORT_PROPETY;
	req->HEADER.ulBodyLength = sizeof(uint32_t);
	req->HEADER.fragment = 0;

	*req_data = PORT_CABLE2_PORT_ID;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( CMM_SUCCESS == __http2cmm_comm(buf, len) )
	{
		pWebVar->eth4speed = ack_data->speed;
		pWebVar->eth4duplex = ack_data->duplex;
		pWebVar->eth4pri = ack_data->pri;
		pWebVar->eth4fc = ack_data->flowControl;
		pWebVar->eth4linksts = ack_data->linkStatus;
		pWebVar->eth4sts = ack_data->portStatus;
	}
	else
	{
		pWebVar->eth4speed = SPEED_AUTO_NEGOTIATION;
		pWebVar->eth4duplex = DUPLEX_AUTO_NEGOTIATION;
		pWebVar->eth4pri = 0;
		pWebVar->eth4fc = BOOL_FALSE;
		pWebVar->eth4linksts = PORT_LINK_DOWN;
		pWebVar->eth4sts = BOOL_TRUE;
	}
	return CMM_SUCCESS;
}

int http2cmm_getPortPropetyAll(PWEB_NTWK_VAR pWebVar)
{
	http2cmm_getEth1Propety(pWebVar);
	http2cmm_getEth2Propety(pWebVar);
	http2cmm_getCable1Propety(pWebVar);
	http2cmm_getCable2Propety(pWebVar);
	return CMM_SUCCESS;
}

int http2cmm_getCbatTemperature(st_temperature *temp_data)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_CMM_PORT_PROPETY_INFO *ack_data = (T_CMM_PORT_PROPETY_INFO *)(ack->BUF);

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_CBAT_TEMPERATURE;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	if( CMM_SUCCESS == __http2cmm_comm(buf, len) )
	{
		memcpy(temp_data, ack_data, sizeof(st_temperature));
		return CMM_SUCCESS;
	}	
	return CMM_FAILED;
}

int http2cmm_upgrade(void)
{
	return CMM_FAILED;
}

int http2cmm_destroy(void)
{
	T_UDP_SK_INFO *sk = &SK_HTTP2CMM;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int http2cmm_init(void)
{
	if( ( SK_HTTP2CMM.sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_FAILED;
	}	

	SK_HTTP2CMM.skaddr.sin_family = PF_INET;
	SK_HTTP2CMM.skaddr.sin_port = htons(CMM_LISTEN_PORT);		/* 目的端口号*/
	SK_HTTP2CMM.skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	return CMM_SUCCESS;
} 

