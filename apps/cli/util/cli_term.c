/****************************************************************************
 文件名  : CLI_Term.c
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述:  1、实现命令行操作终端的任务管理
            2、终端任务数据中的字符串编辑缓冲的处理
 备注   :
 函数列表:
            略
 修改历史:
        1. 修改者   :  may2250
           时间     :
           版本     :
           修改原因 :
        2. ...
 ****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "cli_private.h"
#include "cli_term.h"
#include "cli_io.h"
#include "cli_interpret.h"
#include "../cli_comm.h"
#include <public.h>
#include <dbsapi.h>

ST_CLI_TERM m_stCliTerm;

static struct termios stored_settings;
ULONG  CLI_SendToTerm ( )
{
    m_stCliTerm.iSendLen = (int   )STB_StrLen(m_stCliTerm.szSendBuf);
    if (m_stCliTerm.iSendLen == 0
     || m_stCliTerm.nTermStatus == TERM_SLEEPING)
        return TBS_SUCCESS;

    printf("%s", m_stCliTerm.szSendBuf);

    m_stCliTerm.iSendLen = 0 ;
    m_stCliTerm.szSendBuf[ 0 ] = '\0' ;
    return TBS_SUCCESS ;
}

int Cli_StdGetCh()
{
    return getchar();
}

/*********************************************************************/
/* 函数名称 : CLI_SendToTerm()                                       */
/* 函数功能 : 根据终端选择输入函数                                   */
/* 返回     : 成功、失败                                             */
/*********************************************************************/
ULONG  CLI_ReceiveFromTerm(ULONG  ulSecond)
{
    ULONG  ulRet = TBS_FAILED;

    switch (m_stCliTerm.nTermType)
    {
        case CLI_TERM_FOR_SERIAL:
            if (ulSecond == 0)
                break;

            m_stCliTerm.szRecvBuf[0] = Cli_StdGetCh();
            m_stCliTerm.iRecvLen = 1 ;
            ulRet = TBS_SUCCESS ;
            break;

        case CLI_TERMT_FOR_MML:
            if (m_stCliTerm.szRecvBuf[0] != '\0')
                ulRet = TBS_SUCCESS;
            else
            {
                CLI_DELAY(20);
                ulRet = TBS_FAILED;
            }
            break;

        case CLI_TERM_NOT_INITIAL:
            break;

        default:
            break;
    }

    if (m_stCliTerm.ulTimeLeft == 0 || m_stCliTerm.nTermStatus == TERM_SLEEPING)
    {
        return TBS_FAILED;
    }

    if (ulRet == TBS_SUCCESS)
    {
        m_stCliTerm.ulTimeLeft = DEADLINE_SHORT;
    }

    m_stCliTerm.ulDispLineCount = 0;

    return ulRet;
}

/*********************************************************************/
/* 函数名称 : CLI_TermTaskProc()                                     */
/* 函数功能 : 终端任务处理核心函数                                   */
/* 输入参数 : pTermStruct:终端任务数据                                */
/* 输出参数 : 终端任务数据                                           */
/* 返回     : 无                                                     */
/* 上层函数 : 终端任务函数体                                         */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
_VOID    CLI_TermTaskProc()
{
    if (!IS_TERM_NORMAL(m_stCliTerm))
        return ;

     /*【1】从终端接收字符*/
     m_stCliTerm.iEditStatus = COMMAND_INPUT;
    if (CLI_ReceiveFromTerm(m_stCliTerm.ulTimeLeft) != TBS_SUCCESS)
        return;

    if (!IS_TERM_NORMAL(m_stCliTerm))
    {
        DBG_Trace("\r\n CLI_TermTaskProc::Received but Term Abnormal!");
        return;
    }

    /* 【2】处理命令缓冲区*/
    if (CLI_CmdBufProcess() != TBS_SUCCESS)
        return;

    /* 【3】执行完整接收的命令*/
    ////CLI_ComStatusStart();
    CLI_CmdExecute();
    ////CLI_ComStatusEnd();

    if (!IS_TERM_NORMAL(m_stCliTerm))
    {
        DBG_Trace("\r\n CLI_TermTaskProc::Cmd executed but Term Abnormal!");
        return;
    }

    /* 【4】命令记录与环境清除*/
    CLI_CmdRecord();

    CLI_DisplayPrompt();

    return;
}

/*********************************************************************/
/* 函数名称 : CLI_TermTask()                                         */
/* 函数功能 : 串口任务函数体                                         */
/* 创建者   : liuzequn    */
/* 修改者   : may2250                                                  */
/* 修改记录 :                                                        */
/*********************************************************************/
_VOID    CLI_TermTask( ULONG  ulArg1, ULONG  ulArg2)
{
    m_stCliTerm.nTermType = CLI_TERM_FOR_SERIAL;
    int logcount=1;
	
    /* 向CMM 模块查询用户记录*/
	#if 0
    int i=0;
    T_szUserst userst;
    bzero((unsigned char *)&userst, sizeof(T_szUserst));
    if( msg_get_userinfo(&userst) != TBS_SUCCESS )
    {
	    return;
    }
    #endif
	
    CLI_TermLogin();

    for ( ; ; )
    {
        while(m_stCliTerm.nTermStatus < TERM_LOGED || m_stCliTerm.ulTimeLeft == 0)
        {
            logcount++;
	     CLI_TermDataInit();
            CLI_TermLogout();
	     if(logcount>3)
		 	return;
            m_stCliTerm.nTermType = CLI_TERM_FOR_SERIAL;
            CLI_TermLogin();
        }
        CLI_TermTaskProc();

        if (!IS_TERM_NORMAL(m_stCliTerm))
            return;
    }
}

/*********************************************************************/
/* 函数名称 : CLI_PeriodTask()                                       */
/* 函数功能 : 命令行内部定时任务                                     */
/*********************************************************************/
_VOID    CLI_PeriodProc(_VOID   )
{
    if (m_stCliTerm.ulTimeLeft == 1)
    {
        DBG_Out( "r\n CLI: Term Timeout!");
    }

    if (m_stCliTerm.ulTimeLeft > 0)
    {
        m_stCliTerm.ulTimeLeft--;
    }

    return;
}


