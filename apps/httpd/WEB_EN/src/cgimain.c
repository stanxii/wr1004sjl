#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <net/route.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>

#include <httpd.h>
#include <cgimain.h>
#include <cgintwk.h>
#include <syscall.h>
#include <secdefs.h>
#include <bcmadsl.h>

#include <sysdiag.h>
#include <bcmatmapi.h>
#include <objectdefs.h>
#include <bcmcfmsys.h>
#include <bcmcfm.h>
#include <systemlog.h>
#include <ifcuiweb.h>
#include <http2dbs.h>
#include <http2cmm.h>
#include <upload.h>


#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))


CGI_GET_VAR WebVarTable[] = {
	{ "sysInfo", cgiGetSysInfo },   
};

CGI_FNC_CMD WebFncTable[] = {
   //{ "stsifcupdate",  cgiUpdateStsIfc },   
};

WEB_NTWK_VAR glbWebVar; // this global var is accessed from cgintwk.c, and cgiautoscan.c
int glbUploadMode;     // used for web page upload image or updating settings.

extern void do_ej(char *path, FILE *stream);
extern void do_file(char *path, FILE *stream);


void do_cgi(char *path, FILE *fs) {
   extern void destroy(void);
   char filename[WEB_BUF_SIZE_MAX];
   char logmsg[256]={0};
   char* query = NULL;
   char* ext = NULL;
   int ret = 0;

   query = strchr(path, '?');
   if ( query != NULL )
      cgiParseSet(path);

   filename[0] = '\0';
   ext = strchr(path, '.');
   if ( ext != NULL ) {
      *ext = '\0';
      strcpy(filename, path);
      strcat(filename, ".html");
      if ( strstr(filename, "wecUpgradeInfo.html") != NULL )
	{
		strcpy(filename, "upgrading.upg");
		strcpy(logmsg, "upgrading firmware image");
		ret = cgiUpgradeFirmware(&glbWebVar);
		if( ret != 0 )
		{
			do_upload_finnal();
		}		
		/* 升级结束才写日志*/
		if( glbWebVar.upgStep == 0 )
		{
			/*opt-log*/			
			http2dbs_writeOptlog(ret, logmsg);
		}
	}
	else if ( strstr(filename, "ntwkcfg.html") != NULL )
	{
		strcpy(logmsg, "do network settings");
		ret = http2dbs_setWecNetworkConfig(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "previewNetwork.cgi");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult.html");		
	}
	else if ( strstr(filename, "wecSnmpCfgInfo.html") != NULL )
	{
		strcpy(logmsg, "do snmp settings");				
		ret = http2dbs_setSnmpConfig(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "previewSnmp.cgi");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult.html");
	}
	else if ( strstr(filename, "wecPortPropety.html") != NULL )
	{
		http2cmm_getPortPropetyAll(&glbWebVar);
	}
	else if ( strstr(filename, "wecSaveDbInfo.html") != NULL )
	{
		strcpy(logmsg, "save configuration to databases");
		ret = http2dbs_saveConfig();
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "wecSaveDb.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "wecRebootInfo.html") != NULL )
	{
		strcpy(logmsg, "cbat system reboot");
		ret = http2cmm_sysReboot();
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			strcpy(glbWebVar.returnUrl, "wecReboot.html");
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
	}
	else if ( strstr(filename, "wecRestoreInfo.html") != NULL )
	{
		strcpy(logmsg, "restore default settings");
		ret = http2cmm_restoreDefault();
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			strcpy(glbWebVar.returnUrl, "wecRestoreDefault.html");
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
	}
	else if ( strstr(filename, "wecCliAdmin.html") != NULL )
	{
		strcpy(logmsg, "modify cli admin password");
		ret = http2dbs_setCliAdminPasswd(glbWebVar.cliAdminPwd);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "wecCliUsers.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult.html");
	}
	else if ( strstr(filename, "wecCliOpt.html") != NULL )
	{
		strcpy(logmsg, "modify cli operator password");
		ret = http2dbs_setCliOptPasswd(glbWebVar.cliOptPwd);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "wecCliUsers.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult.html");
	}
	else if ( strstr(filename, "wecCliUser.html") != NULL )
	{
		strcpy(logmsg, "modify cli user password");
		ret = http2dbs_setCliUserPasswd(glbWebVar.cliUserPwd);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "wecCliUsers.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult.html");
	}
	else if ( strstr(filename, "cltReboot.html") != NULL )
	{
		sprintf(logmsg, "reboot clt/%d", glbWebVar.cltid);
		ret = http2cmm_rebootClt(glbWebVar.cltid);
		/*opt-log*/		
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "cltManagement.cmd");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cltReload.html") != NULL )
	{
		sprintf(logmsg, "reload profile for clt/%d", glbWebVar.cltid);
		ret = http2cmm_reloadClt(glbWebVar.cltid);
		/*opt-log*/		
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "cltManagement.cmd");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuCreate.html") != NULL )
	{
		sprintf(logmsg, "create new cnu <%s>", glbWebVar.newCnuMac);
		ret = http2cmm_createCnu(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "previewCnus.cgi");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuReboot.html") != NULL )
	{
		sprintf(logmsg, "reboot cnu/1/%d", glbWebVar.cnuid);
		ret = http2cmm_rebootCnu(glbWebVar.cnuid);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "previewCnus.cgi");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuReload.html") != NULL )
	{
		sprintf(logmsg, "reload profile for cnu/1/%d", glbWebVar.cnuid);
		ret = http2cmm_reloadCnu(glbWebVar.cnuid);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "previewCnus.cgi");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuDelete.html") != NULL )
	{
		sprintf(logmsg, "delete cnu/1/%d", glbWebVar.cnuid);
		ret = http2cmm_deleteCnu(glbWebVar.cnuid);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "previewCnus.cgi");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuPermit.html") != NULL )
	{
		sprintf(logmsg, "permit cnu/1/%d", glbWebVar.cnuid);
		ret = http2cmm_permitCnu(glbWebVar.cnuid);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "previewCnus.cgi");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuUndoPermit.html") != NULL )
	{
		sprintf(logmsg, "undo permit cnu/1/%d", glbWebVar.cnuid);
		ret = http2cmm_undoPermitCnu(glbWebVar.cnuid);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "previewCnus.cgi");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "macLimit.html") != NULL )
	{
		if( glbWebVar.col_macLimit )
		{
			sprintf(logmsg, "do mac address limiting for cnu/1/%d", glbWebVar.cnuid);
		}
		else
		{
			sprintf(logmsg, "uodo mac address limiting for cnu/1/%d", glbWebVar.cnuid);
		}		
		ret = http2cmm_doMacLimiting(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "setAgTime.html") != NULL )
	{
		sprintf(logmsg, "do aging time settings for cnu/1/%d", glbWebVar.cnuid);	
		ret = http2cmm_doAgTimeSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "setSFilter.html") != NULL )
	{
		if(glbWebVar.col_sfbSts|glbWebVar.col_sfuSts|glbWebVar.col_sfmSts)
		{
			sprintf(logmsg, "do storm filter settings for cnu/1/%d", glbWebVar.cnuid);	
		}
		else
		{
			sprintf(logmsg, "undo storm filter settings for cnu/1/%d", glbWebVar.cnuid);	
		}
		ret = http2cmm_doSFilterSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "setCnuVlan.html") != NULL )
	{
		if(glbWebVar.col_vlanSts)
		{
			sprintf(logmsg, "do vlan settings for cnu/1/%d", glbWebVar.cnuid);	
		}
		else
		{
			sprintf(logmsg, "undo vlan settings for cnu/1/%d", glbWebVar.cnuid);	
		}
		ret = http2cmm_doCnuVlanSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "setPLinkSts.html") != NULL )
	{
		if(glbWebVar.col_eth1sts && glbWebVar.col_eth2sts && glbWebVar.col_eth3sts && glbWebVar.col_eth4sts)
		{
			sprintf(logmsg, "undo port shut down settings for cnu/1/%d", glbWebVar.cnuid);	
		}
		else
		{
			sprintf(logmsg, "do port shut down settings for cnu/1/%d", glbWebVar.cnuid);	
		}
		ret = http2cmm_doShutdownSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "doRateLimit.html") != NULL )
	{
		if(glbWebVar.col_rxLimitSts|glbWebVar.col_txLimitSts)
		{
			sprintf(logmsg, "do port speed limit settings for cnu/1/%d", glbWebVar.cnuid);	
		}
		else
		{
			sprintf(logmsg, "undo port speed limit settings for cnu/1/%d", glbWebVar.cnuid);	
		}
		ret = http2cmm_doSpeedLimitSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "saveProfile.html") != NULL )
	{
		sprintf(logmsg, "save profile for cnu/1/%d", glbWebVar.cnuid);
		ret = http2dbs_saveConfig();
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "wecEocMgmt.cmd");
	}
	else if ( strstr(filename, "setCltAgTime.html") != NULL )
	{
		sprintf(logmsg, "do aging time settings for clt/%d", glbWebVar.cltid);	
		ret = http2dbs_doCltAgTimeSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
	}
	else if ( strstr(filename, "setCltDeCap.html") != NULL )
	{
		sprintf(logmsg, "do default cap settings for clt/%d", glbWebVar.cltid);	
		ret = http2dbs_doCltDecapSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
	}
	else if ( strstr(filename, "setCltQoS.html") != NULL )
	{
		sprintf(logmsg, "do qos settings for clt/%d", glbWebVar.cltid);	
		ret = http2dbs_doCltQosSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
	}
	else if ( strstr(filename, "saveCltProfile.html") != NULL )
	{
		sprintf(logmsg, "save profile for clt/%d", glbWebVar.cltid);
		ret = http2dbs_saveConfig();
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "cltManagement.cmd");
	}
	else if ( strstr(filename, "doWlistCtl.html") != NULL )
	{
		sprintf(logmsg, "do white-list control settings");
		ret = http2cmm_doWListCtrlSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "wecWlistCtrl.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult.html");	
	}
	else if ( strstr(filename, "wecLinkDiag.html") != NULL )
	{
		sprintf(logmsg, "do physical link status diagnosis for cnu/1/%d", glbWebVar.cnuid);
		ret = http2cmm_doLinkDiag(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		sprintf(filename, "linkDiagResult.cmd");
	}
	else if ( strstr(filename, "wecPortStas.html") != NULL )
	{
		ret = http2cmm_doPortStas(&glbWebVar);
	}
	else if ( strstr(filename, "clearPortStas.html") != NULL )
	{
		ret = http2cmm_clearPortStas(&glbWebVar);
		strcpy(filename, "/webs/wecPortStas.html");
		/*opt-log*/			
		http2dbs_writeOptlog(ret, "clear port statistics counter");
	}	
	else if ( strstr(filename, "portPropety.html") != NULL )
	{
		if( glbWebVar.portid == 1 )
		{
			strcpy(logmsg, "do port ETH1 proprty settings");
		}
		else if( glbWebVar.portid == 2 )
		{
			strcpy(logmsg, "do port ETH2 proprty settings");
		}
		else if( glbWebVar.portid == 3 )
		{
			strcpy(logmsg, "do port Cable1 proprty settings");
		}
		else
		{
			strcpy(logmsg, "do port Cable4 proprty settings");
		}
		/*opt-log*/			
		http2dbs_writeOptlog(0, logmsg);
		sprintf(filename, "portPropety.cmd?portid=%d", glbWebVar.portid);
	}
	else if ( strstr(filename, "wecWebUsersInfo.html") != NULL )
	{
		ret = http2dbs_setWebAdminPwd(&glbWebVar);		
		/*opt-log*/			
		http2dbs_writeOptlog(ret, "set web admin password");
		strcpy(glbWebVar.returnUrl, "wecWebUsers.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult.html");	
	}
	else if ( strstr(filename, "previewTopology.html") != NULL )
	{
		strcpy(glbWebVar.frmloadUrl, "wecTopology.cmd");
		strcpy(filename, "/webs/wecPreView.html");	
	}
	else if ( strstr(filename, "previewCnus.html") != NULL )
	{
		strcpy(glbWebVar.frmloadUrl, "wecEocMgmt.cmd");
		strcpy(filename, "/webs/wecPreView.html");	
	}
	else if ( strstr(filename, "previewLinkDiag.html") != NULL )
	{
		strcpy(glbWebVar.frmloadUrl, "wecLinkDiag.cmd");
		strcpy(filename, "/webs/wecPreView.html");	
	}
	else if ( strstr(filename, "previewNetwork.html") != NULL )
	{
		strcpy(glbWebVar.frmloadUrl, "wecNetworkConfig.html");
		strcpy(filename, "/webs/wecPreView.html");	
	}
	else if ( strstr(filename, "previewSnmp.html") != NULL )
	{
		strcpy(glbWebVar.frmloadUrl, "wecSnmpCfg.html");
		strcpy(filename, "/webs/wecPreView.html");	
	}
	
	if( strstr(filename, ".cmd") != NULL )
	{
		do_cmd_cgi(filename, fs);
	}
	else if( strstr(filename, ".upg") != NULL )
	{
		cgiWriteUpgPage(ret, fs, &glbWebVar);
	}
	else
	{
		do_ej(filename, fs);
	}
   } else
      cgiWriteMessagePage(fs, "Message", "The selected web page is not implemented yet.", 0);
}

