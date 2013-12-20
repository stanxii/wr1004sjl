#ifndef SETTXGAIN_SOURCE
#define SETTXGAIN_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_SetTxGain(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], uint8_t tx_gain) 
{
	struct __packed vs_set_tx_gain_req 
	{
		vs_set_property_req_header_t set_property_cnf_header;
		uint8_t TX_GAIN;
	}
	* request = (struct vs_set_tx_gain_req *)(buffer);
	size_t offset = 0;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_SET_PROPERTY | MMTYPE_REQ));
	if (offset < sizeof (struct header_vs)) 
	{
		return (0);
	}
	if (bufferLen < IHPAPI_ETHER_MIN_LEN) 
	{
		errno = ERANGE;
		return (0);
	}
	if (offset < IHPAPI_ETHER_MIN_LEN) 
	{
		memset (buffer + offset, 0, IHPAPI_ETHER_MIN_LEN - offset);
	}

	/* Unique ID for the request */
	request->set_property_cnf_header.COOKIE = ihtonl(0x01);
	/* 0x01: ApplyNow_DoNotReset */
	request->set_property_cnf_header.OPTION = 0x01;
	/* Reserved field */
	request->set_property_cnf_header.RESERVED[0] = 0x00;
	request->set_property_cnf_header.RESERVED[1] = 0x00;
	request->set_property_cnf_header.RESERVED[2] = 0x00;
	/* Property version */
	request->set_property_cnf_header.PROP_VERSION = ihtonl(0);
	/* Property ID */
	request->set_property_cnf_header.PROP_ID = ihtonl(111);
	/* Property Data Length */
	request->set_property_cnf_header.PROP_DATA_LENGTH = ihtonl(1);
	/* Property data */
	request->TX_GAIN = tx_gain;
	
	return (IHPAPI_ETHER_MIN_LEN);
}

#endif
 

