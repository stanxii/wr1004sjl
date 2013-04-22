#include <dbsapi.h>
#include <boardapi.h>
#include "reg_dbs.h"
#include "register.h"

static uint8_t save_flag = 0;

/* 与DBS  通讯的设备文件*/
T_DBS_DEV_INFO *dbsdev = NULL;

T_DBS_DEV_INFO * reg_dbsOpen(void)
{
	return dbsOpen(MID_REGISTER);
}

int reg_dbsClose(void)
{
	return dbsClose(dbsdev);
}

int db_init_clt(int index)
{
	st_dbsClt clt;

	if( CMM_SUCCESS != dbsGetClt(dbsdev, index, &clt))
	{
		return CMM_FAILED;
	}
	else
	{
		clt.col_numStas = 0;
		clt.col_sts = DEV_STS_OFFLINE;
		return dbsUpdateClt(dbsdev, index, &clt);
	}
}

int db_init_cnu(int index)
{
	st_dbsCnu cnu;
	
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, index, &cnu))
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(cnu.col_att, "0 db");
		strcpy(cnu.col_bpc, "0%");
		cnu.col_rx = 0;
		strcpy(cnu.col_snr, "0%");
		cnu.col_sts = DEV_STS_OFFLINE;
		cnu.col_tx = 0;
		return dbsUpdateCnu(dbsdev, index, &cnu);
	}	
}

int db_init_all(void)
{
	int i = 0;
	
	for( i=0; i<MAX_CLT_AMOUNT_LIMIT; i++ )
	{
		if( CMM_SUCCESS != db_init_clt(i+1) )
		{
			return CMM_FAILED;
		}
	}
	for( i=0; i<(MAX_CLT_AMOUNT_LIMIT*MAX_CNU_AMOUNT_LIMIT); i++ )
	{
		if( CMM_SUCCESS != db_init_cnu(i+1))
		{
			return CMM_FAILED;
		}
	}
	return CMM_SUCCESS;
}

int db_get_clt(int index, st_dbsClt *clt)
{
	return dbsGetClt(dbsdev, index, clt);
}

int db_get_cnu(int index, st_dbsCnu *cnu)
{
	return dbsGetCnu(dbsdev, index, cnu);
}

int db_update_clt(int index, st_dbsClt *clt)
{
	return dbsUpdateClt(dbsdev, index, clt);
}

int db_update_cnu(int index, st_dbsCnu *cnu)
{
	return dbsUpdateCnu(dbsdev, index, cnu);
}

int db_unregister_clt(int index)
{
	return db_init_clt(index);
}

int db_unregister_cnu(int index)
{
	return db_init_cnu(index);
}

int db_new_cnu(int index, st_dbsCnu *cnu)
{
	int status = 0;
	if( 0 == db_get_anonymous_access_sts(&status) )
	{
		if( 0 == status )
		{
			switch(cnu->col_model)
			{
				case WEC701_C2:
				case WEC701_C4:
				{
					if( 0 == dbsCreateDeblProfileForWec701Cnu(dbsdev, index) )
					{
						return dbsUpdateCnu(dbsdev, index, cnu);
					}
					break;
				}
				default:
				{
					if( 0 == dbsCreateDeblProfileForCnu(dbsdev, index) )
					{
						return dbsUpdateCnu(dbsdev, index, cnu);
					}
					break;
				}
			}			
		}
		else
		{
			switch(cnu->col_model)
			{
				case WEC701_C2:
				case WEC701_C4:
				{
					if( 0 == dbsCreateDewlProfileForWec701Cnu(dbsdev, index) )
					{
						return dbsUpdateCnu(dbsdev, index, cnu);
					}
					break;
				}
				default:
				{
					if( 0 == dbsCreateDewlProfileForCnu(dbsdev, index) )
					{
						return dbsUpdateCnu(dbsdev, index, cnu);
					}
					break;
				}
			}
		}
	}
	return CMM_FAILED;
}

int db_new_su(int index, st_dbsCnu *cnu)
{
	switch(cnu->col_model)
	{
		case WEC701_C2:
		case WEC701_C4:
		{
			if( 0 == dbsCreateSuProfileForWec701Cnu(dbsdev, index) )
			{
				return dbsUpdateCnu(dbsdev, index, cnu);
			}
			break;
		}
		default:
		{
			if( 0 == dbsCreateSuProfileForCnu(dbsdev, index) )
			{
				return dbsUpdateCnu(dbsdev, index, cnu);
			}
			break;
		}
	}
	return CMM_FAILED;
}

int db_delete_cnu(int index)
{
	if( 0 == dbsDestroyRowCnu(dbsdev, index) )
	{
		if( 0 == dbsDestroyRowProfile(dbsdev, index) )
		{
			return CMM_SUCCESS;
		}
	}
	return CMM_FAILED;
}

int db_get_user_type(uint32_t clt_index, uint32_t cnu_index, uint32_t *userType)
{
	DB_INTEGER_V st_iValue;

	st_iValue.ci.tbl = DBS_SYS_TBL_ID_CNU;
	st_iValue.ci.row = (clt_index-1)*MAX_CNU_AMOUNT_LIMIT+cnu_index;
	st_iValue.ci.col = DBS_SYS_TBL_CNU_COL_ID_AUTH;
	st_iValue.ci.colType = DBS_INTEGER;

	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &st_iValue))
	{
		printf("db_get_user_type(%d, %d, %d) : CMM_FAILED", st_iValue.ci.tbl, st_iValue.ci.row, st_iValue.ci.col);
		return CMM_FAILED;
	}
	else
	{
		if( DBS_NULL == st_iValue.ci.colType )
		{
			*userType = 0;
		}
		else
		{
			*userType = st_iValue.integer;
		}
		return CMM_SUCCESS;
	}
}

