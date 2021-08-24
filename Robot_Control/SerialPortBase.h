/**
 * @file SerialPortBase.h
 * @author itas109 (itas109@gmail.com) \n\n
 * Blog : http://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief the CSerialPort Base class ���ڻ���
 * @date 2018-12-20
 *
 * @copyright Copyright (c) 2019 - itas109
 *
 */
#ifndef __CSERIALPORTBASE_H__
#define __CSERIALPORTBASE_H__

#include "SerialPort_global.h"
#include <string>

 /**
  * @brief the CSerialPort Base class ���ڻ���
  *
  */
class CSerialPortBase
{
public:
	/**
	 * @brief Construct a new CSerialPortBase object ���캯��
	 *
	 */
	CSerialPortBase();
	/**
	 * @brief Construct a new CSerialPortBase object ͨ���������ƹ��캯��
	 *
	 * @param portName [in] the port name �������� Windows:COM1 Linux:/dev/ttyS0
	 */
	CSerialPortBase(const std::string& portName);
	/**
	 * @brief Destroy the CSerialPortBase object ��������
	 *
	 */
	virtual ~CSerialPortBase();

	/**
	* @brief parameter init ������ʼ��
	*
	*/
	virtual void construct() = 0;

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
	virtual void init(std::string portName, int baudRate, itas109::Parity parity, itas109::DataBits dataBits, itas109::StopBits stopbits, itas109::FlowConctrol flowConctrol, int64 readBufferSize) = 0;

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
	virtual void init(int port, int baudRate, itas109::Parity parity, itas109::DataBits dataBits, itas109::StopBits stopbits, itas109::FlowConctrol flowConctrol, int64 readBufferSize) = 0;

	/**
	 * @brief Set the Operate Mode object ���ô��ڲ���ģʽ
	 *
	 * @param operateMode [in] the operate mode ���ڲ���ģʽ {@link itas109::OperateMode}
	 */
	virtual void setOperateMode(itas109::OperateMode operateMode);

	/**
	* @brief open serial port �򿪴���
	*
	* @return
	* @retval true open success �򿪳ɹ�
	* @retval false open failed ��ʧ��
	*/
	virtual bool open() = 0;
	/**
	* @brief close �رմ���
	*
	*/
	virtual void close() = 0;

	/**
	* @brief if serial port is open success �����Ƿ�򿪳ɹ�
	*
	* @return
	* @retval true serial port open success ���ڴ򿪳ɹ�
	* @retval false serial port open failed ���ڴ�ʧ��
	*/
	virtual bool isOpened() = 0;

	/**
	* @brief read specified length data ��ȡָ����������
	*
	* @param data [out] read data result ��ȡ���
	* @param maxSize [in] read length ��ȡ����
	* @return return number Of bytes read ���ض�ȡ�ֽ���
	* @retval -1 read error ��ȡ����
	* @retval [other] return number Of bytes read ���ض�ȡ�ֽ���
	*/
	virtual int readData(char* data, int maxSize) = 0;
	/**
	* @brief read all data ��ȡ��������
	*
	* @param data [out] read data result ��ȡ���
	* @return return number Of bytes read ���ض�ȡ�ֽ���
	* @retval -1 read error ��ȡ����
	* @retval [other] return number Of bytes read ���ض�ȡ�ֽ���
	*/
	virtual int readAllData(char* data) = 0;
	/**
	* @brief read line data ��ȡһ���ַ���
	* @todo Not implemented δʵ��
	*
	* @param data
	* @param maxSize
	* @return int
	*/
	virtual int	readLineData(char* data, int maxSize) = 0;
	/**
	* @brief write specified lenfth data д��ָ����������
	*
	* @param data [in] write data ��д������
	* @param maxSize [in] wtite length д�볤��
	* @return return number Of bytes write ����д���ֽ���
	* @retval -1 read error д�����
	* @retval [other] return number Of bytes write ����д���ֽ���
	*/
	virtual int writeData(const char* data, int maxSize) = 0;

