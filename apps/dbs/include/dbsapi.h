#ifndef __DBS_API_H__
#define __DBS_API_H__

#include <public.h>

int dbsSelectCnuIndexByMacAddress(char *mac, stCnuNode *index);
int dbsGetInteger(DB_INTEGER_V *v);
int dbsGetText(DB_TEXT_V *v);
int dbsGetCliRole(uint16_t id, st_dbsCliRole *row);
int dbsGetClt(uint16_t id, st_dbsClt *row);
int dbsGetCltconf(uint16_t id, st_dbsCltConf *row);
int dbsGetCnu(uint16_t id, st_dbsCnu *row);
int dbsGetDepro(uint16_t id, st_dbsCnuDefaultProfile *row);
int dbsGetNetwork(uint16_t id, st_dbsNetwork *row);
int dbsGetProfile(uint16_t id, st_dbsProfile *row);
int dbsGetSnmp(uint16_t id, st_dbsSnmp *row);
int dbsGetSwmgmt(uint16_t id, st_dbsSwmgmt *row);
int dbsGetSysinfo(uint16_t id, st_dbsSysinfo *row);
int dbsUpdateInteger(DB_INTEGER_V *v);
int dbsUpdateText(DB_TEXT_V *v);
int dbsUpdateCliRole(uint16_t id, st_dbsCliRole *row);
int dbsUpdateClt(uint16_t id, st_dbsClt *row);
int dbsUpdateCltconf(uint16_t id, st_dbsCltConf *row);
int dbsUpdateCnu(uint16_t id, st_dbsCnu *row);
int dbsUpdateDepro(uint16_t id, st_dbsCnuDefaultProfile *row);
int dbsUpdateNetwork(uint16_t id, st_dbsNetwork *row);
int dbsUpdateProfile(uint16_t id, st_dbsProfile *row);
int dbsUpdateSnmp(uint16_t id, st_dbsSnmp *row);
int dbsUpdateSwmgmt(uint16_t id, st_dbsSwmgmt *row);
int dbsUpdateSysinfo(uint16_t id, st_dbsSysinfo *row);
int dbsDestroyRowClt(uint16_t id);
int dbsDestroyRowCltconf(uint16_t id);
int dbsDestroyRowCnu(uint16_t id);
int dbsDestroyRowProfile(uint16_t id);
int dbsCreateSuProfileForCnu(uint16_t id);
int dbsCreateDewlProfileForCnu(uint16_t id);
int dbsCreateDeblProfileForCnu(uint16_t id);
int dbsCreateSuProfileForWec701Cnu(uint16_t id);
int dbsCreateDewlProfileForWec701Cnu(uint16_t id);
int dbsCreateDeblProfileForWec701Cnu(uint16_t id);
int dbsSetDsdtRgmiiDelay(st_dsdtRgmiiTimingDelay *dsdtRgmiiTimingDelay);
int dbsLogCount(uint16_t tbl, uint32_t *n);
int dbsGetSyslog(uint32_t row, st_dbsSyslog *log);
int dbsGetOptlog(uint32_t row, st_dbsOptlog *log);
int dbsGetAlarmlog(uint32_t row, st_dbsAlarmlog *log);
int dbs_sys_log(uint32_t priority, const char *message);
int dbs_opt_log(st_dbsOptlog *log);
int dbs_alarm_log(st_dbsAlarmlog *log);
int dbsRegisterModuleById(uint16_t mid);
int dbsDestroyModuleById(uint16_t mid);
void dbsWaitModule(uint32_t MF);
BOOLEAN dbsGetModuleStatus(uint16_t mid);
int dbsMsgDebug(uint32_t status);
int dbsSQLDebug(uint32_t status);
int dbsFflush(void);
int dbsNoWaitOpen(uint16_t srcMod);
int dbsOpen(uint16_t srcMod);
int dbsClose(void);

#endif

