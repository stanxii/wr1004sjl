#ifndef __CMM_MMEAD_H__
#define __CMM_MMEAD_H__

#include <public.h>

int mmead_get_ar8236_phy(uint8_t ODA[], T_szMdioPhy *v);
int mmead_set_ar8236_phy(uint8_t ODA[], T_szMdioPhy *v);
int mmead_get_ar8236_reg(uint8_t ODA[], T_szMdioSw *v);
int mmead_set_ar8236_reg(uint8_t ODA[], T_szMdioSw *v);

int mmead_do_link_diag
(
	uint8_t ODA[], 
	T_MMEAD_LINK_DIAG_INFO *inputInfo, 
	T_MMEAD_LINK_DIAG_RESULT *outputInfo
);

int destroy_cmm_mmead(void);
int init_cmm_mmead(void);

#endif