/*********************************************************************/
/* 函数名称 : CLI_TermLogin()                                        */
/* 函数功能 : 终端登录函数入口                                       */
/* 输入参数 :                                                        */
/* 输出参数 : 终端任务数据。该函数中登录成功将改变任务的运行状态，同 */
/*            时改变所登录的用户信息的状态                           */
/* 返回     : 无                                                     */
/* 上层函数 : 终端任务执行主函数体                                   */
/*********************************************************************/
_VOID    CLI_TermLogin(void)
{
	//char   szUsername[USERNAME_LEN] = UNLOGON_USER_NAME;
	char loginUser[16] = {0};
	char   szPassword[USERNAME_LEN] = "";
	ULONG  ulFailCount;

	int i = 0;
	st_dbsCliRoles roles;
	st_dbsCliRole role;

	for( i=1; i<=3; i++ )
	{
		if( 0 == dbsGetCliRole(i, &role) )
		{
			roles.role[i-1].id = role.id;
			strcpy(roles.role[i-1].col_user, role.col_user);
			strcpy(roles.role[i-1].col_pwd, role.col_pwd);
		}
		else
		{
			fprintf(stderr, "ERROR: CLI_TermLogin->dbsGetCliRole[%d] !\n", i);
			return;
		}
	}
    
	/* 因为在用户名与密码接收过程中有状态的判断,*/
	/* 故此处先置一个允许正常进行的状态         */
	CLI_SetTermStatus(TERM_ACTIVE);
	m_stCliTerm.ulTimeLeft   = DEADLINE_SHORT;
	ulFailCount = 0;

    for (; ;)
    {
    	 
	 //while(IO_GetCharByPeek() !='\0')
	 //	;
	 //printf("\nUsername:");
	 STB_StrCpy(m_stCliTerm.szSendBuf,"\nUsername:");
	 CLI_SendToTerm();
	 if (IO_GetString((char *)&loginUser,16,FALSE) != TBS_SUCCESS) 
                return; 

	 if(strcmp(loginUser," admin")==0)
	{
		STB_StrCpy(m_stCliTerm.szSendBuf,"\nPassword:");
	 	CLI_SendToTerm();
		m_stCliTerm.szInputCmd[0] = '\0';
		if (IO_GetString((char *)&szPassword,16,TRUE) != TBS_SUCCESS) 
                	return; 
		//printf("\nThe entered string is:%s\n",szPassword);
		if(strcmp(szPassword, roles.role[0].col_pwd)==0)
		{
			/* 成功登录后的一些用户与终端数据初始化 */
        		CLI_SetTermStatus(TERM_LOGED);
       		 m_stCliTerm.ucUserLevel  = 2;
        		m_stCliWS.ulLevel = 2;
		 	m_stCliWS.ulCurrentMode=1;
        		STB_StrCpy(m_stCliTerm.szCurUserName, loginUser);
			
        		CLI_DisplayPrompt();
			m_stCliTerm.szInputCmd[0] = '\0';
            		m_stCliWS.ulStatus |= CLI_STAT_ENV_NEWLINE;
        		return;
		}
		else
	 	{
			printf("\npassword error!\n");
			return;
	 	}
	 }
	 else if(strcmp(loginUser," operator")==0)
	 {
	 	//printf("\nPassword: ");
		STB_StrCpy(m_stCliTerm.szSendBuf,"\nPassword:");
	 	CLI_SendToTerm();
		m_stCliTerm.szInputCmd[0] = '\0';
		if (IO_GetString((char *)&szPassword,16,TRUE) != TBS_SUCCESS) 
                	return; 
		//printf("\nThe entered string is:%s\n",szPassword);
		if(strcmp(szPassword, roles.role[1].col_pwd)==0)
		{
			/* 成功登录后的一些用户与终端数据初始化 */
        		CLI_SetTermStatus(TERM_LOGED);
       		 m_stCliTerm.ucUserLevel  = 1;
        		m_stCliWS.ulLevel = 1;
			m_stCliWS.ulCurrentMode=1;
        		STB_StrCpy(m_stCliTerm.szCurUserName, loginUser);

        		CLI_DisplayPrompt();
			m_stCliTerm.szInputCmd[0] = '\0';
            		m_stCliWS.ulStatus |= CLI_STAT_ENV_NEWLINE;
        		return;
		}
		else
	 	{
			printf("\npassword error!\n");
			return;
	 	}
	 }
	 else if(strcmp(loginUser," user")==0)
	 {
	 	//printf("\nPassword: ");
		STB_StrCpy(m_stCliTerm.szSendBuf,"\nPassword:");
	 	CLI_SendToTerm();
		m_stCliTerm.szInputCmd[0] = '\0';
		if (IO_GetString((char *)&szPassword,16,TRUE) != TBS_SUCCESS) 
                	return; 
		//printf("\nThe entered string is:%s\n",szPassword);
		if(strcmp(szPassword, roles.role[2].col_pwd)==0)
		{
			/* 成功登录后的一些用户与终端数据初始化 */
        		CLI_SetTermStatus(TERM_LOGED);
       		 m_stCliTerm.ucUserLevel  = 0;
        		m_stCliWS.ulLevel = 0;
			m_stCliWS.ulCurrentMode=1;
        		STB_StrCpy(m_stCliTerm.szCurUserName, loginUser);

        		CLI_DisplayPrompt();
			m_stCliTerm.szInputCmd[0] = '\0';
            		m_stCliWS.ulStatus |= CLI_STAT_ENV_NEWLINE;
        		return;
		}
		else
	 	{
			printf("\npassword error!\n");
			return;
	 	}
	 }
	 else
	 {
		printf("\n no such username! Please enter (admin operator or user)...\n");
	 }
	 return;
	 
    }
}

/*********************************************************************/
/* 函数名称 : CLI_TermLogout()                                       */
/* 函数功能 : 终端注销函数入口                                       */
/* 返回     : 无                                                     */
/* 上层函数 : 终端任务执行主函数体                                   */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
_VOID    CLI_TermLogout()
{
    /* 分两种情况考虑: 1、未成功登录的注销; 2、登录后的注销  */
    /* 两种情况由用户名是否为初始化的用户名来判定            */
    if (!STB_StriCmp(m_stCliTerm.szCurUserName, UNLOGON_USER_NAME))
    {
        if (m_stCliTerm.nTermType == CLI_TERM_FOR_SERIAL)
            return ;
    }
    else
    {
    }

    /* 将该终端任务返回到根模式 */
    CLI_ReturnRootMode();

    IO_Print("\nLogout");
    //CLI_DELAY(100);
    //CLI_CommDestroy();
    CLI_TermDataInit();
    CLI_ResetEnviroment();
    CLI_SetTermStatus(TERM_SLEEPING);
    
    return;
}


/*********************************************************************/
/* 函数名称 : CLI_ReturnRootMode()                                   */
/* 函数功能 : 将当前终端任务返回到根模式                             */
/* 返回     : 成功、失败                                             */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_ReturnRootMode()
{
    CLI_SetWSStatus((ULONG )CLI_STAT_ENV_RUNCFG) ;

    if (m_stCliWS.ulCurrentMode != CTM_GENL)
    {
        STB_StrCpy(m_stCliTerm.szCommandBuf, CMD_CM "root") ;
        CLI_CmdExecute() ;
    }
    CLI_SetWSStatus(0) ;
    return TBS_SUCCESS;
}

