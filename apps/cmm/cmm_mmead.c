#include <assert.h>
#include "cmm_mmead.h"

T_UDP_SK_INFO SK_CMM_MMEAD;

int __cmm_mmead_communicate(uint8_t *buf, uint32_t len)
{
	fd_set fdsr;
	int maxsock;
	struct timeval tv;
	int ret = 0;
	int sendn = 0;
	T_REQ_Msg_MMEAD *r = NULL;
	struct sockaddr_in from;
	int FromAddrSize = 0;
	int rev_len = 0;
	T_UDP_SK_INFO *sk = &SK_CMM_MMEAD;

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		fprintf(stderr, "ERROR: cmm communicate with mmead sendto failed\n");
		return -1;
	}
	
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
		fprintf(stderr, "ERROR: cmm communicate with mmead select failed\n");
		return -1;
	}
	
	// check whether a new connection comes
	if (FD_ISSET(sk->sk, &fdsr))
	{
		FromAddrSize = sizeof(from);
		rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);

		if ( -1 == rev_len )
		{
			fprintf(stderr, "ERROR: cmm communicate with mmead recvfrom failed\n");
			return -1;
		}
		else
		{
			r = (T_REQ_Msg_MMEAD *)buf;
			/*
			if( r->result )
			{
				fprintf(stderr, "ERROR: cmm communicate with mmead result = %d\n", r->result);
			}
			*/
			return r->result;
		}			
	}
	else
	{
		fprintf(stderr, "ERROR: cmm communicate with mmead FD_ISSET failed\n");
		return -1;
	}
}

int mmead_get_ar8236_phy(uint8_t ODA[], T_szMdioPhy *v)
{
	assert( NULL != v );
	
	int len = 0;
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;
	T_szMdioPhy *ar8236Phy = NULL;

	bzero(buf, MAX_UDP_SIZE);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;		/* WEC_3702I 的SWITCH芯片为AR8236 */
	h.MM_TYPE = MMEAD_AR8236_PHY_REG_READ;
	h.fragment = 0;
	memcpy(h.ODA, ODA, 6);
	h.LEN = sizeof(T_szMdioPhy);

	memcpy(buf, &h, sizeof(T_Msg_Header_MMEAD));
	memcpy(buf+sizeof(T_Msg_Header_MMEAD), v, sizeof(T_szMdioPhy));
	len = sizeof(T_Msg_Header_MMEAD) + sizeof(T_szMdioPhy);

	if( __cmm_mmead_communicate(buf, len) == CMM_SUCCESS )
	{
		ar8236Phy = (T_szMdioPhy *)(r->BUF);
		v->value = ar8236Phy->value;
		return CMM_SUCCESS;
	}
	return CMM_FAILED;
}

int mmead_set_ar8236_phy(uint8_t ODA[], T_szMdioPhy *v)
{
	assert( NULL != v );
	
	int len = 0;
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;

	bzero(buf, MAX_UDP_SIZE);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;		/* WEC_3702I 的SWITCH芯片为AR8236 */
	h.MM_TYPE = MMEAD_AR8236_PHY_REG_WRITE;
	h.fragment = 0;
	memcpy(h.ODA, ODA, 6);
	h.LEN = sizeof(T_szMdioPhy);

	memcpy(buf, &h, sizeof(T_Msg_Header_MMEAD));
	memcpy(buf+sizeof(T_Msg_Header_MMEAD), v, sizeof(T_szMdioPhy));
	len = sizeof(T_Msg_Header_MMEAD) + sizeof(T_szMdioPhy);

	return __cmm_mmead_communicate(buf, len);
}

