#ifndef DECODESETFREQUENCYBANDSELECTION_SOURCE
#define DECODESETFREQUENCYBANDSELECTION_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeSetFrequencyBandSelection(const uint8_t buffer [ ], size_t length, ihpapi_result_t * result)

{
	struct __packed vs_set_frequency_band_selection_cnf 
	{
		struct header_vs header;
		uint32_t MSTATUS;
		uint32_t COOKIE;
	}
	* confirm = (struct vs_set_frequency_band_selection_cnf *)(buffer);

#if INTELLON_SAFEMODE
 
	if (length < sizeof (struct vs_set_frequency_band_selection_cnf)) 
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
	
	return (0);
}

#endif
 