/*==================================================================*/
/*      函数名      :CLI_SetTermStatus                              */
/*      函数功能    :设置终端状态                                   */
/*      输入参数    :nTermStatus 设置的状态                        */
/*      返回值      :无                                             */
/*      被调函数    :                                               */
/*==================================================================*/
_VOID    CLI_SetTermStatus(UCHAR nTermStatus)
{
/*    char  szInfo[TELNET_MAX_STATUS][15] =
            {"SLEEPING","ACTIVE","LOGED","EXEC-CMD","WAIT-RESPONSE"};
*/

    DBG_Trace(
        "\r\n CLI: Term status terned from [%s] to [%s]",
        szInfo[m_stCliTerm.nTermStatus], szInfo[nTermStatus]);

    if (m_stCliTerm.nTermStatus == TERM_WAIT_RESPONSE && nTermStatus == TERM_LOGED)
    {
        m_stCliTerm.nTermStatus = nTermStatus;
        CLI_DisplayPrompt();
        return;
    }

    m_stCliTerm.nTermStatus = nTermStatus;
    return ;
}


/* 字符串编辑部分 */

/*==================================================================*/
/*      函数名      :CLI_EditString                                 */
/*      函数功能    :扫描接收缓冲区,并编辑字符串编辑缓冲区          */
/*      返回值      :ULONG 0:  接收到一完整命令行并准备提交该命令   */
/*                         1:  只接收到一字符并准备继续接收字符     */
/*      调用函数    :CLI_CharProcess ( )                            */
/*      被调函数    :                                               */
/*==================================================================*/
ULONG  CLI_EditString( )
{
    ULONG   ulTmp ;
    UCHAR   ucTmp ;

    /* 接收到的字符串的处理 */
    if ( m_stCliTerm.nTermType == CLI_TERM_FOR_SERIAL )
    {
        for ( ulTmp = 0 ; ulTmp < (ULONG )m_stCliTerm.iRecvLen ; ulTmp ++ )
        {
            if (m_stCliTerm.szRecvBuf[ ulTmp ] == '\0')
                return TBS_FAILED ;
            ucTmp = (UCHAR)m_stCliTerm.szRecvBuf[ ulTmp ] ;

            if ( ! CLI_CharProcess ( ucTmp ) )
                return TBS_SUCCESS ;
        }
    }
    return TBS_FAILED ;
}

/*==================================================================*/
/*      函数名      :CLI_CharProcess                                */
/*      函数功能    :对单个字符进行处理包括删除,插入,特殊字符处理等 */
/*      输入参数    :UCHAR ucTmp  待处理的字符                      */
/*      返回值      :ULONG 0:  接收到一完整命令行并准备提交该命令   */
/*                         1:  只接收到一字符并准备继续接收字符     */
/*==================================================================*/
ULONG  CLI_CharProcess ( UCHAR ucTmp )
{
    //BEGIN of 新增处理箭头符号功能
    if (m_stCliTerm.ucInputStatus == KEY_ESC)
    {
        if (ucTmp == '[')
        {
            m_stCliTerm.ucInputStatus = '[';
            return TBS_FAILED;
        }
        else if (ucTmp == KEY_ESC)
        {
            ucTmp = KEY_CTRL_C;
        }
        else
            m_stCliTerm.ucInputStatus = '\0';
    }
    else if (m_stCliTerm.ucInputStatus == '[')
    {
        switch(ucTmp)
        {
            case 'a':
            case 'A':
                ucTmp = KEY_MOVE_P;
                break;
            case 'b':
            case 'B':
                ucTmp = KEY_MOVE_N;
                break;
            case 'd':
            case 'D':
                ucTmp = KEY_MOVE_L;
                break;
            case 'c':
            case 'C':
                ucTmp = KEY_MOVE_R;
                break;
            default:
                break;
        }
        m_stCliTerm.ucInputStatus = '\0';
    }
    //END of 新增处理箭头符号功能

    if (ucTmp > 0x20 && ucTmp <= 0x7E
     && ucTmp != KEY_MOVE_L && ucTmp != KEY_MOVE_R
     && ucTmp != KEY_MOVE_P && ucTmp != KEY_MOVE_N)
    {
        CLI_CharInsert ( (char )ucTmp ) ;
        if (m_stCliTerm.iEditStatus != STRING_INPUT && (ucTmp == '?' ))
            return TBS_SUCCESS ;
    }
    else
    {
        switch (ucTmp)
        {
            case KEY_CTRL_C:   /* 终止字符 */
            case KEY_CTRL_Z:
                m_stCliTerm.szEditBuf[0] = (char )ucTmp ;
                m_stCliTerm.szEditBuf[1] = '\0' ;
                m_stCliTerm.iCurrentPos = 0 ;
                return TBS_SUCCESS ;

            case KEY_ESC:
                m_stCliTerm.ucInputStatus = KEY_ESC;
                break;

            case KEY_ENTER: /* 回车换行符*/
            case KEY_RETURN:
                /* 如果是命令编辑状态，需要进行命令输入完整的相应处理 */
                if(m_stCliTerm.iEditStatus != STRING_INPUT)
                {
                    CLI_DealWitchCommand();
                }
                m_stCliTerm.iCurrentPos = 0 ;
                return TBS_SUCCESS ;

            case KEY_SPACE: /* 空格 */
                /* 如果是命令编辑状态，进行命令的自动联想补全 */
                if(m_stCliTerm.iEditStatus != STRING_INPUT)
                {
                    CLI_PatchCmdWord();
                }
                else   /* 否则直接插入空格 */
                {
                    CLI_CharInsert (' ');
                }
                break;
            case KEY_TABLE:
                /* 如果是TABLE键，酌情插入空格 */
                if(m_stCliTerm.iEditStatus != STRING_INPUT)
                {
                    if (ISSPACEBAR(m_stCliTerm.szEditBuf[m_stCliTerm.iCurrentPos-1]))
                    {
                        CLI_CharInsert('?');
                        return TBS_SUCCESS;
                    }
                    else if (m_stCliTerm.iCurrentPos == (int   )STB_StrLen(m_stCliTerm.szEditBuf))
                    {
                        CLI_PatchCmdWord();
                    }
                    else
                    {
                        CLI_CharInsert (' ');
                    }
                }
                else   /* 否则直接插入空格 */
                {
                    CLI_CharInsert ('\t');
                }
                break;

            /* 以下为编辑控制字符的处理*/
            case KEY_MOVE_L:
                /* 光标在行首,不能再左移,警告 */
                if ( m_stCliTerm.iCurrentPos == 0
                 || (m_stCliTerm.szInputCmd[0] != 0 && m_stCliTerm.iCurrentPos <= (int   )strlen(m_stCliTerm.szInputCmd)))
                {
                    CLI_Bell ( ) ;
                }
                else
                {
                    int    iLentgh = strlen(m_stCliTerm.szEditBuf);

                    // 如果是命令编辑状态，左移时删除最后一个空格，减少
                    // 用户的操作失误(如果光标不在最后，解释器将拒绝联想)
                    if (m_stCliTerm.iCurrentPos == iLentgh
                     && m_stCliTerm.szEditBuf[iLentgh - 1] == 0x20
                     && m_stCliTerm.iEditStatus != STRING_INPUT)
                    {
                        CLI_DealWithBackSpace();
                    }
                    else
                    {
                        /* 光标左移一格 */
                        m_stCliTerm.iCurrentPos -- ;
                        CLI_MoveToCurrentPos ( (ULONG )m_stCliTerm.iCurrentPos + 1 ) ;
                    }
                }
                break;
            case KEY_MOVE_R:
                if ( (ULONG )m_stCliTerm.iCurrentPos < (ULONG )STB_StrLen ( m_stCliTerm.szEditBuf ) )
                {
                    /* 将光标右移一格 */
                    m_stCliTerm.iCurrentPos ++ ;
                    CLI_MoveToCurrentPos ( (ULONG )m_stCliTerm.iCurrentPos - 1 ) ;
                }
                /* 光标在行首末,警告 */
                else
                    CLI_Bell () ;
                break;
            case KEY_CTRL_W:
            case KEY_MOVE_P: //DOSKEY功能上一条命令
                // 命令的交互部分输入时也不允许进行DOSKEY功能
                if ( m_stCliTerm.iEditStatus == COMMAND_INPUT )
                {
                    CLI_PrevCmdDisp( );
                }
                else   /*非命令编辑状态不能进行DOSKEY功能*/
                {
                    CLI_Bell();
                }
                break;
            case KEY_CTRL_S:
            case KEY_MOVE_N: //DOSKEY功能下一条命令
                // 命令的交互部分输入时也不允许进行DOSKEY功能
                if ( m_stCliTerm.iEditStatus == COMMAND_INPUT )
                {
                    CLI_NextCmdDisp( );
                }
                else   /*非命令编辑状态不能进行DOSKEY功能*/
                {
                    CLI_Bell();
                }
                break;
            default:
                CLI_EditSpecialKey ( (char )ucTmp ) ;
                break;

        }
    }
    return TBS_FAILED;

}


