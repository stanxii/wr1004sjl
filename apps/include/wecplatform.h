#ifndef __WEC_PLATFORM_H__
#define __WEC_PLATFORM_H__

#include "config.h"

//CFG_USE_PLATFORM_XXX is defined by makefile

#ifdef CFG_USE_PLATFORM_WEC9720EK_C22
 #define PORT_CPU_PORT_ID			5
 #define PORT_CPU_PORT_ADDR		0x15
 #define PORT_ETH1_PHY_ID			0
 #define PORT_ETH1_PHY_ADDR		0
 #define PORT_ETH1_PORT_ID			0
 #define PORT_ETH1_PORT_ADDR		0x10
 #define PORT_ETH2_PHY_ID			1
 #define PORT_ETH2_PHY_ADDR		1
 #define PORT_ETH2_PORT_ID			1
 #define PORT_ETH2_PORT_ADDR		0x11
 #define PORT_CABLE1_PORT_ID		6
 #define PORT_CABLE1_PORT_ADDR	0x16
 #define PORT_CABLE2_PORT_ID		2
 #define PORT_CABLE2_PORT_ADDR	0x12
#endif

#ifdef CFG_USE_PLATFORM_WEC9720EK_S220
 #define PORT_CPU_PORT_ID			5
 #define PORT_CPU_PORT_ADDR		0x15
 #define PORT_ETH1_PHY_ID			0
 #define PORT_ETH1_PHY_ADDR		0
 #define PORT_ETH1_PORT_ID			0
 #define PORT_ETH1_PORT_ADDR		0x10
 #define PORT_ETH2_PHY_ID			1
 #define PORT_ETH2_PHY_ADDR		1
 #define PORT_ETH2_PORT_ID			1
 #define PORT_ETH2_PORT_ADDR		0x11
 #define PORT_CABLE1_PORT_ID		6
 #define PORT_CABLE1_PORT_ADDR	0x16
 #define PORT_CABLE2_PORT_ID		2
 #define PORT_CABLE2_PORT_ADDR	0x12
#endif

#ifdef CFG_USE_PLATFORM_WEC9720EK_XD25
 #define PORT_CPU_PORT_ID			5
 #define PORT_CPU_PORT_ADDR		0x15
 #define PORT_ETH1_PHY_ID			0
 #define PORT_ETH1_PHY_ADDR		0
 #define PORT_ETH1_PORT_ID			0
 #define PORT_ETH1_PORT_ADDR		0x10
 #define PORT_ETH2_PHY_ID			1
 #define PORT_ETH2_PHY_ADDR		1
 #define PORT_ETH2_PORT_ID			1
 #define PORT_ETH2_PORT_ADDR		0x11
 #define PORT_CABLE1_PORT_ID		6
 #define PORT_CABLE1_PORT_ADDR	0x16
 #define PORT_CABLE2_PORT_ID		2
 #define PORT_CABLE2_PORT_ADDR	0x12
#endif

#ifdef CFG_USE_PLATFORM_WR1004SJL 

#endif

#endif 

