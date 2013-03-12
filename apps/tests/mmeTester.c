/****************************************************************************************
*	文件名:tmmead.c
*	功能:mmead 模块的测试程序
*	作者:frank
*	时间:2010-11-30
*
*****************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <linux/if.h>
#include <public.h>
#include <boardapi.h>

uint8_t cltMac[6] = {0};
uint8_t oda_cnu[6] = {0x30, 0x71, 0xb2, 0x00, 0x00, 0x10};
//uint8_t oda_clt[6] = {0x30, 0x71, 0xb2, 0x00, 0x00, 0x01};

void print_usage(void)
{
	printf("\nParameter error:\n");
	printf("\nUsage:\n");
	printf("mmeTester debug {on|off}\n");
	printf("	--on:	turn on mmead module debug massage\n");
	printf("	--off:	turn on mmead module debug massage\n");	
	printf("mmeTester test {1|2|...} <mac>\n");
	printf("	--1:	test mmead case [MMEAD_READ_MODULE_OPERATION]\n");
	printf("	--2:	test mmead case [MMEAD_WRITE_MODULE_OPERATION]\n");
	printf("	--3:	test mmead case [MMEAD_GET_CLT_MAC]\n");
	printf("	--4:	test mmead case [MMEAD_GET_TOPOLOGY]\n");
	printf("	--5:	test mmead case [MMEAD_GET_SOFTWARE_VERSION]\n");
	printf("	--6:	test mmead case [MMEAD_GET_MANUFACTURER_INFO]\n");
	printf("	--7:	test mmead case [MMEAD_READ_MODULE_OPERATION_CRC]\n");
	printf("	--8:	test mmead case [MMEAD_RESET_DEV]\n");
	printf("	--9:	test mmead case [MMEAD_READ_PIB_CRC]\n");
	printf("	--10:	test mmead case [MMEAD_WRITE_PIB]\n");
	printf("	--11:	test mmead case [MMEAD_BOOT_OUT_CNU]\n");
	printf("	--12:	test mmead case [MMEAD_GET_TOPOLOGY] [LOOP]\n");
	printf("	--13:	test mmead case [MMEAD_LINK_DIAG:RX] \n");
	printf("	--14:	test mmead case [MMEAD_LINK_DIAG:TX] \n");
	printf("	--15:	test mmead read pib from cnu \n");
	printf("\n\n");
}

int init_socket(T_UDP_SK_INFO *sk)
{
	/*创建外部SOCKET接口*/
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		printf("\ncreate socket error, exited !\n");
		return -1;
	}
	
	sk->skaddr.sin_family = PF_INET;
	sk->skaddr.sin_port = htons(MMEAD_LISTEN_PORT);
	sk->skaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	return 0;
}

int close_socket(T_UDP_SK_INFO *sk)
{
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return 0;
}

int gen_mod_file_path(uint8_t *path, const uint8_t ODA[])
{
	//uint8_t path_mac[32] = {0};
	sprintf(path, "/var/tmp/%02X_%02X_%02X_%02X_%02X_%02X.BIN", 
		ODA[0], ODA[1], ODA[2], ODA[3], ODA[4], ODA[5]);
	//debug_printf("gen_mod_file_path = [%s]\n", path);
	return 0;
}

int msg_communicate(T_UDP_SK_INFO *sk, uint8_t *buf, uint32_t len)
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

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		perror("MOD tester call sendto error, continue.\n");
		return -1;
	}
	
	// initialize file descriptor set
	FD_ZERO(&fdsr);
	FD_SET(sk->sk, &fdsr);

	// timeout setting
	tv.tv_sec = 60;
	tv.tv_usec = 0;
	
	maxsock = sk->sk;

	ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
	if( ret <= 0 )
	{
		printf("Select time out\n");
		return -1;
	}
	
	// check whether a new connection comes
	if (FD_ISSET(sk->sk, &fdsr))
	{
		FromAddrSize = sizeof(from);
		rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);

		if ( -1 == rev_len )
		{
			perror("DB tester call recvfrom error, continue.\n");
			return -1;
		}
		else
		{
			r = (T_REQ_Msg_MMEAD *)buf;
			if( MMEAD_MSG_ID != r->HEADER.M_TYPE )
			{
				perror("Non-matched msg revieved by MMEAD tester, skip.\n");
				return -1;
			}
			
			if( r->result )
			{
				printf("MOD tester recieved server apply: result = %d\n", r->result);
			}
			return r->result;
		}			
	}
	else
	{
		printf("fd is not setted, continue\n");
		return -1;
	}
}

