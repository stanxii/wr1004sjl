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


