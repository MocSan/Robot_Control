/**
 * @file SerialPort.h
 * @author itas109 (itas109@gmail.com) \n\n
 * Blog : http://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * QQ Group : 12951803
 * @brief a lightweight library of serial port, which can easy to read and write serical port on windows and linux with C++ ��������ƽ̨���ڶ�д���
 * @version 4.0.0.181210
 * @date 2018-12-10
 *
 * @copyright Copyright (c) 2019 - itas109
 *
 */
#ifndef __CSERIALPORT_H__
#define __CSERIALPORT_H__

#include "SerialPort_global.h"
#include <string>

#include "sigslot.h"
using namespace sigslot;

class CSerialPortBase;

namespace itas109
{
	/**
	 * @brief the CSerialPort class �������
	 * @see reference ���� CSerialPortBase
	 */
	class DLL_EXPORT CSerialPort : public has_slots<>
	{
	public:
		/**
		 * @brief Construct a new CSerialPort object ���캯��
		 *
		 */
		CSerialPort();
		/**
		 * @brief Construct a new CSerialPort object ͨ���������ƹ��캯��
		 *
		 * @param portName [in] the port name �������� Windows:COM1 Linux:/dev/ttyS0
		 */
		CSerialPort(const std::string& portName);
		/**
		 * @brief Destroy the CSerialPort object ��������
		 *
		 */
		~CSerialPort();

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
		void init(std::string portName, int baudRate = itas109::BaudRate9600, itas109::Parity parity = itas109::ParityNone, itas109::DataBits dataBits = itas109::DataBits8, itas109::StopBits stopbits = itas109::StopOne, itas109::FlowConctrol flowConctrol = itas109::FlowNone, int64 readBufferSize = 512);
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
		void init(int port, int baudRate = itas109::BaudRate9600, itas109::Parity parity = itas109::ParityNone, itas109::DataBits dataBits = itas109::DataBits8, itas109::StopBits stopbits = itas109::StopOne, itas109::FlowConctrol flowConctrol = itas109::FlowNone, int64 readBufferSize = 512);

		/**
		 * @brief open serial port �򿪴���
		 *
		 * @return
		 * @retval true open success �򿪳ɹ�
		 * @retval false open failed ��ʧ��
		 */
		bool open();
		/**
		 * @brief close �رմ���
		 *
		 */
		void close();

		/**
		 * @brief if serial port is open success �����Ƿ�򿪳ɹ�
		 *
		 * @return
		 * @retval true serial port open success ���ڴ򿪳ɹ�
		 * @retval false serial port open failed ���ڴ�ʧ��
		 */
		bool isOpened();

		/**
		 * @brief read specified length data ��ȡָ����������
		 *
		 * @param data [out] read data result ��ȡ���
		 * @param maxSize [in] read length ��ȡ����
		 * @return return number Of bytes read ���ض�ȡ�ֽ���
		 * @retval -1 read error ��ȡ����
		 * @retval [other] return number Of bytes read ���ض�ȡ�ֽ���
		 */
		int readData(char* data, int maxSize);
		/**
		 * @brief read all data ��ȡ��������
		 *
		 * @param data [out] read data result ��ȡ���
		 * @return return number Of bytes read ���ض�ȡ�ֽ���
		 * @retval -1 read error ��ȡ����
		 * @retval [other] return number Of bytes read ���ض�ȡ�ֽ���
		 */
		int readAllData(char* data);
		/**
		 * @brief read line data ��ȡһ���ַ���
		 * @todo Not implemented δʵ��
		 *
		 * @param data
		 * @param maxSize
		 * @return int
		 */
		int	readLineData(char* data, int maxSize);
		/**
		 * @brief write specified lenfth data д��ָ����������
		 *
		 * @param data [in] write data ��д������
		 * @param maxSize [in] wtite length д�볤��
		 * @return return number Of bytes write ����д���ֽ���
		 * @retval -1 read error д�����
		 * @retval [other] return number Of bytes write ����д���ֽ���
		 */
		int writeData(const char* data, int maxSize);

		/**
		 * @brief Set Debug Model ���õ���ģʽ
		 * @details output serial port read and write details info ������ڶ�д����ϸ��Ϣ
		 * @todo  Not implemented δʵ��
		 *
		 * @param isDebug true if enable trueΪ����
		 */
		void setDebugModel(bool isDebug);