int TEST_MODULE_OPERATION_READ(T_UDP_SK_INFO *sk)
{
	FILE *fp;
	uint8_t MOD_PATH[64] = {0};
	T_Msg_Header_MMEAD h;
	
	uint8_t buf[MAX_UDP_SIZE];
	T_MMEAD_RD_MOD_REQ_INFO p_mod_date; 
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;
	T_MMEAD_RD_MOD_ACK_INFO *comfirm = (T_MMEAD_RD_MOD_ACK_INFO *)(r->BUF);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_READ_MODULE_OPERATION;
	h.fragment = 0;
	memcpy(h.ODA, oda_cnu, sizeof(oda_cnu));
	h.LEN = sizeof(p_mod_date);

	p_mod_date.MODULE_ID = 0x1000;
	p_mod_date.MODULE_SUB_ID = 0;
	p_mod_date.LENGTH = 1024;
	p_mod_date.OFFSET = 0;	

	memcpy(buf, &h, sizeof(h));
	memcpy(buf+sizeof(h), &p_mod_date, sizeof(p_mod_date));
	if( 0 == msg_communicate(sk, buf, sizeof(h)+sizeof(p_mod_date)) )
	{
		printf("Read Module Data Len : %x\n", comfirm->LENGTH);
		gen_mod_file_path(MOD_PATH, oda_cnu);
		if( (fp = fopen(MOD_PATH, "wb+")) < 0 )
		{
			perror("\r\n  create mod Error!");
			return 1;
		}

		if(fwrite(comfirm->MODULE_DATA, comfirm->LENGTH, 1, fp) != 1)
		{
			perror("\r\n  Pib Write Error!");
			
			return 1;
		}

		fclose(fp);
		return 0;
	}

	return -1;
}

int TEST_MMEAD_GET_MOD_CRC(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	
	uint8_t buf[MAX_UDP_SIZE];
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_READ_MODULE_OPERATION_CRC;
	h.fragment = 0;
	memcpy(h.ODA, oda_cnu, sizeof(oda_cnu));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		printf("Read Module Data CRC : 0x%x\n", *(uint32_t *)(r->BUF));
		return 0;
	}

	return -1;
}

int TEST_MMEAD_GET_PIB_CRC(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	
	uint8_t buf[MAX_UDP_SIZE];
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_READ_PIB_CRC;
	h.fragment = 0;
	memcpy(h.ODA, oda_cnu, sizeof(oda_cnu));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		printf("Read PIB CRC : 0x%x\n", *(uint32_t *)(r->BUF));
		return 0;
	}

	return -1;
}

int TEST_MMEAD_READ_PIB(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	
	uint8_t buf[MAX_UDP_SIZE];
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_READ_PIB;
	h.fragment = 0;
	memcpy(h.ODA, oda_cnu, sizeof(oda_cnu));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		printf("\n  Success\n");
		return 0;
	}
	else
	{
		printf("\n  Failed\n");
		return -1;
	}	
}

