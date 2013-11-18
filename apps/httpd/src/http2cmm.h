#ifndef __HTTP_CMM_H__
#define __HTTP_CMM_H__

#include <stdio.h>
#include <string.h>
#include <public.h>
#include "cgimain.h"


int http2cmm_rebootClt(int id);
int http2cmm_reloadClt(int id);
int http2cmm_rebootCnu(int id);
int http2cmm_reloadCnu(int id);
int http2cmm_deleteCnu(int id);
int http2cmm_permitCnu(int id);
int http2cmm_undoPermitCnu(int id);
int http2cmm_doPortStas(PWEB_NTWK_VAR pWebVar);
int http2cmm_clearPortStas(PWEB_NTWK_VAR pWebVar);
int http2cmm_doLinkDiag( PWEB_NTWK_VAR pWebVar );
int http2cmm_doWListCtrlSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doSpeedLimitSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doShutdownSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doCnuVlanSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doSFilterSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doAgTimeSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doMacLimiting( PWEB_NTWK_VAR pWebVar );
int http2cmm_createCnu( PWEB_NTWK_VAR pWebVar );

int http2cmm_getPortPropetyAll(PWEB_NTWK_VAR pWebVar);
int http2cmm_getCbatTemperature(st_temperature *temp_data);
int http2cmm_sysReboot(void);
int http2cmm_restoreDefault(void);
int http2cmm_upgrade(void);

int http2cmm_readSwitchSettings(PWEB_NTWK_VAR pWebVar);
int http2cmm_writeSwitchSettings(PWEB_NTWK_VAR pWebVar);

int http2cmm_destroy(void);
int http2cmm_init(void);

#endif 