void do_test_cgi(char *path, FILE *fs) {
   char filename[WEB_BUF_SIZE_MAX];
   char* query = NULL;
   char* ext = NULL;

   query = strchr(path, '?');
   if ( query != NULL )
      cgiTestParseSet(path);

   filename[0] = '\0';
   ext = strchr(path, '.');
   if ( ext != NULL ) {
      *ext = '\0';
      strcpy(filename, path);
      strcat(filename, ".html");
      do_ej(filename, fs);
   }
}

void cgiGetVarOther(int argc, char **argv, char *varValue) {
   int i = 0;

   if ( argc < 2) return;

   for ( ; i < ARRAY_SIZE(WebVarTable); i++ )
   
      if ( strcmp(argv[1], WebVarTable[i].cgiGetName) == 0 )
         break;
   if ( i >= ARRAY_SIZE(WebVarTable) )
      return;

   (*(WebVarTable[i].cgiGetHdlr))(argc, argv, varValue);
}

void cgiFncCmd(int argc, char **argv) {
   int i = 0;

   for ( ; i < ARRAY_SIZE(WebFncTable); i++ )
      if ( strcmp(argv[1], WebFncTable[i].cgiFncName) == 0 )
         break;
   if ( i >= ARRAY_SIZE(WebFncTable) )
      return;

   (*(WebFncTable[i].cgiFncHdlr))();
}