int TEST_MODULE_OPERATION_WRITE(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	int r_success,r_failed,r_count;
	//T_MMEAD_WR_MOD_REQ_INFO p_mod_date;
	
	uint8_t buf[MAX_UDP_SIZE];
	/*
	uint8_t ar8236_mii_phy_mode_init_data[76] = {
		0x01, 0x03, 0x85, 0x81, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xAD, 0x3F, 0x00, 0xFF, 0xFF, 0x05, 0xAF,
		0x3F, 0x7E, 0xFF, 0xFF, 0x85, 0x81, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0x85, 0x00, 0x05, 0xFF, 0xFF,
		0x05, 0x87, 0x00, 0x00, 0xFF, 0xFF, 0x85, 0x81, 0x00, 0x00, 0xFF, 0xFF, 0x45, 0x81, 0x7D, 0x00,
		0xFF, 0xFF, 0x45, 0x83, 0x00, 0x00, 0xFF, 0xFF, 0x85, 0x81, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xB1,
		0xF2, 0x05, 0xFF, 0xFF, 0x05, 0xB3, 0xF0, 0x19, 0xFF, 0xFF, 0x00, 0x00		
	};*/
	 
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;
	//T_MMEAD_WR_MOD_ACK_INFO *comfirm = (T_MMEAD_WR_MOD_ACK_INFO *)(r->BUF);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_WRITE_MODULE_OPERATION;
	h.fragment = 0;
	memcpy(h.ODA, oda_cnu, sizeof(oda_cnu));
	//h.LEN = sizeof(p_mod_date);
	h.LEN = 0;
	//printf("h.LEN = %d\n", h.LEN);
	/*
	p_mod_date.MODULE_ID = 0x1000;
	p_mod_date.MODULE_SUB_ID = 0;
	p_mod_date.MODULE_LENGTH = 76;

	if (p_mod_date.MODULE_LENGTH > 1400) 
	{
		printf("lseek error 1!\n");
	}
	if (p_mod_date.MODULE_LENGTH % sizeof (uint32_t)) 
	{
		printf("lseek error 2!\n");
	}
	*/
	
	//memcpy(p_mod_date.MODULE_DATA, ar8236_mii_phy_mode_init_data, 76);
	memcpy(buf, &h, sizeof(h));
	//memcpy(buf+sizeof(h), &p_mod_date, sizeof(p_mod_date));
	for(r_count = 0; r_count<1000; r_count++)
	{
		memcpy(buf, &h, sizeof(h));
		printf("=========> : %d\n", r_count);
		if( 0 == msg_communicate(sk, buf, sizeof(h)) )
		{
			printf("Write Module Data status : %d\n", r->result);
			if(r->result == 0)
			{
				r_success++;
			}
			else
			{
				r_failed++;
			}
			
		}
		else
		{
			//continue;
		}
		sleep(15);
		
	}
	
	printf("success : %d\n failed:%d\n", r_success,r_failed);
	return -1;
}

int TEST_MMEAD_GET_CLT_MAC(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];

	uint8_t oda[6] = {0x00, 0xb0, 0x52, 0x00, 0x00, 0x01};
	
	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_CLT_MAC;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		r = (T_REQ_Msg_MMEAD *)buf;
		memcpy(cltMac, r->BUF, 6);
		printf("TEST_MMEAD_GET_CLT_MAC : MAC = [%02X:%02X:%02X:%02X:%02X:%02X]\n",
						cltMac[0], cltMac[1], cltMac[2], cltMac[3], cltMac[4], cltMac[5]);
		return 0;
	}
	return -1;
}

