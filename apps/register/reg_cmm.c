/*****************************************************************************************
  文件名称 : reg_tm.c
  文件描述 : 注册模块与tm模块连接的处理函数
  修订记录 :
           1 创建 : frank
             日期 : 2010-12-07
             描述 : 创建文件

 *****************************************************************************************/
#include <assert.h>
#include "reg_cmm.h"

int __reg2cmm_comm(T_UDP_SK_INFO *sk, uint8_t *buf, uint32_t len)
{
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
			return CMM_FAILED;
		}
		// check whether a new connection comes
		if (FD_ISSET(sk->sk, &fdsr))
		{
			FromAddrSize = sizeof(from);
			rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);
			if ( -1 == rev_len )
			{
				return CMM_FAILED;
			}
			else
			{
				ack = (T_REQ_Msg_CMM *)buf;
				if( msgType != ack->HEADER.usMsgType )
				{
					fprintf(stderr, "WARNNING: sysMonitor2cmm: msgType[%d!=%d], [continue] !\n", 
						ack->HEADER.usMsgType, msgType);
					continue;
				}
				else if( ack->result != CMM_SUCCESS )
				{	
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
			return CMM_FAILED;
		}
	}
}

int reg2cmm_getCltPortLinkStatus(T_UDP_SK_INFO *sk, uint32_t cltid)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	int linkStatus = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);

	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	uint32_t *ack_data = (uint32_t *)(ack->BUF);

	req->HEADER.usSrcMID = MID_REGISTER;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_CLT_PORT_LINK_STS;
	req->HEADER.ulBodyLength = sizeof(uint32_t);
	req->HEADER.fragment = 0;

	*req_data = cltid;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( CMM_SUCCESS ==  __reg2cmm_comm(sk, buf, len) )
	{
		linkStatus = (*ack_data == 0)?0:1;
	}
	return linkStatus;
}

int reg2cmm_bindingAtheroesAddr2CablePort(T_UDP_SK_INFO *sk, int portid)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);

	req->HEADER.usSrcMID = MID_REGISTER;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_ADD_ATHEROS_ADDR;
	req->HEADER.ulBodyLength = sizeof(uint32_t);
	req->HEADER.fragment = 0;

	*req_data = portid;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __reg2cmm_comm(sk, buf, len);
}

int reg2cmm_delAtheroesAddrFromCablePort(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_REGISTER;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_DEL_ATHEROS_ADDR;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __reg2cmm_comm(sk, buf, len);
}

int reg2cmm_destroy(T_UDP_SK_INFO *sk)
{
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int reg2cmm_init(T_UDP_SK_INFO *sk)
{
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_FAILED;
	}	

	sk->skaddr.sin_family = PF_INET;
	sk->skaddr.sin_port = htons(CMM_LISTEN_PORT);		/* 目的端口号*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	return CMM_SUCCESS;
} 


