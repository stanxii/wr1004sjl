#ifndef DECODEGETFREQUENCYBANDSELECTION_SOURCE
#define DECODEGETFREQUENCYBANDSELECTION_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeGetFrequencyBandSelection (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct __packed vs_get_frequency_band_selection_cnf 
	{
		struct header_vs header;
		uint32_t MSTATUS;
		uint32_t COOKIE;
		uint8_t OUTPUT_FORMAT;
		uint8_t RESERVED[3];
		uint32_t PROP_STR_LENGTH;
		uint8_t FBSTATUS;
		uint16_t START_BAND;
		uint16_t STOP_BAND;		
	}
	* confirm = (struct vs_get_frequency_band_selection_cnf *)(buffer);

#if INTELLON_SAFEMODE
 
	if (length < sizeof (struct vs_get_frequency_band_selection_cnf)) 
	{
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}

#endif

	result->validData = true;
	result->opStatus.status = intohl(confirm->MSTATUS);	
	
	result->data.FrequencyBandSelectionInfo.FBSTATUS = confirm->FBSTATUS;
	result->data.FrequencyBandSelectionInfo.START_BAND = intohs(confirm->START_BAND);
	result->data.FrequencyBandSelectionInfo.STOP_BAND = intohs(confirm->STOP_BAND);
	
	return (0);
}

#endif
 

