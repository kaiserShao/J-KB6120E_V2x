/* CH376оƬ Ӳ����׼SPI�������ӵ�Ӳ������� V1.0 */
/* �ṩI/O�ӿ��ӳ��� */

#include	"hal.h"
void	mDelay0_5uS( void )  /* ������ʱ0.5uS,���ݵ�Ƭ����Ƶ���� */
{
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP();
}

void	CH376_PORT_INIT( void )  /* ����ʹ��SPI��дʱ��,���Խ��г�ʼ�� */
{
	INT_IRQ_Enable();
	SPI3_GPIO_Config();
/* �����Ӳ��SPI�ӿ�,��ô��ʹ��mode3(CPOL=1&CPHA=1)��mode0(CPOL=0&CPHA=0),CH376��ʱ�������ز�������,�½������,����λ�Ǹ�λ��ǰ */
	CH376_END_CMD();/* ��ֹSPIƬѡ */
}

UINT8	Spi376Exchange( UINT8 d )  /* Ӳ��SPI���������8��λ���� */
{  
	return SPI3_RW( d );
}

void	xEndCH376Cmd( )	
{ 
	CH376_END_CMD(); 
}  /* SPIƬѡ��Ч,����CH376����,������SPI�ӿڷ�ʽ */

void	xWriteCH376Cmd( UINT8 mCmd )  /* ��CH376д���� */
{
	WriteCH376Cmd( mCmd );	
	mDelay0_5uS( );
	mDelay0_5uS( );
	mDelay0_5uS( );
// #ifdef	CH376_SPI_BZ
// 	UINT8	i;
// #endif
// 	CH376_SPI_SCS = 1;  /* ��ֹ֮ǰδͨ��xEndCH376Cmd��ֹSPIƬѡ */
// /* ����˫��I/O����ģ��SPI�ӿ�,��ô����ȷ���Ѿ�����SPI_SCS,SPI_SCK,SPI_SDIΪ�������,SPI_SDOΪ���뷽�� */
// 	CH376_SPI_SCS = 0;  /* SPIƬѡ��Ч */
// 	Spi376Exchange( mCmd );  /* ���������� */
// #ifdef	CH376_SPI_BZ
// 	for ( i = 30; i != 0 && CH376_SPI_BZ; -- i );  /* SPIæ״̬��ѯ,�ȴ�CH376��æ,��������һ�е���ʱ1.5uS���� */
// #else
//   /* ��ʱ1.5uSȷ����д���ڴ���1.5uS,����������һ�е�״̬��ѯ���� */
// #endif
}

#ifdef	FOR_LOW_SPEED_MCU  /* ����Ҫ��ʱ */
#define	xWriteCH376Data( d )	{ WriteCH376Data( d ); }  /* ��CH376д���� */
#define	xReadCH376Data( )		(  ReadCH376Data( void ); )  /* ��CH376������ */
#else
void	xWriteCH376Data( UINT8 mData )  /* ��CH376д���� */
{
	WriteCH376Data( mData );
	mDelay0_5uS( );  /* ȷ����д���ڴ���0.6uS */
}
UINT8	xReadCH376Data( void )  /* ��CH376������ */
{
	mDelay0_5uS( );  /* ȷ����д���ڴ���0.6uS */
	return( ReadCH376Data( ) );
}
#endif
/* ��ѯCH376�ж�(INT#�͵�ƽ) */
UINT8	Query376Interrupt( void )
{
	return ( CH376QueryInterrupt() );
}

UINT8	mInitCH376Host( UINT8 USBSelect )  /* ��ʼ��CH376 */
{
	UINT8	res;
	CH376_PORT_INIT( );  /* �ӿ�Ӳ����ʼ�� */
	xWriteCH376Cmd( CMD11_CHECK_EXIST );  /* ���Ե�Ƭ����CH376֮���ͨѶ�ӿ� */
	xWriteCH376Data( 0xaa );
	res = xReadCH376Data( );
	xEndCH376Cmd( );
	if ( res != 0x55 ) return( ERR_USB_UNKNOWN );  /* ͨѶ�ӿڲ�����,����ԭ����:�ӿ������쳣,�����豸Ӱ��(Ƭѡ��Ψһ),���ڲ�����,һֱ�ڸ�λ,���񲻹��� */
	xWriteCH376Cmd( CMD11_SET_USB_MODE );  /* �豸USB����ģʽ */
	xWriteCH376Data( USBSelect );  ///06  U�� 03 SD��
	mDelayuS( 20 );
	res = xReadCH376Data( );
	xEndCH376Cmd( );
#ifndef	CH376_INT_WIRE
#ifdef	CH376_SPI_SDO
	xWriteCH376Cmd( CMD20_SET_SDO_INT );  /* ����SPI��SDO���ŵ��жϷ�ʽ */
	xWriteCH376Data( 0x16 );
	xWriteCH376Data( 0x90 );  /* SDO������SCSƬѡ��Чʱ�����ж�������� */
	xEndCH376Cmd( );
#endif
#endif
	if ( res == CMD_RET_SUCCESS ) return( USB_INT_SUCCESS );
	else return( ERR_USB_UNKNOWN );  /* ����ģʽ���� */
}