	/**
	* @brief Set Debug Model ���õ���ģʽ
	* @details output serial port read and write details info ������ڶ�д����ϸ��Ϣ
	*
	* @param isDebug true if enable trueΪ����
	*/
	virtual void setDebugModel(bool isDebug) = 0;

	/**
	* @brief Set the Read Time Interval object
	* @details use timer import effectiveness ʹ�ö�ʱ�����Ч��
	*
	* @param msecs read time micro second ��ȡ���ʱ�䣬��λ������
	*/
	virtual void setReadTimeInterval(int msecs) = 0;

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
	virtual void setPortName(std::string portName) = 0;
	/**
	* @brief Get the Port Name object ��ȡ��������
	*
	* @return return port name ���ش�������
	*/
	virtual std::string getPortName() const = 0;

	/**
	* @brief Set the Baud Rate object ���ò�����
	*
	* @param baudRate [in] the baudRate ������
	*/
	virtual void setBaudRate(int baudRate) = 0;
	/**
	* @brief Get the Baud Rate object ��ȡ������
	*
	* @return return baudRate ���ز�����
	*/
	virtual int getBaudRate() const = 0;

	/**
	* @brief Set the Parity object ����У��λ
	*
	* @param parity [in] the parity У��λ {@link itas109::Parity}
	*/
	virtual void setParity(itas109::Parity parity) = 0;
	/**
	* @brief Get the Parity object ��ȡУ��λ
	*
	* @return return parity ����У��λ {@link itas109::Parity}
	*/
	virtual itas109::Parity getParity() const = 0;

	/**
	* @brief Set the Data Bits object ��������λ
	*
	* @param dataBits [in] the dataBits ����λ {@link itas109::DataBits}
	*/
	virtual void setDataBits(itas109::DataBits dataBits) = 0;
	/**
	* @brief Get the Data Bits object ��ȡ����λ
	*
	* @return return dataBits ��������λ {@link itas109::DataBits}
	*/
	virtual itas109::DataBits getDataBits() const = 0;

	/**
	* @brief Set the Stop Bits object ����ֹͣλ
	*
	* @param stopbits [in] the stopbits ֹͣλ {@link itas109::StopBits}
	*/
	virtual void setStopBits(itas109::StopBits stopbits) = 0;
	/**
	* @brief Get the Stop Bits object ��ȡֹͣλ
	*
	* @return return stopbits ����ֹͣλ {@link itas109::StopBits}
	*/
	virtual itas109::StopBits getStopBits() const = 0;

	/**
	* @brief Set the Flow Conctrol object ����������
	*
	* @param flowConctrol [in]
	*/
	virtual void setFlowConctrol(itas109::FlowConctrol flowConctrol) = 0;
	/**
	* @brief Get the Flow Conctrol object ��ȡ������
	*
	* @return itas109::FlowConctrol
	*/
	virtual itas109::FlowConctrol getFlowConctrol() const = 0;

	/**
	* @brief Set the Read Buffer Size object ���ö�ȡ��������С
	*
	* @param size [in] read buffer size ��ȡ��������С
	*/
	virtual void setReadBufferSize(int64 size) = 0;
	/**
	* @brief Get the Read Buffer Size object ��ȡ��ȡ��������С
	*
	* @return return read buffer size ���ض�ȡ��������С
	*/
	virtual int64 getReadBufferSize() const = 0;

	/**
	* @brief Set the Dtr object ����DTR
	*
	* @param set [in]
	*/
	virtual void setDtr(bool set = true) = 0;
	/**
	* @brief Set the Rts object ����RTS
	*
	* @param set [in]
	*/
	virtual void setRts(bool set = true) = 0;

	/**
	* @brief Get the Version object ��ȡ�汾��Ϣ
	*
	* @return return version ���ذ汾��Ϣ
	*/
	std::string getVersion();

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

protected:
	int lastError;///< last error code ���Ĵ������
	itas109::OperateMode m_operateMode;///< operate mode ���ڲ�������
private:

};
#endif //__CSERIALPORTBASE_H__