/*==================================================================*/
/*      函数名      :CLI_CharInsert                                 */
/*      函数功能    :在字符串编辑缓冲的当前光标位置插入一字符       */
/*      输入参数    :char  ch  待插入的字符                          */
/*      返回值      :无                                             */
/*      调用函数    :CLI_MoveToCurrentPos ( )                       */
/*      被调函数    :                                               */
/*==================================================================*/
_VOID    CLI_CharInsert( char  ch )
{
    int     iTmp ;

    if ( (ULONG )STB_StrLen( m_stCliTerm.szEditBuf ) + 1 >= (ULONG )m_stCliTerm.iMaxLen )
    {
        DBG_Out( "\r\n 编辑缓冲濒临溢出.");
        CLI_Bell();
        return ;
    }


    /* 将光标后的字符向右移动一格,为待插入的字符留出插入空位*/
    iTmp = (int   )STB_StrLen ( m_stCliTerm.szEditBuf ) ;
    if (m_stCliTerm.szEditBuf[iTmp -1] == '?')
        return;

    while ( iTmp >= m_stCliTerm.iCurrentPos )
    {
        m_stCliTerm.szEditBuf[ iTmp + 1 ]
            = m_stCliTerm.szEditBuf[ iTmp ] ;
        iTmp-- ;
    }

    /* 在当前光标位置出入该字符 */
    m_stCliTerm.szEditBuf[ m_stCliTerm.iCurrentPos ] = ch ;

    /* 控制用户端的屏幕显示,刷新当前光标以后的显示字符,             */
    /* 即刷新发送缓冲区                                             */
    if (m_stCliTerm.nTermType != CLI_TERMT_FOR_MML)  // 对透传命令不作回显
    {
        m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ] = ch ;
    }
    if ( (ULONG )STB_StrLen( m_stCliTerm.szEditBuf ) - 1 > (ULONG )m_stCliTerm.iCurrentPos )
    {
        iTmp = m_stCliTerm.iCurrentPos + 1 ;
        while ( m_stCliTerm.szEditBuf[ iTmp ] )
            m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ]
                = m_stCliTerm.szEditBuf[ iTmp ++ ] ;
    }
    m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ] = '\0' ;

    /* 光标位置右移一格 */
    m_stCliTerm.iCurrentPos ++ ;

    /* 将光标从行末移到当前光标所在位置  */
    if ( (ULONG )STB_StrLen( m_stCliTerm.szEditBuf ) > (ULONG )m_stCliTerm.iCurrentPos )
        CLI_MoveToCurrentPos ( STB_StrLen ( m_stCliTerm.szEditBuf ) ) ;

    return ;
}

/*==================================================================*/
/*      函数名      :CLI_EditSpecialKey                             */
/*      函数功能    :对接收到的特殊字符进行编辑处理                 */
/*      输入参数    :char  ch  待处理的特殊字符                      */
/*      返回值      :无                                             */
/*      被调函数    :                                               */
/*==================================================================*/
_VOID    CLI_EditSpecialKey( char  ch )
{
    if ( ISESC( ch ) )
    {
        CLI_Bell ( ) ;
    }

    else if ( ISBACKSPACE( ch ) )
    {
        /*--------------------------------------------------*/
        /* 如果用户输入了Backspace键,则对之进行处理         */
        /*--------------------------------------------------*/
        CLI_DealWithBackSpace ( ) ;
    }

    else if ( ISDELETE( ch ) )
    {
        /*--------------------------------------------------*/
        /* 如果用户输入了Delete键,则对之进行处理            */
        /*--------------------------------------------------*/
        CLI_DealWithDelete ( ) ;

    }

    else if ( ISTAB( ch ) )
    {
        CLI_Bell ( ) ;
    }

    else if ( ISCTRLCHAR( ch ) )
    {
        CLI_CharInsert ( '^' ) ;
        CLI_CharInsert ( (char )(ISCTRLCHAR( ch )) ) ;
    }
    else
    {
        //CLI_Bell ( ) ;
    }
    return ;
}



/*==================================================================*/
/*      函数名      :CLI_DealWithBackSpace                          */
/*      函数功能    :对Backspace键的处理                            */
/*      返回值      :无                                             */
/*      调用函数    :CLI_MoveToCurrentPos ( )                       */
/*      被调函数    :                                               */
/*==================================================================*/
_VOID    CLI_DealWithBackSpace ( )
{
    ULONG  ulTmp ;


    /*--------------------------------------------------------------*/
    /* 光标前无字符可删除,则告警                                    */
    /*--------------------------------------------------------------*/
    if ( m_stCliTerm.iCurrentPos == 0
     || (m_stCliTerm.szInputCmd[0] != 0 && m_stCliTerm.iCurrentPos <= (int   )strlen(m_stCliTerm.szInputCmd)))
    {
        CLI_Bell ( ) ;
        return;
    }


    /* 终端类型是vt100的处理 */
    {
        m_stCliTerm.iCurrentPos -- ;
        CLI_MoveToCurrentPos ( (ULONG )m_stCliTerm.iCurrentPos + 1 ) ;
        ulTmp = (ULONG )m_stCliTerm.iCurrentPos ;
        /* 处理编辑缓冲区删除光标前的一字符,并刷新发送缓冲区    */
        do
        {
            if ( m_stCliTerm.szEditBuf[ ulTmp + 1 ] )
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ]
                    = m_stCliTerm.szEditBuf[ ulTmp + 1 ] ;

            else
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ] = ' ' ;

            m_stCliTerm.szEditBuf[ ulTmp ]
                = m_stCliTerm.szEditBuf[ ulTmp + 1 ] ;
            ulTmp ++ ;

        } while ( m_stCliTerm.szEditBuf[ ulTmp - 1 ] ) ;
        m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ] = '\0' ;
        /*------------------------------------------------------*/
        /* 重新显示当前光标后的字符后,刷新光标位置              */
        /*------------------------------------------------------*/
        CLI_MoveToCurrentPos ( STB_StrLen ( m_stCliTerm.szEditBuf ) + 1 ) ;

    }
    return ;
}