int cgiPasswordNeedRefresh(char *attrname)
{
    int i = 0;
    const char* cgiTblPasswordNeedRefreshName[]= 
    {
        "sysPassword", 
        "sptPassword", 
        "usrPassword",
        NULL
    }; 
	
    if (attrname == NULL)
        return 0;

    for (i = 0; NULL != cgiTblPasswordNeedRefreshName[i];i++)
        if (!strcmp(attrname,cgiTblPasswordNeedRefreshName[i]))
            return 1;
    return 0;
}

CGI_ITEM CgiGetTable[] = {
   { "sysUserName", (void *)glbWebVar.sysUserName, CGI_TYPE_STR },
   { "sysPassword", (void *)glbWebVar.sysPassword, CGI_TYPE_MARK_STR },
   { "sptUserName", (void *)glbWebVar.sptUserName, CGI_TYPE_STR },
   { "sptPassword", (void *)glbWebVar.sptPassword, CGI_TYPE_MARK_STR },
   { "usrUserName", (void *)glbWebVar.usrUserName, CGI_TYPE_STR },
   { "usrPassword", (void *)glbWebVar.usrPassword, CGI_TYPE_MARK_STR },
   { "curUserName", (void *)glbWebVar.curUserName, CGI_TYPE_STR },

   { "wecDevSerial", (void *)&glbWebVar.wecDevSerial, CGI_TYPE_STR },
   { "wecDevModel", (void *)glbWebVar.wecDevModel, CGI_TYPE_STR },
   { "wecEoCType", (void *)&glbWebVar.wecEoCType, CGI_TYPE_STR },
   { "wecCltNumber", (void *)&glbWebVar.wecCltNumber, CGI_TYPE_NUM },
   { "wecCnuStation", (void *)&glbWebVar.wecCnuStation, CGI_TYPE_NUM },
   { "wecWlistStatus", (void *)&glbWebVar.wecWlistStatus, CGI_TYPE_NUM },
   { "wecWDTStatus", (void *)&glbWebVar.wecWDTStatus, CGI_TYPE_NUM },
   { "wecHwVersion", (void *)glbWebVar.wecHwVersion, CGI_TYPE_STR },
   { "wecBootVersion", (void *)glbWebVar.wecBootVersion, CGI_TYPE_STR },
   { "wecKernelVersion", (void *)glbWebVar.wecKernelVersion, CGI_TYPE_STR },
   { "wecAppVersion", (void *)glbWebVar.wecAppVersion, CGI_TYPE_STR },
   { "wecFlashSize", (void *)&glbWebVar.wecFlashSize, CGI_TYPE_NUM },
   { "wecSdramSize", (void *)&glbWebVar.wecSdramSize, CGI_TYPE_NUM },
   { "wecManufactory", (void *)glbWebVar.wecManufactory, CGI_TYPE_STR },

   { "cltid", (void *)&glbWebVar.cltid, CGI_TYPE_NUM },
   { "cnuid", (void *)&glbWebVar.cnuid, CGI_TYPE_NUM },
   { "diagDir", (void *)&glbWebVar.diagDir, CGI_TYPE_NUM },
   { "diagResult", (void *)&glbWebVar.diagResult, CGI_TYPE_NUM },
   
   { "upgStep", (void *)&glbWebVar.upgStep, CGI_TYPE_NUM },
   { "upgErrCode", (void *)&glbWebVar.upgErrCode, CGI_TYPE_NUM },

   { "eth1txbc", (void *)&glbWebVar.eth1txbc, CGI_TYPE_NUM },
   { "eth1txu", (void *)&glbWebVar.eth1txu, CGI_TYPE_NUM },
   { "eth1txm", (void *)&glbWebVar.eth1txm, CGI_TYPE_NUM },
   { "eth1txp", (void *)&glbWebVar.eth1txp, CGI_TYPE_NUM },
   { "eth1txb", (void *)&glbWebVar.eth1txb, CGI_TYPE_NUM },
   { "eth1rxbc", (void *)&glbWebVar.eth1rxbc, CGI_TYPE_NUM },
   { "eth1rxu", (void *)&glbWebVar.eth1rxu, CGI_TYPE_NUM },
   { "eth1rxm", (void *)&glbWebVar.eth1rxm, CGI_TYPE_NUM },
   { "eth1rxp", (void *)&glbWebVar.eth1rxp, CGI_TYPE_NUM },
   { "eth1rxb", (void *)&glbWebVar.eth1rxb, CGI_TYPE_NUM },
   
   { "eth2txbc", (void *)&glbWebVar.eth2txbc, CGI_TYPE_NUM },
   { "eth2txu", (void *)&glbWebVar.eth2txu, CGI_TYPE_NUM },
   { "eth2txm", (void *)&glbWebVar.eth2txm, CGI_TYPE_NUM },
   { "eth2txp", (void *)&glbWebVar.eth2txp, CGI_TYPE_NUM },
   { "eth2txb", (void *)&glbWebVar.eth2txb, CGI_TYPE_NUM },
   { "eth2rxbc", (void *)&glbWebVar.eth2rxbc, CGI_TYPE_NUM },
   { "eth2rxu", (void *)&glbWebVar.eth2rxu, CGI_TYPE_NUM },
   { "eth2rxm", (void *)&glbWebVar.eth2rxm, CGI_TYPE_NUM },
   { "eth2rxp", (void *)&glbWebVar.eth2rxp, CGI_TYPE_NUM },
   { "eth2rxb", (void *)&glbWebVar.eth2rxb, CGI_TYPE_NUM },
   
   { "eth3txbc", (void *)&glbWebVar.eth3txbc, CGI_TYPE_NUM },
   { "eth3txu", (void *)&glbWebVar.eth3txu, CGI_TYPE_NUM },
   { "eth3txm", (void *)&glbWebVar.eth3txm, CGI_TYPE_NUM },
   { "eth3txp", (void *)&glbWebVar.eth3txp, CGI_TYPE_NUM },
   { "eth3txb", (void *)&glbWebVar.eth3txb, CGI_TYPE_NUM },
   { "eth3rxbc", (void *)&glbWebVar.eth3rxbc, CGI_TYPE_NUM },
   { "eth3rxu", (void *)&glbWebVar.eth3rxu, CGI_TYPE_NUM },
   { "eth3rxm", (void *)&glbWebVar.eth3rxm, CGI_TYPE_NUM },
   { "eth3rxp", (void *)&glbWebVar.eth3rxp, CGI_TYPE_NUM },
   { "eth3rxb", (void *)&glbWebVar.eth3rxb, CGI_TYPE_NUM },

   { "eth4txbc", (void *)&glbWebVar.eth4txbc, CGI_TYPE_NUM },
   { "eth4txu", (void *)&glbWebVar.eth4txu, CGI_TYPE_NUM },
   { "eth4txm", (void *)&glbWebVar.eth4txm, CGI_TYPE_NUM },
   { "eth4txp", (void *)&glbWebVar.eth4txp, CGI_TYPE_NUM },
   { "eth4txb", (void *)&glbWebVar.eth4txb, CGI_TYPE_NUM },
   { "eth4rxbc", (void *)&glbWebVar.eth4rxbc, CGI_TYPE_NUM },
   { "eth4rxu", (void *)&glbWebVar.eth4rxu, CGI_TYPE_NUM },
   { "eth4rxm", (void *)&glbWebVar.eth4rxm, CGI_TYPE_NUM },
   { "eth4rxp", (void *)&glbWebVar.eth4rxp, CGI_TYPE_NUM },
   { "eth4rxb", (void *)&glbWebVar.eth4rxb, CGI_TYPE_NUM },   

   { "portid", (void *)&glbWebVar.portid, CGI_TYPE_NUM }, 

   { "eth1speed", (void *)&glbWebVar.eth1speed, CGI_TYPE_NUM }, 
   { "eth1duplex", (void *)&glbWebVar.eth1duplex, CGI_TYPE_NUM }, 
   { "eth1pri", (void *)&glbWebVar.eth1pri, CGI_TYPE_NUM }, 
   { "eth1fc", (void *)&glbWebVar.eth1fc, CGI_TYPE_NUM }, 
   { "eth1sts", (void *)&glbWebVar.eth1sts, CGI_TYPE_NUM }, 
   { "eth1linksts", (void *)&glbWebVar.eth1linksts, CGI_TYPE_NUM }, 

   { "eth2speed", (void *)&glbWebVar.eth2speed, CGI_TYPE_NUM }, 
   { "eth2duplex", (void *)&glbWebVar.eth2duplex, CGI_TYPE_NUM }, 
   { "eth2pri", (void *)&glbWebVar.eth2pri, CGI_TYPE_NUM }, 
   { "eth2fc", (void *)&glbWebVar.eth2fc, CGI_TYPE_NUM }, 
   { "eth2sts", (void *)&glbWebVar.eth2sts, CGI_TYPE_NUM }, 
   { "eth2linksts", (void *)&glbWebVar.eth2linksts, CGI_TYPE_NUM }, 

   { "eth3speed", (void *)&glbWebVar.eth3speed, CGI_TYPE_NUM }, 
   { "eth3duplex", (void *)&glbWebVar.eth3duplex, CGI_TYPE_NUM }, 
   { "eth3pri", (void *)&glbWebVar.eth3pri, CGI_TYPE_NUM }, 
   { "eth3fc", (void *)&glbWebVar.eth3fc, CGI_TYPE_NUM }, 
   { "eth3sts", (void *)&glbWebVar.eth3sts, CGI_TYPE_NUM }, 
   { "eth3linksts", (void *)&glbWebVar.eth3linksts, CGI_TYPE_NUM }, 

   { "eth4speed", (void *)&glbWebVar.eth4speed, CGI_TYPE_NUM }, 
   { "eth4duplex", (void *)&glbWebVar.eth4duplex, CGI_TYPE_NUM }, 
   { "eth4pri", (void *)&glbWebVar.eth4pri, CGI_TYPE_NUM }, 
   { "eth4fc", (void *)&glbWebVar.eth4fc, CGI_TYPE_NUM }, 
   { "eth4sts", (void *)&glbWebVar.eth4sts, CGI_TYPE_NUM }, 
   { "eth4linksts", (void *)&glbWebVar.eth4linksts, CGI_TYPE_NUM }, 

   { "wecSysupHours", (void *)&glbWebVar.wecSysupHours, CGI_TYPE_NUM }, 
   { "wecSysupMins", (void *)&glbWebVar.wecSysupMins, CGI_TYPE_NUM }, 
   { "wecSysupSecs", (void *)&glbWebVar.wecSysupSecs, CGI_TYPE_NUM }, 

   { "wecSys0loads", (void *)&glbWebVar.wecSys0loads, CGI_TYPE_NUM }, 
   { "wecSys1loads", (void *)&glbWebVar.wecSys1loads, CGI_TYPE_NUM }, 
   { "wecSys2loads", (void *)&glbWebVar.wecSys2loads, CGI_TYPE_NUM }, 
   { "wecTotalram", (void *)&glbWebVar.wecTotalram, CGI_TYPE_NUM }, 
   { "wecFreeram", (void *)&glbWebVar.wecFreeram, CGI_TYPE_NUM }, 

   { "wecTemprature", (void *)glbWebVar.wecTemprature, CGI_TYPE_STR },

   { "frmloadUrl", (void *)glbWebVar.frmloadUrl, CGI_TYPE_STR },
   { "returnUrl", (void *)glbWebVar.returnUrl, CGI_TYPE_STR },
   { "wecOptCode", (void *)&glbWebVar.wecOptCode, CGI_TYPE_NUM }, 
   
   { NULL, NULL, CGI_TYPE_NONE }
};

