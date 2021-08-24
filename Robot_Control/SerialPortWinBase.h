/**
 * @file SerialPortWinBase.h
 * @author itas109 (itas109@gmail.com) \n\n
 * Blog : http://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief the CSerialPort Windows Base class windows���ڻ���
 * @date 2018-12-20
 *
 * @copyright Copyright (c) 2019 - itas109
 *
 */
#ifndef __CSERIALPORTWINBASE_H__
#define __CSERIALPORTWINBASE_H__

#include "SerialPortBase.h"

 // windows need
#include <windows.h>
#include <tchar.h>//tchar
#include <process.h>//_beginthreadex

// sigslot
#include "sigslot.h"

/**
 * @brief the CSerialPort Windows Base class windows���ڻ���
 * @see inherit �̳� CSerialPortBase
 * @todo
 * 1. ���߳��ڲ��ӹܣ����迪���߹��� \n
 * 2. ʹ���Զ���������̻߳���������� \n
 * 3. ���Ϊ�麯���������߿��Լ̳��޸� \n
 * 4. ֧���Ȳ�� \n
 * 5. ֧��ͨ��ͨ��Э��:֡ͷ + ���� + У�� + ֡β \n
 * 6. ֧�ֻ���ʱ��Ƭ��������װ \n
 * 7. ����ʱ��������ݻ������ - ���ݴ浽�������������ʱ��� �����߰���һ�����Ի�ȡ����װ���� \n
 * 8. ��дwindows��ReadFile��WriteFile \n
 * @warning More than one character can trigger read event ע�⣺ֻ�г���һ���ַ����ܳ��������¼�
 */
class CSerialPortWinBase : public CSerialPortBase
{
public:
	/**
	 * @brief Construct a new CSerialPortWinBase object ���캯��
	 *
	 */
	CSerialPortWinBase();
	/**
	 * @brief Construct a new CSerialPortWinBase object ͨ���������ƹ��캯��
	 *
	 * @param portName  [in] the port name �������� Windows:COM1 Linux:/dev/ttyS0
	 */
	CSerialPortWinBase(const std::string& portName);
	/**
	 * @brief Destroy the CSerialPortWinBase object ��������
	 *
	 */
	virtual ~CSerialPortWinBase();

	/**
	 * @brief parameter init ������ʼ��
	 *
	 */
	virtual void construct();

	/**
	 * @brief init ��ʼ������
	 *
	 * @param portName [in] the port name�������� Windows:COM1 Linux:/dev/ttyS0
	 * @param baudRate [in] the baudRate ������
	 * @param parity [in] the parity У��λ
	 * @param dataBits [in] the dataBits ����λ
	 * @param stopbits [in] the stopbits ֹͣλ
	 * @param flowConctrol [in] flowConctrol type ������
	 * @param readBufferSize [in] the read buffer size ��ȡ��������С
	 */
	virtual void init(std::string portName, int baudRate = itas109::BaudRate9600, itas109::Parity parity = itas109::ParityNone, itas109::DataBits dataBits = itas109::DataBits8, itas109::StopBits stopbits = itas109::StopOne, itas109::FlowConctrol flowConctrol = itas109::FlowNone, int64 readBufferSize = 512);
	/**
	 * @brief  init ��ʼ������
	 *
	 * @param port [in] the port number ���ں�
	 * @param baudRate [in] the baudRate ������
	 * @param parity [in] the parity У��λ
	 * @param dataBits [in] the dataBits ����λ
	 * @param stopbits [in] the stopbits ֹͣλ
	 * @param flowConctrol [in] flowConctrol type ������
	 * @param readBufferSize [in] the read buffer size ��ȡ��������С
	 */
	virtual void init(int port, int baudRate = itas109::BaudRate9600, itas109::Parity parity = itas109::ParityNone, itas109::DataBits dataBits = itas109::DataBits8, itas109::StopBits stopbits = itas109::StopOne, itas109::FlowConctrol flowConctrol = itas109::FlowNone, int64 readBufferSize = 512);

	/**
	 * @brief open serial port �򿪴���
	 *
	 * @return
	 * @retval true open success �򿪳ɹ�
	 * @retval false open failed ��ʧ��
	 */
	virtual bool open();
	/**
	 * @brief close �رմ���
	 *
	 */
	virtual void close();

