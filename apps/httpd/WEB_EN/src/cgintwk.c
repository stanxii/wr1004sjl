#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/syslog.h>
#include <net/if_arp.h>
#include <net/route.h>

#include <cgicmd.h>
#include <cgintwk.h>
#include <cgimain.h>
#include <syscall.h>
#include <httpd.h>
#include <bcmtypes.h>
#include <ifcuiweb.h>
#include <bcmcfm.h>
#include <http2dbs.h>
#include <dbsapi.h>
#include <boardapi.h>

extern WEB_NTWK_VAR glbWebVar;

int mapRate2SelectIndex(uint32_t rate)
{
	if( (rate==0)||(rate==(32*0x1fff)) )
	{
		return 0;
	}
	else if( (rate>0)&&(rate<=128) )
	{
		return 1;
	}
	else if( (rate>128)&&(rate<=256) )
	{
		return 2;
	}
	else if( (rate>256)&&(rate<=512) )
	{
		return 3;
	}
	else if( (rate>512)&&(rate<=1024) )
	{
		return 4;
	}
	else if( (rate>1024)&&(rate<=1536) )
	{
		return 5;
	}
	else if( (rate>1536)&&(rate<=2048) )
	{
		return 6;
	}
	else if( (rate>2048)&&(rate<=3072) )
	{
		return 7;
	}
	else if( (rate>3072)&&(rate<=4096) )
	{
		return 8;
	}
	else if( (rate>4096)&&(rate<=6144) )
	{
		return 9;
	}
	else if( rate > 6144 )
	{
		return 10;
	}
	return 0;
}

void writePageHeader(FILE *fs) {
	fprintf(fs, "<html><head>\n");
	fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n</head>\n<body>\n<blockquote>\n<form>\n");
}

void writePopErrorPage(FILE *fs)
{
	writePageHeader(fs);		
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='maintitle'><b>System Message</b></td>\n");		
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='mainline' width=450></td>\n");		
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br>This page displays the results of your operating.</br>\n");
	fprintf(fs, "<br><br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagret' width=250>Operating Results</td>\n");	
	fprintf(fs, "		<td class='diagret' width=150>&nbsp;</td>\n");	
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr><td class='diagdata' colspan=2>&nbsp;</td></tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>Operating Status</td>\n");
	fprintf(fs, "		<td class='diagdata'><b><font color=red>Failed</font></b></td>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "	<tr><td class='diagdata' colspan=2>&nbsp;</td></tr>\n");
	fprintf(fs, "	<tr><td class='listend' colspan=2></td></tr>\n");	
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br><br>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<input type='button' class='btn2L' onClick='window.close()' value='Close'>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);	
}

