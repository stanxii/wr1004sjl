#ifndef __CGI_NTWK_H__
#define __CGI_NTWK_H__

#include <stdio.h>
#include <fcntl.h>

/********************** Global Types ****************************************/



#if defined(__cplusplus)
extern "C" {
#endif

//void writePageHeader(FILE *fs);
void cgiNtwkView(char *query, FILE *fs);
void cgiPortPropety(char *query, FILE *fs);
void cgiPortStatsView(char *query, FILE *fs) ;
void cgiOptlogView(char *query, FILE *fs) ;
void cgiSyslogView(char *query, FILE *fs) ;
void cgiAlarmlogView(char *query, FILE *fs) ;
void cgiAlarmlogDetailView(char *query, FILE *fs) ;
void cgiTopologyView(char *query, FILE *fs);
//void cgiCltProfileView(char *query, FILE *fs) ;
void cgiCltProfile(char *query, FILE *fs);
void cgiCltMgmt(char *query, FILE *fs);
//void cgiCnuProfileView(char *query, FILE *fs) ;
void cgiCnuProfile(char *query, FILE *fs);
void cgiCnuProfileExt(char *query, FILE *fs);
void cgiCnuMgmt(char *query, FILE *fs);
void cgiLinkDiag(char *query, FILE *fs) ;
void cgiLinkDiagResult(char *query, FILE *fs);

#if defined(__cplusplus)
}
#endif

#endif