int TEST_MMEAD_GET_TOPOLOGY(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];
	struct timeval start, end;
	
	/* 先获取线卡的MAC 地址*/
	TEST_MMEAD_GET_CLT_MAC(sk);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_TOPOLOGY;
	h.fragment = 0;
	memcpy(h.ODA, cltMac, 6);
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));

	start.tv_sec = 0;
	start.tv_usec = 0;
	end.tv_sec = 0;
	end.tv_usec = 0;

	gettimeofday( &start, NULL );
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		gettimeofday( &end, NULL );
		r = (T_REQ_Msg_MMEAD *)buf;
		T_MMEAD_TOPOLOGY *l = (T_MMEAD_TOPOLOGY *)(r->BUF);
		int i = 0;
		//printf("TEST_MMEAD_GET_TOPOLOGY\n");
		printf("TEST_MMEAD_GET_TOPOLOGY: [Time Used: %d Seconds %ul Microseconds]\n", 
			(int)(end.tv_sec - start.tv_sec),
			(uint32_t)(end.tv_usec - start.tv_usec)
		);
		printf("==========================================================================\n");
		printf( "clt.Mac = [%02X:%02X:%02X:%02X:%02X:%02X], clt.NumStas = [%d], clt.DevType = [%d]\n", 
			l->clt.Mac[0], l->clt.Mac[1], l->clt.Mac[2], l->clt.Mac[3], l->clt.Mac[4], l->clt.Mac[5], 
			l->clt.NumStas, l->clt.DevType );
		
		if( l->clt.NumStas > 0 )
		{
			for( i=0; i<l->clt.NumStas; i++ )
			{
				printf( "	-- cnu[%d].Mac = [%02X:%02X:%02X:%02X:%02X:%02X], TX/RX = [%d/%d], DevType = [%d]\n", 
					i, 
					l->cnu[i].Mac[0], l->cnu[i].Mac[1], l->cnu[i].Mac[2], 
					l->cnu[i].Mac[3], l->cnu[i].Mac[4], l->cnu[i].Mac[5], 
					l->cnu[i].AvgPhyTx, l->cnu[i].AvgPhyRx, l->cnu[i].DevType );
			}
		}
		printf("==========================================================================\n");
		return 0;
	}
	else
	{
		gettimeofday( &end, NULL );
		printf("TEST_MMEAD_GET_TOPOLOGY: [Time Used: %d Seconds %ul Microseconds]\n", 
			(int)(end.tv_sec - start.tv_sec),
			(uint32_t)(end.tv_usec - start.tv_usec)
		);
		printf("TEST_MMEAD_GET_TOPOLOGY: Failed\n");
		return -1;
	}
}

int TEST_MMEAD_GET_TOPOLOGY_LOOP(T_UDP_SK_INFO *sk)
{
	//int i = 0;
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];
	T_MMEAD_TOPOLOGY *l = NULL;
	struct timeval start, end;
	
	/* 先获取线卡的MAC 地址*/
	TEST_MMEAD_GET_CLT_MAC(sk);

	while(1)
	{
		h.M_TYPE = 0xCC08;
		h.DEV_TYPE = WEC_3801I;
		h.MM_TYPE = MMEAD_GET_TOPOLOGY;
		h.fragment = 0;
		memcpy(h.ODA, cltMac, 6);
		h.LEN = 0;
		memcpy(buf, &h, sizeof(h));
		
		printf("=====================================================\n");
		
		start.tv_sec = 0;
		start.tv_usec = 0;
		end.tv_sec = 0;
		end.tv_usec = 0;
		
		gettimeofday( &start, NULL );
		if( 0 == msg_communicate(sk, buf, sizeof(h)) )
		{
			gettimeofday( &end, NULL );
			r = (T_REQ_Msg_MMEAD *)buf;
			l = (T_MMEAD_TOPOLOGY *)(r->BUF);			
			printf("TEST_MMEAD_GET_TOPOLOGY_LOOP: clt.NumStas = [%d]\n", l->clt.NumStas);
		}
		else
		{
			gettimeofday( &end, NULL );
			printf("TEST_MMEAD_GET_TOPOLOGY_LOOP: Failed\n");
		}
		printf("TEST_MMEAD_GET_TOPOLOGY_LOOP: [Time Used: %d Seconds %ul Microseconds]\n", 
			(int)(end.tv_sec - start.tv_sec),
			(uint32_t)(end.tv_usec - start.tv_usec)
		);
		printf("======================================================\n");
	}
	return 0;
}

int TEST_MMEAD_GET_SOFTWARE_VERSION(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_SOFTWARE_VERSION;
	h.fragment = 0;
	memcpy(h.ODA, oda_cnu, sizeof(oda_cnu));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		r = (T_REQ_Msg_MMEAD *)buf;
		printf("TEST_MMEAD_GET_SOFTWARE_VERSION : Version String = [%s]\n", r->BUF);
		return 0;
	}
	return -1;
}

int TEST_MMEAD_GET_MANUFACTURER_INFO(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_MANUFACTURER_INFO;
	h.fragment = 0;
	memcpy(h.ODA, oda_cnu, sizeof(oda_cnu));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		r = (T_REQ_Msg_MMEAD *)buf;
		printf("TEST_MMEAD_GET_MANUFACTURER_INFO : Mnufacturer Info = [%s]\n", r->BUF);
		return 0;
	}
	return -1;
}