int mmead_get_ar8236_reg(uint8_t ODA[], T_szMdioSw *v)
{
	assert( NULL != v );
	
	int len = 0;
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;
	T_szMdioSw *ar8236Reg = NULL;

	bzero(buf, MAX_UDP_SIZE);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;		/* WEC_3702I 的SWITCH芯片为AR8236 */
	h.MM_TYPE = MMEAD_AR8236_SW_REG_READ;
	h.fragment = 0;
	memcpy(h.ODA, ODA, 6);
	h.LEN = sizeof(T_szMdioSw);

	memcpy(buf, &h, sizeof(T_Msg_Header_MMEAD));
	memcpy(buf+sizeof(T_Msg_Header_MMEAD), v, sizeof(T_szMdioSw));
	len = sizeof(T_Msg_Header_MMEAD) + sizeof(T_szMdioSw);

	if( __cmm_mmead_communicate(buf, len) == CMM_SUCCESS )
	{
		ar8236Reg = (T_szMdioSw *)(r->BUF);
		v->value = ar8236Reg->value;
		return CMM_SUCCESS;
	}
	return CMM_FAILED;
}

int mmead_set_ar8236_reg(uint8_t ODA[], T_szMdioSw *v)
{
	assert( NULL != v );
	
	int len = 0;
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;

	bzero(buf, MAX_UDP_SIZE);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;		/* WEC_3702I 的SWITCH芯片为AR8236 */
	h.MM_TYPE = MMEAD_AR8236_SW_REG_WRITE;
	h.fragment = 0;
	memcpy(h.ODA, ODA, 6);
	h.LEN = sizeof(T_szMdioSw);

	memcpy(buf, &h, sizeof(T_Msg_Header_MMEAD));
	memcpy(buf+sizeof(T_Msg_Header_MMEAD), v, sizeof(T_szMdioSw));
	len = sizeof(T_Msg_Header_MMEAD) + sizeof(T_szMdioSw);

	return __cmm_mmead_communicate(buf, len);
}

int mmead_get_rtl8306e_configs(uint8_t ODA[], st_rtl8306eSettings *rtl8306e)
{
	assert( NULL != rtl8306e );
	
	int len = 0;
	uint8_t buf[MAX_UDP_SIZE];
	
	T_MMETS_REQ_MSG *MMETS_REQ = (T_MMETS_REQ_MSG *)buf;
	T_MMETS_ACK_MSG *MMETS_ACK = (T_MMETS_ACK_MSG *)buf;
	st_rtl8306eSettings *MMETS_ACK_DATA = (st_rtl8306eSettings *)(MMETS_ACK->body);
	
	MMETS_REQ->header.M_TYPE = 0xCC08;
	MMETS_REQ->header.DEV_TYPE = WEC_3702I;	
	MMETS_REQ->header.MM_TYPE = MMEAD_GET_RTL8306E_CONFIG;
	MMETS_REQ->header.fragment = 0;
	MMETS_REQ->header.LEN = 0;
	memcpy(MMETS_REQ->header.ODA, ODA, 6);

	len = sizeof(T_MMETS_REQ_MSG) + MMETS_REQ->header.LEN;

	if( 0 == __cmm_mmead_communicate(buf, len) )
	{
		memcpy(rtl8306e, MMETS_ACK_DATA, sizeof(st_rtl8306eSettings));
		return CMM_SUCCESS;
	}
	return CMM_FAILED;
}

int mmead_write_rtl8306e_mod(uint8_t ODA[], uint8_t *mod, uint32_t mod_len)
{
	assert( NULL != mod );
	
	int len = 0;
	uint8_t buf[MAX_UDP_SIZE];
	
	T_MMETS_REQ_MSG *MMETS_REQ = (T_MMETS_REQ_MSG *)buf;
	T_MMETS_ACK_MSG *MMETS_ACK = (T_MMETS_ACK_MSG *)buf;
	
	
	MMETS_REQ->header.M_TYPE = 0xCC08;
	MMETS_REQ->header.DEV_TYPE = WEC_3702I;	
	MMETS_REQ->header.MM_TYPE = MMEAD_WRITE_MOD;
	MMETS_REQ->header.fragment = 0;
	MMETS_REQ->header.LEN = mod_len;
	memcpy(MMETS_REQ->header.ODA, ODA, 6);

	memcpy(MMETS_REQ->body, mod, mod_len);

	len = sizeof(T_MMETS_REQ_MSG) + MMETS_REQ->header.LEN;

	return __cmm_mmead_communicate(buf, len);
}