		/**
		 * @brief Set the Read Time Interval object
		 * @details use timer import effectiveness ʹ�ö�ʱ�����Ч��
		 * @todo  Not implemented δʵ��
		 *
		 * @param msecs read time micro second ��ȡ���ʱ�䣬��λ������
		 */
		void setReadTimeInterval(int msecs);

		/**
		 * @brief Get the Last Error object ��ȡ���Ĵ������
		 *
		 * @return return last error code, refrence {@link itas109::SerialPortError} �������
		 */
		int getLastError() const;
		/**
		 * @brief clear error ���������Ϣ
		 *
		 */
		void clearError();

		/**
		 * @brief Set the Port Name object ���ô�������
		 *
		 * @param portName [in] the port name �������� Windows:COM1 Linux:/dev/ttyS0
		 */
		void setPortName(std::string portName);
		/**
		 * @brief Get the Port Name object ��ȡ��������
		 *
		 * @return return port name ���ش�������
		 */
		std::string getPortName() const;
		/**
		 * @brief Set the Baud Rate object ���ò�����
		 *
		 * @param baudRate [in] the baudRate ������
		 */
		void setBaudRate(int baudRate);
		/**
		 * @brief Get the Baud Rate object ��ȡ������
		 *
		 * @return return baudRate ���ز�����
		*/
		int getBaudRate() const;

		/**
		 * @brief Set the Parity object ����У��λ
		 *
		 * @param parity [in] the parity У��λ {@link itas109::Parity}
		 */
		void setParity(itas109::Parity parity);
		/**
		 * @brief Get the Parity object ��ȡУ��λ
		 *
		 * @return return parity ����У��λ {@link itas109::Parity}
		 */
		itas109::Parity getParity() const;

		/**
		 * @brief Set the Data Bits object ��������λ
		 *
		 * @param dataBits [in] the dataBits ����λ  {@link itas109::DataBits}
		 */
		void setDataBits(itas109::DataBits dataBits);
		/**
		 * @brief Get the Data Bits object ��ȡ����λ
		 *
		 * @return return dataBits ��������λ {@link itas109::DataBits}
		 */
		itas109::DataBits getDataBits() const;

		/**
		 * @brief Set the Stop Bits object ����ֹͣλ
		 *
		 * @param stopbits [in] the stopbits ֹͣλ {@link itas109::StopBits}
		 */
		void setStopBits(itas109::StopBits stopbits);
		/**
		 * @brief Get the Stop Bits object ��ȡֹͣλ
		 *
		 * @return return stopbits ����ֹͣλ {@link itas109::StopBits}
		 */
		itas109::StopBits getStopBits() const;

		/**
		 * @brief Set the Flow Conctrol object ����������
		 * @todo Not implemented δʵ��
		 *
		 * @param flowConctrol [in]
		 */
		void setFlowConctrol(itas109::FlowConctrol flowConctrol);
		/**
		 * @brief Get the Flow Conctrol object ��ȡ������
		 * @todo Not implemented δʵ��
		 *
		 * @return itas109::FlowConctrol
		 */
		itas109::FlowConctrol getFlowConctrol() const;

		/**
		 * @brief Set the Read Buffer Size object
		 *
		 * @param size [in] read buffer size  ���û�������С
		 */
		void setReadBufferSize(int64 size);
		/**
		 * @brief Get the Read Buffer Size object ��ȡ��ȡ��������С
		 *
		 * @return return read buffer size ���ض�ȡ��������С
		 */
		int64 getReadBufferSize() const;

		/**
		 * @brief Set the Dtr object ����DTR
		 * @todo Not implemented δʵ��
		 *
		 * @param set [in]
		 */
		void setDtr(bool set = true);
		/**
		 * @brief Set the Rts object ����RTS
		 * @todo Not implemented δʵ��
		 *
		 * @param set [in]
		 */
		void setRts(bool set = true);

		/**
		 * @brief Get the Version object ��ȡ�汾��Ϣ
		 *
		 * @return return version ���ذ汾��Ϣ
		 */
		std::string getVersion();

	public:
		void onReadReady();
		sigslot::signal0<> readReady;//sigslot

	private:
		CSerialPortBase* p_serialPortBase;
	};
}
#endif //__CSERIALPORT_H__