	/**
	 * @brief if serial port is open success �����Ƿ�򿪳ɹ�
	 *
	 * @return
	 * @retval true serial port open success ���ڴ򿪳ɹ�
	 * @retval false serial port open failed ���ڴ�ʧ��
	 */
	virtual bool isOpened();

	/**
	 * @brief read specified length data ��ȡָ����������
	 *
	 * @param data [out] read data result ��ȡ���
	 * @param maxSize [in] read length ��ȡ����
	 * @return return number Of bytes read ���ض�ȡ�ֽ���
	 * @retval -1 read error ��ȡ����
	 * @retval [other] return number Of bytes read ���ض�ȡ�ֽ���
	 */
	virtual int readData(char* data, int maxSize);
	/**
	 * @brief read all data ��ȡ��������
	 *
	 * @param data [out] read data result ��ȡ���
	 * @return return number Of bytes read ���ض�ȡ�ֽ���
	 * @retval -1 read error ��ȡ����
	 * @retval [other] return number Of bytes read ���ض�ȡ�ֽ���
	 */
	virtual int readAllData(char* data);
	/**
	 * @brief read line data ��ȡһ���ַ���
	 * @todo Not implemented δʵ��
	 *
	 * @param data
	 * @param maxSize
	 * @return int
	 */
	virtual int	readLineData(char* data, int maxSize);
	/**
	 * @brief write specified lenfth data д��ָ����������
	 *
	 * @param data [in] write data ��д������
	 * @param maxSize [in] wtite length д�볤��
	 * @return return number Of bytes write ����д���ֽ���
	 * @retval -1 read error д�����
	 * @retval [other] return number Of bytes write ����д���ֽ���
	 */
	virtual int writeData(const char* data, int maxSize);

	/**
	 * @brief Set Debug Model ���õ���ģʽ
	 * @details output serial port read and write details info ������ڶ�д����ϸ��Ϣ
	 * @todo  Not implemented δʵ��
	 *
	 * @param isDebug true if enable trueΪ����
	 */
	virtual void setDebugModel(bool isDebug);

	/**
	 * @brief Set the Read Time Interval object
	 * @details use timer import effectiveness ʹ�ö�ʱ�����Ч��
	 * @todo  Not implemented δʵ��
	 *
	 * @param msecs read time micro second ��ȡ���ʱ�䣬��λ������
	 */
	virtual void setReadTimeInterval(int msecs);

	/**
	 * @brief Get the Last Error object ��ȡ���Ĵ������
	 *
	 * @return return last error code, refrence {@link itas109::SerialPortError} �������
	 */
	virtual int getLastError() const;
	/**
	 * @brief clear error ���������Ϣ
	 *
	 */
	virtual void clearError();

	/**
	 * @brief Set the Port Name object ���ô�������
	 *
	 * @param portName [in] the port name �������� Windows:COM1 Linux:/dev/ttyS0
	 */
	virtual void setPortName(std::string portName);
	/**
	 * @brief Get the Port Name object ��ȡ��������
	 *
	 * @return return port name ���ش�������
	 */
	virtual std::string getPortName() const;

	/**
	 * @brief Set the Baud Rate object ���ò�����
	 *
	 * @param baudRate [in] the baudRate ������
	 */
	virtual void setBaudRate(int baudRate);
	/**
	 * @brief Get the Baud Rate object ��ȡ������
	 *
	 * @return return baudRate ���ز�����
	 */
	virtual int getBaudRate() const;

	/**
	 * @brief Set the Parity object ����У��λ
	 *
	 * @param parity [in] the parity У��λ {@link itas109::Parity}
	 */
	virtual void setParity(itas109::Parity parity);
	/**
	 * @brief Get the Parity object ��ȡУ��λ
	 *
	 * @return return parity ����У��λ {@link itas109::Parity}
	 */
	virtual itas109::Parity getParity() const;
	/**
	 * @brief Set the Data Bits object ��������λ
	 *
	 * @param dataBits [in] the dataBits ����λ {@link itas109::DataBits}
	 */
	virtual void setDataBits(itas109::DataBits dataBits);
	/**
	 * @brief Get the Data Bits object ��ȡ����λ
	 *
	 * @return return dataBits ��������λ {@link itas109::DataBits}
	 */
	virtual itas109::DataBits getDataBits() const;