int TEST_MMEAD_RESET_DEV(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	uint8_t buf[MAX_UDP_SIZE];

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_RESET_DEV;
	h.fragment = 0;
	memcpy(h.ODA, oda_cnu, sizeof(oda_cnu));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		printf("TEST_MMEAD_RESET_DEV : OK\n");
		return 0;
	}
	return -1;
}

int TEST_MMEAD_WRITE_PIB(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	uint8_t buf[MAX_UDP_SIZE];

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_WRITE_PIB;
	h.fragment = 0;
	memcpy(h.ODA, oda_cnu, sizeof(oda_cnu));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		printf("TEST_MMEAD_WRITE_PIB : OK\n");
		return 0;
	}
	return -1;
}

int TEST_MMEAD_BOOTOUT_DEV(T_UDP_SK_INFO *sk)
{	
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)buf;

	/* 注意填入CNU的MAC地址*/
	uint8_t oda[6] = {0x00, 0x1E, 0xE3, 0x00, 0x88, 0x38};
	
	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_BOOT_OUT_CNU;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 6;
	memcpy(buf, &h, sizeof(h));
	memcpy(msg->BUF, oda_cnu, 6);
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)+6) )
	{
		printf("TEST_MMEAD_RESET_DEV : OK\n");
		return 0;
	}
	return -1;
}

int TEST_MMEAD_LINK_DIAG_RX(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	int len = 0;

	T_Msg_MMEAD *req = (T_Msg_MMEAD *)buf;
	T_MMEAD_LINK_DIAG_INFO *req_data = (T_MMEAD_LINK_DIAG_INFO *)(req->BUF);
	
	T_REQ_Msg_MMEAD *ack = (T_REQ_Msg_MMEAD *)buf;	
	T_MMEAD_LINK_DIAG_RESULT *ack_data = (T_MMEAD_LINK_DIAG_RESULT *)(ack->BUF);	

	req->HEADER.M_TYPE = 0xCC08;
	req->HEADER.DEV_TYPE = WEC_3702I;
	req->HEADER.MM_TYPE = MMEAD_LINK_DIAG;
	req->HEADER.fragment = 0;	
	memcpy(req->HEADER.ODA, oda_cnu, sizeof(oda_cnu));
	req->HEADER.LEN = sizeof(T_MMEAD_LINK_DIAG_INFO);

	req_data->dir = 1;
	memcpy(req_data->peerNodeMac, cltMac, 6);
	memcpy(req_data->ccoMac, cltMac, 6);

	len = sizeof(req->HEADER) + req->HEADER.LEN;
	
	if( 0 == msg_communicate(sk, buf, len) )
	{
		printf("dir: %d\n", ack_data->dir);
		
		printf("mac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->mac[0], ack_data->mac[1], ack_data->mac[2], 
			ack_data->mac[3], ack_data->mac[4], ack_data->mac[5]
		);
		
		printf("tei: %d\n", ack_data->tei);
		
		printf("peerNodeMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->peerNodeMac[0], ack_data->peerNodeMac[1], ack_data->peerNodeMac[2], 
			ack_data->peerNodeMac[3], ack_data->peerNodeMac[4], ack_data->peerNodeMac[5]
		);

		printf("ccoMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->ccoMac[0], ack_data->ccoMac[1], ack_data->ccoMac[2], 
			ack_data->ccoMac[3], ack_data->ccoMac[4], ack_data->ccoMac[5]
		);
		
		printf("ccoNid: %02x%02x%02x%02x%02x%02x%02x\n", ack_data->ccoNid[0], 
			ack_data->ccoNid[1], ack_data->ccoNid[2], ack_data->ccoNid[3], 
			ack_data->ccoNid[4], ack_data->ccoNid[5], ack_data->ccoNid[6]
		);
		printf("ccoSnid: %d\n", ack_data->ccoSnid);
		printf("ccoTei: %d\n", ack_data->ccoTei);
		
		printf("tx: %d\n", ack_data->tx);
		printf("rx: %d\n", ack_data->rx);
		
		printf("bridgedMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->bridgedMac[0], ack_data->bridgedMac[1], ack_data->bridgedMac[2], 
			ack_data->bridgedMac[3], ack_data->bridgedMac[4], ack_data->bridgedMac[5]
		);
		printf("bitRate: %.2f\n", ack_data->bitRate);
		printf("attenuation: %d\n", ack_data->attenuation);
		printf("MPDU_ACKD: %lld\n", ack_data->MPDU_ACKD);
		printf("MPDU_COLL: %lld\n", ack_data->MPDU_COLL);
		printf("MPDU_FAIL: %lld\n", ack_data->MPDU_FAIL);
		printf("PBS_PASS: %lld\n", ack_data->PBS_PASS);
		printf("PBS_FAIL: %lld\n", ack_data->PBS_FAIL);
		return 0;
	}

	return -1;
}