/*==================================================================*/
/*      函数名      :CLI_DealWithDelete                              */
/*      函数功能    :对Delete键的处理                               */
/*      返回值      :无                                             */
/*      调用函数    :CLI_MoveToCurrentPos ( )                       */
/*      被调函数    :                                               */
/*==================================================================*/
_VOID    CLI_DealWithDelete ( )
{
    ULONG  ulTmp ;

    /* 光标后有字符可以删除  */
    if ( m_stCliTerm.szEditBuf[ m_stCliTerm.iCurrentPos ] )
    {
        ulTmp = (ULONG )m_stCliTerm.iCurrentPos ;
        /* 处理编辑缓冲区删除光标后的一字符,并刷新发送缓冲区    */
        do
        {
            if ( m_stCliTerm.szEditBuf[ ulTmp + 1 ] )
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ]
                    = m_stCliTerm.szEditBuf[ ulTmp + 1 ] ;
            else
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ]
                    = ' ' ;

            m_stCliTerm.szEditBuf[ ulTmp ]
                = m_stCliTerm.szEditBuf[ ulTmp + 1 ] ;
            ulTmp ++ ;

        } while ( m_stCliTerm.szEditBuf[ ulTmp - 1 ] ) ;
        m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ] = '\0' ;

        CLI_MoveToCurrentPos ( STB_StrLen ( m_stCliTerm.szEditBuf ) + 1 ) ;

    }
    /* 光标后无字符可以删除,则警告  */
    else
        CLI_Bell ( ) ;

    return ;
}


/*==================================================================*/
/*      函数名      :CLI_PrevCmdDisp                                */
/*      函数功能    :DOSKEY功能的上一条命令自动获取                 */
/*      返回值      :无                                             */
/*      调用函数    :CLI_MoveToCurrentPos ( )                       */
/*      被调函数    :                                               */
/*==================================================================*/
_VOID    CLI_PrevCmdDisp ( )
{
    ULONG  ulTmp ;

    /* 该操作需要历史缓冲中存在命令 */
    if ( m_stCliTerm.iHistoryPos > 0 )
    {
        /* 将光标左移到屏幕显示的第一列上  */
        if ( m_stCliTerm.iCurrentPos )
        {
            ulTmp = (ULONG )m_stCliTerm.iCurrentPos ;
            m_stCliTerm.iCurrentPos = 0 ;
            CLI_MoveToCurrentPos ( ulTmp ) ;
        }
        /* 清除用户的当前输入 */
        if ( STB_StrLen ( m_stCliTerm.szEditBuf ) )
        {
            m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
            ulTmp = 0 ;
            while ( m_stCliTerm.szEditBuf[ ulTmp ++ ] )
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ] = ' ' ;
            m_stCliTerm.szSendBuf [ m_stCliTerm.iSendLen ] = '\0' ;

            CLI_MoveToCurrentPos ( STB_StrLen ( m_stCliTerm.szEditBuf ) ) ;
        }

        /* 用当前命令前的历史命令刷新编辑缓冲区  */
        STB_StrCpy ( m_stCliTerm.szEditBuf,
                 m_stCliTerm.szHistory[ m_stCliTerm.iHistoryPos - 1 ] ) ;
        m_stCliTerm.iHistoryPos -- ;

        /* 重新显示新的命令 */
        STB_StrCat ( m_stCliTerm.szSendBuf, m_stCliTerm.szEditBuf ) ;
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;

        /* 刷新当前的光标位置 */
        m_stCliTerm.iCurrentPos = (int   )STB_StrLen ( m_stCliTerm.szEditBuf ) ;

    }
    /* 当前命令之前无历史命令,鸣警 */
    else
        CLI_Bell ( ) ;

    return ;
}


/*==================================================================*/
/*      函数名      :CLI_NextCmdDisp                                */
/*      函数功能    :DOSKEY功能的下一条命令自动获取                 */
/*      返回值      :无                                             */
/*      调用函数    :CLI_MoveToCurrentPos ( )                       */
/*      被调函数    :                                               */
/*==================================================================*/
_VOID    CLI_NextCmdDisp ( )
{
    ULONG  ulTmp ;

    /* 该操作需要历史缓冲中存在命令 */
    if (    m_stCliTerm.iHistoryPos + 1 < HISTORY_SIZE
         && m_stCliTerm.szHistory[ m_stCliTerm.iHistoryPos + 1 ][0] )
    {

        /* 将光标左移到屏幕显示的第一列上  */
        if ( m_stCliTerm.iCurrentPos )
        {
            ulTmp = (ULONG )m_stCliTerm.iCurrentPos ;
            m_stCliTerm.iCurrentPos = 0 ;
            CLI_MoveToCurrentPos ( ulTmp ) ;
        }

        /* 清除用户的当前输入 */
        if ( STB_StrLen ( m_stCliTerm.szEditBuf ) )
        {
            m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
            ulTmp = 0 ;
            while ( m_stCliTerm.szEditBuf[ ulTmp ++ ] )
                m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ] = ' ' ;
            m_stCliTerm.szSendBuf [ m_stCliTerm.iSendLen ] = '\0' ;

            CLI_MoveToCurrentPos ( STB_StrLen ( m_stCliTerm.szEditBuf ) ) ;
        }

        /* 用当前命令后的历史命令刷新编辑缓冲区  */
        STB_StrCpy (m_stCliTerm.szEditBuf,
                m_stCliTerm.szHistory[ m_stCliTerm.iHistoryPos + 1 ] ) ;
        m_stCliTerm.iHistoryPos ++ ;

        /* 重新显示新的命令 */
        if ( STB_StrLen ( m_stCliTerm.szEditBuf ) )
        {
            STB_StrCat ( m_stCliTerm.szSendBuf, m_stCliTerm.szEditBuf ) ;
            m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
        }

        /* 刷新当前的光标位置 */
        m_stCliTerm.iCurrentPos = (int   )STB_StrLen ( m_stCliTerm.szEditBuf ) ;

    }
    /* 当前命令之后无历史命令,鸣警 */
    else
        CLI_Bell ( ) ;

    return ;
}