int mmead_get_rtl8306e_register(uint8_t ODA[], T_szSwRtl8306eConfig *pRegInfo)
{
	//uint32 rdata;
	int ret = CMM_FAILED;
	T_szMdioPhy mdioInfo;
	pRegInfo->mdioInfo.value = 0x0000;

	if ((pRegInfo->mdioInfo.phy >= 7) || (pRegInfo->mdioInfo.page >= 5))
	{
		printf("\n#ERROR[10]\n");
		return CMM_FAILED;
	}

	/* Select PHY Register Page through configuring PHY 0 Register 16 [bit1 bit15] */
	mdioInfo.phy = 0;
	mdioInfo.reg = 16;
	if( CMM_SUCCESS != mmead_get_ar8236_phy(ODA, &mdioInfo) )
	{
		printf("\n#ERROR[11]\n");
		return CMM_FAILED;
	}
	
	switch (pRegInfo->mdioInfo.page) 
	{
		case 0:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = (mdioInfo.value & 0x7FFF) | 0x0002;
			ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
			if(ret)
			{
				printf("\n#ERROR[12]\n");
			}
			break;
		}
		case 1:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = mdioInfo.value | 0x8002;
			ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
			if(ret)
			{
				printf("\n#ERROR[13]\n");
			}
			break;
		}
		case 2:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = mdioInfo.value & 0x7FFD;
			ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
			if(ret)
			{
				printf("\n#ERROR[14]\n");
			}
			break;
		}
		case 3:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = (mdioInfo.value & 0xFFFD) | 0x8000;
			ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
			if(ret)
			{
				printf("\n#ERROR[15]\n");
			}
			break;
		}
		case 4:
		{
			mdioInfo.phy = 5;
			mdioInfo.reg = 16;
			if( CMM_SUCCESS == mmead_get_ar8236_phy(ODA, &mdioInfo) )
			{
				mdioInfo.value |= 0x2;
				ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
				if(ret)
				{
					printf("\n#ERROR[16]\n");
				}
			}
			else
			{
				printf("\n#ERROR[17]\n");
				ret = CMM_FAILED;
			}
			break;
		}
		default:
		{
			printf("\n#ERROR[18]\n");
			return CMM_FAILED;
		}
	}

	if( CMM_SUCCESS != ret )
	{
		return CMM_FAILED;
	}
	
	//read register in selected page
	mdioInfo.phy = pRegInfo->mdioInfo.phy;
	mdioInfo.reg = pRegInfo->mdioInfo.reg;
	if( CMM_SUCCESS == mmead_get_ar8236_phy(ODA, &mdioInfo) )
	{
		pRegInfo->mdioInfo.value = mdioInfo.value & 0xFFFF;
	}
	else
	{
		printf("\n#ERROR[19]\n");
	}
	if(4 == pRegInfo->mdioInfo.page)
	{
		/*exit page 4*/
		mdioInfo.phy = 5;
		mdioInfo.reg = 16;
		if( CMM_SUCCESS == mmead_get_ar8236_phy(ODA, &mdioInfo) )
		{
			mdioInfo.value &= ~0x2;
			ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
			if(ret)
			{
				printf("\n#ERROR[20]\n");
			}
		}
	} 
	if(ret)
	{
		printf("\n#ERROR[21]\n");
	}
	return ret;	
}