void cgiPortPropety(char *query, FILE *fs)
{
	char action[IFC_LARGE_LEN];
	int id = 0;

	cgiGetValueByName(query, "portid", action);
	id = atoi(action);

	fprintf(fs, "<html><head>\n");
	fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
	
	fprintf(fs, "<script language='javascript'>\n");

	fprintf(fs, "function frmLoad() {\n");
	fprintf(fs, "	with ( document.forms[0] ) {\n");
	
	if( 1 == id )
	{
		fprintf(fs, "		portSpeed.selectedIndex = %d\n", glbWebVar.eth1speed);
		fprintf(fs, "		portDuplex.selectedIndex = %d\n", glbWebVar.eth1duplex);
		fprintf(fs, "		portPri.selectedIndex = %d\n", glbWebVar.eth1pri);
		fprintf(fs, "		portFlowControl.selectedIndex = %d\n", glbWebVar.eth1fc);
		fprintf(fs, "		portStatus.selectedIndex = %d\n", glbWebVar.eth1sts);
	}
	else if( 2 == id )
	{
		fprintf(fs, "		portSpeed.selectedIndex = %d\n", glbWebVar.eth2speed);
		fprintf(fs, "		portDuplex.selectedIndex = %d\n", glbWebVar.eth2duplex);
		fprintf(fs, "		portPri.selectedIndex = %d\n", glbWebVar.eth2pri);
		fprintf(fs, "		portFlowControl.selectedIndex = %d\n", glbWebVar.eth2fc);
		fprintf(fs, "		portStatus.selectedIndex = %d\n", glbWebVar.eth2sts);
	}
	else if( 3 == id )
	{
		fprintf(fs, "		portSpeed.selectedIndex = %d\n", glbWebVar.eth3speed);
		fprintf(fs, "		portDuplex.selectedIndex = %d\n", glbWebVar.eth3duplex);
		fprintf(fs, "		portPri.selectedIndex = %d\n", glbWebVar.eth3pri);
		fprintf(fs, "		portFlowControl.selectedIndex = %d\n", glbWebVar.eth3fc);
		fprintf(fs, "		portStatus.selectedIndex = %d\n", glbWebVar.eth3sts);
	}
	else
	{
		fprintf(fs, "		portSpeed.selectedIndex = %d\n", glbWebVar.eth4speed);
		fprintf(fs, "		portDuplex.selectedIndex = %d\n", glbWebVar.eth4duplex);
		fprintf(fs, "		portPri.selectedIndex = %d\n", glbWebVar.eth4pri);
		fprintf(fs, "		portFlowControl.selectedIndex = %d\n", glbWebVar.eth4fc);
		fprintf(fs, "		portStatus.selectedIndex = %d\n", glbWebVar.eth4sts);
	}

	fprintf(fs, "	}\n");
	fprintf(fs, "}\n");	
	
	fprintf(fs, "function btnSave(portid){\n");
	fprintf(fs, "	var loc = 'portPropety.cgi?portid=%d';\n", id);
	fprintf(fs, "	with (document.forms[0]){\n");
	if( 1 == id )
	{
		fprintf(fs, "		loc += '&eth1speed=' + portSpeed.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth1duplex=' + portDuplex.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth1pri=' + portPri.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth1fc=' + portFlowControl.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth1sts=' + portStatus.selectedIndex;\n");
	}
	else if( 2 == id )
	{
		fprintf(fs, "		loc += '&eth2speed=' + portSpeed.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth2duplex=' + portDuplex.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth2pri=' + portPri.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth2fc=' + portFlowControl.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth2sts=' + portStatus.selectedIndex;\n");		
	}
	else if( 3 == id )
	{
		fprintf(fs, "		loc += '&eth3speed=' + portSpeed.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth3duplex=' + portDuplex.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth3pri=' + portPri.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth3fc=' + portFlowControl.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth3sts=' + portStatus.selectedIndex;\n");		
	}
	else
	{
		fprintf(fs, "		loc += '&eth4speed=' + portSpeed.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth4duplex=' + portDuplex.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth4pri=' + portPri.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth4fc=' + portFlowControl.selectedIndex;\n");
		fprintf(fs, "		loc += '&eth4sts=' + portStatus.selectedIndex;\n");
	}
	fprintf(fs, "		if( portStatus.selectedIndex == 0 ){\n");
	fprintf(fs, "			if(!confirm('Warning: The port will be shutdown. Are you sure to do it?')) return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");		
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");	
	
	fprintf(fs, "</script>\n");
	
	fprintf(fs, "</head>\n<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n\n");

	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	if( 1 == id )
	{
		fprintf(fs, "<tr><td class='maintitle'><b>ETH1 Propety</b></td>\n</tr>\n</table>\n");
	}
	else if( 2 == id )
	{
		fprintf(fs, "<tr><td class='maintitle'><b>ETH2 Propety</b></td>\n</tr>\n</table>\n");
	}
	else if( 3 == id )
	{
		fprintf(fs, "<tr><td class='maintitle'><b>Cable1 Propety</b></td>\n</tr>\n</table>\n");
	}
	else
	{
		fprintf(fs, "<tr><td class='maintitle'><b>Cable2 Propety</b></td>\n</tr>\n</table>\n");
	}

	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br><br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret' width=300>Parameter</td>\n");
	fprintf(fs, "	<td class='diagret' width=260>Value</td>\n");
	fprintf(fs, "</tr>\n");
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>&nbsp;</td>\n");	
	fprintf(fs, "</tr>\n");
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Port Speed</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name=portSpeed style='WIDTH: 150px'>\n");
	fprintf(fs, "			<option value=0>Auto Negotiation\n");
	fprintf(fs, "			<option value=1>10Mbps\n");
	fprintf(fs, "			<option value=2>100Mbps\n");
	fprintf(fs, "			<option value=3>1000Mbps\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Port Duplex</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name=portDuplex style='WIDTH: 150px'>\n");
	fprintf(fs, "			<option value=0>Auto Negotiation\n");
	fprintf(fs, "			<option value=1>Half Duplex\n");
	fprintf(fs, "			<option value=2>Full Duplex\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Port Priority</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name=portPri style='WIDTH: 150px'>\n");
	fprintf(fs, "			<option value=0>0\n");
	fprintf(fs, "			<option value=1>1\n");
	fprintf(fs, "			<option value=2>2\n");
	fprintf(fs, "			<option value=3>3\n");
	fprintf(fs, "			<option value=4>4\n");
	fprintf(fs, "			<option value=5>5\n");
	fprintf(fs, "			<option value=6>6\n");
	fprintf(fs, "			<option value=7>7\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Flow Control</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name=portFlowControl style='WIDTH: 150px'>\n");
	fprintf(fs, "			<option value=0>Disable\n");
	fprintf(fs, "			<option value=1>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Port Status</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name=portStatus style='WIDTH: 150px'>\n");
	fprintf(fs, "			<option value=0>Disable\n");
	fprintf(fs, "			<option value=1>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>&nbsp;</td>\n");	
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='listend' colspan=2></td>\n");	
	fprintf(fs, "</tr>\n");

	fprintf(fs, "</table>\n");

	fprintf(fs, "<p>\n");
	fprintf(fs, "	<input type='button' class='btn2L' onClick='location.href=\"wecPortPropety.html\"' value='Return'>\n");
	fprintf(fs, "	<input type='button' class='btn2L' onClick='btnSave()' value='Save'>\n");
	fprintf(fs, "</p>\n");

	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiPortStatsView(char *query, FILE *fs) 
{
	char action[IFC_LARGE_LEN];
	int id = 0;

	cgiGetValueByName(query, "portid", action);
	id = atoi(action);

	writePageHeader(fs);

	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	if( 1 == id )
	{
		fprintf(fs, "<tr><td class='maintitle'><b>ETH1 Statistics</b></td>\n</tr>\n</table>\n");
	}
	else if( 2 == id )
	{
		fprintf(fs, "<tr><td class='maintitle'><b>ETH2 Statistics</b></td>\n</tr>\n</table>\n");
	}
	else if( 3 == id )
	{
		fprintf(fs, "<tr><td class='maintitle'><b>Cable1 Statistics</b></td>\n</tr>\n</table>\n");
	}
	else
	{
		fprintf(fs, "<tr><td class='maintitle'><b>Cable2 Statistics</b></td>\n</tr>\n</table>\n");
	}
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br><br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret' width=300>RX packets</td>\n");
	fprintf(fs, "	<td class='diagret' width=260>Counter</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>&nbsp;</td>\n");	
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Broadcast</td>\n");
	if( 1 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth1rxbc);
	}
	else if( 2 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth2rxbc);
	}
	else if( 3 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth3rxbc);
	}
	else
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth4rxbc);
	}	
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Unicast</td>\n");
	if( 1 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth1rxu);
	}
	else if( 2 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth2rxu);
	}
	else if( 3 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth3rxu);
	}
	else
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth4rxu);
	}	
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Multicast</td>\n");
	if( 1 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth1rxm);
	}
	else if( 2 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth2rxm);
	}
	else if( 3 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth3rxm);
	}
	else
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth4rxm);
	}	
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Errors</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Runts</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Giants</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>CRC Errors</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Frame</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Aborts</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Ignored</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>&nbsp;</td>\n");	
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret'>TX packets</td>\n");
	fprintf(fs, "	<td class='diagret'>Counter</td>\n");
	fprintf(fs, "</tr>\n");
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>&nbsp;</td>\n");	
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Broadcast</td>\n");
	if( 1 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth1txbc);
	}
	else if( 2 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth2txbc);
	}
	else if( 3 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth3txbc);
	}
	else
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth4txbc);
	}	
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Unicast</td>\n");
	if( 1 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth1txu);
	}
	else if( 2 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth2txu);
	}
	else if( 3 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth3txu);
	}
	else
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth4txu);
	}	
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Multicast</td>\n");
	if( 1 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth1txm);
	}
	else if( 2 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth2txm);
	}
	else if( 3 == id )
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth3txm);
	}
	else
	{
		fprintf(fs, "	<td class='diagdata'>%d</td>\n", glbWebVar.eth4txm);
	}	
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Errors</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Runts</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Giants</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>CRC Errors</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Frame</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Aborts</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Ignored</td>\n");
	fprintf(fs, "	<td class='diagdata'>0</td>\n");
	fprintf(fs, "</tr>\n");
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>&nbsp;</td>\n");	
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='listend' colspan=2></td>\n");	
	fprintf(fs, "</tr>\n");

	fprintf(fs, "</table>\n");

	fprintf(fs, "<p>\n");
	fprintf(fs, "	<input type='button' class='btn2L' onClick='location.href=\"wecPortStas.cgi\"' value='Return'>\n");
	fprintf(fs, "</p>\n");

	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiOptlogViewAll(char *query, FILE *fs)
{   
	int i=0;
	int logCount = 0;
	st_dbsOptlog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	writePageHeader(fs);

	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>Operating Log</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=900></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br>Display operating logs in the table list bellow [Filter condition=All]:</br>\n<br><br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='diagret' width=160>Time</td>\n");
	fprintf(fs, "<td class='diagret' width=100>Operator</td>\n");
	fprintf(fs, "<td class='diagret' width=100>Resulte</td>\n");
	fprintf(fs, "<td class='diagret' width=400>Log Information</td>\n</tr>\n</table>\n<br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=2>\n");
	//fprintf(fs, "\n");
   
	// write body
	for( i=1; i<=512; i++ )
	{
		if( http2dbs_getOptlog(i, &st_log) == CMM_SUCCESS )
		{
			logCount++;
			tim = localtime((const time_t *)&st_log.time);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
			tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			if( st_log.result == 0 )
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='diagdata' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='diagdata' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='diagdata' width=98>SUCCESS</td>\n");
				fprintf(fs, "<td class='diagdata' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
			else
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='logfailed' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='logfailed' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='logfailed' width=98>FAILED</td>\n");
				fprintf(fs, "<td class='logfailed' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
		}
		else
		{
			break;
		}
	}
	if(logCount == 0 )
	{
		fprintf(fs, "<tr>\n<td class='diagdata' colspan=4>No operating log to list</td>\n</tr>\n");
	}
	
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=760></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p><input type='button' class='btn2L' onClick='location.href=\"wecOptlog.html\"' value='Return'>\n");
	fprintf(fs, "<input type='button' class='btn2L' onClick='window.location.reload()' value='Refresh'>\n</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiCltProfileView(char *query, FILE *fs) 
{
	int id = 0;
	st_dbsCltConf profile;
	char action[IFC_LARGE_LEN];

	cgiGetValueByName(query, "viewid", action);
	id = atoi(action);

	writePageHeader(fs);

	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>Profile</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br>Display the current profile parameters of the CLT which you have selected from the topology.</br>\n<br><br>\n");
		
	if( CMM_SUCCESS == dbsGetCltconf(dbsdev, id, &profile) )
	{
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");
		fprintf(fs, "<tr>\n<td class='diagret' width=300>General Information</td>\n");
		fprintf(fs, "<td class='diagret' width=260>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		
		fprintf(fs, "<tr>\n<td class='diagdata'>CLT ID:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", profile.id);

		fprintf(fs, "<tr>\n<td class='diagdata'>Additional PIB ID:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", profile.col_base);

		fprintf(fs, "<tr>\n<td class='diagdata'>Profile Status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_row_sts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Cable Bandwidth Limiting</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>uplink limiting status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_curate?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>uplink limit:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d kbps</td>\n</tr>\n", profile.col_curate);

		fprintf(fs, "<tr>\n<td class='diagdata'>downlink limiting status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_cdrate?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>downlink limit:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d kbps</td>\n</tr>\n", profile.col_cdrate);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Bridged Address Aging</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>local bridged table aging:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d minutes</td>\n</tr>\n", profile.col_loagTime);

		fprintf(fs, "<tr>\n<td class='diagdata'>remote bridged table aging:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d minutes</td>\n</tr>\n", profile.col_reagTime);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Default CAP</td>\n<td class='diagret'>[Lowest Priority Classification]</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>igmp:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_igmpPri);

		fprintf(fs, "<tr>\n<td class='diagdata'>unicast:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_unicastPri);

		fprintf(fs, "<tr>\n<td class='diagdata'>igmp managed multicast stream:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_avsPri);

		fprintf(fs, "<tr>\n<td class='diagdata'>multicast/broadcast:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_mcastPri);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Tx Buffer Allocation Based On Priority</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_tbaPriSts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Assign Priority Using VLAN Tags</td>\n<td class='diagret'>[High]</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_cosPriSts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>cos 0:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos0pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 1:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos1pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 2:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos2pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 3:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos3pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 4:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos4pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 5:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos5pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 6:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos6pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 7:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos7pri);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Assign Priority Using Traffic Class</td>\n<td class='diagret'>[Low]</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_tosPriSts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>tos 0:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos0pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 1:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos1pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 2:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos2pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 3:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos3pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 4:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos4pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 5:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos5pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 6:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos6pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 7:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos7pri);		
		
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='listend'></td>\n<td class='listend'></td>\n</tr>\n");

		fprintf(fs, "</table>\n");
	}
	
	fprintf(fs, "<p><input type='button' class='btn2L' onClick='location.href=\"previewTopology.cgi\"' value='Return'></p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiCnuProfileView(char *query, FILE *fs) 
{
	int id = 0;
	int cltid = 0;
  	int cnuid = 0;
	st_dbsProfile profile;
	char action[IFC_LARGE_LEN];

	cgiGetValueByName(query, "viewid", action);
	id = atoi(action);

	writePageHeader(fs);

	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>Profile</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br>Display the current profile parameters of the CNU which you have selected from the topology.</br>\n<br><br>\n");
		
	if( CMM_SUCCESS == dbsGetProfile(dbsdev, id, &profile) )
	{
		cltid = id/MAX_CNUS_PER_CLT + 1;
		cnuid = id%MAX_CNUS_PER_CLT;
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");
		fprintf(fs, "<tr>\n<td class='diagret' width=300>General Information</td>\n");
		fprintf(fs, "<td class='diagret' width=260>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		
		fprintf(fs, "<tr>\n<td class='diagdata'>CNU ID:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d/%d</td>\n</tr>\n", cltid, cnuid);

		fprintf(fs, "<tr>\n<td class='diagdata'>Additional PIB ID:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", profile.col_base);

		fprintf(fs, "<tr>\n<td class='diagdata'>Profile Status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_row_sts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Mac Address Limiting</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>Status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_macLimit?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>MAX Bridged Hosts:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", (profile.col_macLimit==65)?0:profile.col_macLimit);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Cable Bandwidth Limiting</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>uplink limiting status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_curate?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>uplink limit:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d kbps</td>\n</tr>\n", profile.col_curate);

		fprintf(fs, "<tr>\n<td class='diagdata'>downlink limiting status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_cdrate?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>downlink limit:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d kbps</td>\n</tr>\n", profile.col_cdrate);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Bridged Address Aging</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>local bridged table aging:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d minutes</td>\n</tr>\n", profile.col_loagTime);

		fprintf(fs, "<tr>\n<td class='diagdata'>remote bridged table aging:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d minutes</td>\n</tr>\n", profile.col_reagTime);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Default CAP</td>\n<td class='diagret'>[Lowest Priority Classification]</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>igmp:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_igmpPri);

		fprintf(fs, "<tr>\n<td class='diagdata'>unicast:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_unicastPri);

		fprintf(fs, "<tr>\n<td class='diagdata'>igmp managed multicast stream:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_avsPri);

		fprintf(fs, "<tr>\n<td class='diagdata'>multicast/broadcast:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_mcastPri);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Tx Buffer Allocation Based On Priority</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_tbaPriSts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Assign Priority Using VLAN Tags</td>\n<td class='diagret'>[High]</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_cosPriSts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>cos 0:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos0pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 1:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos1pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 2:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos2pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 3:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos3pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 4:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos4pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 5:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos5pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 6:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos6pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>cos 7:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_cos7pri);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Assign Priority Using Traffic Class</td>\n<td class='diagret'>[Low]</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_tosPriSts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>tos 0:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos0pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 1:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos1pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 2:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos2pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 3:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos3pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 4:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos4pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 5:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos5pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 6:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos6pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>tos 7:</td>\n");
		fprintf(fs, "<td class='diagdata'>CAP %d</td>\n</tr>\n", profile.col_tos7pri);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Storm Filter Settings</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>broadcast storm filter:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_sfbSts?"Enable":"Disable");
		fprintf(fs, "<tr>\n<td class='diagdata'>unicast storm filter:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_sfuSts?"Enable":"Disable");
		fprintf(fs, "<tr>\n<td class='diagdata'>multicast storm filter:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_sfmSts?"Enable":"Disable");
		fprintf(fs, "<tr>\n<td class='diagdata'>storm filter level:</td>\n");
		if( 0 == profile.col_sfRate )
		{			
			fprintf(fs, "<td class='diagdata'>Disable</td>\n</tr>\n");
		}
		else
		{
			fprintf(fs, "<td class='diagdata'>%d Kpps</td>\n</tr>\n", (1<<(profile.col_sfRate-1)));
		}

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>802.1Q VLAN Settings</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_vlanSts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>ETH1 PVID:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", profile.col_eth1vid);
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH2 PVID:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", profile.col_eth2vid);
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH3 PVID:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", profile.col_eth3vid);
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH4 PVID:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", profile.col_eth4vid);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Port Priority Settings</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>Port Priority Settings:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_portPriSts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>ETH1 Priority:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", profile.col_eth1pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH2 Priority:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", profile.col_eth2pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH3 Priority:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", profile.col_eth3pri);
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH4 Priority:</td>\n");
		fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", profile.col_eth4pri);

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Port Speed Limiting</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>Port RX Limiting:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_rxLimitSts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>cpu port rx rate:</td>\n");
		if( (0==profile.col_cpuPortRxRate)||(profile.col_cpuPortRxRate==(32*0x1fff)))
		{			
			fprintf(fs, "<td class='diagdata'>* Kbps</td>\n</tr>\n");
		}
		else
		{
			fprintf(fs, "<td class='diagdata'>%d Kbps</td>\n</tr>\n", profile.col_cpuPortRxRate);
		}

		fprintf(fs, "<tr>\n<td class='diagdata'>ETH1 rx rate:</td>\n");
		if( (0==profile.col_eth1rx)||(profile.col_eth1rx==(32*0x1fff)))
		{			
			fprintf(fs, "<td class='diagdata'>* Kbps</td>\n</tr>\n");
		}
		else
		{
			fprintf(fs, "<td class='diagdata'>%d Kbps</td>\n</tr>\n", profile.col_eth1rx);
		}
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH2 rx rate:</td>\n");
		if( (0==profile.col_eth2rx)||(profile.col_eth2rx==(32*0x1fff)))
		{			
			fprintf(fs, "<td class='diagdata'>* Kbps</td>\n</tr>\n");
		}
		else
		{
			fprintf(fs, "<td class='diagdata'>%d Kbps</td>\n</tr>\n", profile.col_eth2rx);
		}
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH3 rx rate:</td>\n");
		if( (0==profile.col_eth3rx)||(profile.col_eth3rx==(32*0x1fff)))
		{			
			fprintf(fs, "<td class='diagdata'>* Kbps</td>\n</tr>\n");
		}
		else
		{
			fprintf(fs, "<td class='diagdata'>%d Kbps</td>\n</tr>\n", profile.col_eth3rx);
		}
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH4 rx rate:</td>\n");
		if( (0==profile.col_eth4rx)||(profile.col_eth4rx==(32*0x1fff)))
		{			
			fprintf(fs, "<td class='diagdata'>* Kbps</td>\n</tr>\n");
		}
		else
		{
			fprintf(fs, "<td class='diagdata'>%d Kbps</td>\n</tr>\n", profile.col_eth4rx);
		}

		fprintf(fs, "<tr>\n<td class='diagdata'>Port TX Limiting:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_txLimitSts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>cpu port tx rate:</td>\n");
		if( (0==profile.col_cpuPortTxRate)||(profile.col_cpuPortTxRate==(32*0x1fff)))
		{			
			fprintf(fs, "<td class='diagdata'>* Kbps</td>\n</tr>\n");
		}
		else
		{
			fprintf(fs, "<td class='diagdata'>%d Kbps</td>\n</tr>\n", profile.col_cpuPortTxRate);
		}

		fprintf(fs, "<tr>\n<td class='diagdata'>ETH1 tx rate:</td>\n");
		if( (0==profile.col_eth1tx)||(profile.col_eth1tx==(32*0x1fff)))
		{			
			fprintf(fs, "<td class='diagdata'>* Kbps</td>\n</tr>\n");
		}
		else
		{
			fprintf(fs, "<td class='diagdata'>%d Kbps</td>\n</tr>\n", profile.col_eth1tx);
		}
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH2 tx rate:</td>\n");
		if( (0==profile.col_eth2tx)||(profile.col_eth2tx==(32*0x1fff)))
		{			
			fprintf(fs, "<td class='diagdata'>* Kbps</td>\n</tr>\n");
		}
		else
		{
			fprintf(fs, "<td class='diagdata'>%d Kbps</td>\n</tr>\n", profile.col_eth2tx);
		}
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH3 tx rate:</td>\n");
		if( (0==profile.col_eth3tx)||(profile.col_eth3tx==(32*0x1fff)))
		{			
			fprintf(fs, "<td class='diagdata'>* Kbps</td>\n</tr>\n");
		}
		else
		{
			fprintf(fs, "<td class='diagdata'>%d Kbps</td>\n</tr>\n", profile.col_eth3tx);
		}
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH4 tx rate:</td>\n");
		if( (0==profile.col_eth4tx)||(profile.col_eth4tx==(32*0x1fff)))
		{			
			fprintf(fs, "<td class='diagdata'>* Kbps</td>\n</tr>\n");
		}
		else
		{
			fprintf(fs, "<td class='diagdata'>%d Kbps</td>\n</tr>\n", profile.col_eth4tx);
		}

		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagret'>Port Link Status Control</td>\n<td class='diagret'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");

		fprintf(fs, "<tr>\n<td class='diagdata'>status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_psctlSts?"Enable":"Disable");

		fprintf(fs, "<tr>\n<td class='diagdata'>cpu port link status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_cpuPortSts?"Enable":"Disable");
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH1 link status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_eth1sts?"Enable":"Disable");
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH2 link status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_eth2sts?"Enable":"Disable");
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH3 link status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_eth3sts?"Enable":"Disable");
		fprintf(fs, "<tr>\n<td class='diagdata'>ETH4 link status:</td>\n");
		fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", profile.col_eth4sts?"Enable":"Disable");
		
		fprintf(fs, "<tr>\n<td class='diagdata'>&nbsp;</td>\n<td class='diagdata'>&nbsp;</td>\n</tr>\n");
		fprintf(fs, "<tr>\n<td class='listend'></td>\n<td class='listend'></td>\n</tr>\n");

		fprintf(fs, "</table>\n");		
		//fprintf(fs, "\n");
	}
	
	fprintf(fs, "<p><input type='button' class='btn2L' onClick='location.href=\"previewTopology.cgi\"' value='Return'></p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiCltProfile(char *query, FILE *fs) 
{
	int id = 0;
	st_dbsCltConf profile;
	char action[IFC_LARGE_LEN];

	cgiGetValueByName(query, "cltid", action);
	id = atoi(action);

	if( CMM_SUCCESS != dbsGetCltconf(dbsdev, id, &profile) )
	{
		/*Error*/
		strcpy(glbWebVar.returnUrl, "cltManagement.cmd");
		glbWebVar.wecOptCode = CMM_FAILED;
		do_ej("/webs/wecOptResult2.html", fs);
		return;
	}

	fprintf(fs, "<html><head>\n");
	fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
	fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
	fprintf(fs, "<SCRIPT language=JavaScript>\n");
	fprintf(fs, "function frmLoad() {\n");
	fprintf(fs, "	with ( document.forms[0] ) {\n");
	fprintf(fs, "		loagTime.value = %d\n", profile.col_loagTime);
	fprintf(fs, "		reagTime.value = %d\n", profile.col_reagTime);
	fprintf(fs, "		igmpPri.selectedIndex = %d\n", profile.col_igmpPri);
	fprintf(fs, "		unicastPri.selectedIndex = %d\n", profile.col_unicastPri);
	fprintf(fs, "		avsPri.selectedIndex = %d\n", profile.col_avsPri);
	fprintf(fs, "		mcastPri.selectedIndex = %d\n", profile.col_mcastPri);	
	fprintf(fs, "		tbaPriSts.selectedIndex = %d\n", profile.col_tbaPriSts?1:0);
	fprintf(fs, "		asPriType.selectedIndex = %d\n", profile.col_cosPriSts?0:1);
	if( profile.col_cosPriSts )
	{
		fprintf(fs, "		pri0QueueMap.selectedIndex = %d\n", profile.col_cos0pri);
		fprintf(fs, "		pri1QueueMap.selectedIndex = %d\n", profile.col_cos1pri);
		fprintf(fs, "		pri2QueueMap.selectedIndex = %d\n", profile.col_cos2pri);
		fprintf(fs, "		pri3QueueMap.selectedIndex = %d\n", profile.col_cos3pri);
		fprintf(fs, "		pri4QueueMap.selectedIndex = %d\n", profile.col_cos4pri);
		fprintf(fs, "		pri5QueueMap.selectedIndex = %d\n", profile.col_cos5pri);
		fprintf(fs, "		pri6QueueMap.selectedIndex = %d\n", profile.col_cos6pri);
		fprintf(fs, "		pri7QueueMap.selectedIndex = %d\n", profile.col_cos7pri);
	}
	else
	{
		fprintf(fs, "		pri0QueueMap.selectedIndex = %d\n", profile.col_tos0pri);
		fprintf(fs, "		pri1QueueMap.selectedIndex = %d\n", profile.col_tos1pri);
		fprintf(fs, "		pri2QueueMap.selectedIndex = %d\n", profile.col_tos2pri);
		fprintf(fs, "		pri3QueueMap.selectedIndex = %d\n", profile.col_tos3pri);
		fprintf(fs, "		pri4QueueMap.selectedIndex = %d\n", profile.col_tos4pri);
		fprintf(fs, "		pri5QueueMap.selectedIndex = %d\n", profile.col_tos5pri);
		fprintf(fs, "		pri6QueueMap.selectedIndex = %d\n", profile.col_tos6pri);
		fprintf(fs, "		pri7QueueMap.selectedIndex = %d\n", profile.col_tos7pri);
	}
	fprintf(fs, "	}\n");
	fprintf(fs, "}\n");	
	
	fprintf(fs, "function setAgTime()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var msg;\n");
	fprintf(fs, "	var value;\n");
	fprintf(fs, "	var loc = 'setCltAgTime.cgi?';\n");
	fprintf(fs, "	loc += 'cltid=%d';\n", id);
	fprintf(fs, "	with (document.forms[0])\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		if ( isNaN(parseInt(loagTime.value)) == true )\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			msg = 'loagTime \"' + loagTime.value + '\" is invalid.';\n");
	fprintf(fs, "			alert(msg);\n");
	fprintf(fs, "			return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		value = parseInt(loagTime.value);\n");
	fprintf(fs, "		if ( value < 1 || value > 2000 )\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			msg = 'loagTime \"' + value + '\" is out of range [1-2000].';\n");
	fprintf(fs, "			alert(msg);\n");
	fprintf(fs, "			return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		if ( isNaN(parseInt(reagTime.value)) == true )\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			msg = 'reagTime \"' + reagTime.value + '\" is invalid.';\n");
	fprintf(fs, "			alert(msg);\n");
	fprintf(fs, "			return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		value = parseInt(reagTime.value);\n");
	fprintf(fs, "		if ( value < 1 || value > 2000 )\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			msg = 'reagTime \"' + value + '\" is out of range [1-2000].';\n");
	fprintf(fs, "			alert(msg);\n");
	fprintf(fs, "			return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		loc += '&col_loagTime=' + loagTime.value;\n");
	fprintf(fs, "		loc += '&col_reagTime=' + reagTime.value;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function setDefaultCap()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var loc = 'setCltDeCap.cgi?cltid=%d';\n", id);
	fprintf(fs, "	with (document.forms[0])\n");
	fprintf(fs, "	{\n");	
	fprintf(fs, "		loc += '&col_igmpPri=' + igmpPri.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_unicastPri=' + unicastPri.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_avsPri=' + avsPri.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_mcastPri=' + mcastPri.selectedIndex;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function setQoS()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var loc = 'setCltQoS.cgi?cltid=%d';\n", id);
	fprintf(fs, "	with (document.forms[0])\n");
	fprintf(fs, "	{\n");	
	fprintf(fs, "		loc += '&col_tbaPriSts=' + tbaPriSts.selectedIndex;\n");
	fprintf(fs, "		if( 0 != tbaPriSts.selectedIndex )\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			if( 0 == asPriType.selectedIndex )\n");
	fprintf(fs, "			{\n");
	fprintf(fs, "				loc += '&col_cosPriSts=1';\n");
	fprintf(fs, "				loc += '&col_tosPriSts=0';\n");
	fprintf(fs, "				loc += '&col_cos0pri=' + pri0QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_cos1pri=' + pri1QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_cos2pri=' + pri2QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_cos3pri=' + pri3QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_cos4pri=' + pri4QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_cos5pri=' + pri5QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_cos6pri=' + pri6QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_cos7pri=' + pri7QueueMap.selectedIndex;\n");
	fprintf(fs, "			}\n");
	fprintf(fs, "			else\n");
	fprintf(fs, "			{\n");
	fprintf(fs, "				loc += '&col_cosPriSts=0';\n");
	fprintf(fs, "				loc += '&col_tosPriSts=1';\n");
	fprintf(fs, "				loc += '&col_tos0pri=' + pri0QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_tos1pri=' + pri1QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_tos2pri=' + pri2QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_tos3pri=' + pri3QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_tos4pri=' + pri4QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_tos5pri=' + pri5QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_tos6pri=' + pri6QueueMap.selectedIndex;\n");
	fprintf(fs, "				loc += '&col_tos7pri=' + pri7QueueMap.selectedIndex;\n");
	fprintf(fs, "			}\n");
	fprintf(fs, "		}\n");	
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function saveProfile(){\n");
	fprintf(fs, "	var loc = 'saveCltProfile.cgi?cltid=%d';\n", id);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	//fprintf(fs, "\n");
	
	fprintf(fs, "</SCRIPT>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");

	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>Profile Settings</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br>Through this page, you can complete profile settings for CLT/%d.\n", id);
	fprintf(fs, "<br>-- Please click the 'Save' button to submit the appropriate configuration.\n");
	fprintf(fs, "<br>-- Please click the 'Done' button to save all configuration to flash finally.</br>\n");
	fprintf(fs, "<br><br>\n");

	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");		
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret' width=240>Bridged Address Aging</td>\n");
	fprintf(fs, "	<td class='diagret' width=260>&nbsp;</td>\n");
	fprintf(fs, "	<td class='diagret' width=80>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>local bridged table aging:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2><input type='text' name='loagTime' size='8'>Minutes[Range of 1~2000]</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>remote bridged table aging:</td>\n");
	fprintf(fs, "	<td class='diagdata'><input type='text' name='reagTime' size='8'>Minutes[Range of 1~2000]</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='setAgTime()' value='Save'></td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");	

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret'>Default CAP</td>\n");
	fprintf(fs, "	<td class='diagret' colspan=2>[Lowest Priority Classification]</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>igmp:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='igmpPri' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");	
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>unicast:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='unicastPri' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");	
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>igmp managed multicast stream:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='avsPri' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");	
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>multicast/broadcast:</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='mcastPri' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='setDefaultCap()' value='Save'></td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");	

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret'>QoS Settings</td>\n");
	fprintf(fs, "	<td class='diagret' colspan=2>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Status:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='tbaPriSts' size=1>\n");
	fprintf(fs, "			<option value='0'>Disable\n");
	fprintf(fs, "			<option value='1'>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Assign Priority Using:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='asPriType' size=1>\n");
	fprintf(fs, "			<option value='0'>COS\n");
	fprintf(fs, "			<option value='1'>TOS\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");	
	fprintf(fs, "</tr>\n");	
	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");	
	fprintf(fs, "</table>\n");
	
	//Table2
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata' width=120>COS0/TOS0:</td>\n");
	fprintf(fs, "	<td class='diagdata' width=120>\n");
	fprintf(fs, "		<select name='pri0QueueMap' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");	
	fprintf(fs, "	<td class='diagdata' width=130>COS1/TOS1:</td>\n");
	fprintf(fs, "	<td class='diagdata' width=130>\n");
	fprintf(fs, "		<select name='pri1QueueMap' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata' width=80>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>COS2/TOS2:</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='pri2QueueMap' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");	
	fprintf(fs, "	<td class='diagdata'>COS3/TOS3:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='pri3QueueMap' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");	
	fprintf(fs, "</tr>\n");
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>COS4/TOS4:</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='pri4QueueMap' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");	
	fprintf(fs, "	<td class='diagdata'>COS5/TOS5:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='pri5QueueMap' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");	
	fprintf(fs, "</tr>\n");
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>COS6/TOS6:</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='pri6QueueMap' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");	
	fprintf(fs, "	<td class='diagdata'>COS7/TOS7:</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='pri7QueueMap' size=1>\n");
	fprintf(fs, "			<option value='0'>CAP0\n");
	fprintf(fs, "			<option value='1'>CAP1\n");
	fprintf(fs, "			<option value='2'>CAP2\n");
	fprintf(fs, "			<option value='3'>CAP3\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='setQoS()' value='Save'></td>\n");
	fprintf(fs, "</tr>\n");	

	fprintf(fs, "<tr>\n<td class='diagdata' colspan=5>&nbsp;</td>\n</tr>\n");	
	fprintf(fs, "<tr>\n<td class='listend' colspan=5></td>\n</tr>\n");
	
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<input type='button' class='btn2L' onClick='location.href=\"cltManagement.cmd\"' value='Return'>\n");
	fprintf(fs, "	<input type='button' class='btn2L' onClick='saveProfile()' value='Done'>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiCnuProfile(char *query, FILE *fs) 
{
	int id = 0;
	int cltid = 0;
   	int cnuid = 0;
	st_dbsProfile profile;
	char action[IFC_LARGE_LEN];

	cgiGetValueByName(query, "cnuid", action);
	id = atoi(action);

	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id, &profile) )
	{
		/*Error*/
		strcpy(glbWebVar.returnUrl, "previewCnus.cgi");
		glbWebVar.wecOptCode = CMM_FAILED;
		do_ej("/webs/wecOptResult2.html", fs);
		return;
	}

	cltid = id/MAX_CNUS_PER_CLT + 1;
	cnuid = id%MAX_CNUS_PER_CLT;

	fprintf(fs, "<html><head>\n");
	fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
	fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
	fprintf(fs, "<SCRIPT language=JavaScript>\n");
	fprintf(fs, "function frmLoad() {\n");
	fprintf(fs, "	with ( document.forms[0] ) {\n");
	fprintf(fs, "		macLimitSts.selectedIndex = %d\n", profile.col_macLimit?1:0);
	fprintf(fs, "		macLimitNum.selectedIndex = %d\n", ((profile.col_macLimit==65)||(profile.col_macLimit>8))?0:profile.col_macLimit);
	fprintf(fs, "		loagTime.value = %d\n", profile.col_loagTime);
	fprintf(fs, "		reagTime.value = %d\n", profile.col_reagTime);
	fprintf(fs, "		sfbSts.selectedIndex = %d\n", profile.col_sfbSts?1:0);
	fprintf(fs, "		sfuSts.selectedIndex = %d\n", profile.col_sfuSts?1:0);
	fprintf(fs, "		sfmSts.selectedIndex = %d\n", profile.col_sfmSts?1:0);
	fprintf(fs, "		vlanSts.selectedIndex = %d\n", profile.col_vlanSts?1:0);
	fprintf(fs, "		eth1vid.value = %d\n", profile.col_eth1vid);
	fprintf(fs, "		eth2vid.value = %d\n", profile.col_eth2vid);
	fprintf(fs, "		eth3vid.value = %d\n", profile.col_eth3vid);
	fprintf(fs, "		eth4vid.value = %d\n", profile.col_eth4vid);
	//fprintf(fs, "		rxLimitSts.selectedIndex = %d\n", profile.col_rxLimitSts?1:0);
	if( 0 == profile.col_rxLimitSts )
	{
		fprintf(fs, "		cpuPortRx.selectedIndex = 0\n");
		fprintf(fs, "		eth1rx.selectedIndex = 0\n");
		fprintf(fs, "		eth2rx.selectedIndex = 0\n");
		fprintf(fs, "		eth3rx.selectedIndex = 0\n");
		fprintf(fs, "		eth4rx.selectedIndex = 0\n");
	}
	else
	{
		fprintf(fs, "		cpuPortRx.selectedIndex = %d\n", mapRate2SelectIndex(profile.col_cpuPortRxRate));
		fprintf(fs, "		eth1rx.selectedIndex = %d\n", mapRate2SelectIndex(profile.col_eth1rx));
		fprintf(fs, "		eth2rx.selectedIndex = %d\n", mapRate2SelectIndex(profile.col_eth2rx));
		fprintf(fs, "		eth3rx.selectedIndex = %d\n", mapRate2SelectIndex(profile.col_eth3rx));
		fprintf(fs, "		eth4rx.selectedIndex = %d\n", mapRate2SelectIndex(profile.col_eth4rx));
	}
	//fprintf(fs, "		txLimitSts.selectedIndex = %d\n", profile.col_txLimitSts?1:0);
	if( 0 == profile.col_txLimitSts )
	{
		fprintf(fs, "		cpuPortTx.selectedIndex = 0\n");
		fprintf(fs, "		eth1tx.selectedIndex = 0\n");
		fprintf(fs, "		eth2tx.selectedIndex = 0\n");
		fprintf(fs, "		eth3tx.selectedIndex = 0\n");
		fprintf(fs, "		eth4tx.selectedIndex = 0\n");
	}
	else
	{
		fprintf(fs, "		cpuPortTx.selectedIndex = %d\n", mapRate2SelectIndex(profile.col_cpuPortTxRate));
		fprintf(fs, "		eth1tx.selectedIndex = %d\n", mapRate2SelectIndex(profile.col_eth1tx));
		fprintf(fs, "		eth2tx.selectedIndex = %d\n", mapRate2SelectIndex(profile.col_eth2tx));
		fprintf(fs, "		eth3tx.selectedIndex = %d\n", mapRate2SelectIndex(profile.col_eth3tx));
		fprintf(fs, "		eth4tx.selectedIndex = %d\n", mapRate2SelectIndex(profile.col_eth4tx));
	}
	//fprintf(fs, "		psctlSts.selectedIndex = %d\n", profile.col_psctlSts?1:0);
	fprintf(fs, "		eth1sts.selectedIndex = %d\n", profile.col_eth1sts?1:0);
	fprintf(fs, "		eth2sts.selectedIndex = %d\n", profile.col_eth2sts?1:0);
	fprintf(fs, "		eth3sts.selectedIndex = %d\n", profile.col_eth3sts?1:0);
	fprintf(fs, "		eth4sts.selectedIndex = %d\n", profile.col_eth4sts?1:0);
	fprintf(fs, "	}\n");
	fprintf(fs, "}\n");	
	fprintf(fs, "function btn1save() {\n");
	fprintf(fs, "	var loc = 'macLimit.cgi?';\n");
	fprintf(fs, "	loc += 'cnuid=%d';\n", id);
	fprintf(fs, "	with ( document.forms[0] ) {\n");
	fprintf(fs, "		if(macLimitSts.selectedIndex){\n");
	fprintf(fs, "			if( macLimitNum.selectedIndex == 0 ){\n");
	fprintf(fs, "				loc += '&col_macLimit=65';\n");
	fprintf(fs, "			}else{\n");
	fprintf(fs, "				loc += '&col_macLimit=' + macLimitNum.selectedIndex;\n");
	fprintf(fs, "			}\n");
	fprintf(fs, "		}else{\n");
	fprintf(fs, "			loc += '&col_macLimit=0';\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function setAgTime()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var msg;\n");
	fprintf(fs, "	var value;\n");
	fprintf(fs, "	var loc = 'setAgTime.cgi?';\n");
	fprintf(fs, "	loc += 'cnuid=%d';\n", id);
	fprintf(fs, "	with (document.forms[0])\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		if ( isNaN(parseInt(loagTime.value)) == true )\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			msg = 'loagTime \"' + loagTime.value + '\" is invalid.';\n");
	fprintf(fs, "			alert(msg);\n");
	fprintf(fs, "			return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		value = parseInt(loagTime.value);\n");
	fprintf(fs, "		if ( value < 1 || value > 2000 )\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			msg = 'loagTime \"' + value + '\" is out of range [1-2000].';\n");
	fprintf(fs, "			alert(msg);\n");
	fprintf(fs, "			return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		if ( isNaN(parseInt(reagTime.value)) == true )\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			msg = 'reagTime \"' + reagTime.value + '\" is invalid.';\n");
	fprintf(fs, "			alert(msg);\n");
	fprintf(fs, "			return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		value = parseInt(reagTime.value);\n");
	fprintf(fs, "		if ( value < 1 || value > 2000 )\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			msg = 'reagTime \"' + value + '\" is out of range [1-2000].';\n");
	fprintf(fs, "			alert(msg);\n");
	fprintf(fs, "			return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		loc += '&col_loagTime=' + loagTime.value;\n");
	fprintf(fs, "		loc += '&col_reagTime=' + reagTime.value;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function setSFilter(sts)\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var loc = 'setSFilter.cgi?cnuid=%d';\n", id);
	fprintf(fs, "	with (document.forms[0])\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		if(!sts)\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			sfbSts.selectedIndex = 0;\n");
	fprintf(fs, "			sfuSts.selectedIndex = 0;\n");
	fprintf(fs, "			sfmSts.selectedIndex = 0;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		loc += '&col_sfbSts=' + sfbSts.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_sfuSts=' + sfuSts.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_sfmSts=' + sfmSts.selectedIndex;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function setVlan(sts){\n");
	fprintf(fs, "	var loc = 'setCnuVlan.cgi?cnuid=%d';\n", id);
	fprintf(fs, "	with (document.forms[0]){\n");
	fprintf(fs, "		if( (!sts) || (vlanSts.selectedIndex==0)){\n");
	fprintf(fs, "			vlanSts.selectedIndex = 0;\n");
	fprintf(fs, "			eth1vid.value = 1;\n");
	fprintf(fs, "			eth2vid.value = 1;\n");
	fprintf(fs, "			eth3vid.value = 1;\n");
	fprintf(fs, "			eth4vid.value = 1;\n");
	fprintf(fs, "		}else{\n");
	fprintf(fs, "			if( isValidVlanId(eth1vid.value) == false ) return;\n");
	fprintf(fs, "			if( isValidVlanId(eth2vid.value) == false ) return;\n");
	fprintf(fs, "			if( isValidVlanId(eth3vid.value) == false ) return;\n");
	fprintf(fs, "			if( isValidVlanId(eth4vid.value) == false ) return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		loc += '&col_vlanSts=' + vlanSts.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth1vid=' + eth1vid.value;\n");
	fprintf(fs, "		loc += '&col_eth2vid=' + eth2vid.value;\n");
	fprintf(fs, "		loc += '&col_eth3vid=' + eth3vid.value;\n");
	fprintf(fs, "		loc += '&col_eth4vid=' + eth4vid.value;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function setPLinkSts(sts){\n");
	fprintf(fs, "	var loc = 'setPLinkSts.cgi?cnuid=%d';\n", id);
	fprintf(fs, "	with (document.forms[0]){\n");
	fprintf(fs, "		if( !sts){\n");
	//fprintf(fs, "			psctlSts.selectedIndex = 0;\n");
	fprintf(fs, "			eth1sts.selectedIndex = 1;\n");
	fprintf(fs, "			eth2sts.selectedIndex = 1;\n");
	fprintf(fs, "			eth3sts.selectedIndex = 1;\n");
	fprintf(fs, "			eth4sts.selectedIndex = 1;\n");
	fprintf(fs, "		}\n");
	//fprintf(fs, "		loc += '&col_psctlSts=' + psctlSts.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth1sts=' + eth1sts.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth2sts=' + eth2sts.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth3sts=' + eth3sts.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth4sts=' + eth4sts.selectedIndex;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function doRateLimit(sts){\n");
	fprintf(fs, "	var loc = 'doRateLimit.cgi?cnuid=%d';\n", id);
	fprintf(fs, "	with (document.forms[0]){\n");
	fprintf(fs, "		if(!sts){\n");
	fprintf(fs, "			cpuPortRx.selectedIndex = 0;\n");
	fprintf(fs, "			cpuPortTx.selectedIndex = 0;\n");
	fprintf(fs, "			eth1rx.selectedIndex = 0;\n");
	fprintf(fs, "			eth2rx.selectedIndex = 0;\n");
	fprintf(fs, "			eth3rx.selectedIndex = 0;\n");
	fprintf(fs, "			eth4rx.selectedIndex = 0;\n");
	fprintf(fs, "			eth1tx.selectedIndex = 0;\n");
	fprintf(fs, "			eth2tx.selectedIndex = 0;\n");
	fprintf(fs, "			eth3tx.selectedIndex = 0;\n");
	fprintf(fs, "			eth4tx.selectedIndex = 0;\n");
	fprintf(fs, "			loc += '&col_rxLimitSts=0';\n");
	fprintf(fs, "			loc += '&col_txLimitSts=0';\n");
	fprintf(fs, "		}else{\n");
	fprintf(fs, "			loc += '&col_rxLimitSts=1';\n");
	fprintf(fs, "			loc += '&col_txLimitSts=1';\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		loc += '&col_cpuPortRxRate=' + cpuPortRx.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_cpuPortTxRate=' + cpuPortTx.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth1rx=' + eth1rx.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth1tx=' + eth1tx.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth2rx=' + eth2rx.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth2tx=' + eth2tx.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth3rx=' + eth3rx.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth3tx=' + eth3tx.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth4rx=' + eth4rx.selectedIndex;\n");
	fprintf(fs, "		loc += '&col_eth4tx=' + eth4tx.selectedIndex;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function saveProfile(){\n");
	fprintf(fs, "	var loc = 'saveProfile.cgi?cnuid=%d';\n", id);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	//fprintf(fs, "\n");
	fprintf(fs, "</SCRIPT>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");

	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>Profile Settings</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br>Through this page, you can complete profile settings for CNU/%d/%d.\n", cltid, cnuid);
	fprintf(fs, "<br>-- Please click the 'Save' button to submit the appropriate configuration.\n");
	fprintf(fs, "<br>-- Click the 'Clear' button if you want to undo the appropriate configuration.\n");
	fprintf(fs, "<br>-- Please click the 'Done' button to save all configuration to flash finally.</br>\n");
	fprintf(fs, "<br><br>\n");

	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");	
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret' width=240>Mac Address Limiting</td>\n");
	fprintf(fs, "	<td class='diagret' width=260>&nbsp;</td>\n");
	fprintf(fs, "	<td class='diagret' width=80>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Status:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='macLimitSts' size=1>\n");
	fprintf(fs, "			<option value='0'>Disable\n");
	fprintf(fs, "			<option value='1'>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>MAX Bridged Hosts:</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='macLimitNum' size=1>\n");
	fprintf(fs, "			<option value='0'>0\n");
	fprintf(fs, "			<option value='1'>1\n");
	fprintf(fs, "			<option value='2'>2\n");
	fprintf(fs, "			<option value='3'>3\n");
	fprintf(fs, "			<option value='4'>4\n");
	fprintf(fs, "			<option value='5'>5\n");
	fprintf(fs, "			<option value='6'>6\n");
	fprintf(fs, "			<option value='7'>7\n");
	fprintf(fs, "			<option value='8'>8\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='btn1save()' value='Save'></td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret'>Bridged Address Aging</td>\n");
	fprintf(fs, "	<td class='diagret' colspan=2>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>local bridged table aging:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2><input type='text' name='loagTime' size='8'>Minutes[Range of 1~2000]</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>remote bridged table aging:</td>\n");
	fprintf(fs, "	<td class='diagdata'><input type='text' name='reagTime' size='8'>Minutes[Range of 1~2000]</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='setAgTime()' value='Save'></td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");	

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret'>Storm Filter Settings</td>\n");
	fprintf(fs, "	<td class='diagret' colspan=2>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>broadcast storm filter:</td>\n");	
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='sfbSts' size=1>\n");
	fprintf(fs, "			<option value='0'>Disable\n");
	fprintf(fs, "			<option value='1'>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>unicast storm filter:</td>\n");	
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='sfuSts' size=1>\n");
	fprintf(fs, "			<option value='0'>Disable\n");
	fprintf(fs, "			<option value='1'>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='setSFilter(0)' value='Clear'></td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>multicast storm filter:</td>\n");	
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='sfmSts' size=1>\n");
	fprintf(fs, "			<option value='0'>Disable\n");
	fprintf(fs, "			<option value='1'>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>storm filter level:</td>\n");
	if( 0 == profile.col_sfRate )
	{			
		fprintf(fs, "	<td class='diagdata'>Disable</td>\n");
	}
	else
	{
		fprintf(fs, "	<td class='diagdata'>%d Kpps</td>\n", (1<<(profile.col_sfRate-1)));
	}
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='setSFilter(1)' value='Save'></td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");	

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret'>802.1Q VLAN Settings</td>\n");
	fprintf(fs, "	<td class='diagret' colspan=2>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>status:</td>\n");	
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='vlanSts' size=1>\n");
	fprintf(fs, "			<option value='0'>Disable\n");
	fprintf(fs, "			<option value='1'>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH1 PVID:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2><input type='text' name='eth1vid' size='8'>[Range of 1~4094]</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH2 PVID:</td>\n");
	fprintf(fs, "	<td class='diagdata'><input type='text' name='eth2vid' size='8'>[Range of 1~4094]</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='setVlan(0)' value='Clear'></td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH3 PVID:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2><input type='text' name='eth3vid' size='8'>[Range of 1~4094]</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH4 PVID:</td>\n");
	fprintf(fs, "	<td class='diagdata'><input type='text' name='eth4vid' size='8'>[Range of 1~4094]</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='setVlan(1)' value='Save'></td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");	

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret'>Port Link Status Control</td>\n");
	fprintf(fs, "	<td class='diagret' colspan=2>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");	
#if 0
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>status:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='psctlSts' size=1>\n");
	fprintf(fs, "			<option value='0'>Disable\n");
	fprintf(fs, "			<option value='1'>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");
#endif
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH1 link status:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='eth1sts' size=1>\n");
	fprintf(fs, "			<option value='0'>Disable\n");
	fprintf(fs, "			<option value='1'>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH2 link status:</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='eth2sts' size=1>\n");
	fprintf(fs, "			<option value='0'>Disable\n");
	fprintf(fs, "			<option value='1'>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='setPLinkSts(0)' value='Clear'></td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH3 link status:</td>\n");
	fprintf(fs, "	<td class='diagdata' colspan=2>\n");
	fprintf(fs, "		<select name='eth3sts' size=1>\n");
	fprintf(fs, "			<option value='0'>Disable\n");
	fprintf(fs, "			<option value='1'>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "</tr>\n");
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH4 link status:</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='eth4sts' size=1>\n");
	fprintf(fs, "			<option value='0'>Disable\n");
	fprintf(fs, "			<option value='1'>Enable\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='setPLinkSts(1)' value='Save'></td>\n");
	fprintf(fs, "</tr>\n");	

	fprintf(fs, "<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");	
	fprintf(fs, "</table>\n");

	//table 2
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");	
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagret' width=160>Port Speed Limiting</td>\n");
	fprintf(fs, "	<td class='diagret' width=170>&nbsp;</td>\n");
	fprintf(fs, "	<td class='diagret' width=170>&nbsp;</td>\n");
	fprintf(fs, "	<td class='diagret' width=80>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");
	
	fprintf(fs, "<tr>\n<td class='diagdata' colspan=4>&nbsp;</td>\n</tr>\n");
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>Port</td>\n");
	fprintf(fs, "	<td class='diagdata'>Uplink Speed</td>\n");
	fprintf(fs, "	<td class='diagdata'>Downlink Speed</td>\n");
	fprintf(fs, "	<td class='diagdata'>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>cpu port</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='cpuPortTx' size=1>\n");
	fprintf(fs, "			<option value='0'>Unlimited\n");
	fprintf(fs, "			<option value='1'>128 Kbps\n");
	fprintf(fs, "			<option value='2'>256 Kbps\n");
	fprintf(fs, "			<option value='3'>512 Kbps\n");
	fprintf(fs, "			<option value='4'>1 Mbps\n");
	fprintf(fs, "			<option value='5'>1.5 Mbps\n");
	fprintf(fs, "			<option value='6'>2 Mbps\n");
	fprintf(fs, "			<option value='7'>3 Mbps\n");	
	fprintf(fs, "			<option value='8'>4 Mbps\n");
	fprintf(fs, "			<option value='9'>6 Mbps\n");
	fprintf(fs, "			<option value='10'>8 Mbps\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='cpuPortRx' size=1>\n");
	fprintf(fs, "			<option value='0'>Unlimited\n");
	fprintf(fs, "			<option value='1'>128 Kbps\n");
	fprintf(fs, "			<option value='2'>256 Kbps\n");
	fprintf(fs, "			<option value='3'>512 Kbps\n");
	fprintf(fs, "			<option value='4'>1 Mbps\n");
	fprintf(fs, "			<option value='5'>1.5 Mbps\n");
	fprintf(fs, "			<option value='6'>2 Mbps\n");
	fprintf(fs, "			<option value='7'>3 Mbps\n");	
	fprintf(fs, "			<option value='8'>4 Mbps\n");
	fprintf(fs, "			<option value='9'>6 Mbps\n");
	fprintf(fs, "			<option value='10'>8 Mbps\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata'>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");
	
	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH1</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='eth1rx' size=1>\n");
	fprintf(fs, "			<option value='0'>Unlimited\n");
	fprintf(fs, "			<option value='1'>128 Kbps\n");
	fprintf(fs, "			<option value='2'>256 Kbps\n");
	fprintf(fs, "			<option value='3'>512 Kbps\n");
	fprintf(fs, "			<option value='4'>1 Mbps\n");
	fprintf(fs, "			<option value='5'>1.5 Mbps\n");
	fprintf(fs, "			<option value='6'>2 Mbps\n");
	fprintf(fs, "			<option value='7'>3 Mbps\n");	
	fprintf(fs, "			<option value='8'>4 Mbps\n");
	fprintf(fs, "			<option value='9'>6 Mbps\n");
	fprintf(fs, "			<option value='10'>8 Mbps\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='eth1tx' size=1>\n");
	fprintf(fs, "			<option value='0'>Unlimited\n");
	fprintf(fs, "			<option value='1'>128 Kbps\n");
	fprintf(fs, "			<option value='2'>256 Kbps\n");
	fprintf(fs, "			<option value='3'>512 Kbps\n");
	fprintf(fs, "			<option value='4'>1 Mbps\n");
	fprintf(fs, "			<option value='5'>1.5 Mbps\n");
	fprintf(fs, "			<option value='6'>2 Mbps\n");
	fprintf(fs, "			<option value='7'>3 Mbps\n");	
	fprintf(fs, "			<option value='8'>4 Mbps\n");
	fprintf(fs, "			<option value='9'>6 Mbps\n");
	fprintf(fs, "			<option value='10'>8 Mbps\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata'>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH2</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='eth2rx' size=1>\n");
	fprintf(fs, "			<option value='0'>Unlimited\n");
	fprintf(fs, "			<option value='1'>128 Kbps\n");
	fprintf(fs, "			<option value='2'>256 Kbps\n");
	fprintf(fs, "			<option value='3'>512 Kbps\n");
	fprintf(fs, "			<option value='4'>1 Mbps\n");
	fprintf(fs, "			<option value='5'>1.5 Mbps\n");
	fprintf(fs, "			<option value='6'>2 Mbps\n");
	fprintf(fs, "			<option value='7'>3 Mbps\n");	
	fprintf(fs, "			<option value='8'>4 Mbps\n");
	fprintf(fs, "			<option value='9'>6 Mbps\n");
	fprintf(fs, "			<option value='10'>8 Mbps\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='eth2tx' size=1>\n");
	fprintf(fs, "			<option value='0'>Unlimited\n");
	fprintf(fs, "			<option value='1'>128 Kbps\n");
	fprintf(fs, "			<option value='2'>256 Kbps\n");
	fprintf(fs, "			<option value='3'>512 Kbps\n");
	fprintf(fs, "			<option value='4'>1 Mbps\n");
	fprintf(fs, "			<option value='5'>1.5 Mbps\n");
	fprintf(fs, "			<option value='6'>2 Mbps\n");
	fprintf(fs, "			<option value='7'>3 Mbps\n");	
	fprintf(fs, "			<option value='8'>4 Mbps\n");
	fprintf(fs, "			<option value='9'>6 Mbps\n");
	fprintf(fs, "			<option value='10'>8 Mbps\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='doRateLimit(0)' value='Clear'></td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH3</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='eth3rx' size=1>\n");
	fprintf(fs, "			<option value='0'>Unlimited\n");
	fprintf(fs, "			<option value='1'>128 Kbps\n");
	fprintf(fs, "			<option value='2'>256 Kbps\n");
	fprintf(fs, "			<option value='3'>512 Kbps\n");
	fprintf(fs, "			<option value='4'>1 Mbps\n");
	fprintf(fs, "			<option value='5'>1.5 Mbps\n");
	fprintf(fs, "			<option value='6'>2 Mbps\n");
	fprintf(fs, "			<option value='7'>3 Mbps\n");	
	fprintf(fs, "			<option value='8'>4 Mbps\n");
	fprintf(fs, "			<option value='9'>6 Mbps\n");
	fprintf(fs, "			<option value='10'>8 Mbps\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='eth3tx' size=1>\n");
	fprintf(fs, "			<option value='0'>Unlimited\n");
	fprintf(fs, "			<option value='1'>128 Kbps\n");
	fprintf(fs, "			<option value='2'>256 Kbps\n");
	fprintf(fs, "			<option value='3'>512 Kbps\n");
	fprintf(fs, "			<option value='4'>1 Mbps\n");
	fprintf(fs, "			<option value='5'>1.5 Mbps\n");
	fprintf(fs, "			<option value='6'>2 Mbps\n");
	fprintf(fs, "			<option value='7'>3 Mbps\n");	
	fprintf(fs, "			<option value='8'>4 Mbps\n");
	fprintf(fs, "			<option value='9'>6 Mbps\n");
	fprintf(fs, "			<option value='10'>8 Mbps\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata'>&nbsp;</td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n");
	fprintf(fs, "	<td class='diagdata'>ETH4</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='eth4rx' size=1>\n");
	fprintf(fs, "			<option value='0'>Unlimited\n");
	fprintf(fs, "			<option value='1'>128 Kbps\n");
	fprintf(fs, "			<option value='2'>256 Kbps\n");
	fprintf(fs, "			<option value='3'>512 Kbps\n");
	fprintf(fs, "			<option value='4'>1 Mbps\n");
	fprintf(fs, "			<option value='5'>1.5 Mbps\n");
	fprintf(fs, "			<option value='6'>2 Mbps\n");
	fprintf(fs, "			<option value='7'>3 Mbps\n");	
	fprintf(fs, "			<option value='8'>4 Mbps\n");
	fprintf(fs, "			<option value='9'>6 Mbps\n");
	fprintf(fs, "			<option value='10'>8 Mbps\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata'>\n");
	fprintf(fs, "		<select name='eth4tx' size=1>\n");
	fprintf(fs, "			<option value='0'>Unlimited\n");
	fprintf(fs, "			<option value='1'>128 Kbps\n");
	fprintf(fs, "			<option value='2'>256 Kbps\n");
	fprintf(fs, "			<option value='3'>512 Kbps\n");
	fprintf(fs, "			<option value='4'>1 Mbps\n");
	fprintf(fs, "			<option value='5'>1.5 Mbps\n");
	fprintf(fs, "			<option value='6'>2 Mbps\n");
	fprintf(fs, "			<option value='7'>3 Mbps\n");	
	fprintf(fs, "			<option value='8'>4 Mbps\n");
	fprintf(fs, "			<option value='9'>6 Mbps\n");
	fprintf(fs, "			<option value='10'>8 Mbps\n");
	fprintf(fs, "		</select>\n");
	fprintf(fs, "	</td>\n");
	fprintf(fs, "	<td class='diagdata' align='right'><input type='button' class='btn2L' onClick='doRateLimit(1)' value='Save'></td>\n");
	fprintf(fs, "</tr>\n");

	fprintf(fs, "<tr>\n<td class='diagdata' colspan=4>&nbsp;</td>\n</tr>\n");	
	fprintf(fs, "<tr>\n<td class='listend' colspan=4></td>\n</tr>\n");	
	fprintf(fs, "</table>\n");

	fprintf(fs, "<p>\n");
	fprintf(fs, "	<input type='button' class='btn2L' onClick='location.href=\"previewCnus.cgi\"' value='Return'>\n");
	fprintf(fs, "	<input type='button' class='btn2L' onClick='saveProfile()' value='Done'>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiCltMgmt(char *query, FILE *fs)
{
	int i=0;
	int iCount = 0;
	st_dbsClt clt;

	fprintf(fs, "<html><head>\n");
	fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
	fprintf(fs, "<script language='javascript'>\n");
	fprintf(fs, "function cltAction(opt, cltid){\n");
	fprintf(fs, "	if( opt == 0 ){var loc = 'editCltPro.cmd?cltid=';}\n");
	fprintf(fs, "	else if( opt == 1 ) {\n");
	fprintf(fs, "		var loc = 'cltReboot.cgi?cltid=';\n");
	fprintf(fs, "		if(!confirm('Are you sure to reboot the clt right now?')) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else if( opt == 2 ) {\n");
	fprintf(fs, "		var loc = 'cltReload.cgi?cltid=';\n");
	fprintf(fs, "		if(!confirm('Are you sure that you want to reload the profile from CBAT to the CLT?')) return;\n");
	fprintf(fs, "	}\n");	
	fprintf(fs, "	else {var loc = 'editCltPro.cmd?cltid=';}\n");
	fprintf(fs, "	loc += cltid;\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");	
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n\n");
	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>CLT Management</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=900></td>\n</tr>\n</table>\n");

	fprintf(fs, "<br>Through this page, you can complete the CLT configuration management.\n");
	fprintf(fs, "<br>-- Please click the 'profile' image if you want to view or modify CLT's profile.\n");	
	fprintf(fs, "<br>-- You can click the 'reload' button to force the profile re-loaded to the CLT.</br>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<br>\n");

	fprintf(fs, "<table border=1 cellpadding=3 cellspacing=0>\n");	
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='hd' align='center' width=50>Index</td>\n");
	fprintf(fs, "		<td class='hd' align='center' width=140>MAC Address</td>\n");	
	fprintf(fs, "		<td class='hd' align='center' width=120>Max Stations</td>\n");
	fprintf(fs, "		<td class='hd' align='center' width=220>Firmware Version</td>\n");
	fprintf(fs, "		<td class='hd' align='center' width=60>Status</td>\n");
	fprintf(fs, "		<td class='hd' align='center' width=60>Profile</td>\n");
	fprintf(fs, "	</tr>\n");
	for( i=1; i<=MAX_CLT_AMOUNT_LIMIT; i++ )
	{
		if( dbsGetClt(dbsdev, i, &clt) != CMM_SUCCESS )
		{
			/* Get CNU failed, exit */
			break;
		}
		else
		{
			if( clt.col_row_sts == 0 )
			{
				/* CNU column unused, get next */ 
				continue;
			}
			else
			{
				iCount++;
				fprintf(fs, "	<tr>\n");
				fprintf(fs, "		<td align='center'>%d</td>\n", clt.id);
				fprintf(fs, "		<td align='center'>%s</td>\n", clt.col_mac);
				fprintf(fs, "		<td align='center'>%d</td>\n", clt.col_maxStas);
				fprintf(fs, "		<td align='center'>%s</td>\n", clt.col_swVersion);				
				fprintf(fs, "		<td align='center'><IMG src='true.png' width='14' height='15'></td>\n");
				fprintf(fs, "		<td align='center'><IMG src='ico_Editor.gif' width=16 height=16 onclick='cltAction(0,%d)'></td>\n", clt.id);
				fprintf(fs, "	</tr>\n");
			}
		}
	}
	if( 0 == iCount )
	{
		/* No CNU to Show */
		fprintf(fs, "<tr>\n<td align='center' colspan=7>** No CLT discovered ! **</td>\n</tr>\n");
	}
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<input type='button' class='btn2L' onClick='location.href=\"cltManagement.cmd\"' value='Refresh'>\n");
	if( 0 != iCount )
	{
		fprintf(fs, "	<input type='button' class='btn2L' value='Reboot' onclick='cltAction(1,%d)'>\n", clt.id);
		fprintf(fs, "	<input type='button' class='btn2L' value='Reload' onclick='cltAction(2,%d)'>\n", clt.id);
	}
	fprintf(fs, "</p>\n");
	
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);	
}


void cgiCnuMgmt(char *query, FILE *fs)
{
	int i=0;
	int isCnuSupported = BOOL_TRUE;
	int cltid = 0;
	int cnuid = 0;
	int iCount = 0;
	st_dbsCnu cnu;

	fprintf(fs, "<html><head>\n");
	fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
	fprintf(fs, "<script language='javascript'>\n");
	fprintf(fs, "function cnuAction(opt, cnuid){\n");
	fprintf(fs, "	if( opt == 0 ){var loc = 'editCnuPro.cmd?cnuid=';}\n");
	fprintf(fs, "	else if( opt == 1 ) {\n");
	fprintf(fs, "		var loc = 'cnuReboot.cgi?cnuid=';\n");
	fprintf(fs, "		if(!confirm('Are you sure to reboot the device right now?')) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else if( opt == 2 ) {\n");
	fprintf(fs, "		var loc = 'cnuReload.cgi?cnuid=';\n");
	fprintf(fs, "		if(!confirm('Are you sure that you want to reload the profile from CBAT to the CNU device?')) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else if( opt == 3 ) {\n");
	fprintf(fs, "		var loc = 'cnuDelete.cgi?cnuid=';\n");
	fprintf(fs, "		if(!confirm('The CNU will be deleted from the topology. Are you sure?')) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else if( opt == 4 ) {var loc = 'cnuPermit.cgi?cnuid=';}\n");
	fprintf(fs, "	else if( opt == 5 ) {var loc = 'cnuUndoPermit.cgi?cnuid=';}\n");
	fprintf(fs, "	else {var loc = 'editCnuPro.cmd?cnuid=';}\n");
	fprintf(fs, "	loc += cnuid;\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function btnNewCnu(){\n");
	fprintf(fs, "	window.location='wecNewCnu.html';\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function rtl8306e_config(cnuid){\n");
	fprintf(fs, "	var loc = 'previewCnuSwConfig.cgi?cnuid=';\n");
	fprintf(fs, "	loc += cnuid;\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n\n");
	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>CNU Management</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=900></td>\n</tr>\n</table>\n");

	fprintf(fs, "<br>Through this page, you can complete the user service and configuration management, such as: new/delete CNUs,\n");
	fprintf(fs, "<br>change user business license status, view/modify user profile and so on...\n");
	fprintf(fs, "<br>-- Please click the 'profile' image if you want to view or modify user's profile.\n");
	fprintf(fs, "<br>-- Please click 'permit'/'undo-permit' button if you want to change the user business license status.\n");
	fprintf(fs, "<br>-- You can click the 'reload' button to force the profile re-loaded to the cnu equipment.\n");
	fprintf(fs, "<br>-- And you can delete a CNU from the topology through the 'delete' image.\n");
	fprintf(fs, "<br>Notice: Prohibited to delete online users.</br>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<br>\n");

	fprintf(fs, "<table border=1 cellpadding=3 cellspacing=0>\n");	
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='hd' align='center' width=40>Index</td>\n");
	fprintf(fs, "		<td class='hd' align='center' width=130>MAC Address</td>\n");
	fprintf(fs, "		<td class='hd' align='center' width=90>Model</td>\n");
	fprintf(fs, "		<td class='hd' align='center' width=70>Permition</td>\n");
	fprintf(fs, "		<td class='hd' align='center' width=50>Profile</td>\n");
	fprintf(fs, "		<td class='hd' align='center' width=220>Management</td>\n");
	fprintf(fs, "		<td class='hd' align='center' width=50>Delete</td>\n");
	fprintf(fs, "	</tr>\n");
	for( i=1; i<=MAX_CNU_AMOUNT_LIMIT; i++ )
	{
		cltid = i/MAX_CNUS_PER_CLT + 1;
		cnuid = i%MAX_CNUS_PER_CLT;
		if( dbsGetCnu(dbsdev, i, &cnu) != CMM_SUCCESS )
		{
			/*Read CNU Error, Exit*/
			break;
		}
		else
		{
			if( cnu.col_row_sts == 0 )
			{
				/*CNU column unused, get next*/
				continue;
			}
			else
			{
				iCount++;
				isCnuSupported = boardapi_isCnuSupported(cnu.col_model);
				fprintf(fs, "	<tr>\n");
				fprintf(fs, "		<td align='center'>%d/%d</td>\n", cltid, cnuid);
				fprintf(fs, "		<td align='center'>%s</td>\n", cnu.col_mac);
				fprintf(fs, "		<td align='center'>%s</td>\n", boardapi_getDeviceModelStr(cnu.col_model));
				fprintf(fs, "		<td align='center'>%s</td>\n", cnu.col_auth?"Yes":"No");
				if(isCnuSupported)
				{
					fprintf(fs, "		<td align='center'><IMG src='ico_Editor.gif' width=16 height=16 onclick='cnuAction(0,%d)'></td>\n", cnu.id);
				}
				else
				{
					fprintf(fs, "		<td align='center'><IMG src='suppress.png' width=16 height=16'></td>\n", cnu.id);
				}
				
				fprintf(fs, "		<td>\n");
				fprintf(fs, "			<input type='button' class='btn1Tbd' value='Reboot' onclick='cnuAction(1,%d)'>\n", cnu.id);
				if(isCnuSupported)
				{
					fprintf(fs, "			<input type='button' class='btn2Tbd' value='Reload' onclick='cnuAction(2,%d)'>\n", cnu.id);
					if(cnu.col_auth)
					{
						fprintf(fs, "			<input type='button' class='btn5L' value='Undo-Permit' onclick='cnuAction(5,%d)'>\n", cnu.id);
					}
					else
					{
						fprintf(fs, "			<input type='button' class='btn2L' value='Permit' onclick='cnuAction(4,%d)'>\n", cnu.id);
					}
				}
				else
				{
					if(cnu.col_sts)
					{
						fprintf(fs, "			<input type='button' class='btn8L' value='Switch Settings' onclick='rtl8306e_config(%d)'>\n", cnu.id);
					}
					else
					{
						fprintf(fs, "			<input type='button' class='btn8Ldis' value='Switch Settings' disabled='disabled'>\n", cnu.id);
					}
				}
				
				fprintf(fs, "		</td>\n");
				fprintf(fs, "		<td align='center'><IMG src='delete.gif' width=12 height=12 onclick='cnuAction(3,%d)'></td>\n", cnu.id);
				fprintf(fs, "	</tr>\n");
			}
		}
	}
	if( 0 == iCount )
	{
		/*No CNU to show*/
		fprintf(fs, "<tr>\n<td align='center' colspan=7>** No user discovered ! **</td>\n</tr>\n");
	}
	fprintf(fs, "</table>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<input type='button' class='btn2L' onClick='location.href=\"previewCnus.cgi\"' value='Refresh'>\n");
	fprintf(fs, "	<input type='button' class='btn8L' onClick='btnNewCnu()' value='Create New CNU'>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);	
}

void cgiLinkDiag(char *query, FILE *fs) 
{
	int i=0;
	//int n = 0;
	int cltid = 0;
	int cnuid = 0;
	//int iCount = 0;
	st_dbsCnu cnu;

	fprintf(fs, "<html><head>\n");
	fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
	fprintf(fs, "<script language='javascript'>\n");
	fprintf(fs, "function wecLinkDiag(){\n");
	fprintf(fs, "	var loc = 'wecLinkDiag.cgi?';\n");
	fprintf(fs, "	with (document.forms[0]){\n");
	fprintf(fs, "		if( cnuDev.selectedIndex == 0 ){\n");
	fprintf(fs, "			alert('Please select a CNU.');\n");
	fprintf(fs, "			return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		if( fangxiang.selectedIndex == 0 ){\n");
	fprintf(fs, "			alert('Please select the target direction.');\n");
	fprintf(fs, "			return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "		loc += 'cnuid=' + cnuDev.value;\n");
	fprintf(fs, "		loc += '&diagDir=' + fangxiang.selectedIndex;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");

	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>Link Diagnosis</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=900></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br>Through this page, you can complete the diagnosis of the physical link status of the end-users.\n");
	fprintf(fs, "<br>You can diagnose the user's uplink or downlink physical link status respectively. Please select a CNU from the devices list\n");
	fprintf(fs, "<br>and select the target direction, then click the 'Begin Diagnostics' button to start diagnostic tests.</br>\n");
	fprintf(fs, "<br><br>\n");

	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagret' width=300>Do Link Diagnostics</td>\n");
	fprintf(fs, "		<td class='diagret' width=260>&nbsp;</td>\n");
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr><td class='diagdata' colspan=2>&nbsp;</td></tr>\n");	
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>CNU Device List:</td>\n");
	fprintf(fs, "		<td class='diagdata'>\n");
	fprintf(fs, "			<select name='cnuDev'>\n");
	fprintf(fs, "				<option value='0' selected='selected'>Please select a device\n");

	for( i=1; i<=MAX_CNU_AMOUNT_LIMIT; i++ )
	{
		cltid = i/MAX_CNUS_PER_CLT + 1;
		cnuid = i%MAX_CNUS_PER_CLT;
		if( dbsGetCnu(dbsdev, i, &cnu) != CMM_SUCCESS )
		{
			/* Read CNU failed, exit */
			break;
		}
		else
		{
			if( cnu.col_row_sts == 0 )
			{
				/*CNU column empty, get next */
				continue;
			}
			else
			{
				//iCount++;
				if( DEV_STS_ONLINE == cnu.col_sts )
				{
					//n = i/MAX_CNUS_PER_CLT+1;
					fprintf(fs, "			<option value='%d'>CNU/%d/%d: [%s]\n", i, cltid, cnuid, cnu.col_mac);
				}
			}
		}
	}
	fprintf(fs, "			</select>\n");
	fprintf(fs, "		</td>\n");
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>Target Direction:</td>\n");
	fprintf(fs, "		<td class='diagdata'>\n");
	fprintf(fs, "			<select name='fangxiang'>\n");
	fprintf(fs, "				<option value=0 selected='selected'>Undetermined\n");
	fprintf(fs, "				<option value=1>uplink\n");
	fprintf(fs, "				<option value=2>downlink\n");
	fprintf(fs, "			</select>\n");
	fprintf(fs, "		</td>\n");
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr><td class='diagdata' colspan=2>&nbsp;</td></tr>\n");
	fprintf(fs, "	<tr><td class='listend' colspan=2></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br><b>Note:</b> Only online CNUs can be diagnosed correctly.</br>\n");	
	fprintf(fs, "<br><br>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<input type='button' class='btn2L' onClick='location.href=\"previewLinkDiag.cgi\"' value='Refresh'>\n");
	fprintf(fs, "	<input type='button' class='btn8L' onClick='wecLinkDiag()' value='Begin Diagnostics'>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiLinkDiagResult(char *query, FILE *fs)
{
	int cltid = 0;
   	int cnuid = 0;

	cltid = glbWebVar.cnuid/MAX_CNUS_PER_CLT + 1;
	cnuid = glbWebVar.cnuid%MAX_CNUS_PER_CLT;
	
	writePageHeader(fs);
	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>Link Diagnosis</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=900></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br>Through this page, you can view the diagnostic results.</br>\n<br><br>\n");
	
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");
	
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagret' width=350>Diagnostic conclusions</td>\n");
	if( CMM_SUCCESS == glbWebVar.diagResult )
	{
		fprintf(fs, "		<td class='diagret' width=200><font color='green'>Success</font></td>\n");
	}
	else
	{
		fprintf(fs, "		<td class='diagret' width=200><font color='red'>Failed</font></td>\n");
	}
	fprintf(fs, "		<td class='diagret' width=50>&nbsp;</td>\n");
	fprintf(fs, "	</tr>\n");

	fprintf(fs, "	<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");	

	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>Index:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>CNU/%d/%d</td>\n", cltid, cnuid);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>MAC Address:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%s</td>\n", glbWebVar.diagCnuMac);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>TEI:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%d</td>\n", glbWebVar.diagCnuTei);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>Device Model:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%s</td>\n", boardapi_getDeviceModelStr(glbWebVar.diagCnuModel));
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>Target Direction:</td>\n");
	if( 1 == glbWebVar.diagDir )
	{
		fprintf(fs, "		<td class='diagdata' colspan=2>TX(uplink)</td>\n");
	}
	else if( 2 == glbWebVar.diagDir )
	{
		fprintf(fs, "		<td class='diagdata' colspan=2>RX(downlink)</td>\n");
	}
	else
	{
		fprintf(fs, "		<td class='diagdata' colspan=2>Undetermined</td>\n");
	}
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>CCo MAC Address:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%s</td>\n", glbWebVar.ccoMac);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>CCo NID:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%s</td>\n", glbWebVar.ccoNid);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>CCo SNID:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%d</td>\n", glbWebVar.ccoSnid);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>CCo TEI:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%d</td>\n", glbWebVar.ccoTei);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>RX Rate:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%d Mbps</td>\n", glbWebVar.diagCnuRxRate);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>TX Rate:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%d Mbps</td>\n", glbWebVar.diagCnuTxRate);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>Avg. Bits/Carrier:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%s</td>\n", glbWebVar.bitCarrier);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>Avg. Attenuation/Carrier:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%d dB</td>\n", glbWebVar.diagCnuAtten);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>Bridged MAC Address:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%s</td>\n", glbWebVar.bridgedMac);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>MPDUs Transmitted & Acknowledged:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%s</td>\n", glbWebVar.MPDU_ACKD);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>MPDUs Transmitted & Collided:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%s</td>\n", glbWebVar.MPDU_COLL);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>MPDUs Transmitted & Failed:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%s</td>\n", glbWebVar.MPDU_FAIL);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>PBs Transmitted Successfully:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%s</td>\n", glbWebVar.PBS_PASS);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>PBs Transmitted Unsuccessfully:</td>\n");
	fprintf(fs, "		<td class='diagdata' colspan=2>%s</td>\n", glbWebVar.PBS_FAIL);
	fprintf(fs, "	</tr>\n");

	fprintf(fs, "	<tr>\n<td class='diagdata' colspan=3>&nbsp;</td>\n</tr>\n");
	
	fprintf(fs, "	<tr>\n<td class='listend' colspan=3></td>\n</tr>\n");
	
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<p><input type='button' class='btn2L' onClick='location.href=\"previewLinkDiag.cgi\"' value='Return'>\n</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiTopologyView(char *query, FILE *fs) 
{
	int i=0;
	int j = 0;
	int n = 0;
	int iCount = 0;
	st_dbsClt clt;
	st_dbsCnu cnu;
	char logmsg[256]={0};

	writePageHeader(fs);

	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>Topology</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=900></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br>Display the EoC network topology</br>\n<br><br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='diagret' align='center' width=60>Index</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=160>MAC Address</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=120>Model</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=80>Permition</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=120>RX/TX</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=60>Status</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=100>Profile</td>\n</tr>\n</table>\n<br>\n");

	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n");
	for( i=1; i<=MAX_CLT_AMOUNT_LIMIT; i++ )
	{	
		if( CMM_SUCCESS == dbsGetClt(dbsdev, i, &clt) )
		{
			/* no clt present in this port */
			if( 0 == clt.col_row_sts )
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='clt' align='center' width=60>%d</td>\n", i);
				//fprintf(fs, "<td class='clt' align='center' width=160>[%s]</td>\n", clt.col_mac);
				fprintf(fs, "<td class='clt' align='center' width=160><img src='help.gif'></td>\n");
				fprintf(fs, "<td class='clt' align='center' width=120>CLT</td>\n");
				fprintf(fs, "<td class='clt' align='center' width=80>--</td>\n");
				fprintf(fs, "<td class='clt' align='center' width=120>--</td>\n");
				fprintf(fs, "<td class='clt' align='center' width=60><IMG src='suppress.png'></td>\n");
				fprintf(fs, "<td class='clt' align='center' width=100><img src='show.gif'></td>\n");
				fprintf(fs, "<tr>\n");
			}
			else
			{
				iCount = 0;
				
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='clt' align='center' width=60>%d</td>\n", i);
				fprintf(fs, "<td class='clt' align='center' width=160>[%s]</td>\n", clt.col_mac);
				fprintf(fs, "<td class='clt' align='center' width=120>CLT</td>\n");
				fprintf(fs, "<td class='clt' align='center' width=80>--</td>\n");
				fprintf(fs, "<td class='clt' align='center' width=120>--</td>\n");
				//fprintf(fs, "<td class='clt' align='center' width=60><IMG src='true.png' width='14' height='15'></td>\n");
				fprintf(fs, "<td class='clt' align='center' width=60><IMG src='%s'></td>\n", clt.col_sts?"true.png":"net_down.gif");
				fprintf(fs, "<td class='clt' align='center' width=100><img src='show.gif' style='cursor:pointer' onclick='window.location=\"cltProfile.cmd?viewid=%d\"'></td>\n", i);
				fprintf(fs, "<tr>\n");
				for( j=1; j<=MAX_CNUS_PER_CLT; j++ )
				{
					n = (i-1)*MAX_CNUS_PER_CLT+j;
					if( dbsGetCnu(dbsdev, n, &cnu) != CMM_SUCCESS )
					{
						break;
					}
					else
					{
						if( cnu.col_row_sts == 0 )
						{
							/**/
							continue;
						}
						else
						{
							
							iCount++;
							if( (iCount % 2) == 0 )
							{
								/**/
								fprintf(fs, "<tr>\n");
								fprintf(fs, "<td class='cnub' align='center'>%d/%d</td>\n", i, j);
								fprintf(fs, "<td class='cnub' align='center'>%s</td>\n", cnu.col_mac);
								fprintf(fs, "<td class='cnub' align='center'>%s</td>\n", boardapi_getDeviceModelStr(cnu.col_model));
								fprintf(fs, "<td class='cnub' align='center'>%s</td>\n", cnu.col_auth?"Yes":"No");
								fprintf(fs, "<td class='cnub' align='center'>%d/%d</td>\n", cnu.col_rx, cnu.col_tx);
								fprintf(fs, "<td class='cnub' align='center'><IMG src='%s'></td>\n", cnu.col_sts?"net_up.gif":"net_down.gif");
								fprintf(fs, "<td class='cnub' align='center'><img src='show.gif' style='cursor:pointer' onclick='window.location=\"cnuProfile.cmd?viewid=%d\"'></td>\n", n);
								fprintf(fs, "</tr>\n");
							}
							else
							{
								/**/
								fprintf(fs, "<tr>\n");
								fprintf(fs, "<td class='cnua' align='center'>%d/%d</td>\n", i, j);
								fprintf(fs, "<td class='cnua' align='center'>%s</td>\n", cnu.col_mac);
								fprintf(fs, "<td class='cnua' align='center'>%s</td>\n", boardapi_getDeviceModelStr(cnu.col_model));
								fprintf(fs, "<td class='cnua' align='center'>%s</td>\n", cnu.col_auth?"Yes":"No");
								fprintf(fs, "<td class='cnua' align='center'>%d/%d</td>\n", cnu.col_rx, cnu.col_tx);
								fprintf(fs, "<td class='cnua' align='center'><IMG src='%s'></td>\n", cnu.col_sts?"net_up.gif":"net_down.gif");
								//fprintf(fs, "<td class='cnua' align='center'><a href='cnuProfile.cmd?viewid=%d'><font color=blue>Show</font></a></td>\n", cnu.id);
								fprintf(fs, "<td class='cnua' align='center'><img src='show.gif' style='cursor:pointer' onclick='window.location=\"cnuProfile.cmd?viewid=%d\"'></td>\n", n);
								fprintf(fs, "</tr>\n");
							}
						}
					}
				}
			}			
		}
		else
		{
			/**/
			fprintf(fs, "<tr>\n<td class='clt' align='center' width=700>** System Error **</td>\n<tr>\n");
			strcpy(logmsg, "show topology");
			http2dbs_writeOptlog(-1, logmsg);
		}
	}	
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=700></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p><input type='button' class='btn2L' onClick='location.href=\"previewTopology.cgi\"' value='Refresh'>\n</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiOptlogViewByMid(char *query, FILE *fs, int mid)
{   
	int i=0;
	int logCount = 0;
	st_dbsOptlog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	writePageHeader(fs);

	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>Operating Log</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=900></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br>Display operating logs in the table list bellow [Filter condition=%s]:</br>\n<br><br>\n", boardapi_getModNameStr(mid));
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='diagret' width=160>Time</td>\n");
	fprintf(fs, "<td class='diagret' width=100>Operator</td>\n");
	fprintf(fs, "<td class='diagret' width=100>Resulte</td>\n");
	fprintf(fs, "<td class='diagret' width=400>Log Information</td>\n</tr>\n</table>\n<br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=2>\n");
	//fprintf(fs, "\n");
   
	// write body
	for( i=1; i<=512; i++ )
	{
		if( http2dbs_getOptlog(i, &st_log) == CMM_SUCCESS )
		{
			if( mid != st_log.who ) continue;
			logCount++;
			tim = localtime((const time_t *)&st_log.time);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
			tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			if( st_log.result == 0 )
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='diagdata' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='diagdata' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='diagdata' width=98>SUCCESS</td>\n");
				fprintf(fs, "<td class='diagdata' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
			else
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='logfailed' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='logfailed' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='logfailed' width=98>FAILED</td>\n");
				fprintf(fs, "<td class='logfailed' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
		}
		else
		{
			break;
		}
	}
	if(logCount == 0 )
	{
		fprintf(fs, "<tr>\n<td class='diagdata' colspan=4>No operating log to list</td>\n</tr>\n");
	}
	
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=760></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p><input type='button' class='btn2L' onClick='location.href=\"wecOptlog.html\"' value='Return'>\n");
	fprintf(fs, "<input type='button' class='btn2L' onClick='window.location.reload()' value='Refresh'>\n</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiOptlogViewByStatus(char *query, FILE *fs, int status)
{   
	int i=0;
	int logCount = 0;
	st_dbsOptlog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	writePageHeader(fs);

	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>Operating Log</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='mainline' width=900></td>\n</tr>\n</table>\n");
	fprintf(fs, "<br>Display operating logs in the table list bellow [Filter condition=%s]:</br>\n<br><br>\n", status?"Failed":"Success");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='diagret' width=160>Time</td>\n");
	fprintf(fs, "<td class='diagret' width=100>Operator</td>\n");
	fprintf(fs, "<td class='diagret' width=100>Resulte</td>\n");
	fprintf(fs, "<td class='diagret' width=400>Log Information</td>\n</tr>\n</table>\n<br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=2>\n");
	//fprintf(fs, "\n");
   
	// write body
	for( i=1; i<=512; i++ )
	{
		if( http2dbs_getOptlog(i, &st_log) == CMM_SUCCESS )
		{
			if( status != st_log.result ) continue;
			logCount++;
			tim = localtime((const time_t *)&st_log.time);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
			tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			if( st_log.result == 0 )
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='diagdata' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='diagdata' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='diagdata' width=98>SUCCESS</td>\n");
				fprintf(fs, "<td class='diagdata' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
			else
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='logfailed' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='logfailed' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='logfailed' width=98>FAILED</td>\n");
				fprintf(fs, "<td class='logfailed' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
		}
		else
		{
			break;
		}
	}
	if(logCount == 0 )
	{
		fprintf(fs, "<tr>\n<td class='diagdata' colspan=4>No operating log to list</td>\n</tr>\n");
	}
	
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=760></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p><input type='button' class='btn2L' onClick='location.href=\"wecOptlog.html\"' value='Return'>\n");
	fprintf(fs, "<input type='button' class='btn2L' onClick='window.location.reload()' value='Refresh'>\n</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiSyslogViewByLevel(char *query, FILE *fs, int level)
{
	int i=0;
	int logCount = 0;	
	st_dbsSyslog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	writePageHeader(fs);
	
	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>System Log</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
   	fprintf(fs, "<td class='mainline' width=900></td>\n</tr>\n</table>\n");
	switch(level)
	{
		case DBS_LOG_EMERG:
		{
			fprintf(fs, "<br>Display system logs in the table list bellow [Filter condition=Emergency]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_ALERT:
		{
			fprintf(fs, "<br>Display system logs in the table list bellow [Filter condition=Alert]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_CRIT:
		{
			fprintf(fs, "<br>Display system logs in the table list bellow [Filter condition=Cratical]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_ERR:
		{
			fprintf(fs, "<br>Display system logs in the table list bellow [Filter condition=Error]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_WARNING:
		{
			fprintf(fs, "<br>Display system logs in the table list bellow [Filter condition=Warnning]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_NOTICE:
		{
			fprintf(fs, "<br>Display system logs in the table list bellow [Filter condition=Notice]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_INFO:
		{
			fprintf(fs, "<br>Display system logs in the table list bellow [Filter condition=Informational]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_DEBUG:
		{
			fprintf(fs, "<br>Display system logs in the table list bellow [Filter condition=Debugging]:</br>\n<br><br>\n");
			break;
		}
		default:
		{
			fprintf(fs, "<br>Display system logs in the table list bellow [Filter condition=All]:</br>\n<br><br>\n");
			break;
		}
	}	
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	
	fprintf(fs, "<td class='diagret' width=150>Time</td>\n");
	fprintf(fs, "<td class='diagret' width=110>Module</td>\n");
	fprintf(fs, "<td class='diagret' width=110>Level</td>\n");
	fprintf(fs, "<td class='diagret' width=400>Log Information</td>\n</tr>\n</table>\n<br>\n");

	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=2>\n");
	
	/*loop insert log here*/
	for( i=1; i<=1024; i++ )
	{
		if( http2dbs_getSyslog(i, &st_log) == CMM_SUCCESS )
		{
			if( st_log.level > level ) continue;
			logCount++;
			tim = localtime((const time_t *)&st_log.time);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
				tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			switch(st_log.level)
			{
				case DBS_LOG_EMERG:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logemg' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logemg' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logemg' width=108>Emergency</td>\n");
					fprintf(fs, "<td class='logemg' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_ALERT:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logalert' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logalert' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logalert' width=108>Alert</td>\n");
					fprintf(fs, "<td class='logalert' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_CRIT:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logcrt' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logcrt' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logcrt' width=108>Cratical</td>\n");
					fprintf(fs, "<td class='logcrt' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_ERR:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logerr' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logerr' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logerr' width=108>Error</td>\n");
					fprintf(fs, "<td class='logerr' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_WARNING:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logwarn' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logwarn' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logwarn' width=108>Warnning</td>\n");
					fprintf(fs, "<td class='logwarn' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_NOTICE:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='lognotice' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='lognotice' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='lognotice' width=108>Notice</td>\n");
					fprintf(fs, "<td class='lognotice' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_INFO:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='loginfo' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='loginfo' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='loginfo' width=108>Informational</td>\n");
					fprintf(fs, "<td class='loginfo' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				default:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logdbg' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logdbg' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logdbg' width=108>Debugging</td>\n");
					fprintf(fs, "<td class='logdbg' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	if(logCount == 0 )
	{
		fprintf(fs, "<tr>\n<td class='diagdata' colspan=4>No system log to list</td>\n</tr>\n");
	}
	
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=770></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p><input type='button' class='btn2L' onClick='location.href=\"wecSyslog.html\"' value='Return'>\n");
	fprintf(fs, "<input type='button' class='btn2L' onClick='window.location.reload()' value='Refresh'>\n</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	
	fflush(fs);
}

void cgiAlarmlogViewByLevel(char *query, FILE *fs, int level)
{
	int i=0;
	int logCount = 0;
	int alarmLevel = 0;
	st_dbsAlarmlog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	writePageHeader(fs);
	
	fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>Alarm Log</b></td>\n</tr>\n</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
   	fprintf(fs, "<td class='mainline' width=900></td>\n</tr>\n</table>\n");
	switch(level)
	{
		case DBS_LOG_EMERG:
		{
			fprintf(fs, "<br>Display alarm logs in the table list bellow [Filter condition=Emergency]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_ALERT:
		{
			fprintf(fs, "<br>Display alarm logs in the table list bellow [Filter condition=Alert]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_CRIT:
		{
			fprintf(fs, "<br>Display alarm logs in the table list bellow [Filter condition=Cratical]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_ERR:
		{
			fprintf(fs, "<br>Display alarm logs in the table list bellow [Filter condition=Error]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_WARNING:
		{
			fprintf(fs, "<br>Display alarm logs in the table list bellow [Filter condition=Warnning]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_NOTICE:
		{
			fprintf(fs, "<br>Display alarm logs in the table list bellow [Filter condition=Notice]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_INFO:
		{
			fprintf(fs, "<br>Display alarm logs in the table list bellow [Filter condition=Informational]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_DEBUG:
		{
			fprintf(fs, "<br>Display alarm logs in the table list bellow [Filter condition=Debugging]:</br>\n<br><br>\n");
			break;
		}
		default:
		{
			fprintf(fs, "<br>Display alarm logs in the table list bellow [Filter condition=All]:</br>\n<br><br>\n");
			break;
		}
	}	
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	
	fprintf(fs, "<td class='diagret' width=80>ID</td>\n");
	fprintf(fs, "<td class='diagret' width=60>Code</td>\n");
	fprintf(fs, "<td class='diagret' width=150>Time</td>\n");
	fprintf(fs, "<td class='diagret' width=50>Node</td>\n");
	fprintf(fs, "<td class='diagret' width=100>Alarm Type</td>\n");
	fprintf(fs, "<td class='diagret' width=60>Value</td>\n");
	fprintf(fs, "<td class='diagret' width=400>Trap Information</td>\n</tr>\n</table>\n<br>\n");

	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=2>\n");
	/*loop insert log here*/
	for( i=1; i<=512; i++ )
	{
		if( http2dbs_getAlarmlog(i, &st_log) == CMM_SUCCESS )
		{
			alarmLevel = boardapi_getAlarmLevel(&st_log);			
			if( alarmLevel > level ) continue;
			logCount++;
			tim = localtime((const time_t *)&st_log.realTime);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
				tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			/*convert temperature value*/
			if( 200903 == st_log.alarmCode )
			{
				if( 0 == (st_log.alarmValue>>24) )
				{
					st_log.alarmValue = (st_log.alarmValue>>16)&0x000000ff;
				}
				else
				{
					st_log.alarmValue = (~(((st_log.alarmValue>>16)&0x000000ff)-1));
				}
			}
			switch(alarmLevel)
			{
				case DBS_LOG_EMERG:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logemg' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=370'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logemg' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logemg' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logemg' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logemg' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logemg' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logemg' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_ALERT:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logalert' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=370'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logalert' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logalert' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logalert' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logalert' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logalert' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logalert' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_CRIT:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logcrt' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=370'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logcrt' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logcrt' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logcrt' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logcrt' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logcrt' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logcrt' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_ERR:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logerr' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=370'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logerr' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logerr' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logerr' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logerr' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logerr' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logerr' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_WARNING:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logwarn' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=370'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logwarn' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logwarn' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logwarn' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logwarn' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logwarn' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logwarn' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_NOTICE:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='lognotice' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=370'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='lognotice' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='lognotice' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='lognotice' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='lognotice' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='lognotice' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='lognotice' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_INFO:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='loginfo' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=370'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='loginfo' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='loginfo' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='loginfo' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='loginfo' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='loginfo' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='loginfo' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				/*DBS_LOG_DEBUG*/
				default:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logdbg' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=370'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logdbg' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logdbg' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logdbg' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logdbg' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logdbg' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logdbg' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	if(logCount == 0 )
	{
		fprintf(fs, "<tr>\n<td class='diagdata' colspan=7>No alarm log to list</td>\n</tr>\n");
	}
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=900></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p><input type='button' class='btn2L' onClick='location.href=\"wecAlarmlog.html\"' value='Return'>\n");
	fprintf(fs, "<input type='button' class='btn2L' onClick='window.location.reload()' value='Refresh'>\n</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiAlarmlogDetailView(char *query, FILE *fs) 
{
	char action[IFC_LARGE_LEN];
	int id = 0;
	int alarmLevel = 0;
	st_dbsAlarmlog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	cgiGetValueByName(query, "viewid", action);
	id = atoi(action);

	if( http2dbs_getAlarmlog(id, &st_log) != CMM_SUCCESS )
	{
		writePopErrorPage(fs);
		return;
	}

	tim = localtime((const time_t *)&st_log.realTime);
	sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
		tim->tm_mday, tim->tm_hour, tim->tm_min
	);
	alarmLevel = boardapi_getAlarmLevel(&st_log);
	
	writePageHeader(fs);
	
	fprintf(fs, "<br><table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");	
	fprintf(fs, "<td class='diagret' width=150>Item</td>\n");
	fprintf(fs, "<td class='diagret' width=300>Value</td>\n</tr>\n</table>\n<br>\n");	
	
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=2>\n");
	/*serialFlow*/
	fprintf(fs, "<tr>\n<td class='diagdata' width=150>Serial Number</td>\n");
	fprintf(fs, "<td class='diagdata' width=300>%d</td>\n</tr>\n", st_log.serialFlow);
	/*realTime*/
	fprintf(fs, "<tr>\n<td class='diagdata'>Real Time</td>\n");
	fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", timenow);
	/*alarmCode*/
	fprintf(fs, "<tr>\n<td class='diagdata'>Alarm Code</td>\n");
	fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", st_log.alarmCode);
	/*oid*/
	fprintf(fs, "<tr>\n<td class='diagdata'>Alarm OID</td>\n");
	fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", st_log.oid);	
	/*alarm level*/
	fprintf(fs, "<tr>\n<td class='diagdata'>Alarm Level</td>\n");
	switch(alarmLevel)
	{
		case DBS_LOG_EMERG:
		{
			fprintf(fs, "<td class='diagdata'>Emergency</td>\n</tr>\n");
			break;
		}
		case DBS_LOG_ALERT:
		{
			fprintf(fs, "<td class='diagdata'>Alert</td>\n</tr>\n");
			break;
		}
		case DBS_LOG_CRIT:
		{
			fprintf(fs, "<td class='diagdata'>Cratical</td>\n</tr>\n");
			break;
		}
		case DBS_LOG_ERR:
		{
			fprintf(fs, "<td class='diagdata'>Error</td>\n</tr>\n");
			break;
		}
		case DBS_LOG_WARNING:
		{
			fprintf(fs, "<td class='diagdata'>Warnning</td>\n</tr>\n");
			break;
		}
		case DBS_LOG_NOTICE:
		{
			fprintf(fs, "<td class='diagdata'>Notice</td>\n</tr>\n");
			break;
		}
		case DBS_LOG_INFO:
		{
			fprintf(fs, "<td class='diagdata'>Informational</td>\n</tr>\n");
			break;
		}
		default:
		{
			fprintf(fs, "<td class='diagdata'>Debugging</td>\n</tr>\n");
			break;
		}
	}		
	/*cbatMac*/
	fprintf(fs, "<tr>\n<td class='diagdata'>Agent Host MAC</td>\n");
	fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", st_log.cbatMac);
	/*cltId.cnuId*/
	fprintf(fs, "<tr>\n<td class='diagdata'>Alarm Node</td>\n");
	fprintf(fs, "<td class='diagdata'>%d.%d</td>\n</tr>\n", st_log.cltId, st_log.cnuId);
	/*alarmType*/
	fprintf(fs, "<tr>\n<td class='diagdata'>Alarm Type</td>\n");
	fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
	/*alarmValue*/
	fprintf(fs, "<tr>\n<td class='diagdata'>Parameter Value</td>\n");
	fprintf(fs, "<td class='diagdata'>%d</td>\n</tr>\n", st_log.alarmValue);	
	/*trap_info*/
	fprintf(fs, "<tr>\n<td class='diagdata'>Trap Information</td>\n");
	fprintf(fs, "<td class='diagdata'>%s</td>\n</tr>\n", st_log.trap_info);

	fprintf(fs, "</table>\n");
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=450></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p>\n<input type='button' class='btn2L' onClick='window.close()' value='Close'>\n</p>\n");
	fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
	fflush(fs);
	
}

void cgiOptlogView(char *query, FILE *fs) 
{
	char action[IFC_LARGE_LEN];
	int condition = 0;

	cgiGetValueByName(query, "action", action);
	condition = atoi(action);

	switch(condition)
	{
		case 0:
		{
			cgiOptlogViewByMid(query, fs, MID_HTTP);
			break;
		}
		case 1:
		{
			cgiOptlogViewByMid(query, fs, MID_CLI);
			break;
		}
		case 2:
		{
			cgiOptlogViewByMid(query, fs, MID_SNMP);
			break;
		}
		case 3:
		{
			cgiOptlogViewByStatus(query, fs, CMM_SUCCESS);
			break;
		}
		case 4:
		{
			cgiOptlogViewByStatus(query, fs, CMM_FAILED);
			break;
		}
		case 5:
		{
			cgiOptlogViewAll(query, fs);
			break;
		}
		default:
		{
			strcpy(glbWebVar.returnUrl, "wecOptlog.html");
			glbWebVar.wecOptCode = CMM_FAILED;
			do_ej("/webs/wecOptResult2.html", fs);
			break;
		}
	}
}

void cgiSyslogView(char *query, FILE *fs) 
{
	char action[IFC_LARGE_LEN];
	int condition = 0;

	cgiGetValueByName(query, "action", action);
	condition = atoi(action);

	switch(condition)
	{
		case DBS_LOG_EMERG:
		case DBS_LOG_ALERT:
		case DBS_LOG_CRIT:
		case DBS_LOG_ERR:
		case DBS_LOG_WARNING:
		case DBS_LOG_NOTICE:
		case DBS_LOG_INFO:
		case DBS_LOG_DEBUG:
		case DBS_LOG_DEBUG+1:
		{
			cgiSyslogViewByLevel(query, fs, condition);
			break;
		}
		default:
		{
			strcpy(glbWebVar.returnUrl, "wecSyslog.html");
			glbWebVar.wecOptCode = CMM_FAILED;
			do_ej("/webs/wecOptResult2.html", fs);
			break;
		}
	}
}

void cgiAlarmlogView(char *query, FILE *fs) 
{
	char action[IFC_LARGE_LEN];
	int condition = 0;

	cgiGetValueByName(query, "action", action);
	condition = atoi(action);

	switch(condition)
	{
		case DBS_LOG_EMERG:
		case DBS_LOG_ALERT:
		case DBS_LOG_CRIT:
		case DBS_LOG_ERR:
		case DBS_LOG_WARNING:
		case DBS_LOG_NOTICE:
		case DBS_LOG_INFO:
		case DBS_LOG_DEBUG:
		case DBS_LOG_DEBUG+1:
		{
			cgiAlarmlogViewByLevel(query, fs, condition);
			break;
		}
		default:
		{
			strcpy(glbWebVar.returnUrl, "wecAlarmlog.html");
			glbWebVar.wecOptCode = CMM_FAILED;
			do_ej("/webs/wecOptResult2.html", fs);
			break;
		}
	}
}