/*==================================================================*/
/*      函数名      :CLI_MoveToCurrentPos                           */
/*      函数功能    :将光标移到当前位置                             */
/*      输入参数    :ULONG  iPos 光标移动的起始位置                    */
/*      返回值      :无                                             */
/*      调用函数    :无                                             */
/*      被调函数    :                                               */
/*==================================================================*/
_VOID    CLI_MoveToCurrentPos ( ULONG  iPos )
{
    int     iTmp ;
    char  szTmp[4] ;
	int nInputCmdLen = STB_StrLen(m_stCliTerm.szInputCmd);

    /* 将光标上下移动 */
    iTmp = ((int)iPos + m_stCliTerm.iPromptLen - nInputCmdLen) / 80
           - (m_stCliTerm.iCurrentPos + m_stCliTerm.iPromptLen - nInputCmdLen) / 80 ;
    /* 将光标向上移动 */
    if ( iTmp > 0)
    {
        STB_StrCat( m_stCliTerm.szSendBuf, "\x1b[" ) ;
        STB_Sprintf ( szTmp, "%d", iTmp ) ;
        STB_StrCat(m_stCliTerm.szSendBuf, szTmp ) ;
        STB_StrCat(m_stCliTerm.szSendBuf, "A" ) ;
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    }
    /* 将光标向下移动   */
    else if ( iTmp < 0 )
    {
        STB_StrCat ( m_stCliTerm.szSendBuf, "\x1b[" ) ;
        STB_Sprintf ( szTmp, "%d", 0 - iTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, szTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, "B" ) ;
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    }

    /* 将光标左右移动  */
    iTmp = ( (int   )iPos + m_stCliTerm.iPromptLen - nInputCmdLen) % 80
        - ( m_stCliTerm.iCurrentPos + m_stCliTerm.iPromptLen -nInputCmdLen ) % 80 ;

    /* 将光标向左移动 */
    if ( iTmp > 0 )
    {
        STB_StrCat ( m_stCliTerm.szSendBuf, "\x1b[" ) ;
        STB_Sprintf ( szTmp, "%d", iTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, szTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, "D" ) ;
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    }
    /* 将光标向右移动   */
    else if ( iTmp < 0 )
    {
        STB_StrCat ( m_stCliTerm.szSendBuf, "\x1b[" ) ;
        STB_Sprintf ( szTmp, "%d", 0 - iTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, szTmp ) ;
        STB_StrCat ( m_stCliTerm.szSendBuf, "C" ) ;
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    }
    return ;
}


/*==================================================================*/
/*      函数名      :CLI_Bell                                       */
/*      函数功能    :如果接收到的字符不能处理,则警告                */
/*      返回值      :无                                             */
/*      调用函数    :无                                             */
/*      被调函数    :                                               */
/*==================================================================*/
_VOID    CLI_Bell ( )
{
//    m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ++ ] = 0x07 ;
//    m_stCliTerm.szSendBuf[ m_stCliTerm.iSendLen ] = '\0' ;
    return ;
}


/*==================================================================*/
/*      函数名      :CLI_PatchCommand                               */
/*      函数功能    :将不完整命令粘贴完整                           */
/*      返回值      :ULONG TBS_FAILED 失败 TBS_SUCCESS  成功       */
/*      被调函数    :                                               */
/*==================================================================*/
ULONG  CLI_PatchCommand()
{
    char  *s8Patch;
    ULONG  ulRet = TBS_FAILED;

    s8Patch = CLI_GetCommandPatch( (ULONG )m_stCliTerm.iCurrentPos );

    while(*s8Patch)
    {
        if (((ULONG )STB_StrLen(m_stCliTerm.szEditBuf) + 1) < (ULONG )m_stCliTerm.iMaxLen)
        {
            CLI_CharInsert (*(s8Patch++));

            if(*(s8Patch - 1) == '?')
                ulRet = TBS_SUCCESS ;
        }
        else
        {
            DBG_Out(
                    "\r\n Edit buffer overflow");
            return ulRet;
        }
    }

    return ulRet;
}

/*==================================================================*/
/*      函数名      :CLI_PatchCmdWord                               */
/*      函数功能    :补全用户输入的命令字                           */
/*      返回值      :ULONG  TBS_SUCCESS  成功                       */
/*      被调函数    :                                               */
/*==================================================================*/
ULONG  CLI_PatchCmdWord()
{
    ULONG  ulTmp;
    char  *pacNewInput;

    ulTmp = (ULONG )m_stCliTerm.iCurrentPos;
    if ( ulTmp != 0 )
    {
        if ( !ISSPACEBAR(m_stCliTerm.szEditBuf[ulTmp-1])
            && (ulTmp == STB_StrLen(m_stCliTerm.szEditBuf)) )
        {
            CLI_SetCommandString( m_stCliTerm.szEditBuf);
            CLI_SetWSStatus(CLI_STAT_ENV_SILENT);

            if ((m_stCliWS.ulStatus & CLI_STAT_ENV_NEWLINE) != 0)
            {
                //如果是在交互状态下，且已经输入过空格，则当前期望输入的已经不是交互提示的参数
                pacNewInput = m_stCliTerm.szEditBuf + strlen(m_stCliTerm.szInputCmd);
                if (strchr(pacNewInput, 0x20) != 0)
                {
                    m_stCliWS.ulExpectEle = 0;
                }
            }

            CLI_IntpKernelProc();

            CLI_SetWSStatus(0);
            CLI_PatchCommand( );
            return TBS_SUCCESS;
        }
    }

    return TBS_SUCCESS;
}

/*==================================================================*/
/*      函数名      :CLI_DealWitchCommand                           */
/*      函数功能    :处理输入结束的命令                             */
/*      返回值      :ULONG  TBS_SUCCESS  成功                       */
/*      被调函数    :                                               */
/*==================================================================*/
ULONG  CLI_DealWitchCommand()
{
    char  *pacNewInput;

    //运行到此处，必定是用户按下回车的结果，此时为让系统进入最后
    //一个命令字的联想，强制将光标移至最后，此处不需要对光标显示
    //进行处理，因为马上要结束当前的行编辑
    m_stCliTerm.iCurrentPos = STB_StrLen(m_stCliTerm.szEditBuf);

    if (m_stCliTerm.iCurrentPos != 0) //如果有效输入为空，就不要浪费时间了
    {
        CLI_SetCommandString( m_stCliTerm.szEditBuf);
        CLI_SetWSStatus(CLI_STAT_ENV_SILENT);
        m_stCliWS.ulStatus |= CLI_STAT_ENV_ENTER ;

        if ((m_stCliWS.ulStatus & CLI_STAT_ENV_NEWLINE) != 0)
        {
            //如果是在交互状态下，且已经输入过空格，则当前期望输入的已经不是交互提示的参数
            pacNewInput = m_stCliTerm.szEditBuf + strlen(m_stCliTerm.szInputCmd);
            if (strchr(pacNewInput, 0x20) != 0)
            {
                m_stCliWS.ulExpectEle = 0;
            }
        }
        CLI_IntpKernelProc();

        CLI_SetWSStatus(0);
        CLI_PatchCommand( );
    }

    return TBS_SUCCESS;

}

