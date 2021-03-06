/**************** (C) COPYRIGHT 2014 青岛金仕达电子科技有限公司 ****************
* 文 件 名: USBCopy.C
* 创 建 者: Kaiser
* 描  述  : 文件拷贝 ( U 盘 )
* 最后修改: 2015/07/30
*********************************** 修订记录 ***********************************
* 版  本:
* 修订人:
*******************************************************************************/
#include "USBCopy.h"
#include "SD_USBPort.h"
//代码折叠edit-outlining-show
BOOL	USBSave( const char * BUF_Name, uint32_t address, uint8_t *buffer, uint32_t count )
{
	BOOL	state;

	state = SecSave( BUF_Name ,  (uint8_t *)buffer ,  address, count );

	return	state;
}
/***/
BOOL	USBLoad(const char * BUF_Name, uint32_t address, uint8_t * buffer, uint32_t count )
{
	BOOL	state;

	state = SecLoad( BUF_Name , address, buffer, count );

	return	state;
}
// /**********************************************************************************/
BOOL USB_PrintInit(void)
{
	BOOL Flag = TRUE;
	cls();

	switch( USB_Init() )
	{
		case 0:
			Flag = TRUE;
			break;
		case 1:
			MsgBox( "硬件故障 !", vbOKOnly ); //ch376故障
			Flag = FALSE;
			break;
		case 2:
			MsgBox( "U盘未连接 !", vbOKOnly );
			Flag = FALSE;
			break;
		case 3:
			MsgBox( "连接已断开 !", vbOKOnly );//接触不良
			Flag = FALSE;
			break;
		case 4:
			Flag = TRUE;
			break;
		case 5:
			MsgBox( "未知故障 !", vbOKOnly );
			Flag = FALSE;
			break;
	}

	if( Flag )
		USB_File_Creat();
	else
		SD_Init();

	return Flag;
}
/***************************************************************************/
BOOL USBPrint_TSP( uint16_t FileNum, struct uFile_TSP_SHI_R24  const * pFile )
{
	struct	tm	t_tm;
	time_t	now;
	CHAR	sbuffer[512];
	CHAR	sbuffert[40];
	static	uint32_t s;
	s = ByteGetSize("\\SAMPLER\\TSP") / 512 ;
	memset( sbuffer, 0x00, 512 );
	sprintf( sbuffert, "\r\n%s型 %s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "    粉尘采样记录\r\n");
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "文件:%4u [第%2u次/共%2u次]\r\n", FileNum, pFile->run_loops, pFile->set_loops );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	(void)_localtime_r ( &pFile->sample_begin, &t_tm );
	sprintf( sbuffert, "开始时间:   %d月%d日 %02d:%02d\r\n", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样时间:    %02u:%02u\r\n", pFile->set_time / 60u, pFile->set_time % 60u );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样流量:    %5.1f L/m\r\n", pFile->set_flow * 0.1f );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "累计采样时间:    %02u:%02u\r\n", pFile->sum_min / 60u, pFile->sum_min % 60u );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "采样体积(工): %8.1f L\r\n", pFile->vd  );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "采样体积(标): %8.1f L\r\n", pFile->vnd );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均流量(工):    %5.1f L/m\r\n", (FP32)pFile->vd  / pFile->sum_min );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均流量(标):    %5.1f L/m\r\n", (FP32)pFile->vnd / pFile->sum_min );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均计前温度:   %6.2f ℃\r\n", pFile->sum_tr / pFile->sum_min );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均计前压力:   %6.2f kPa\r\n", pFile->sum_pr / pFile->sum_min );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均大气压力:   %6.2f kPa\r\n", pFile->sum_Ba / pFile->sum_min );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	now = get_Now( );
	(void)_localtime_r ( &now, &t_tm );
	sprintf( sbuffert, "  == %4d/%2d/%2d %02d:%02d:%02d ==\r\n",
	         t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	delay_us(1);

	if( !USBSave("\\SAMPLER\\TSP", s + 1, ( uint8_t *)sbuffer, ( sizeof( sbuffer ) + 511 ) / 512 ) )
	{
		SD_Init();
		return FALSE;
	}

	SD_Init();
	MsgBox( "OK! 请拔出U盘 !", vbOKOnly );
	return TRUE;
}
/************************************************************************/
char	* const	TableSampler[] =
{
	"    粉 尘采样记录\r\n",
	"    日均A采样记录\r\n",
	"    日均B采样记录\r\n",
	"    时均C采样记录\r\n",
	"    时均D采样记录\r\n",
};

BOOL USBPrint_R24_SHI( enum enumSamplerSelect SamplerSelect, uint16_t FileNum, struct uFile_TSP_SHI_R24 const * pFile )
{
	struct	tm	t_tm;
	time_t	now;
	static	uint32_t s;
	CHAR	sbuffer[512];
	CHAR	sbuffert[40];

	s = ByteGetSize( Save_NameChar[SamplerSelect] ) / 512;	//	选择文件
	memset( sbuffer, 0x00, 512 );
	sprintf( sbuffert, "\r\n%s型 %s\r\n",szTypeIdent[Configure.InstrumentType],szNameIdent[Configure.InstrumentName] );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, TableSampler[SamplerSelect]);				//	显示类型
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "文件:%3u [第%2u次/共%2u次]\r\n", FileNum, pFile->run_loops, pFile->set_loops );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	(void)_localtime_r ( &pFile->sample_begin, &t_tm );
	sprintf( sbuffert, "开始时间:%d月%d日 %02d:%02d\r\n", t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样时间:  %02u:%02u\r\n",	pFile->set_time / 60u, pFile->set_time % 60u );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "设置采样流量:  %5.3f L/m\r\n",	pFile->set_flow * 0.001f );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "累计采样时间:  %02u:%02u\r\n",	pFile->sum_min / 60u, pFile->sum_min % 60u );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "采样体积(标):%7.2f L\r\n",		pFile->vnd );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均流量(标):  %5.3f L/m\r\n",	(FP32)pFile->vnd / pFile->sum_min );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均计前温度: %6.2f ℃\r\n",	pFile->sum_tr / pFile->sum_min );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均计前压力: %6.2f kPa\r\n",	pFile->sum_pr / pFile->sum_min );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	sprintf( sbuffert, "平均大气压力: %6.2f kPa\r\n",	pFile->sum_Ba / pFile->sum_min );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	now = get_Now();
	(void)_localtime_r ( &now, &t_tm );
	sprintf( sbuffert, "  == %4d/%2d/%2d %02d:%02d:%02d == \r\n",
	         t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday, t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec );
	strcat( sbuffer, sbuffert );
	memset( sbuffert, 0x00, 40 );
	delay_us(1);

	if( !USBSave( Save_NameChar[SamplerSelect], s + 1, ( uint8_t *)sbuffer, ( sizeof( sbuffer ) + 511 ) / 512 ) )
	{
		SD_Init();
		return FALSE;
	}

	SD_Init();
	MsgBox( "OK! 请拔出U盘 !", vbOKOnly );
	return TRUE;
}
// /**********************************************************************/

/********  (C) COPYRIGHT 2015 青岛金仕达电子科技有限公司  **** End Of File **********/

