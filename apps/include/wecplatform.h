#ifndef __WEC_PLATFORM_H__
#define __WEC_PLATFORM_H__

#include "config.h"

//CFG_USE_PLATFORM_XXX is defined by makefile

#ifdef CFG_USE_PLATFORM_WEC9720EK_C22
 #define MAX_CLT_AMOUNT_LIMIT		1

 /* CPU port */
 #define PORT_CPU_PORT_ID			5
 #define PORT_CPU_PORT_ADDR		0x15

 /* Uplink port 1 */
 #define PORT_ETH1_PHY_ID			0
 #define PORT_ETH1_PHY_ADDR		0
 #define PORT_ETH1_PORT_ID			0
 #define PORT_ETH1_PORT_ADDR		0x10

 /* Uplink port 2 */
 #define PORT_ETH2_PHY_ID			1
 #define PORT_ETH2_PHY_ADDR		1
 #define PORT_ETH2_PORT_ID			1
 #define PORT_ETH2_PORT_ADDR		0x11

 /* CLT1 port */
 #define PORT_CABLE1_PORT_ID		6
 #define PORT_CABLE1_PORT_ADDR	0x16
 
 #define PORT_CABLE2_PORT_ID		2
 #define PORT_CABLE2_PORT_ADDR	0x12
#endif

#ifdef CFG_USE_PLATFORM_WEC9720EK_S220
 #define MAX_CLT_AMOUNT_LIMIT		1

 /* CPU port */
 #define PORT_CPU_PORT_ID			5
 #define PORT_CPU_PORT_ADDR		0x15

 /* Uplink port 1 */
 #define PORT_ETH1_PHY_ID			0
 #define PORT_ETH1_PHY_ADDR		0
 #define PORT_ETH1_PORT_ID			0
 #define PORT_ETH1_PORT_ADDR		0x10

 /* Uplink port 2 */
 #define PORT_ETH2_PHY_ID			1
 #define PORT_ETH2_PHY_ADDR		1
 #define PORT_ETH2_PORT_ID			1
 #define PORT_ETH2_PORT_ADDR		0x11

 /* CLT1 port */
 #define PORT_CABLE1_PORT_ID		6
 #define PORT_CABLE1_PORT_ADDR	0x16
 
 #define PORT_CABLE2_PORT_ID		2
 #define PORT_CABLE2_PORT_ADDR	0x12
#endif

#ifdef CFG_USE_PLATFORM_WEC9720EK_XD25
 #define MAX_CLT_AMOUNT_LIMIT		1

 /* CPU port */
 #define PORT_CPU_PORT_ID			5
 #define PORT_CPU_PORT_ADDR		0x15

 /* Uplink port 1 */
 #define PORT_ETH1_PHY_ID			0
 #define PORT_ETH1_PHY_ADDR		0
 #define PORT_ETH1_PORT_ID			0
 #define PORT_ETH1_PORT_ADDR		0x10

 /* Uplink port 2 */
 #define PORT_ETH2_PHY_ID			1
 #define PORT_ETH2_PHY_ADDR		1
 #define PORT_ETH2_PORT_ID			1
 #define PORT_ETH2_PORT_ADDR		0x11

 /* CLT1 port */
 #define PORT_CABLE1_PORT_ID		6
 #define PORT_CABLE1_PORT_ADDR	0x16
 
 #define PORT_CABLE2_PORT_ID		2
 #define PORT_CABLE2_PORT_ADDR	0x12
#endif

#ifdef CFG_USE_PLATFORM_WR1004SJL 
 #define MAX_CLT_AMOUNT_LIMIT		4
 
 #define PORT_CPU_PORT_ID			5
 #define PORT_CPU_PORT_ADDR		0x15

 /* ONU | Uplink port */
 #define PORT_ETH1_PHY_ID			0
 #define PORT_ETH1_PHY_ADDR		0
 #define PORT_ETH1_PORT_ID			0
 #define PORT_ETH1_PORT_ADDR		0x10

 /* Opetical reveiver port */
 //#define PORT_ETH2_PHY_ID			6
 //#define PORT_ETH2_PHY_ADDR		6
 #define PORT_ETH2_PORT_ID			6
 #define PORT_ETH2_PORT_ADDR		0x16

 #define PORT_CABLE1_PHY_ID		1
 #define PORT_CABLE1_PHY_ADDR		1
 #define PORT_CABLE1_PORT_ID		1
 #define PORT_CABLE1_PORT_ADDR	0x11
 
 #define PORT_CABLE2_PHY_ID		2
 #define PORT_CABLE2_PHY_ADDR		2
 #define PORT_CABLE2_PORT_ID		2
 #define PORT_CABLE2_PORT_ADDR	0x12

 #define PORT_CABLE3_PHY_ID		3
 #define PORT_CABLE3_PHY_ADDR		3
 #define PORT_CABLE3_PORT_ID		3
 #define PORT_CABLE3_PORT_ADDR	0x13

 #define PORT_CABLE4_PHY_ID		4
 #define PORT_CABLE4_PHY_ADDR		4
 #define PORT_CABLE4_PORT_ID		4
 #define PORT_CABLE4_PORT_ADDR	0x14
#endif

#endif 