/*==============================================================*/
/*      函数名      :CLI_CmdBufProcess                          */
/*      函数功能    :对命令串缓冲进行处理                       */
/*      返回值      :ULONG  0:  接收到一完整命令行              */
/*                          1:  只接收到一字符                  */
/*      调用函数    :CLI_EditString ( )                         */
/*                   CLI_SendToTerm ( )                         */
/*                   CLI_MoveToCurrentPos ( )                   */
/*      被调函数    :                                           */
/*==============================================================*/
ULONG  CLI_CmdBufProcess (  )
{
    ULONG  ulTmp ;

    /*----------------------------------------------------------*/
    /* 编辑命令字符串                                           */
    /*----------------------------------------------------------*/
    ulTmp = CLI_EditString () ;

    /*----------------------------------------------------------*/
    /* 回显接收到的字符串                                       */
    /*----------------------------------------------------------*/
    if(m_stCliTerm.bRecurCmd)
    {
        m_stCliTerm.bRecurCmd = FALSE;
        if ( m_stCliTerm.iSendLen )
        {
            CLI_DisplayPrompt() ;

            /* 移动到光标当前位置 */
            CLI_MoveToCurrentPos ( (ULONG )STB_StrLen(m_stCliTerm.szEditBuf) ) ;
            CLI_SendToTerm (  ) ;
        }
    }
    else if ( m_stCliTerm.iSendLen )
    {
        CLI_SendToTerm ( ) ;
    }

    /*----------------------------------------------------------*/
    /* 接收到一完整的命令行                                     */
    /*----------------------------------------------------------*/
    if ( ! ulTmp )
    {
        /*------------------------------------------------------*/
        /* 将光标移动到命令行的末尾                             */
        /*------------------------------------------------------*/
        ulTmp = (ULONG )m_stCliTerm.iCurrentPos ;
        m_stCliTerm.iCurrentPos = (int   )STB_StrLen ( m_stCliTerm.szEditBuf ) ;
        if (m_stCliTerm.nTermType != CLI_TERMT_FOR_MML)
            CLI_MoveToCurrentPos ( ulTmp ) ;

        /*------------------------------------------------------*/
        /* 将命令提交给命令缓冲区,并清空命令编辑缓冲区          */
        /*------------------------------------------------------*/
        STB_StrCpy ( m_stCliTerm.szCommandBuf, m_stCliTerm.szEditBuf ) ;
        m_stCliTerm.szEditBuf[0] = '\0' ;
        m_stCliTerm.iCurrentPos = 0 ;
        if (m_stCliTerm.nTermType == CLI_TERMT_FOR_MML)
        {
            m_stCliTerm.szRecvBuf[0] = '\0';
            m_stCliTerm.iRecvLen = 0;
        }

        return TBS_SUCCESS ;
    }

    return TBS_FAILED ;
}

/*==============================================================*/
/*      函数名      :CLI_DisplayPrompt                           */
/*      函数功能    :显示提示符                                 */
/*      调用函数    :CLI_SendToTerm ( )                         */
/*      被调函数    :                                           */
/*==============================================================*/
_VOID    CLI_DisplayPrompt ( )
{
    char  szNewLine[] = "\r\n \r\n" ;
    char  *szPrompt;
    char  *szPortID;
    char  szHostName[ 80 ] = "";
    char  szUsrname[USERNAME_LEN]="";
    char  szTmp[USERNAME_LEN]="";
	
    if (m_stCliTerm.nTermType == CLI_TERMT_FOR_MML || m_stCliTerm.nTermStatus == TERM_WAIT_RESPONSE)
        return ;


    STB_StrCpy ( szHostName, m_szHostName) ;

    szHostName[HOSTNAME_LENGTH - 1] = 0;

    /* 当截取到的主机名以中文字符结束时，做以下处理 */
    if ((UCHAR)szHostName[HOSTNAME_LENGTH - 2] > 127)
    {
        int i,CnNum = 0;
        for (i = 0; i < HOSTNAME_LENGTH - 2; i++)
        {
            /*统计中文字符长度*/
            if ((UCHAR)szHostName[i] > 127)
            {
                CnNum++;
            }
        }
        if(CnNum % 2 == 0)
        {
            szHostName[HOSTNAME_LENGTH - 2] = '\0';
        }
    }

    CLI_SendToTerm( ) ;
    szPrompt = CLI_GetCurrentPrompt();
    STB_StrCat(m_stCliTerm.szSendBuf,szNewLine) ;
    STB_StrCat(m_stCliTerm.szSendBuf,szHostName);
    memcpy(szTmp,m_stCliTerm.szCurUserName+1,USERNAME_LEN-1);

    sprintf(szUsrname,">>%s",szTmp);
    STB_StrCat(m_stCliTerm.szSendBuf,szUsrname);
    if (NULL != szPrompt)
        STB_StrCat ( m_stCliTerm.szSendBuf, szPrompt);

    szPortID = CLI_GetModeParameter(m_stCliWS.ulCurrentMode,2);
    if (NULL != szPortID)
    {
        STB_Sprintf(m_stCliTerm.szCommandBuf, m_stCliTerm.szSendBuf, szPortID);
        STB_StrCpy(m_stCliTerm.szSendBuf,m_stCliTerm.szCommandBuf) ;
        m_stCliTerm.szCommandBuf[0] = '\0';
    }

    m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    m_stCliTerm.iPromptLen = m_stCliTerm.iSendLen - (int   )STB_StrLen(szNewLine) ;

    DBG_ASSERT(m_stCliTerm.iSendLen < 80);

    /* 回显 ? 前的命令字 */
    if(m_stCliTerm.szEditBuf[0] != '\0')
    {
        STB_StrCat(m_stCliTerm.szSendBuf, m_stCliTerm.szEditBuf);
        m_stCliTerm.iSendLen = (int   )STB_StrLen ( m_stCliTerm.szSendBuf ) ;
    }

    CLI_SendToTerm( ) ;

    return ;
}

/*********************************************************************/
/* 函数名称 : CLI_SetPromptLen()                                      */
/* 函数功能 : 设置当前提示符的长度                                   */
/*********************************************************************/
ULONG  CLI_SetPromptLen(ULONG  ulStrLen)
{
    m_stCliTerm.iPromptLen = (int   )ulStrLen ;
    return TBS_SUCCESS ;
}

/*-----------------------------------------------------------------------------
 函数名称    : CLI_GetTermLevel();
 功能        : 获得当前用户的用户权限
 输出参数    : 无.
 返回值      : 用户操作级别, 枚举CLI_OPERAT_LEVEL_T
 函数调用说明:
 典型使用示例:
-----------------------------------------------------------------------------*/
ULONG  CLI_GetTermLevel( )
{
    DBG_ASSERT( m_stCliTerm.ucUserLevel < CLI_AL_NULL );

    return m_stCliTerm.ucUserLevel;
}


