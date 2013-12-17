#ifndef __WEC_BOARD_API_H__
#define __WEC_BOARD_API_H__

#include <public.h>
#include "nvm-utils.h"
#include "hexdump.h"
#include "md5.h"

/********************************************************************************************
*	函数名称:boardapi_checkCpuEndian
*	函数功能:判断处理器的字节序是大端还是小端
*	return true: little-endian, return false: big-endian
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_checkCpuEndian(void);

/********************************************************************************************
*	函数名称:boardapi_getMacAddress
*	函数功能:获取CBAT的MAC地址
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getMacAddress(void);

/********************************************************************************************
*	函数名称:boardapi_isValidUnicastMacb
*	函数功能:判断是否为有效的单播MAC
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_isValidUnicastMacb(uint8_t *bin);

/********************************************************************************************
*	函数名称:boardapi_macs2b
*	函数功能:将字符串形式的MAC地址转换为6位二进制格式
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_macs2b(const char *str, uint8_t *bin);

/********************************************************************************************
*	函数名称:boardapi_getDeviceModelStr
*	函数功能:获取字符串表示的设备型号
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getDeviceModelStr(uint32_t model);

/********************************************************************************************
*	函数名称:boardapi_getCltStandardStr
*	函数功能:get clt serial type
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
const char *boardapi_getCltStandardStr(void);

/********************************************************************************************
*	函数名称:boardapi_getMenufactoryStr
*	函数功能:get clt serial type
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
const char *boardapi_getMenufactoryStr(void);

/********************************************************************************************
*	函数名称:boardapi_getModNameStr
*	函数功能:根据模块ID获取字符串表示的模块名称
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getModNameStr(uint16_t mid);

/********************************************************************************************
*	函数名称:boardapi_getCnuHfid
*	函数功能:根据设备型号获取烧录在PIB中存储的HFID
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
const char *boardapi_getCnuHfid(uint32_t devType);

/********************************************************************************************
*	函数名称:boardapi_isCnuSupported
*	函数功能:根据输入的设备型号判断系统是否支持该设备
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_isCnuSupported(uint32_t DevType);
int boardapi_isAr6400Device(uint32_t DevType);
int boardapi_isAr7400Device(uint32_t DevType);
int boardapi_getCnuSwitchType(uint32_t DevType);

/********************************************************************************************
*	函数名称:boardapi_mapDevModel
*	函数功能:将CBAT中定义的设备型号印射为NMS定义的设备型号
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_mapDevModel(int model);

/********************************************************************************************
*	函数名称:boardapi_umapDevModel
*	函数功能:将NMS定义的设备型号印射为CBAT中定义的设备型号
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_umapDevModel(int model);

/********************************************************************************************
*	函数名称:boardapi_getAlarmTypeStr
*	函数功能:获取字符串表示的告警类型
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getAlarmTypeStr(uint16_t alarmType);

/********************************************************************************************
*	函数名称:boardapi_getAlarmLevelByCode
*	函数功能:根据告警码获取该告警的等级
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_getAlarmLevelByCode(uint32_t alarmCode);

/********************************************************************************************
*	函数名称:boardapi_getAlarmLevelStr
*	函数功能:获取字符串表示的告警等级
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
char * boardapi_getAlarmLevelStr(uint16_t alarmLevel);

/********************************************************************************************
*	函数名称:boardapi_getAlarmLevel
*	函数功能:根据告警码获取该告警的等级
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_getAlarmLevel(st_dbsAlarmlog *alarm);

/********************************************************************************************
*	函数名称:boardapi_setMTParameters
*	函数功能:烧录NVM参数的接口函数
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int boardapi_setMTParameters(stMTmsgInfo *para);

/********************************************************************************************
*	函数名称:boardapi_getCltDsdtPortid
*	函数功能:根据CLT索引号找到其对应的交换端口
*	作者:frank
*	时间:2013-08-19
*********************************************************************************************/
uint32_t boardapi_getCltDsdtPortid(uint32_t cltid);

#endif 