void cgiGetVar(char *varName, char *varValue) {
   int i = 0;
   //char ver[IFC_TINY_LEN];
   //char id[IFC_TINY_LEN];
   struct sysinfo wecSysinfo;
#ifdef __AT30TK175STK__
	st_temperature temp_data;
#endif

   varValue[0] = '\0';
   /*You can add any source code here to synchronize the date you instresting between webgui and dbs*/
   if ( strcmp(varName, "wecIpaddr") == 0 )
   {
   	/* 从数据库获取*/
	http2dbs_getWecIpaddr(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecNetmask") == 0 )
   {
	http2dbs_getWecNetmask(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecDefaultGw") == 0 )
   {
	http2dbs_getWecDefaultGw(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecMgmtVlanSts") == 0 )
   {
	http2dbs_getWecMgmtVlanSts(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecMgmtVlanId") == 0 )
   {
	http2dbs_getWecMgmtVlanId(varValue);		
	return;
   }
   else if ( strcmp(varName, "snmpRoCommunity") == 0 )
   {
	http2dbs_getSnmpRoCommunity(varValue);		
	return;
   } 
   else if ( strcmp(varName, "snmpRwCommunity") == 0 )
   {
	http2dbs_getSnmpRwCommunity(varValue);		
	return;
   } 
   else if ( strcmp(varName, "snmpTrapIpaddr") == 0 )
   {
	http2dbs_getSnmpTrapIpaddr(varValue);		
	return;
   } 
   else if ( strcmp(varName, "snmpTrapDport") == 0 )
   {
	http2dbs_getSnmpTrapDport(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecFtpIpaddr") == 0 )
   {
	http2dbs_getFtpIpaddr(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecFtpPort") == 0 )
   {
	http2dbs_getFtpPort(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecFtpUser") == 0 )
   {
	http2dbs_getFtpUser(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecFtpPasswd") == 0 )
   {
	http2dbs_getFtpPasswd(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecFtpFilePath") == 0 )
   {
	http2dbs_getFtpFilePath(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecWlistStatus") == 0 )
   {
	http2dbs_getWlistStatus(varValue);
	return;
   }   
   else if ( strcmp(varName, "wecTemprature") == 0 )
   {
#ifdef __AT30TK175STK__
	if( CMM_SUCCESS == http2cmm_getCbatTemperature(&temp_data) )
	{
		if( 0 == temp_data.sign )
		{
			sprintf(varValue, "+%d.%d Centigrade", temp_data.itemp, temp_data.ftemp);
		}
		else
		{
			sprintf(varValue, "-%d.%d Centigrade", temp_data.itemp, temp_data.ftemp);
		}
	}
	else
	{
		strcpy(varValue, "Unknown");
	}
#else
	strcpy(varValue, "Not supported");
#endif
	return;
   }
   else if ( strcmp(varName, "wecSysupHours") == 0 )
   {
   	sysinfo(&wecSysinfo);	
	glbWebVar.wecSysupHours = wecSysinfo.uptime/3600;
	glbWebVar.wecSysupMins = (wecSysinfo.uptime%3600)/60;
	glbWebVar.wecSysupSecs = (wecSysinfo.uptime%3600)%60;
	glbWebVar.wecSys0loads = wecSysinfo.loads[0];
	glbWebVar.wecSys1loads = wecSysinfo.loads[1];
	glbWebVar.wecSys2loads = wecSysinfo.loads[2];
	glbWebVar.wecTotalram = wecSysinfo.totalram/1024;
	glbWebVar.wecFreeram = wecSysinfo.freeram/1024;
	sprintf(varValue, "%02d", glbWebVar.wecSysupHours);
	return;
   }
   else if ( strcmp(varName, "wecSysupMins") == 0 )
   {   	
	sprintf(varValue, "%02d", glbWebVar.wecSysupMins);
	return;
   }
   else if ( strcmp(varName, "wecSysupSecs") == 0 )
   {   	
	sprintf(varValue, "%02d", glbWebVar.wecSysupSecs);
	return;
   }
   
   for ( ; CgiGetTable[i].variable != NULL; i++ )
      if ( strcmp(varName, CgiGetTable[i].variable) == 0 )
         break;
         
   if ( CgiGetTable[i].variable != NULL ) {
      switch ( CgiGetTable[i].type ) {
      case CGI_TYPE_STR:
         strcpy(varValue, (char *)CgiGetTable[i].value);
         break;
      case CGI_TYPE_MARK_STR:
         strcpy(varValue, (char *)CgiGetTable[i].value);
         //bcmProcessMarkStrChars(varValue);
         break;
      case CGI_TYPE_NUM:
         sprintf(varValue, "%d", *((int *)CgiGetTable[i].value));
         break;
      case CGI_TYPE_SYS_VERSION:         
         break;
      case CGI_TYPE_CFE_VERSION:
         break;
      case CGI_TYPE_BOARD_ID:
         break;
      case CGI_TYPE_ADSL_FLAG:
         break;
      case CGI_TYPE_EXIST_PROTOCOL:
         break;
      case CGI_TYPE_DHCP_LEASES:
         break;
      case CGI_TYPE_ENET_DIAG:
         break;
      default:
         varValue[0] = '\0';
         break;
      }
   }
}

CGI_ITEM CgiSetTable[] = {
   { "sysUserName", (void *)glbWebVar.sysUserName, CGI_TYPE_STR },
   { "sysPassword", (void *)glbWebVar.sysPassword, CGI_TYPE_STR },
   { "sptUserName", (void *)glbWebVar.sptUserName, CGI_TYPE_STR },
   { "sptPassword", (void *)glbWebVar.sptPassword, CGI_TYPE_STR },
   { "usrUserName", (void *)glbWebVar.usrUserName, CGI_TYPE_STR },
   { "usrPassword", (void *)glbWebVar.usrPassword, CGI_TYPE_STR },
   { "curUserName", (void *)glbWebVar.curUserName, CGI_TYPE_STR },
   
   { "wecFtpIpaddr", (void *)&glbWebVar.wecFtpIpaddr, CGI_TYPE_STR },
   { "wecFtpPort", (void *)&glbWebVar.wecFtpPort, CGI_TYPE_NUM },
   { "wecFtpUser", (void *)&glbWebVar.wecFtpUser, CGI_TYPE_STR },
   { "wecFtpPasswd", (void *)&glbWebVar.wecFtpPasswd, CGI_TYPE_STR },
   { "wecFtpFilePath", (void *)&glbWebVar.wecFtpFilePath, CGI_TYPE_STR },

   { "wecIpaddr", (void *)&glbWebVar.wecIpaddr, CGI_TYPE_STR },
   { "wecNetmask", (void *)&glbWebVar.wecNetmask, CGI_TYPE_STR },
   { "wecDefaultGw", (void *)&glbWebVar.wecDefaultGw, CGI_TYPE_STR },
   { "wecMgmtVlanSts", (void *)&glbWebVar.wecMgmtVlanSts, CGI_TYPE_NUM },
   { "wecMgmtVlanId", (void *)&glbWebVar.wecMgmtVlanId, CGI_TYPE_NUM },

   { "snmpRoCommunity", (void *)&glbWebVar.snmpRoCommunity, CGI_TYPE_STR },
   { "snmpRwCommunity", (void *)&glbWebVar.snmpRwCommunity, CGI_TYPE_STR },
   { "snmpTrapIpaddr", (void *)&glbWebVar.snmpTrapIpaddr, CGI_TYPE_STR },
   { "snmpTrapDport", (void *)&glbWebVar.snmpTrapDport, CGI_TYPE_NUM },

   //for create new cnu
   { "newCnuMac", (void *)&glbWebVar.newCnuMac, CGI_TYPE_STR },
   { "newCnuModel", (void *)&glbWebVar.newCnuModel, CGI_TYPE_NUM },
   { "newCnuPro", (void *)&glbWebVar.newCnuPro, CGI_TYPE_NUM },

   { "cltid", (void *)&glbWebVar.cltid, CGI_TYPE_NUM },
   { "cnuid", (void *)&glbWebVar.cnuid, CGI_TYPE_NUM },
   { "col_macLimit", (void *)&glbWebVar.col_macLimit, CGI_TYPE_NUM },
   { "col_loagTime", (void *)&glbWebVar.col_loagTime, CGI_TYPE_NUM },
   { "col_reagTime", (void *)&glbWebVar.col_reagTime, CGI_TYPE_NUM },
   { "col_sfbSts", (void *)&glbWebVar.col_sfbSts, CGI_TYPE_NUM },
   { "col_sfuSts", (void *)&glbWebVar.col_sfuSts, CGI_TYPE_NUM },
   { "col_sfmSts", (void *)&glbWebVar.col_sfmSts, CGI_TYPE_NUM },

   { "col_vlanSts", (void *)&glbWebVar.col_vlanSts, CGI_TYPE_NUM },
   { "col_eth1vid", (void *)&glbWebVar.col_eth1vid, CGI_TYPE_NUM },
   { "col_eth2vid", (void *)&glbWebVar.col_eth2vid, CGI_TYPE_NUM },
   { "col_eth3vid", (void *)&glbWebVar.col_eth3vid, CGI_TYPE_NUM },
   { "col_eth4vid", (void *)&glbWebVar.col_eth4vid, CGI_TYPE_NUM },

   { "col_psctlSts", (void *)&glbWebVar.col_psctlSts, CGI_TYPE_NUM },
   { "col_eth1sts", (void *)&glbWebVar.col_eth1sts, CGI_TYPE_NUM },
   { "col_eth2sts", (void *)&glbWebVar.col_eth2sts, CGI_TYPE_NUM },
   { "col_eth3sts", (void *)&glbWebVar.col_eth3sts, CGI_TYPE_NUM },
   { "col_eth4sts", (void *)&glbWebVar.col_eth4sts, CGI_TYPE_NUM },

   { "col_rxLimitSts", (void *)&glbWebVar.col_rxLimitSts, CGI_TYPE_NUM },
   { "col_txLimitSts", (void *)&glbWebVar.col_txLimitSts, CGI_TYPE_NUM },
   { "col_cpuPortRxRate", (void *)&glbWebVar.col_cpuPortRxRate, CGI_TYPE_NUM },
   { "col_cpuPortTxRate", (void *)&glbWebVar.col_cpuPortTxRate, CGI_TYPE_NUM },
   { "col_eth1rx", (void *)&glbWebVar.col_eth1rx, CGI_TYPE_NUM },
   { "col_eth1tx", (void *)&glbWebVar.col_eth1tx, CGI_TYPE_NUM },
   { "col_eth2rx", (void *)&glbWebVar.col_eth2rx, CGI_TYPE_NUM },
   { "col_eth2tx", (void *)&glbWebVar.col_eth2tx, CGI_TYPE_NUM },
   { "col_eth3rx", (void *)&glbWebVar.col_eth3rx, CGI_TYPE_NUM },
   { "col_eth3tx", (void *)&glbWebVar.col_eth3tx, CGI_TYPE_NUM },
   { "col_eth4rx", (void *)&glbWebVar.col_eth4rx, CGI_TYPE_NUM },
   { "col_eth4tx", (void *)&glbWebVar.col_eth4tx, CGI_TYPE_NUM },

   { "col_igmpPri", (void *)&glbWebVar.col_igmpPri, CGI_TYPE_NUM },
   { "col_unicastPri", (void *)&glbWebVar.col_unicastPri, CGI_TYPE_NUM },
   { "col_avsPri", (void *)&glbWebVar.col_avsPri, CGI_TYPE_NUM },
   { "col_mcastPri", (void *)&glbWebVar.col_mcastPri, CGI_TYPE_NUM },
   { "col_tbaPriSts", (void *)&glbWebVar.col_tbaPriSts, CGI_TYPE_NUM },
   { "col_cosPriSts", (void *)&glbWebVar.col_cosPriSts, CGI_TYPE_NUM },
   { "col_tosPriSts", (void *)&glbWebVar.col_tosPriSts, CGI_TYPE_NUM },
   { "col_cos0pri", (void *)&glbWebVar.col_cos0pri, CGI_TYPE_NUM },
   { "col_cos1pri", (void *)&glbWebVar.col_cos1pri, CGI_TYPE_NUM },
   { "col_cos2pri", (void *)&glbWebVar.col_cos2pri, CGI_TYPE_NUM },
   { "col_cos3pri", (void *)&glbWebVar.col_cos3pri, CGI_TYPE_NUM },
   { "col_cos4pri", (void *)&glbWebVar.col_cos4pri, CGI_TYPE_NUM },
   { "col_cos5pri", (void *)&glbWebVar.col_cos5pri, CGI_TYPE_NUM },
   { "col_cos6pri", (void *)&glbWebVar.col_cos6pri, CGI_TYPE_NUM },
   { "col_cos7pri", (void *)&glbWebVar.col_cos7pri, CGI_TYPE_NUM },
   { "col_tos0pri", (void *)&glbWebVar.col_tos0pri, CGI_TYPE_NUM },
   { "col_tos1pri", (void *)&glbWebVar.col_tos1pri, CGI_TYPE_NUM },
   { "col_tos2pri", (void *)&glbWebVar.col_tos2pri, CGI_TYPE_NUM },
   { "col_tos3pri", (void *)&glbWebVar.col_tos3pri, CGI_TYPE_NUM },
   { "col_tos4pri", (void *)&glbWebVar.col_tos4pri, CGI_TYPE_NUM },
   { "col_tos5pri", (void *)&glbWebVar.col_tos5pri, CGI_TYPE_NUM },
   { "col_tos6pri", (void *)&glbWebVar.col_tos6pri, CGI_TYPE_NUM },
   { "col_tos7pri", (void *)&glbWebVar.col_tos7pri, CGI_TYPE_NUM },

   { "diagDir", (void *)&glbWebVar.diagDir, CGI_TYPE_NUM },

   { "upgStep", (void *)&glbWebVar.upgStep, CGI_TYPE_NUM },
   { "upgErrCode", (void *)&glbWebVar.upgErrCode, CGI_TYPE_NUM },

   { "portid", (void *)&glbWebVar.portid, CGI_TYPE_NUM }, 

   { "eth1speed", (void *)&glbWebVar.eth1speed, CGI_TYPE_NUM }, 
   { "eth1duplex", (void *)&glbWebVar.eth1duplex, CGI_TYPE_NUM }, 
   { "eth1pri", (void *)&glbWebVar.eth1pri, CGI_TYPE_NUM }, 
   { "eth1fc", (void *)&glbWebVar.eth1fc, CGI_TYPE_NUM }, 
   { "eth1sts", (void *)&glbWebVar.eth1sts, CGI_TYPE_NUM }, 

   { "eth2speed", (void *)&glbWebVar.eth2speed, CGI_TYPE_NUM }, 
   { "eth2duplex", (void *)&glbWebVar.eth2duplex, CGI_TYPE_NUM }, 
   { "eth2pri", (void *)&glbWebVar.eth2pri, CGI_TYPE_NUM }, 
   { "eth2fc", (void *)&glbWebVar.eth2fc, CGI_TYPE_NUM }, 
   { "eth2sts", (void *)&glbWebVar.eth2sts, CGI_TYPE_NUM }, 

   { "eth3speed", (void *)&glbWebVar.eth3speed, CGI_TYPE_NUM }, 
   { "eth3duplex", (void *)&glbWebVar.eth3duplex, CGI_TYPE_NUM }, 
   { "eth3pri", (void *)&glbWebVar.eth3pri, CGI_TYPE_NUM }, 
   { "eth3fc", (void *)&glbWebVar.eth3fc, CGI_TYPE_NUM }, 
   { "eth3sts", (void *)&glbWebVar.eth3sts, CGI_TYPE_NUM }, 

   { "eth4speed", (void *)&glbWebVar.eth4speed, CGI_TYPE_NUM }, 
   { "eth4duplex", (void *)&glbWebVar.eth4duplex, CGI_TYPE_NUM }, 
   { "eth4pri", (void *)&glbWebVar.eth4pri, CGI_TYPE_NUM }, 
   { "eth4fc", (void *)&glbWebVar.eth4fc, CGI_TYPE_NUM }, 
   { "eth4sts", (void *)&glbWebVar.eth4sts, CGI_TYPE_NUM }, 

   { "wecWlistStatus", (void *)&glbWebVar.wecWlistStatus, CGI_TYPE_NUM }, 

   { "cliAdminPwd", (void *)&glbWebVar.cliAdminPwd, CGI_TYPE_STR },
   { "cliOptPwd", (void *)&glbWebVar.cliOptPwd, CGI_TYPE_STR },
   { "cliUserPwd", (void *)&glbWebVar.cliUserPwd, CGI_TYPE_STR },
   
   { NULL, NULL, CGI_TYPE_NONE }
};

void cgiSetVar(char *varName, char *varValue) {
   int i = 0;

   for ( ; CgiSetTable[i].variable != NULL; i++ )
      if ( strcmp(varName, CgiSetTable[i].variable) == 0 )
         break;
         
   if ( CgiSetTable[i].variable != NULL ) {
      switch ( CgiSetTable[i].type ) {
      case CGI_TYPE_STR:
         strcpy((char *)CgiSetTable[i].value, varValue);
         break;
      case CGI_TYPE_NUM:
         *((int *)CgiSetTable[i].value) = atoi(varValue);
         break;
      case CGI_TYPE_PORT:
         break;
      case CGI_TYPE_VPI:
         break;
      case CGI_TYPE_VCI:
         break;
      default:
         break;
      }
   }
}

void cgiGetTestVar(char *varName, char *varValue) {

   
}

void cgiSetTestVar(char *varName, char *varValue) {
  
}


void cgiGetAllInfo(void) {
   BcmWeb_getAllInfo(&glbWebVar);
}

int UpgradeBegin(PWEB_NTWK_VAR pWebVar)
{
	pWebVar->upgErrCode = RC_UPG_ON_GOING;
	return 0;
}

int UpgradePrepare(PWEB_NTWK_VAR pWebVar)
{
	return 0;
}

int UpgradeDownload(PWEB_NTWK_VAR pWebVar)
{
	return 0;
}

int UpgradeCheckfile(PWEB_NTWK_VAR pWebVar)
{
	//check file MD5
	if( checkFile() == BOOL_FALSE )
	{
		pWebVar->upgErrCode = RC_UPG_MD_CHECK_ERROR;
		return -1;
	}
	return 0;
}

int UpgradeErase(PWEB_NTWK_VAR pWebVar)
{
	//erase flash
	if( eraseFlash() == BOOL_FALSE )
	{
		pWebVar->upgErrCode = RC_UPG_FLASH_ERASE_ERROR;
		return -1;
	}
	else
	{
		return 0;
	}
}

int UpgradeWrite(PWEB_NTWK_VAR pWebVar)
{
	//write flash
	if( do_upload_post() == RC_UPG_OK )
	{
		pWebVar->upgErrCode = RC_UPG_OK;
		pWebVar->upgStep = 0;
		return 0;
	}
	else
	{
		pWebVar->upgErrCode = RC_UPG_FLASH_WRITE_ERROR;
		pWebVar->upgStep = 0;
		return -1;
	}
}

int cgiUpgradeFirmware(PWEB_NTWK_VAR pWebVar)
{
	switch(pWebVar->upgStep)
	{
		case 1:
		{
			return UpgradeBegin(pWebVar);
		}
		case 2:
		{
			return UpgradePrepare(pWebVar);
		}
		case 3:
		{
			return UpgradeDownload(pWebVar);
		}
		case 4:
		{
			return UpgradeCheckfile(pWebVar);
		}
		case 5:
		{
			return UpgradeErase(pWebVar);
		}
		case 6:
		{
			return UpgradeWrite(pWebVar);
		}
		default:
		{
			return RC_UPG_GENERIC_ERROR;
		}
	}
}

void cgiGetSysInfo(int argc, char **argv, char *varValue) {
   extern int glbUploadStatus;  // it is defined in upload.c

   if ( strcmp(argv[2], "upldSts") == 0 )
      sprintf(varValue, "%d", glbUploadStatus); 
   else
      strcpy(varValue, "&nbsp");
}

int cgiReboot(void)
{
	printf("cgiReboot()\n");
	return -1;
}

void cgiWriteUpgStep1Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function upgAction(){\n");
		fprintf(fs, "	var loc = 'wecUpgradeInfo.cgi?upgStep=2';\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('upgAction()', 1000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");		
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");		
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}	
}

void cgiWriteUpgStep2Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function upgAction(){\n");
		fprintf(fs, "	var loc = 'wecUpgradeInfo.cgi?upgStep=3';\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('upgAction()', 1000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}	
}

void cgiWriteUpgStep3Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function upgAction(){\n");
		fprintf(fs, "	var loc = 'wecUpgradeInfo.cgi?upgStep=4';\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('upgAction()', 1000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}	
}

void cgiWriteUpgStep4Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function upgAction(){\n");
		fprintf(fs, "	var loc = 'wecUpgradeInfo.cgi?upgStep=5';\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('upgAction()', 1000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'main.html';\n");
		fprintf(fs, "	var code = 'window.top.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='red'>Firmware uploading failed</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
		bcmSystemReboot();
	}	
}

void cgiWriteUpgStep5Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function upgAction(){\n");
		fprintf(fs, "	var loc = 'wecUpgradeInfo.cgi?upgStep=6';\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('upgAction()', 2000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Write image to flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'main.html';\n");
		fprintf(fs, "	var code = 'window.top.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='red'>Firmware uploading failed</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
		bcmSystemReboot();
	}	
}

void cgiWriteUpgStep0Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'main.html';\n");
		fprintf(fs, "	var code = 'window.top.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Write image to flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='green'>Firmware uploading success</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'main.html';\n");
		fprintf(fs, "	var code = 'window.top.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");			
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Write image to flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='red'>Firmware uploading failed</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}	
	bcmSystemReboot();
}

void cgiWriteUpgPage(int ret, FILE *fs, PWEB_NTWK_VAR pWebVar)
{
	switch(pWebVar->upgStep)
	{
		case 0:
		{
			return cgiWriteUpgStep0Page(fs, ret);
		}
		case 1:
		{
			return cgiWriteUpgStep1Page(fs, ret);
		}
		case 2:
		{
			return cgiWriteUpgStep2Page(fs, ret);
		}
		case 3:
		{
			return cgiWriteUpgStep3Page(fs, ret);
		}
		case 4:
		{
			return cgiWriteUpgStep4Page(fs, ret);
		}
		case 5:
		{
			return cgiWriteUpgStep5Page(fs, ret);
		}
		default:
		{
			//return cgiWriteUpgStep6Page(fs, ret);
			break;
		}
	}
	
}

void cgiWriteMessagePage(FILE *fs, char *title,
                         char *msg, char *location) {
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<title>EoC</title>\n");

   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n");
   fprintf(fs, "function btnBack() {\n");
   fprintf(fs, "   var code = 'location=\"%s\"';\n", location);
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

   fprintf(fs, "</head>\n");
   fprintf(fs, "<body>\n<blockquote>\n<form>\n");
   fprintf(fs, "<b>%s</b><br><br>\n", title);
   fprintf(fs, "%s<br><br>\n", msg);

   if ( location != NULL ) {
      fprintf(fs, "<center>\n");
      fprintf(fs, "<input type='button' " \
                    "value='&nbsp;Back&nbsp;' " \
                    "onClick='btnBack()'>\n");
      fprintf(fs, "</center>\n");
   }

   fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
   fflush(fs);
}

void do_auth(char *userid, char *passwd, char *realm,
             char *sptPasswd, char *usrPasswd)
{
   /*
     http://192.168.1.1 after sysPassword and usrPassword changed by TR069.
     we should refresh password information to make it pass auth.
   */
   //cgiRefreshPassword("sysPassword");
   //cgiRefreshPassword("sptPassword");
   //cgiRefreshPassword("usrPassword");
   strcpy(userid, glbWebVar.sysUserName);
   strcpy(passwd, glbWebVar.sysPassword);
   strcpy(sptPasswd, glbWebVar.sptPassword);
   strcpy(usrPasswd, glbWebVar.usrPassword);
   strcpy(realm, "EoC CBAT");
}

/* Converts hexadecimal to decimal (character): */

char hexToDec(char *what) {
   char digit;

   digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
   digit *= 16;
   digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));

   return (digit);
}

/* Unescapes "%"-escaped characters in a query: */

void unescapeUrl(char *url) {
   int x,y,len;

   len = strlen(url);

   for ( x = 0, y = 0; url[y]; x++, y++) {
      if ( (url[x] = url[y]) == '%' &&
           y < (len - 2) ) {
         url[x] = hexToDec(&url[y+1]);
         y += 2;
      }
   }
   url[x] = '\0';
}

void cgiUrlDecode(char *s) {
   char *pstr = s;

   /* convert plus (+) to space (' ') */
   for ( pstr = s;
          pstr != NULL && *pstr != '\0';
          pstr++ )
      if ( *pstr == '+' ) *pstr = ' ';

   unescapeUrl(s);
}

void cgiParseSet(char *path) {
   char *query = strchr(path, '?');
   char *name, *value, *next;

   /* Parse name=value&name=value& ... &name=value */
   if (query) {
      for (value = ++query; value; value = next) {
         name = strsep(&value, "=");
         if (name) {
            next = value;
            value = strsep(&next, "&");
            if (!value) {
               value = next;
               next = NULL;
            }
            cgiUrlDecode(value);
            cgiSetVar(name, value);
         } else
            next = NULL;
      }
   }
}

void cgiTestParseSet(char *path) {
   char *query = strchr(path, '?');
   char *name, *value, *next;

   /* Parse name=value&name=value& ... &name=value */
   if (query) {
      for (value = ++query; value; value = next) {
         name = strsep(&value, "=");
         if (name) {
            next = value;
            value = strsep(&next, "&");
            if (!value) {
               value = next;
               next = NULL;
            }
            cgiUrlDecode(value);
            cgiSetTestVar(name, value);
         } else
            next = NULL;
      }
   }
}

CGI_STATUS cgiGetValueByName(char *query, char *id, char *val) {
   int ret = CGI_STS_ERR_GENERAL;
   int idlen;
   char *name = NULL, *value = NULL, *pc = NULL;

   /* validate & initialize return value */
   if ( val == NULL ) return ret;
   *val = '\0';

   if ( query == NULL || id == NULL) return ret;

   if ( *query =='\0' || *id == '\0') return ret;

   /* search for the given id */
   /* Parse name=value&name=value& ... &name=value */
   name = strstr(query, id);
   idlen = strlen(id);
   while ( name != NULL ) {
	 if (name[idlen] != '=')  
             name = strstr(name+idlen, id);
         else
	     break;
   }	 
   
   if ( name == NULL ) return CGI_STS_ERR_FIND;
   
   value = name+idlen;
   for ( pc = val, value++;
             value != NULL && *value != '&' && *value != '\0';
             pc++, value++ )
         *pc = *value;
   *pc = '\0';
   cgiUrlDecode(val);
   
   ret = CGI_STS_OK;
   return ret;
}


