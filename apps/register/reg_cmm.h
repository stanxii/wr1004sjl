#ifndef __REGISTER_CMM_MSG_H__
#define __REGISTER_CMM_MSG_H__

#include <public.h>

int reg2cmm_getCltPortLinkStatus(T_UDP_SK_INFO *sk, uint32_t cltid);
int reg2cmm_bindingAtheroesAddr2CablePort(T_UDP_SK_INFO *sk, int portid);
int reg2cmm_delAtheroesAddrFromCablePort(T_UDP_SK_INFO *sk);
int reg2cmm_init(T_UDP_SK_INFO *sk);
int reg2cmm_destroy(T_UDP_SK_INFO *sk);

#endif