int TEST_MMEAD_LINK_DIAG_TX(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	int len = 0;

	T_Msg_MMEAD *req = (T_Msg_MMEAD *)buf;
	T_MMEAD_LINK_DIAG_INFO *req_data = (T_MMEAD_LINK_DIAG_INFO *)(req->BUF);
	
	T_REQ_Msg_MMEAD *ack = (T_REQ_Msg_MMEAD *)buf;	
	T_MMEAD_LINK_DIAG_RESULT *ack_data = (T_MMEAD_LINK_DIAG_RESULT *)(ack->BUF);	

	req->HEADER.M_TYPE = 0xCC08;
	req->HEADER.DEV_TYPE = WEC_3702I;
	req->HEADER.MM_TYPE = MMEAD_LINK_DIAG;
	req->HEADER.fragment = 0;	
	memcpy(req->HEADER.ODA, cltMac, sizeof(cltMac));
	req->HEADER.LEN = sizeof(T_MMEAD_LINK_DIAG_INFO);

	req_data->dir = 0;
	memcpy(req_data->peerNodeMac, oda_cnu, 6);
	memcpy(req_data->ccoMac, cltMac, 6);

	len = sizeof(req->HEADER) + req->HEADER.LEN;
	
	if( 0 == msg_communicate(sk, buf, len) )
	{
		printf("dir: %d\n", ack_data->dir);
		
		printf("mac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->mac[0], ack_data->mac[1], ack_data->mac[2], 
			ack_data->mac[3], ack_data->mac[4], ack_data->mac[5]
		);
		
		printf("tei: %d\n", ack_data->tei);
		
		printf("peerNodeMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->peerNodeMac[0], ack_data->peerNodeMac[1], ack_data->peerNodeMac[2], 
			ack_data->peerNodeMac[3], ack_data->peerNodeMac[4], ack_data->peerNodeMac[5]
		);

		printf("ccoMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->ccoMac[0], ack_data->ccoMac[1], ack_data->ccoMac[2], 
			ack_data->ccoMac[3], ack_data->ccoMac[4], ack_data->ccoMac[5]
		);
		
		printf("ccoNid: %02x%02x%02x%02x%02x%02x%02x\n", ack_data->ccoNid[0], 
			ack_data->ccoNid[1], ack_data->ccoNid[2], ack_data->ccoNid[3], 
			ack_data->ccoNid[4], ack_data->ccoNid[5], ack_data->ccoNid[6]
		);
		printf("ccoSnid: %d\n", ack_data->ccoSnid);
		printf("ccoTei: %d\n", ack_data->ccoTei);
		
		printf("tx: %d\n", ack_data->tx);
		printf("rx: %d\n", ack_data->rx);
		
		printf("bridgedMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->bridgedMac[0], ack_data->bridgedMac[1], ack_data->bridgedMac[2], 
			ack_data->bridgedMac[3], ack_data->bridgedMac[4], ack_data->bridgedMac[5]
		);
		printf("bitRate: %.2f\n", ack_data->bitRate);
		printf("attenuation: %d\n", ack_data->attenuation);
		printf("MPDU_ACKD: %lld\n", ack_data->MPDU_ACKD);
		printf("MPDU_COLL: %lld\n", ack_data->MPDU_COLL);
		printf("MPDU_FAIL: %lld\n", ack_data->MPDU_FAIL);
		printf("PBS_PASS: %lld\n", ack_data->PBS_PASS);
		printf("PBS_FAIL: %lld\n", ack_data->PBS_FAIL);
		return 0;
	}

	return -1;
}

