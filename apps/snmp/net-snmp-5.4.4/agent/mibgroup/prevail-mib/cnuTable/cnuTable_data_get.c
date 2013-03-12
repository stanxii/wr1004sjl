/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 12088 $ of $ 
 *
 * $Id:$
 */
/* standard Net-SNMP includes */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/* include our parent header */
#include "cnuTable.h"


/** @defgroup data_get data_get: Routines to get data
 *
 * TODO:230:M: Implement cnuTable get routines.
 * TODO:240:M: Implement cnuTable mapping routines (if any).
 *
 * These routine are used to get the value for individual objects. The
 * row context is passed, along with a pointer to the memory where the
 * value should be copied.
 *
 * @{
 */
/**********************************************************************
 **********************************************************************
 ***
 *** Table cnuTable
 ***
 **********************************************************************
 **********************************************************************/
/*
 * prevail-mib::cnuTable is subid 1 of modEoCMib.
 * Its status is Current.
 * OID: .1.3.6.1.4.1.36186.8.1, length: 9
*/

/* ---------------------------------------------------------------------
 * TODO:200:r: Implement cnuTable data context functions.
 */


/**
 * set mib index(es)
 *
 * @param tbl_idx mib index structure
 * @param cnuTid_val
 *
 * @retval MFD_SUCCESS     : success.
 * @retval MFD_ERROR       : other error.
 *
 * @remark
 *  This convenience function is useful for setting all the MIB index
 *  components with a single function call. It is assume that the C values
 *  have already been mapped from their native/rawformat to the MIB format.
 */
int
cnuTable_indexes_set_tbl_idx(cnuTable_mib_index *tbl_idx, long cnuTid_val)
{
    DEBUGMSGTL(("verbose:cnuTable:cnuTable_indexes_set_tbl_idx","called\n"));

    /* cnuTid(1)/INTEGER/ASN_INTEGER/long(long)//l/A/w/e/R/d/h */
    tbl_idx->cnuTid = cnuTid_val;
    

    return MFD_SUCCESS;
} /* cnuTable_indexes_set_tbl_idx */

/**
 * @internal
 * set row context indexes
 *
 * @param reqreq_ctx the row context that needs updated indexes
 *
 * @retval MFD_SUCCESS     : success.
 * @retval MFD_ERROR       : other error.
 *
 * @remark
 *  This function sets the mib indexs, then updates the oid indexs
 *  from the mib index.
 */