int mmead_set_rtl8306e_register(uint8_t ODA[], T_szSwRtl8306eConfig *pRegInfo)
{
	int ret = CMM_FAILED;
	T_szMdioPhy mdioInfo;

	if ((pRegInfo->mdioInfo.phy >= 7) || (pRegInfo->mdioInfo.page >= 5))
	{
		return CMM_FAILED;
	}

	/* Select PHY Register Page through configuring PHY 0 Register 16 [bit1 bit15] */
	pRegInfo->mdioInfo.value  &= 0xFFFF;

	mdioInfo.phy = 0;
	mdioInfo.reg = 16;
	if( CMM_SUCCESS != mmead_get_ar8236_phy(ODA, &mdioInfo) )
	{
		return CMM_FAILED;
	}

	switch (pRegInfo->mdioInfo.page) 
	{
		case 0:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = (mdioInfo.value & 0x7FFF) | 0x0002;
			ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
			break;
		}
		case 1:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = mdioInfo.value | 0x8002;
			ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
			break;
		}
		case 2:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = mdioInfo.value & 0x7FFD;
			ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
			break;
		}
		case 3:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = (mdioInfo.value & 0xFFFD) | 0x8000;
			ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
			break;
		}
		case 4:
		{
			mdioInfo.phy = 5;
			mdioInfo.reg = 16;
			if( CMM_SUCCESS == mmead_get_ar8236_phy(ODA, &mdioInfo) )
			{
				mdioInfo.value |= 0x2;
				ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
			}
			else
			{
				ret = CMM_FAILED;
			}
			break;
		}
		default:
		{
			return CMM_FAILED;
		}
	}

	if( CMM_SUCCESS != ret )
	{
		return CMM_FAILED;
	}
	
	//write register in selected page
	mdioInfo.phy = pRegInfo->mdioInfo.phy;
	mdioInfo.reg = pRegInfo->mdioInfo.reg;
	mdioInfo.value = pRegInfo->mdioInfo.value;
	ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
	
	if(4 == pRegInfo->mdioInfo.page)
	{
		/*exit page 4*/
		mdioInfo.phy = 5;
		mdioInfo.reg = 16;
		if( CMM_SUCCESS == mmead_get_ar8236_phy(ODA, &mdioInfo) )
		{
			mdioInfo.value &= ~0x2;
			ret = mmead_set_ar8236_phy(ODA, &mdioInfo);
		}
	}        
	return ret;	
}

int mmead_do_link_diag
(
	uint8_t ODA[], 
	T_MMEAD_LINK_DIAG_INFO *inputInfo, 
	T_MMEAD_LINK_DIAG_RESULT *outputInfo
)
{
	assert( NULL != inputInfo );
	assert( NULL != outputInfo );	
	
	uint8_t buf[MAX_UDP_SIZE];
	int len = 0;

	T_Msg_MMEAD *req = (T_Msg_MMEAD *)buf;
	T_REQ_Msg_MMEAD *ack = (T_REQ_Msg_MMEAD *)buf;

	req->HEADER.M_TYPE = 0xCC08;
	req->HEADER.DEV_TYPE = WEC_3702I;
	req->HEADER.MM_TYPE = MMEAD_LINK_DIAG;
	req->HEADER.fragment = 0;	
	memcpy(req->HEADER.ODA, ODA, 6);
	req->HEADER.LEN = sizeof(T_MMEAD_LINK_DIAG_INFO);

	memcpy(req->BUF, inputInfo, req->HEADER.LEN);

	len = sizeof(req->HEADER) + req->HEADER.LEN;

	if( __cmm_mmead_communicate(buf, len) == CMM_SUCCESS )
	{
		memcpy(outputInfo, ack->BUF, sizeof(T_MMEAD_LINK_DIAG_RESULT));
		return CMM_SUCCESS;
	}
	return CMM_FAILED;
}

int destroy_cmm_mmead(void)
{
	T_UDP_SK_INFO *sk = &SK_CMM_MMEAD;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int init_cmm_mmead(void)
{
	T_UDP_SK_INFO *sk = &SK_CMM_MMEAD;
	
	if( ( sk->sk = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	sk->skaddr.sin_family = AF_INET;
	sk->skaddr.sin_port = htons(MMEAD_LISTEN_PORT);		/* 目的端口号*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");		/* 目的地址*/
	return CMM_SUCCESS;
}