int MMEAD_MSG_DEBUG_ENABLE(T_UDP_SK_INFO *sk, uint32_t enable)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = (enable?MMEAD_MODULE_MSG_DEBUG_ENABLE:MMEAD_MODULE_MSG_DEBUG_DISABLE);
	h.fragment = 0;
	memcpy(h.ODA, oda_cnu, sizeof(oda_cnu));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		r = (T_REQ_Msg_MMEAD *)buf;
		printf("MMEAD_MSG_DEBUG_ENABLE : OK\n");
		return 0;
	}
	return -1;
}

int main(int argc, char *argv[])
{
	int cmd = 0;
	uint8_t bMac[6] = {0};
	T_UDP_SK_INFO sk;
	
	if( (argc != 3) && (argc != 4) )
	{
		print_usage();
		return 0;
	}

	if( 0 != init_socket(&sk) )
	{
		return 0;
	}
	
	if( strcmp(argv[1], "debug") == 0)
	{
		if( strcmp(argv[2], "on") == 0 )
		{
			MMEAD_MSG_DEBUG_ENABLE(&sk, 1);
		}
		else if( strcmp(argv[2], "off") == 0 )
		{
			MMEAD_MSG_DEBUG_ENABLE(&sk, 0);
		}
		else
		{
			print_usage();
		}
	}	
	else if( strcmp(argv[1], "test") == 0)
	{
		//get test cmd
		cmd = atoi(argv[2]);
		//get des mac address
		if( argc == 4)
		{
			if( CMM_SUCCESS == boardapi_macs2b(argv[3], bMac) )
			{
				memcpy(oda_cnu, bMac, sizeof(oda_cnu));
			}
			else
			{
				printf("\n  MAC address invalid\n");	
			}
		}
		switch(cmd)
		{
			case 1:
			{
				TEST_MODULE_OPERATION_READ(&sk);
				break;
			}
			case 2:
			{
				TEST_MODULE_OPERATION_WRITE(&sk);
				break;
			}
			case 3:
			{
				TEST_MMEAD_GET_CLT_MAC(&sk);
				break;
			}
			case 4:
			{
				TEST_MMEAD_GET_TOPOLOGY(&sk);
				break;
			}
			case 5:
			{
				TEST_MMEAD_GET_SOFTWARE_VERSION(&sk);
				break;
			}
			case 6:
			{
				TEST_MMEAD_GET_MANUFACTURER_INFO(&sk);
				break;
			}
			case 7:
			{
				TEST_MMEAD_GET_MOD_CRC(&sk);
				break;
			}
			case 8:
			{
				TEST_MMEAD_RESET_DEV(&sk);
				break;
			}
			case 9:
			{
				TEST_MMEAD_GET_PIB_CRC(&sk);
				break;
			}
			case 10:
			{
				TEST_MMEAD_WRITE_PIB(&sk);
				break;
			}
			case 11:
			{
				TEST_MMEAD_BOOTOUT_DEV(&sk);
				break;
			}
			case 12:
			{
				TEST_MMEAD_GET_TOPOLOGY_LOOP(&sk);
				break;
			}
			case 13:
			{
				TEST_MMEAD_LINK_DIAG_RX(&sk);
				break;
			}
			case 14:
			{
				TEST_MMEAD_LINK_DIAG_TX(&sk);
				break;
			}
			case 15:
			{
				TEST_MMEAD_READ_PIB(&sk);
				break;
			}
			default:
			{
				print_usage();
				break;
			}
		}		
	}
	else
	{
		print_usage();
	}
	close_socket(&sk);
	return 0;	
}