int
cnuTable_indexes_set(cnuTable_rowreq_ctx *rowreq_ctx, long cnuTid_val)
{
    DEBUGMSGTL(("verbose:cnuTable:cnuTable_indexes_set","called\n"));

    if(MFD_SUCCESS != cnuTable_indexes_set_tbl_idx(&rowreq_ctx->tbl_idx
                                   , cnuTid_val
           ))
        return MFD_ERROR;

    /*
     * convert mib index to oid index
     */
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if(0 != cnuTable_index_to_oid(&rowreq_ctx->oid_idx,
                                    &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }

    return MFD_SUCCESS;
} /* cnuTable_indexes_set */


/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuIndex
 * cnuIndex is subid 2 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.2
 * Description:
the index of cnu entry in databases.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  0      hasdefval 0
 *   readable   1     iscolumn 1     ranges 1      hashint   0
 *   settable   0
 *
 * Ranges:  1 - 64;
 *
 * Its syntax is INTEGER (based on perltype INTEGER)
 * The net-snmp type is ASN_INTEGER. The C type decl is long (long)
 */
/**
 * Extract the current value of the cnuIndex data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuIndex_val_ptr
 *        Pointer to storage for a long variable
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
 */
int
cnuIndex_get( cnuTable_rowreq_ctx *rowreq_ctx, long * cnuIndex_val_ptr )
{
   /** we should have a non-NULL pointer */
   netsnmp_assert( NULL != cnuIndex_val_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuIndex_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuIndex data.
 * copy (* cnuIndex_val_ptr ) from rowreq_ctx->data
 */
    (* cnuIndex_val_ptr ) = rowreq_ctx->data.cnuIndex;

    return MFD_SUCCESS;
} /* cnuIndex_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuModel
 * cnuModel is subid 3 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.3
 * Description:
the model of EoC products in prevail.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  1      hasdefval 0
 *   readable   1     iscolumn 1     ranges 0      hashint   0
 *   settable   0
 *
 * Enum range: 9/16. Values:  X7(1), E31(2), Q31(3), C22(4), S220(5), S60(6), L2(7), L3(8), C2(9), C4(10), UNKNOWN(256)
 *
 * Its syntax is DevModelValue (based on perltype INTEGER)
 * The net-snmp type is ASN_INTEGER. The C type decl is long (u_long)
 */
/**
 * Extract the current value of the cnuModel data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuModel_val_ptr
 *        Pointer to storage for a long variable
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
 */
int
cnuModel_get( cnuTable_rowreq_ctx *rowreq_ctx, u_long * cnuModel_val_ptr )
{
   /** we should have a non-NULL pointer */
   netsnmp_assert( NULL != cnuModel_val_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuModel_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuModel data.
 * copy (* cnuModel_val_ptr ) from rowreq_ctx->data
 */
    (* cnuModel_val_ptr ) = rowreq_ctx->data.cnuModel;

    return MFD_SUCCESS;
} /* cnuModel_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuMacAddress
 * cnuMacAddress is subid 4 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.4
 * Description:
the mac address of CNU.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  0      hasdefval 0
 *   readable   1     iscolumn 1     ranges 1      hashint   1
 *   settable   0
 *   hint: 1x:
 *
 * Ranges:  6;
 *
 * Its syntax is MacAddress (based on perltype OCTETSTR)
 * The net-snmp type is ASN_OCTET_STR. The C type decl is char (char)
 * This data type requires a length.  (Max 6)
 */
/**
 * Extract the current value of the cnuMacAddress data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuMacAddress_val_ptr_ptr
 *        Pointer to storage for a char variable
 * @param cnuMacAddress_val_ptr_len_ptr
 *        Pointer to a size_t. On entry, it will contain the size (in bytes)
 *        pointed to by cnuMacAddress.
 *        On exit, this value should contain the data size (in bytes).
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
*
 * @note If you need more than (*cnuMacAddress_val_ptr_len_ptr) bytes of memory,
 *       allocate it using malloc() and update cnuMacAddress_val_ptr_ptr.
 *       <b>DO NOT</b> free the previous pointer.
 *       The MFD helper will release the memory you allocate.
 *
 * @remark If you call this function yourself, you are responsible
 *         for checking if the pointer changed, and freeing any
 *         previously allocated memory. (Not necessary if you pass
 *         in a pointer to static memory, obviously.)
 */
int
cnuMacAddress_get( cnuTable_rowreq_ctx *rowreq_ctx, char **cnuMacAddress_val_ptr_ptr, size_t *cnuMacAddress_val_ptr_len_ptr )
{
   /** we should have a non-NULL pointer and enough storage */
   netsnmp_assert( (NULL != cnuMacAddress_val_ptr_ptr) && (NULL != *cnuMacAddress_val_ptr_ptr));
   netsnmp_assert( NULL != cnuMacAddress_val_ptr_len_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuMacAddress_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuMacAddress data.
 * copy (* cnuMacAddress_val_ptr_ptr ) data and (* cnuMacAddress_val_ptr_len_ptr ) from rowreq_ctx->data
 */
    /*
     * make sure there is enough space for cnuMacAddress data
     */
    if ((NULL == (* cnuMacAddress_val_ptr_ptr )) ||
        ((* cnuMacAddress_val_ptr_len_ptr ) <
         (rowreq_ctx->data.cnuMacAddress_len* sizeof(rowreq_ctx->data.cnuMacAddress[0])))) {
        /*
         * allocate space for cnuMacAddress data
         */
        (* cnuMacAddress_val_ptr_ptr ) = malloc(rowreq_ctx->data.cnuMacAddress_len* sizeof(rowreq_ctx->data.cnuMacAddress[0]));
        if(NULL == (* cnuMacAddress_val_ptr_ptr )) {
            snmp_log(LOG_ERR,"could not allocate memory\n");
            return MFD_ERROR;
        }
    }
    (* cnuMacAddress_val_ptr_len_ptr ) = rowreq_ctx->data.cnuMacAddress_len* sizeof(rowreq_ctx->data.cnuMacAddress[0]);
    memcpy( (* cnuMacAddress_val_ptr_ptr ), rowreq_ctx->data.cnuMacAddress, rowreq_ctx->data.cnuMacAddress_len* sizeof(rowreq_ctx->data.cnuMacAddress[0]) );

    return MFD_SUCCESS;
} /* cnuMacAddress_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuOnlineStatus
 * cnuOnlineStatus is subid 5 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.5
 * Description:
The online status of CNU in EoC topology
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  1      hasdefval 0
 *   readable   1     iscolumn 1     ranges 0      hashint   0
 *   settable   0
 *
 * Enum range: 2/8. Values:  true(1), false(2)
 *
 * Its syntax is TruthValue (based on perltype INTEGER)
 * The net-snmp type is ASN_INTEGER. The C type decl is long (u_long)
 */
/**
 * Extract the current value of the cnuOnlineStatus data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuOnlineStatus_val_ptr
 *        Pointer to storage for a long variable
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
 */
int
cnuOnlineStatus_get( cnuTable_rowreq_ctx *rowreq_ctx, u_long * cnuOnlineStatus_val_ptr )
{
   /** we should have a non-NULL pointer */
   netsnmp_assert( NULL != cnuOnlineStatus_val_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuOnlineStatus_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuOnlineStatus data.
 * copy (* cnuOnlineStatus_val_ptr ) from rowreq_ctx->data
 */
    (* cnuOnlineStatus_val_ptr ) = rowreq_ctx->data.cnuOnlineStatus;

    return MFD_SUCCESS;
} /* cnuOnlineStatus_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuAuthorized
 * cnuAuthorized is subid 6 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.6
 * Description:
the CNU authorization status in EoC network.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  1      hasdefval 0
 *   readable   1     iscolumn 1     ranges 0      hashint   0
 *   settable   0
 *
 * Enum range: 2/8. Values:  true(1), false(2)
 *
 * Its syntax is TruthValue (based on perltype INTEGER)
 * The net-snmp type is ASN_INTEGER. The C type decl is long (u_long)
 */
/**
 * Extract the current value of the cnuAuthorized data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuAuthorized_val_ptr
 *        Pointer to storage for a long variable
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
 */
int
cnuAuthorized_get( cnuTable_rowreq_ctx *rowreq_ctx, u_long * cnuAuthorized_val_ptr )
{
   /** we should have a non-NULL pointer */
   netsnmp_assert( NULL != cnuAuthorized_val_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuAuthorized_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuAuthorized data.
 * copy (* cnuAuthorized_val_ptr ) from rowreq_ctx->data
 */
    (* cnuAuthorized_val_ptr ) = rowreq_ctx->data.cnuAuthorized;

    return MFD_SUCCESS;
} /* cnuAuthorized_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuSoftwareVersion
 * cnuSoftwareVersion is subid 7 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.7
 * Description:
the software version of the CNU.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  0      hasdefval 0
 *   readable   1     iscolumn 1     ranges 1      hashint   1
 *   settable   0
 *   hint: 255a
 *
 * Ranges:  0 - 128;
 *
 * Its syntax is DisplayString (based on perltype OCTETSTR)
 * The net-snmp type is ASN_OCTET_STR. The C type decl is char (char)
 * This data type requires a length.  (Max 128)
 */
/**
 * Extract the current value of the cnuSoftwareVersion data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuSoftwareVersion_val_ptr_ptr
 *        Pointer to storage for a char variable
 * @param cnuSoftwareVersion_val_ptr_len_ptr
 *        Pointer to a size_t. On entry, it will contain the size (in bytes)
 *        pointed to by cnuSoftwareVersion.
 *        On exit, this value should contain the data size (in bytes).
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
*
 * @note If you need more than (*cnuSoftwareVersion_val_ptr_len_ptr) bytes of memory,
 *       allocate it using malloc() and update cnuSoftwareVersion_val_ptr_ptr.
 *       <b>DO NOT</b> free the previous pointer.
 *       The MFD helper will release the memory you allocate.
 *
 * @remark If you call this function yourself, you are responsible
 *         for checking if the pointer changed, and freeing any
 *         previously allocated memory. (Not necessary if you pass
 *         in a pointer to static memory, obviously.)
 */
int
cnuSoftwareVersion_get( cnuTable_rowreq_ctx *rowreq_ctx, char **cnuSoftwareVersion_val_ptr_ptr, size_t *cnuSoftwareVersion_val_ptr_len_ptr )
{
   /** we should have a non-NULL pointer and enough storage */
   netsnmp_assert( (NULL != cnuSoftwareVersion_val_ptr_ptr) && (NULL != *cnuSoftwareVersion_val_ptr_ptr));
   netsnmp_assert( NULL != cnuSoftwareVersion_val_ptr_len_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuSoftwareVersion_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuSoftwareVersion data.
 * copy (* cnuSoftwareVersion_val_ptr_ptr ) data and (* cnuSoftwareVersion_val_ptr_len_ptr ) from rowreq_ctx->data
 */
    /*
     * make sure there is enough space for cnuSoftwareVersion data
     */
    if ((NULL == (* cnuSoftwareVersion_val_ptr_ptr )) ||
        ((* cnuSoftwareVersion_val_ptr_len_ptr ) <
         (rowreq_ctx->data.cnuSoftwareVersion_len* sizeof(rowreq_ctx->data.cnuSoftwareVersion[0])))) {
        /*
         * allocate space for cnuSoftwareVersion data
         */
        (* cnuSoftwareVersion_val_ptr_ptr ) = malloc(rowreq_ctx->data.cnuSoftwareVersion_len* sizeof(rowreq_ctx->data.cnuSoftwareVersion[0]));
        if(NULL == (* cnuSoftwareVersion_val_ptr_ptr )) {
            snmp_log(LOG_ERR,"could not allocate memory\n");
            return MFD_ERROR;
        }
    }
    (* cnuSoftwareVersion_val_ptr_len_ptr ) = rowreq_ctx->data.cnuSoftwareVersion_len* sizeof(rowreq_ctx->data.cnuSoftwareVersion[0]);
    memcpy( (* cnuSoftwareVersion_val_ptr_ptr ), rowreq_ctx->data.cnuSoftwareVersion, rowreq_ctx->data.cnuSoftwareVersion_len* sizeof(rowreq_ctx->data.cnuSoftwareVersion[0]) );

    return MFD_SUCCESS;
} /* cnuSoftwareVersion_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuRxRate
 * cnuRxRate is subid 8 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.8
 * Description:
the rxRate of CNU.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  0      hasdefval 0
 *   readable   1     iscolumn 1     ranges 0      hashint   0
 *   settable   0
 *
 *
 * Its syntax is INTEGER (based on perltype INTEGER)
 * The net-snmp type is ASN_INTEGER. The C type decl is long (long)
 */
/**
 * Extract the current value of the cnuRxRate data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuRxRate_val_ptr
 *        Pointer to storage for a long variable
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
 */
int
cnuRxRate_get( cnuTable_rowreq_ctx *rowreq_ctx, long * cnuRxRate_val_ptr )
{
   /** we should have a non-NULL pointer */
   netsnmp_assert( NULL != cnuRxRate_val_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuRxRate_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuRxRate data.
 * copy (* cnuRxRate_val_ptr ) from rowreq_ctx->data
 */
    (* cnuRxRate_val_ptr ) = rowreq_ctx->data.cnuRxRate;

    return MFD_SUCCESS;
} /* cnuRxRate_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuTxRate
 * cnuTxRate is subid 9 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.9
 * Description:
the txRate of CNU.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  0      hasdefval 0
 *   readable   1     iscolumn 1     ranges 0      hashint   0
 *   settable   0
 *
 *
 * Its syntax is INTEGER (based on perltype INTEGER)
 * The net-snmp type is ASN_INTEGER. The C type decl is long (long)
 */
/**
 * Extract the current value of the cnuTxRate data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuTxRate_val_ptr
 *        Pointer to storage for a long variable
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
 */
int
cnuTxRate_get( cnuTable_rowreq_ctx *rowreq_ctx, long * cnuTxRate_val_ptr )
{
   /** we should have a non-NULL pointer */
   netsnmp_assert( NULL != cnuTxRate_val_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuTxRate_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuTxRate data.
 * copy (* cnuTxRate_val_ptr ) from rowreq_ctx->data
 */
    (* cnuTxRate_val_ptr ) = rowreq_ctx->data.cnuTxRate;

    return MFD_SUCCESS;
} /* cnuTxRate_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuSnr
 * cnuSnr is subid 10 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.10
 * Description:
the snr between clt and cnu.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  0      hasdefval 0
 *   readable   1     iscolumn 1     ranges 1      hashint   1
 *   settable   0
 *   hint: 255a
 *
 * Ranges:  0 - 16;
 *
 * Its syntax is DisplayString (based on perltype OCTETSTR)
 * The net-snmp type is ASN_OCTET_STR. The C type decl is char (char)
 * This data type requires a length.  (Max 16)
 */
/**
 * Extract the current value of the cnuSnr data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuSnr_val_ptr_ptr
 *        Pointer to storage for a char variable
 * @param cnuSnr_val_ptr_len_ptr
 *        Pointer to a size_t. On entry, it will contain the size (in bytes)
 *        pointed to by cnuSnr.
 *        On exit, this value should contain the data size (in bytes).
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
*
 * @note If you need more than (*cnuSnr_val_ptr_len_ptr) bytes of memory,
 *       allocate it using malloc() and update cnuSnr_val_ptr_ptr.
 *       <b>DO NOT</b> free the previous pointer.
 *       The MFD helper will release the memory you allocate.
 *
 * @remark If you call this function yourself, you are responsible
 *         for checking if the pointer changed, and freeing any
 *         previously allocated memory. (Not necessary if you pass
 *         in a pointer to static memory, obviously.)
 */
int
cnuSnr_get( cnuTable_rowreq_ctx *rowreq_ctx, char **cnuSnr_val_ptr_ptr, size_t *cnuSnr_val_ptr_len_ptr )
{
   /** we should have a non-NULL pointer and enough storage */
   netsnmp_assert( (NULL != cnuSnr_val_ptr_ptr) && (NULL != *cnuSnr_val_ptr_ptr));
   netsnmp_assert( NULL != cnuSnr_val_ptr_len_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuSnr_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuSnr data.
 * copy (* cnuSnr_val_ptr_ptr ) data and (* cnuSnr_val_ptr_len_ptr ) from rowreq_ctx->data
 */
    /*
     * make sure there is enough space for cnuSnr data
     */
    if ((NULL == (* cnuSnr_val_ptr_ptr )) ||
        ((* cnuSnr_val_ptr_len_ptr ) <
         (rowreq_ctx->data.cnuSnr_len* sizeof(rowreq_ctx->data.cnuSnr[0])))) {
        /*
         * allocate space for cnuSnr data
         */
        (* cnuSnr_val_ptr_ptr ) = malloc(rowreq_ctx->data.cnuSnr_len* sizeof(rowreq_ctx->data.cnuSnr[0]));
        if(NULL == (* cnuSnr_val_ptr_ptr )) {
            snmp_log(LOG_ERR,"could not allocate memory\n");
            return MFD_ERROR;
        }
    }
    (* cnuSnr_val_ptr_len_ptr ) = rowreq_ctx->data.cnuSnr_len* sizeof(rowreq_ctx->data.cnuSnr[0]);
    memcpy( (* cnuSnr_val_ptr_ptr ), rowreq_ctx->data.cnuSnr, rowreq_ctx->data.cnuSnr_len* sizeof(rowreq_ctx->data.cnuSnr[0]) );

    return MFD_SUCCESS;
} /* cnuSnr_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuBpc
 * cnuBpc is subid 11 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.11
 * Description:
the bpc between clt and cnu.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  0      hasdefval 0
 *   readable   1     iscolumn 1     ranges 1      hashint   1
 *   settable   0
 *   hint: 255a
 *
 * Ranges:  0 - 16;
 *
 * Its syntax is DisplayString (based on perltype OCTETSTR)
 * The net-snmp type is ASN_OCTET_STR. The C type decl is char (char)
 * This data type requires a length.  (Max 16)
 */
/**
 * Extract the current value of the cnuBpc data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuBpc_val_ptr_ptr
 *        Pointer to storage for a char variable
 * @param cnuBpc_val_ptr_len_ptr
 *        Pointer to a size_t. On entry, it will contain the size (in bytes)
 *        pointed to by cnuBpc.
 *        On exit, this value should contain the data size (in bytes).
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
*
 * @note If you need more than (*cnuBpc_val_ptr_len_ptr) bytes of memory,
 *       allocate it using malloc() and update cnuBpc_val_ptr_ptr.
 *       <b>DO NOT</b> free the previous pointer.
 *       The MFD helper will release the memory you allocate.
 *
 * @remark If you call this function yourself, you are responsible
 *         for checking if the pointer changed, and freeing any
 *         previously allocated memory. (Not necessary if you pass
 *         in a pointer to static memory, obviously.)
 */
int
cnuBpc_get( cnuTable_rowreq_ctx *rowreq_ctx, char **cnuBpc_val_ptr_ptr, size_t *cnuBpc_val_ptr_len_ptr )
{
   /** we should have a non-NULL pointer and enough storage */
   netsnmp_assert( (NULL != cnuBpc_val_ptr_ptr) && (NULL != *cnuBpc_val_ptr_ptr));
   netsnmp_assert( NULL != cnuBpc_val_ptr_len_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuBpc_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuBpc data.
 * copy (* cnuBpc_val_ptr_ptr ) data and (* cnuBpc_val_ptr_len_ptr ) from rowreq_ctx->data
 */
    /*
     * make sure there is enough space for cnuBpc data
     */
    if ((NULL == (* cnuBpc_val_ptr_ptr )) ||
        ((* cnuBpc_val_ptr_len_ptr ) <
         (rowreq_ctx->data.cnuBpc_len* sizeof(rowreq_ctx->data.cnuBpc[0])))) {
        /*
         * allocate space for cnuBpc data
         */
        (* cnuBpc_val_ptr_ptr ) = malloc(rowreq_ctx->data.cnuBpc_len* sizeof(rowreq_ctx->data.cnuBpc[0]));
        if(NULL == (* cnuBpc_val_ptr_ptr )) {
            snmp_log(LOG_ERR,"could not allocate memory\n");
            return MFD_ERROR;
        }
    }
    (* cnuBpc_val_ptr_len_ptr ) = rowreq_ctx->data.cnuBpc_len* sizeof(rowreq_ctx->data.cnuBpc[0]);
    memcpy( (* cnuBpc_val_ptr_ptr ), rowreq_ctx->data.cnuBpc, rowreq_ctx->data.cnuBpc_len* sizeof(rowreq_ctx->data.cnuBpc[0]) );

    return MFD_SUCCESS;
} /* cnuBpc_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuAttenuation
 * cnuAttenuation is subid 12 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.12
 * Description:
the attenuation between clt and cnu.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  0      hasdefval 0
 *   readable   1     iscolumn 1     ranges 1      hashint   1
 *   settable   0
 *   hint: 255a
 *
 * Ranges:  0 - 16;
 *
 * Its syntax is DisplayString (based on perltype OCTETSTR)
 * The net-snmp type is ASN_OCTET_STR. The C type decl is char (char)
 * This data type requires a length.  (Max 16)
 */
/**
 * Extract the current value of the cnuAttenuation data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuAttenuation_val_ptr_ptr
 *        Pointer to storage for a char variable
 * @param cnuAttenuation_val_ptr_len_ptr
 *        Pointer to a size_t. On entry, it will contain the size (in bytes)
 *        pointed to by cnuAttenuation.
 *        On exit, this value should contain the data size (in bytes).
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
*
 * @note If you need more than (*cnuAttenuation_val_ptr_len_ptr) bytes of memory,
 *       allocate it using malloc() and update cnuAttenuation_val_ptr_ptr.
 *       <b>DO NOT</b> free the previous pointer.
 *       The MFD helper will release the memory you allocate.
 *
 * @remark If you call this function yourself, you are responsible
 *         for checking if the pointer changed, and freeing any
 *         previously allocated memory. (Not necessary if you pass
 *         in a pointer to static memory, obviously.)
 */
int
cnuAttenuation_get( cnuTable_rowreq_ctx *rowreq_ctx, char **cnuAttenuation_val_ptr_ptr, size_t *cnuAttenuation_val_ptr_len_ptr )
{
   /** we should have a non-NULL pointer and enough storage */
   netsnmp_assert( (NULL != cnuAttenuation_val_ptr_ptr) && (NULL != *cnuAttenuation_val_ptr_ptr));
   netsnmp_assert( NULL != cnuAttenuation_val_ptr_len_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuAttenuation_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuAttenuation data.
 * copy (* cnuAttenuation_val_ptr_ptr ) data and (* cnuAttenuation_val_ptr_len_ptr ) from rowreq_ctx->data
 */
    /*
     * make sure there is enough space for cnuAttenuation data
     */
    if ((NULL == (* cnuAttenuation_val_ptr_ptr )) ||
        ((* cnuAttenuation_val_ptr_len_ptr ) <
         (rowreq_ctx->data.cnuAttenuation_len* sizeof(rowreq_ctx->data.cnuAttenuation[0])))) {
        /*
         * allocate space for cnuAttenuation data
         */
        (* cnuAttenuation_val_ptr_ptr ) = malloc(rowreq_ctx->data.cnuAttenuation_len* sizeof(rowreq_ctx->data.cnuAttenuation[0]));
        if(NULL == (* cnuAttenuation_val_ptr_ptr )) {
            snmp_log(LOG_ERR,"could not allocate memory\n");
            return MFD_ERROR;
        }
    }
    (* cnuAttenuation_val_ptr_len_ptr ) = rowreq_ctx->data.cnuAttenuation_len* sizeof(rowreq_ctx->data.cnuAttenuation[0]);
    memcpy( (* cnuAttenuation_val_ptr_ptr ), rowreq_ctx->data.cnuAttenuation, rowreq_ctx->data.cnuAttenuation_len* sizeof(rowreq_ctx->data.cnuAttenuation[0]) );

    return MFD_SUCCESS;
} /* cnuAttenuation_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuAction
 * cnuAction is subid 13 of cnuEntry.
 * Its status is Current, and its access level is ReadWrite.
 * OID: .1.3.6.1.4.1.36186.8.1.1.13
 * Description:
reset or force re-registation the CNU in EoC topology
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  1      hasdefval 0
 *   readable   1     iscolumn 1     ranges 0      hashint   0
 *   settable   1
 *
 * Enum range: 3/8. Values:  cnu_no_action(0), cnu_reset(1), cnu_reload_profile(2), cnu_permit(3), cnu_undo_permit(4), cnu_diagnose(5), cnu_delete(6)
 *
 * Its syntax is CnuRstActValue (based on perltype INTEGER)
 * The net-snmp type is ASN_INTEGER. The C type decl is long (u_long)
 */
/**
 * Extract the current value of the cnuAction data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuAction_val_ptr
 *        Pointer to storage for a long variable
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
 */
int
cnuAction_get( cnuTable_rowreq_ctx *rowreq_ctx, u_long * cnuAction_val_ptr )
{
   /** we should have a non-NULL pointer */
   netsnmp_assert( NULL != cnuAction_val_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuAction_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuAction data.
 * copy (* cnuAction_val_ptr ) from rowreq_ctx->data
 */
    (* cnuAction_val_ptr ) = rowreq_ctx->data.cnuAction;

    return MFD_SUCCESS;
} /* cnuAction_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuSynchronized
 * cnuSynchronized is subid 14 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.14
 * Description:
The CNU profile synchronization flag.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  1      hasdefval 0
 *   readable   1     iscolumn 1     ranges 0      hashint   0
 *   settable   0
 *
 * Enum range: 2/8. Values:  true(1), false(2)
 *
 * Its syntax is TruthValue (based on perltype INTEGER)
 * The net-snmp type is ASN_INTEGER. The C type decl is long (u_long)
 */
/**
 * Extract the current value of the cnuSynchronized data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuSynchronized_val_ptr
 *        Pointer to storage for a long variable
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
 */
int
cnuSynchronized_get( cnuTable_rowreq_ctx *rowreq_ctx, u_long * cnuSynchronized_val_ptr )
{
   /** we should have a non-NULL pointer */
   netsnmp_assert( NULL != cnuSynchronized_val_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuSynchronized_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuSynchronized data.
 * copy (* cnuSynchronized_val_ptr ) from rowreq_ctx->data
 */
    (* cnuSynchronized_val_ptr ) = rowreq_ctx->data.cnuSynchronized;

    return MFD_SUCCESS;
} /* cnuSynchronized_get */

/*---------------------------------------------------------------------
 * prevail-mib::cnuEntry.cnuRowStatus
 * cnuRowStatus is subid 15 of cnuEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.4.1.36186.8.1.1.15
 * Description:
The CNU node entry status of CNU.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  1      hasdefval 0
 *   readable   1     iscolumn 1     ranges 0      hashint   0
 *   settable   0
 *
 * Enum range: 2/8. Values:  true(1), false(2)
 *
 * Its syntax is TruthValue (based on perltype INTEGER)
 * The net-snmp type is ASN_INTEGER. The C type decl is long (u_long)
 */
/**
 * Extract the current value of the cnuRowStatus data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param cnuRowStatus_val_ptr
 *        Pointer to storage for a long variable
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
 */
int
cnuRowStatus_get( cnuTable_rowreq_ctx *rowreq_ctx, u_long * cnuRowStatus_val_ptr )
{
   /** we should have a non-NULL pointer */
   netsnmp_assert( NULL != cnuRowStatus_val_ptr );


    DEBUGMSGTL(("verbose:cnuTable:cnuRowStatus_get","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

/*
 * TODO:231:o: |-> Extract the current value of the cnuRowStatus data.
 * copy (* cnuRowStatus_val_ptr ) from rowreq_ctx->data
 */
    (* cnuRowStatus_val_ptr ) = rowreq_ctx->data.cnuRowStatus;

    return MFD_SUCCESS;
} /* cnuRowStatus_get */



/** @} */