	/**
	 * @brief Set the Stop Bits object ����ֹͣλ
	 *
	 * @param stopbits [in] the stopbits ֹͣλ {@link itas109::StopBits}
	 */
	virtual void setStopBits(itas109::StopBits stopbits);
	/**
	 * @brief Get the Stop Bits object ��ȡֹͣλ
	 *
	 * @return return stopbits ����ֹͣλ {@link itas109::StopBits}
	 */
	virtual itas109::StopBits getStopBits() const;

	/**
	 * @brief Set the Flow Conctrol object ����������
	 * @todo Not implemented δʵ��
	 *
	 * @param flowConctrol [in]
	 */
	virtual void setFlowConctrol(itas109::FlowConctrol flowConctrol);
	/**
	 * @brief Get the Flow Conctrol object ��ȡ������
	 * @todo Not implemented δʵ��
	 *
	 * @return itas109::FlowConctrol
	 */
	virtual itas109::FlowConctrol getFlowConctrol() const;

	/**
	 * @brief Set the Read Buffer Size object ���ö�ȡ��������С
	 *
	 * @param size [in] read buffer size ��ȡ��������С
	 */
	virtual void setReadBufferSize(int64 size);
	/**
	 * @brief Get the Read Buffer Size object ��ȡ��ȡ��������С
	 *
	 * @return return read buffer size ���ض�ȡ��������С
	 */
	virtual int64 getReadBufferSize() const;

	/**
	 * @brief Set the Dtr object ����DTR
	 * @todo Not implemented δʵ��
	 *
	 * @param set [in]
	 */
	virtual void setDtr(bool set = true);
	/**
	 * @brief Set the Rts object ����RTS
	 * @todo Not implemented δʵ��
	 *
	 * @param set [in]
	 */
	virtual void setRts(bool set = true);

	/**
	 * @brief Get the Version object ��ȡ�汾��Ϣ
	 *
	 * @return return version ���ذ汾��Ϣ
	 */
	std::string getVersion();

public:
	/**
	 * @brief Get the Overlap Monitor object ��ȡ��������overlapped
	 *
	 * @return return monitor overlapped ���ؼ�������overlapped
	 */
	OVERLAPPED getOverlapMonitor();
	/**
	 * @brief Get the Main Handle object ��ȡhandle
	 *
	 * @return retrun handle ����handle
	 */
	HANDLE getMainHandle();

protected:
	/**
	 * @brief lock ��
	 *
	 */
	void lock();
	/**
	 * @brief unlock ����
	 *
	 */
	void unlock();

private:
	/**
	 * @brief thread monitor ���̼߳�����
	 *
	 */
	static unsigned int __stdcall commThreadMonitor(LPVOID pParam);
	/**
	 * @brief start thread monitor �������̼߳�����
	 *
	 * @return
	 * @retval true start success �����ɹ�
	 * @retval false start failed ����ʧ��
	 */
	bool startThreadMonitor();
	/**
	 * @brief stop thread monitor ֹͣ���̼߳�����
	 *
	 * @return
	 * @retval true stop success ֹͣ�ɹ�
	 * @retval false stop failed ֹͣʧ��
	 */
	bool stopThreadMonitor();

public:
	static sigslot::signal0<> readReady;///< sigslot for read �������ź�

private:
	std::string m_portName;
	int m_baudRate;
	itas109::Parity m_parity;
	itas109::DataBits m_dataBits;
	itas109::StopBits m_stopbits;
	enum itas109::FlowConctrol m_flowConctrol;
	int64 m_readBufferSize;

private:
	HANDLE m_handle;

	HANDLE m_monitorThread;
	OVERLAPPED overlapMonitor;	///< monitor overlapped

	OVERLAPPED m_overlapRead;	///< read overlapped
	OVERLAPPED m_overlapWrite;	///< write overlapped

	COMMCONFIG m_comConfigure;
	COMMTIMEOUTS m_comTimeout;

	CRITICAL_SECTION m_communicationMutex;///< mutex

	static bool isThreadRunning;
};
#endif //__CSERIALPORTWINBASE_H__

