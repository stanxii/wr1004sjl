#ifndef __CGI_MAIN_H__
#define __CGI_MAIN_H__

#include <stdio.h>
#include <fcntl.h>

#include <ifcdefs.h>

/********************** Global Types ****************************************/

#define WEB_BUF_SIZE_MAX      1024
#define WEB_MD_BUF_SIZE_MAX   264
#define WEB_BIG_BUF_SIZE_MAX  10000

#define WEB_DIAG_TYPE        0
#define WEB_DIAG_PREV        1
#define WEB_DIAG_CURR        2
#define WEB_DIAG_NEXT        3
#define WEB_DIAG_MAX         4

#define OPT_BUFF_MAX_LEN     16 

typedef struct {   
   char sysUserName[IFC_TINY_LEN];
   char sysPassword[IFC_PASSWORD_LEN];
   char sptUserName[IFC_TINY_LEN];
   char sptPassword[IFC_PASSWORD_LEN];
   char usrUserName[IFC_TINY_LEN];
   char usrPassword[IFC_PASSWORD_LEN];
   char curUserName[IFC_TINY_LEN];
   
   //add by frank
   char	wecFtpIpaddr[IFC_TINY_LEN];
   int		wecFtpPort;
   char	wecFtpUser[IFC_HOST_LEN];
   char	wecFtpPasswd[IFC_HOST_LEN];
   char	wecFtpFilePath[256];

   char	wecIpaddr[IFC_TINY_LEN];
   char	wecNetmask[IFC_TINY_LEN];
   char	wecDefaultGw[IFC_TINY_LEN];
   int		wecMgmtVlanSts;
   int		wecMgmtVlanId;

   char	snmpRoCommunity[IFC_HOST_LEN];
   char	snmpRwCommunity[IFC_HOST_LEN];
   char	snmpTrapIpaddr[IFC_TINY_LEN];
   int		snmpTrapDport;

   char wecDevSerial[IFC_HOST_LEN];
   char wecDevModel[IFC_HOST_LEN];
   char wecEoCType[IFC_TINY_LEN];
   int wecCltNumber;
   int wecCnuStation;
   int wecWlistStatus;
   int wecWDTStatus;
   char wecHwVersion[IFC_HOST_LEN];
   char wecBootVersion[IFC_HOST_LEN];
   char wecKernelVersion[IFC_HOST_LEN];
   char wecAppVersion[IFC_HOST_LEN];
   int wecFlashSize;
   int wecSdramSize;
   char wecManufactory[128];

   char cliAdminName[IFC_TINY_LEN];
   char cliAdminPwd[IFC_TINY_LEN];
   char cliSupportName[IFC_TINY_LEN];
   char cliOptPwd[IFC_TINY_LEN];
   char cliUserName[IFC_TINY_LEN];
   char cliUserPwd[IFC_TINY_LEN];

   char newCnuMac[IFC_SMALL_LEN];
   int newCnuModel;
   int newCnuPro;
   int cltid;
   int cnuid;
   int col_macLimit;
   int col_loagTime;
   int col_reagTime;
   int col_sfbSts;
   int col_sfuSts;
   int col_sfmSts;

   int col_vlanSts;
   int col_eth1vid;
   int col_eth2vid;
   int col_eth3vid;
   int col_eth4vid;

   int col_psctlSts;
   int col_eth1sts;
   int col_eth2sts;
   int col_eth3sts;
   int col_eth4sts;
   
   int col_rxLimitSts;
   int col_txLimitSts;
   int col_cpuPortRxRate;
   int col_cpuPortTxRate;
   int col_eth1rx;
   int col_eth1tx;
   int col_eth2rx;
   int col_eth2tx;
   int col_eth3rx;
   int col_eth3tx;
   int col_eth4rx;
   int col_eth4tx;

   int col_igmpPri;
   int col_unicastPri;
   int col_avsPri;
   int col_mcastPri;

   int col_tbaPriSts;
   int col_cosPriSts;
   int col_tosPriSts;

   int col_cos0pri;
   int col_cos1pri;
   int col_cos2pri;
   int col_cos3pri;
   int col_cos4pri;
   int col_cos5pri;
   int col_cos6pri;
   int col_cos7pri;

   int col_tos0pri;
   int col_tos1pri;
   int col_tos2pri;
   int col_tos3pri;
   int col_tos4pri;
   int col_tos5pri;
   int col_tos6pri;
   int col_tos7pri;

   int diagDir;
   char diagCnuMac[IFC_SMALL_LEN];
   int diagCnuModel;
   int diagCnuTei;

   char ccoMac[IFC_SMALL_LEN];
   char ccoNid[IFC_TINY_LEN];
   int ccoSnid;
   int ccoTei;

   int diagCnuRxRate;
   int diagCnuTxRate;
   char bitCarrier[IFC_TINY_LEN];
   int diagCnuAtten;

   char bridgedMac[IFC_SMALL_LEN];
   
   char MPDU_ACKD[IFC_SMALL_LEN];
   char MPDU_COLL[IFC_SMALL_LEN];
   char MPDU_FAIL[IFC_SMALL_LEN];
   char PBS_PASS[IFC_SMALL_LEN];
   char PBS_FAIL[IFC_SMALL_LEN];
   
   int diagResult;   

   //for upgrade
   int upgStep;
   int upgErrCode;

   //for package counter
   int eth1txbc;	/*发送广播包数量*/
   int eth1txu;	/*发送单播包数量*/
   int eth1txm;	/*发送多播报数量*/
   int eth1txp;	/*发送报文总和*/
   int eth1txb;	/*发送字节数*/
   int eth1rxbc;	/*接收广播包数量*/
   int eth1rxu;	/*接收单播包数量*/
   int eth1rxm;	/*接收多播报数量*/
   int eth1rxp;	/*接收报文总和*/
   int eth1rxb;	/*接收字节数*/

   int eth2txbc;	/*发送广播包数量*/
   int eth2txu;	/*发送单播包数量*/
   int eth2txm;	/*发送多播报数量*/
   int eth2txp;	/*发送报文总和*/
   int eth2txb;	/*发送字节数*/
   int eth2rxbc;	/*接收广播包数量*/
   int eth2rxu;	/*接收单播包数量*/
   int eth2rxm;	/*接收多播报数量*/
   int eth2rxp;	/*接收报文总和*/
   int eth2rxb;	/*接收字节数*/

   int eth3txbc;	/*发送广播包数量*/
   int eth3txu;	/*发送单播包数量*/
   int eth3txm;	/*发送多播报数量*/
   int eth3txp;	/*发送报文总和*/
   int eth3txb;	/*发送字节数*/
   int eth3rxbc;	/*接收广播包数量*/
   int eth3rxu;	/*接收单播包数量*/
   int eth3rxm;	/*接收多播报数量*/
   int eth3rxp;	/*接收报文总和*/
   int eth3rxb;	/*接收字节数*/

   int eth4txbc;	/*发送广播包数量*/
   int eth4txu;	/*发送单播包数量*/
   int eth4txm;	/*发送多播报数量*/
   int eth4txp;	/*发送报文总和*/
   int eth4txb;	/*发送字节数*/
   int eth4rxbc;	/*接收广播包数量*/
   int eth4rxu;	/*接收单播包数量*/
   int eth4rxm;	/*接收多播报数量*/
   int eth4rxp;	/*接收报文总和*/
   int eth4rxb;	/*接收字节数*/

   int portid;
   
   int eth1speed;
   int eth1duplex;
   int eth1pri;
   int eth1fc;
   int eth1sts;
   int eth1linksts;

   int eth2speed;
   int eth2duplex;
   int eth2pri;
   int eth2fc;
   int eth2sts;
   int eth2linksts;

   int eth3speed;
   int eth3duplex;
   int eth3pri;
   int eth3fc;
   int eth3sts;
   int eth3linksts;

   int eth4speed;
   int eth4duplex;
   int eth4pri;
   int eth4fc;
   int eth4sts;
   int eth4linksts;

   int wecSysupHours;
   int wecSysupMins;
   int wecSysupSecs;
   int wecSys0loads;
   int wecSys1loads;
   int wecSys2loads;
   int wecTotalram;
   int wecFreeram;

   char wecTemprature[IFC_SMALL_LEN];	/* host envirument temperature */

   char frmloadUrl[IFC_MEDIUM_LEN];
   char returnUrl[IFC_MEDIUM_LEN];
   int wecOptCode;
} WEB_NTWK_VAR, *PWEB_NTWK_VAR;

