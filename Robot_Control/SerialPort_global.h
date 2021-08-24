/**
 * @file SerialPort_global.h
 * @author itas109 (itas109@gmail.com) \n\n
 * Blog : http://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 129518033
 * @brief global difine of CSerialPort ����ȫ�ֶ���
 * @date 2018-12-10
 *
 * @copyright Copyright (c) 2019 - itas109
 *
 */
#ifndef __CSERIALPORT_GLOBAL_H__
#define __CSERIALPORT_GLOBAL_H__

 // enum is not a class or namespace error
 // https://stackoverflow.com/questions/5188554/my-enum-is-not-a-class-or-namespace

#include "osplatformutil.h"

#ifdef I_OS_WIN
typedef __int64 int64;           ///< 64 bit signed windows ����windows int64
#define DLL_EXPORT __declspec(dllexport) ///< define DLL_EXPORT windows ����windows��������
#elif defined I_OS_UNIX
typedef long long int64;           ///< 64 bit signed unix ����Unix int64
#define DLL_EXPORT     __attribute__((visibility("default"))) ///< define DLL_EXPORT unix ����Unix��������
#else
//Not support
#endif // I_OS_WIN

namespace itas109
{
	/**
	 * @brief the read and write serial port mode enum ��д����ģʽ
	 *
	 */
	enum OperateMode
	{
		AsynchronousOperate,	///< Asynchronous �첽
		SynchronousOperate		///< Synchronous ͬ��
	};

	/**
	 * @brief the BaudRate enum ������
	 *
	 */
	enum BaudRate
	{
		BaudRate1200 = 1200,		///< 1200
		BaudRate2400 = 2400,		///< 2400
		BaudRate4800 = 4800,		///< 4800
		BaudRate9600 = 9600,		///< 9600 recommend �Ƽ�
		BaudRate19200 = 19200,		///< 19200
		BaudRate38400 = 38400,		///< 38400
		BaudRate115200 = 115200,	///< 115200
	};

	/**
	 * @brief the DataBits enum ����λ
	 * @warning
	 * by QextSerialPort:\n
	 * 5 data bits cannot be used with 2 stop bits 5λ����λ����ʹ��2λֹͣλ\n
	 * 1.5 stop bits can only be used with 5 data bits 1.5λֹͣλ����ʹ��5λ����λ\n
	 * 8 data bits cannot be used with space parity on POSIX systems POSIXϵͳ8λ����λ����ʹ��0У��
	 * @warning windows Number of bits/byte, 4-8 windows����λ��ΧΪ4 - 8
	 *
	 */
	enum DataBits
	{
		DataBits5 = 5,			///< 5 data bits 5λ����λ
		DataBits6 = 6,			///< 6 data bits 6λ����λ
		DataBits7 = 7,			///< 7 data bits 7λ����λ
		DataBits8 = 8			///< 8 data bits 8λ����λ
	};

	/**
	 * @brief the Parity enum У��λ
	 * @warning windows 0-4=None,Odd,Even,Mark,Space
	 *
	 */
	enum Parity
	{
		ParityNone = 0,			///< No Parity ��У��
		ParityOdd = 1,			///< Odd Parity ��У��
		ParityEven = 2,			///< Even Parity żУ��
		ParityMark = 3,            ///< Mark Parity 1У��
		ParitySpace = 4,			///< Space Parity 0У��	 
	};

	/**
	 * @brief the StopBits enum ֹͣλ
	 * @warning 1.5 stop bit only for the Windows platform 1.5λֹͣλ����windows��Ч
	 * @warning windows 0,1,2 = 1, 1.5, 2
	 *
	 */
	enum StopBits
	{
		StopOne = 0,				///< 1 stop bit 1λֹͣλ
		StopOneAndHalf = 1,			///< 1.5 stop bit 1.5λֹͣλ - This is only for the Windows platform
		StopTwo = 2					///< 2 stop bit 2λֹͣλ
	};

	/**
	 * @brief the FlowConctrol enum ������
	 *
	 */
	enum FlowConctrol
	{
		FlowNone = 0,		///< No flow control ��������
		FlowHardware = 1,	///< Hardware(RTS / CTS) flow control Ӳ��������
		FlowSoftware = 2	///< Software(XON / XOFF) flow control ���������
	};

	/**
	 * @brief the SerialPort error code ���ڴ������
	 *
	 */
	enum SerialPortError
	{
		NoError,							///< No error occurred û�д���
		DeviceNotFoundError,				///< device not found δ�ҵ��豸
		PermissionError,					///< permission error Ȩ�޴���
		OpenError,							///< open error �򿪴��ڴ���
		ParityError,						///< parity error У��λ����
		FramingError,						///< 
		BreakConditionError,				///< 
		WriteError,							///< write error д���ݴ���
		ReadError,							///< read error �����ݴ���
		ResourceError,						///< 
		UnsupportedOperationError,			///< 
		UnknownError,						///< unknown error δ֪����
		TimeoutError,						///< time out error ��ʱ
		NotOpenError						///< not open serial port error ����δ��	
	};
}

#endif //__CSERIALPORT_GLOBAL_H__