int db_get_user_onused(uint32_t clt_index, uint32_t cnu_index, uint32_t *onUsed)
{
	DB_INTEGER_V st_iValue;

	st_iValue.ci.tbl = DBS_SYS_TBL_ID_CNU;
	st_iValue.ci.row = (clt_index-1)*MAX_CNU_AMOUNT_LIMIT+cnu_index;
	st_iValue.ci.col = DBS_SYS_TBL_CNU_COL_ID_ROWSTS;
	st_iValue.ci.colType = DBS_INTEGER;

	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &st_iValue))
	{
		printf("db_get_user_onused(%d, %d, %d) : CMM_FAILED", st_iValue.ci.tbl, st_iValue.ci.row, st_iValue.ci.col);
		return CMM_FAILED;
	}
	else
	{
		if( DBS_NULL == st_iValue.ci.colType )
		{
			*onUsed = 0;
		}
		else
		{
			*onUsed = st_iValue.integer;
		}
		return CMM_SUCCESS;
	}
}

int db_get_anonymous_access_sts(uint32_t *anonyAccSts)
{
	DB_INTEGER_V st_iValue;

	st_iValue.ci.tbl = DBS_SYS_TBL_ID_SYSINFO;
	st_iValue.ci.row = 1;
	st_iValue.ci.col = DBS_SYS_TBL_SYSINFO_COL_ID_WLCTL;
	st_iValue.ci.colType = DBS_INTEGER;

	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &st_iValue))
	{
		printf("db_get_anonymous_access_sts(%d, %d, %d) : CMM_FAILED", st_iValue.ci.tbl, st_iValue.ci.row, st_iValue.ci.col);
		return CMM_FAILED;
	}
	else
	{
		if( DBS_NULL == st_iValue.ci.colType )
		{
			*anonyAccSts = 0;
		}
		else
		{
			*anonyAccSts = (st_iValue.integer?BOOL_FALSE:BOOL_TRUE);
		}
		return CMM_SUCCESS;
	}	
}

int db_get_user_access_sts(uint32_t clt_index, uint32_t cnu_index, uint32_t *userAccSts)
{
	*userAccSts = 1;
	return CMM_SUCCESS;
}

int db_get_auto_config_sts(uint32_t *autoCfgSts)
{
	DB_INTEGER_V st_iValue;

	st_iValue.ci.tbl = DBS_SYS_TBL_ID_SYSINFO;
	st_iValue.ci.row = 1;
	st_iValue.ci.col = DBS_SYS_TBL_SYSINFO_COL_ID_AC;
	st_iValue.ci.colType = DBS_INTEGER;

	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &st_iValue))
	{
		printf("db_get_auto_config_sts(%d, %d, %d) : CMM_FAILED", st_iValue.ci.tbl, st_iValue.ci.row, st_iValue.ci.col);
		return CMM_FAILED;
	}
	else
	{
		if( DBS_NULL == st_iValue.ci.colType )
		{
			*autoCfgSts = 0;
		}
		else
		{
			*autoCfgSts = st_iValue.integer;
		}
		return CMM_SUCCESS;
	}
}

int db_get_user_auto_config_sts(uint32_t clt_index, uint32_t cnu_index, uint32_t *userAutoCfgSts)
{
	*userAutoCfgSts = 1;
	return CMM_SUCCESS;
}

int db_init_nelib(T_TOPOLOGY_INFO *this)
{
	int i = 0;
	st_dbsClt clt;
	st_dbsCnu cnu;

	/* 获取CLT的信息,这里假设只有1个CLT线卡，否则逻辑需要重新设计*/
	for( i=0; i<MAX_CLT_AMOUNT_LIMIT; i++ )
	{
		if( CMM_SUCCESS != db_get_clt(i+1, &clt) )
		{
			return CMM_DB_ACCESS_ERROR;
		}
		else
		{
			this->tb_clt.DevType = clt.col_model;
			this->tb_clt.online = ((clt.col_sts)?BOOL_TRUE:BOOL_FALSE);
			this->tb_clt.NumStas = clt.col_numStas;
			this->tb_clt.MaxStas = clt.col_maxStas;
			if( 0 != boardapi_macs2b(clt.col_mac, this->tb_clt.Mac) )
			{
				return CMM_FAILED;
			}
		}
	}
	
	/* 获取CNU列表信息*/
	for( i=0; i<(MAX_CLT_AMOUNT_LIMIT*MAX_CNU_AMOUNT_LIMIT); i++)
	{
		if( CMM_SUCCESS == db_get_cnu(i+1, &cnu) )
		{
			this->tb_cnu[i].DevType = cnu.col_model;
			if( 0 != boardapi_macs2b(cnu.col_mac, this->tb_cnu[i].Mac) )
			{
				return CMM_FAILED;
			}
			this->tb_cnu[i].online = ((cnu.col_sts)?BOOL_TRUE:BOOL_FALSE);
			this->tb_cnu[i].RxRate = cnu.col_rx;
			this->tb_cnu[i].TxRate = cnu.col_tx;
			this->tb_cnu[i].OnUsed = ((cnu.col_row_sts)?BOOL_TRUE:BOOL_FALSE);
		}
		else
		{
			return CMM_DB_ACCESS_ERROR;
		}
	}	
	return CMM_SUCCESS;
}

/* 将DB数据由内存数据库写入flash 数据库*/
int db_fflush(void)
{
	save_flag = 1;
	return CMM_SUCCESS;
}

int db_real_fflush(void)
{
	if(save_flag)
	{
		if( 0 == dbsFflush(dbsdev) )
		{
			save_flag = 0;
			return CMM_SUCCESS;
		}
		else
		{
			return CMM_DB_ACCESS_ERROR;
		}
	}
	return CMM_SUCCESS;
}