/*********************************************************************/
/* 函数名称 : CLI_GetHostName()                                      */
/* 函数功能 : 对外提供的主机名获取函数                               */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     : 主机名字符串                                           */
/*********************************************************************/
BOOL CLI_IsTermBusy()
{
    if (m_stCliTerm.nTermStatus > TERM_LOGED)
        return TRUE;
    return FALSE;
}
/*==================================================================*/
/*      函数名      :CLI_GetTermData                                */
/*      函数功能    :获取当前任务的任务数据                         */
/*      输入参数    :ULONG  ulPid  进程号                           */
/*                                                                  */
/*      输出参数    :无                                             */
/*      返回值      :PTerm_Data_S 任务数据指针                      */
/*      调用函数    :CLI_GetTaskData ( )                            */
/*      被调函数    :                                               */
/*==================================================================*/
ST_CLI_TERM* CLI_GetTermData ( _VOID    )
{
    return &m_stCliTerm;
}


/*==================================================================*/
/*      函数名      :CLI_TermInit                                   */
/*      函数功能    :初始化终端                                     */
/*      备注        :环境变量的初始化不在该函数中进行               */
/*==================================================================*/
_VOID    CLI_TermDataInit( )
{
    ULONG          ulTmp;

    /* 初始化任务数据   */
    m_stCliTerm.nTermStatus = TERM_SLEEPING;
    m_stCliTerm.szRecvBuf[0]    = '\0' ;
    m_stCliTerm.iRecvLen        = 0 ;
    m_stCliTerm.szSendBuf[0]    = '\0' ;
    m_stCliTerm.iSendLen        = 0 ;
    for ( ulTmp = 0 ; ulTmp < HISTORY_SIZE ; ulTmp ++ )
        m_stCliTerm.szHistory[ulTmp][0] = '\0' ;
    m_stCliTerm.iHistoryPos     = 0 ;
    m_stCliTerm.szEditBuf[0]    = '\0' ;
    m_stCliTerm.iCurrentPos     = 0 ;
    m_stCliTerm.iPromptLen      = 1 ;
    m_stCliTerm.szCommandBuf[0] = '\0' ;
    m_stCliTerm.szInputCmd[0] = '\0';
    m_stCliTerm.iEditStatus     = COMMAND_INPUT;
    m_stCliTerm.iMaxLen         = CLI_MAX_CMDBUF;
    m_stCliTerm.ulDispLineCount = 0;
    STB_StrCpy (m_stCliTerm.szCurUserName, UNLOGON_USER_NAME );
    m_stCliTerm.ucUserLevel     = 0 ;
    m_stCliTerm.ucScroll        = SCROLL_MANUAL;
    m_stCliTerm.ulTimeLeft      = DEADLINE_LONG;
    m_stCliTerm.bRecurCmd      = FALSE ;
    m_stCliTerm.bCommandExecOk = FALSE ;
    m_stCliTerm.ucInputStatus   = 0;


    /* 初始化telnet 任务数据参数  */
    m_stCliTerm.nTermType = CLI_TERM_NOT_INITIAL;

{
     struct termios new_settings;
     tcgetattr(0,&stored_settings);
     new_settings = stored_settings;
     new_settings.c_lflag &= (~ECHO);
     /* Disable canonical mode, and set buffer size to 1 byte */
     new_settings.c_lflag &= (~ICANON);
     new_settings.c_cc[VTIME] = 0;
     new_settings.c_cc[VMIN] = 1;
     tcsetattr(0,TCSANOW,&new_settings);
}

    return ;
}

_VOID CLI_CmdRecord ( )
{
    ULONG  ulTmp ;
    char   szCommandBuf[CLI_MAX_CMDBUF] ;

    /* 去掉多余空格 */
    _TrimString( szCommandBuf, m_stCliTerm.szCommandBuf ) ;

    /* 如果命令缓冲区非空则保存该命令到历史命令缓冲区中         */
    if ( szCommandBuf[0] == '\0'
        || szCommandBuf[0] == KEY_CTRL_Z
        || szCommandBuf[0] == KEY_CTRL_C
        || strchr (szCommandBuf, '?') != NULL)
    {
        if (szCommandBuf[STB_StrLen(szCommandBuf) - 1] == '?')
        {
             szCommandBuf[STB_StrLen(szCommandBuf) - 1] = '\0' ;
             STB_StrCpy ( m_stCliTerm.szEditBuf, szCommandBuf ) ;
             m_stCliTerm.iCurrentPos = (int)STB_StrLen (m_stCliTerm.szEditBuf) ;
        }

        m_stCliTerm.bCommandExecOk = FALSE ;
        m_stCliTerm.szCommandBuf[0] = '\0' ;
        m_stCliWS.szCmdString[0] = '\0';
        return ;
    }

    /* 搜寻第一个空闲的历史命令缓冲区                       */
    for ( ulTmp = 0 ; ulTmp < HISTORY_SIZE ; ulTmp ++ )
    {
        if ( ! ( m_stCliTerm.szHistory[ulTmp][0] ) )
            break ;
    }
    m_stCliTerm.iHistoryPos = (int   )ulTmp;

    if (ulTmp == 0 || STB_StriCmp(szCommandBuf, m_stCliTerm.szHistory[ulTmp - 1]))
    {

        /* 如果历史命令缓冲区已满,则将最早的历史命令清除        */
        if ( ulTmp == HISTORY_SIZE )
        {
            for ( ulTmp = 1 ; ulTmp < HISTORY_SIZE ; ulTmp ++ )
                STB_StrCpy ( m_stCliTerm.szHistory[ ulTmp - 1 ],
                         m_stCliTerm.szHistory[ ulTmp ] ) ;
            ulTmp = HISTORY_SIZE - 1 ;
        }

        /* 将当前命令写入到找到的空闲历史命令缓冲区中去         */
        STB_StrCpy ( m_stCliTerm.szHistory[ ulTmp ], szCommandBuf ) ;
        m_stCliTerm.iHistoryPos = (int   )ulTmp + 1 ;
    }

    m_stCliTerm.bCommandExecOk = FALSE ;

    /* 清空命令缓冲区,等待接收下一个命令                        */
    m_stCliTerm.szCommandBuf[0] = '\0' ;
    m_stCliWS.szCmdString[0] = '\0';

    return ;
}



/*==================================================================*/
/*      函数名      :CLI_TermDestroy                                */
/*      函数功能    :销毁终端                                       */
/*==================================================================*/
_VOID    CLI_TermDestroy( )
{
     stored_settings.c_lflag |= ECHO;
     /* Disable canonical mode, and set buffer size to 1 byte */
     stored_settings.c_lflag |= ICANON;
     stored_settings.c_cc[VTIME] = 1;
     stored_settings.c_cc[VMIN] = 1;
     tcsetattr(0,TCSANOW,&stored_settings);
}

/* CTRL _ C 什么事情都不作 */
void CLI_ProcForCtrlC(int n)
{
    m_stCliTerm.szEditBuf[0] = '\0' ;
    m_stCliTerm.iCurrentPos = 0 ;

    CLI_DisplayPrompt();

    fflush(0);
}



#ifdef __cplusplus
}
#endif