#define ADSL_BERT_STATE_STOP 0
#define ADSL_BERT_STATE_RUN 1
typedef struct {
   int berState;
   unsigned long berTime;
} WEB_TEST_VAR, *PWEB_TEST_VAR;

typedef void (*CGI_GET_HDLR) (int argc, char **argv, char *varValue);

typedef struct {
   char *cgiGetName;
   CGI_GET_HDLR cgiGetHdlr;
} CGI_GET_VAR, *PCGI_GET_VAR;

typedef void (*CGI_FNC_HDLR) (void);

typedef struct {
   char *cgiFncName;
   CGI_FNC_HDLR cgiFncHdlr;
} CGI_FNC_CMD, *PCGI_FNC_CMD;

typedef enum {
   CGI_STS_OK = 0,
   CGI_STS_ERR_GENERAL,
   CGI_STS_ERR_MEMORY,
   CGI_STS_ERR_FIND
} CGI_STATUS;

typedef enum {
   CGI_IFC_ETH = 0,
   CGI_IFC_USB
} CGI_IFC;

typedef enum {
   CGI_TYPE_NONE = 0,
   CGI_TYPE_STR,
   CGI_TYPE_MARK_STR,
   CGI_TYPE_NUM,
   CGI_TYPE_SYS_VERSION,
   CGI_TYPE_CFE_VERSION,
   CGI_TYPE_BOARD_ID,
   CGI_TYPE_ADSL_FLAG,
   CGI_TYPE_EXIST_PROTOCOL,
   CGI_TYPE_NUM_ENET,
   CGI_TYPE_NUM_PVC,
   CGI_TYPE_DHCP_LEASES,
   CGI_TYPE_PORT,
   CGI_TYPE_VPI,
   CGI_TYPE_VCI,
   CGI_TYPE_IPSEC_TABLE,
   CGI_TYPE_IPSEC_SETTINGS,
   CGI_TYPE_CERT_LIST,
   CGI_TYPE_ENET_DIAG,
   CGI_TYPE_VDSL_VERSION
} CGI_TYPE;

typedef struct {
   char *variable;
   void *value;
   CGI_TYPE type;
} CGI_ITEM, *PCGI_ITEM;


void do_cgi(char *path, FILE *fs);
void cgiFncCmd(int argc, char **argv);
void cgiGetVarOther(int argc, char **argv, char *varValue);
void cgiGetVar(char *varName, char *varValue);
void cgiSetVar(char *varName, char *varValue);

void cgiGetAllInfo(void);

int cgiReboot(void);
void cgiUrlDecode(char *s);
void cgiParseSet(char *path);
CGI_STATUS cgiGetValueByName(char *query, char *id, char *val);
void cgiWriteMessagePage(FILE *fs, char *msg, char *title, char *location);

void do_test_cgi(char *path, FILE *fs);
void cgiGetTestVar(char *varName, char *varValue);
void cgiSetTestVar(char *varName, char *varValue);

void cgiTestParseSet(char *path);

int cgiUpgradeFirmware(PWEB_NTWK_VAR pWebVar);

void cgiGetSysInfo(int argc, char **argv, char *varValue);
void cgiWriteUpgPage(int ret, FILE *fs, PWEB_NTWK_VAR pWebVar);


#endif
