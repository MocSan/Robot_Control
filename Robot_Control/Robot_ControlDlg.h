//---------------------------------------------------------------------------------
// Robot_ControlDlg.h: 头文件
//---------------------------------------------------------------------------------

#pragma once

#include "GxIAPI.h"
#include "DxImageProc.h"
#include "Halcon.h"
#include "HalconCpp.h"
#include "SerialPort.h"
#include "Dbt.h"

using namespace HalconCpp;

// CRobotControlDlg 对话框

class CRobotControlDlg : public CDialogEx
{
// 构造
public:
	CRobotControlDlg(CWnd* pParent = nullptr);  // 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROBOT_CONTROL_DIALOG };
#endif

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);  // 分发函数主要处理曝光和增益的 Edit 框响应回车键的消息

    // 线程函数声明
	static DWORD WINAPI ThreadProc1(LPVOID lpParameter);
	static DWORD WINAPI ThreadProc2(LPVOID lpParameter);
	static DWORD WINAPI ThreadProc3(LPVOID lpParameter);
	static DWORD WINAPI ThreadProc4(LPVOID lpParameter);
	static DWORD WINAPI ThreadProc5(LPVOID lpParameter);
	static DWORD WINAPI ThreadProc6(LPVOID lpParameter);
	static DWORD WINAPI ThreadProc7(LPVOID lpParameter);
	static DWORD WINAPI ThreadProc8(LPVOID lpParameter);
	static DWORD WINAPI ThreadProc9(LPVOID lpParameter);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnComm(WPARAM ch, LPARAM port);
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void Open_Device();
	afx_msg void Close_Device();
	afx_msg void Start_Snap();
	afx_msg void Stop_Snap();
	afx_msg void Open_Serial();
	afx_msg void Send_Data();
	afx_msg void Close_Serial();
	afx_msg void Clear_Data();
	afx_msg void Rectangle();
	afx_msg void Circular();
	afx_msg void Triangle();
	afx_msg void Pentagon();
	afx_msg void Edit_Exposure();
	afx_msg void Edit_Gain();

public:
	// 初始化相机参数
	GX_STATUS InitDeviceParam();

	// 为图像显示准备资源
	bool PrepareForShowImg();

	// 显示图像
	void DrawImage(BYTE* pImageBuf, int nWidth, int nHeight, BITMAPINFO* pBmpInfo);

	// 刷新 UI 界面
	void UpDateUI();

	// 初始化 UI 界面
	void InitUI();

	// 释放为图像显示准备资源
	void UnPrepareForShowImg();

	// 回调函数
	static void __stdcall OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM* pFrame);

	// 将字符串转换为十六进制
	int Str2Hex(CString str, char* data);

	// 将字符转换为十六进制
	char HexChar(char c);

	// 向组合框中添加串口设备
	void AddCom();

	// 刷新串口组合框
	void RefreshCom();

	// 重载 OnOK 函数
	virtual void OnOK();

    // Set font independent of OS 
	void set_display_font(HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, HTuple hv_Slant);

	// Short Description: This procedure writes a text message
	void disp_message(HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);

public:
	GX_DEV_HANDLE        m_hDevice;              // 设备句柄
	BITMAPINFO*          m_pBmpInfo;	         // BITMAPINFO 结构指针，显示图像时使用
	CHAR                 m_chBmpBuf[2048];	     // BIMTAPINFO 存储缓冲区，m_pBmpInfo 即指向此缓冲区
	BOOL                 m_bDevOpened;           // 标识是否已打开设备
	BOOL                 m_bIsSnap;              // 标识是否开始采集
	int64_t              m_nPayLoadSize;         // 图像大小
	int64_t              m_nImageHeight;         // 原始图像高
	int64_t              m_nImageWidth;          // 原始图像宽
	BYTE*                m_pBufferRaw;           // 原始图像数据
	BYTE*                m_pImageBuffer;	     // 保存翻转后的图像用于显示用
	HDC                  m_hDC;                  // 图像显示窗口的 DC
	CWnd*                m_pWnd;                 // 显示图像的窗口句柄
	RECT                 m_objRect;              // 图像显示对象

	SerialPort           m_SerialPort;           // 串口类对象
	BOOL                 m_bSerialOpened;        // 标识是否已打开串口
	CComboBox            m_ctrlComboComPort;     // Combox 控件下拉列表变量
	CString              m_strEditReceiveMsg;    // 接收数据区变量
	CString              m_strEditSendMsg;       // 发送数据区变量
	CString              m_strCurrent;           // 当前连接的串口号

	CSliderCtrl	         m_sliderExposure;       // 滚动条曝光对象
	CSliderCtrl	         m_sliderGain;           // 滚动条增益对象
	DOUBLE               m_dExposure;            // 曝光值
	DOUBLE               m_dGain;                // 增益值

	HANDLE               m_Thread1;              // 线程句柄1
	HANDLE               m_Thread2;              // 线程句柄2
	HANDLE               m_Thread3;              // 线程句柄3
	HANDLE               m_Thread4;              // 线程句柄4
	HANDLE               m_Thread5;              // 线程句柄5
	HANDLE               m_Thread6;              // 线程句柄6
	HANDLE               m_Thread7;              // 线程句柄7
	HANDLE               m_Thread8;              // 线程句柄8
	HANDLE               m_Thread9;              // 线程句柄9

	CString              str1;                   // 矩形信息显示
	CString              str2;                   // 圆形信息显示
	CString              str3;                   // 三角形信息显示
	CString              str4;                   // 五边形信息显示

	CProgressCtrl        m_Progress;             // 进度条对象
	CStatic              m_Percentage;           // 进度条百分比
	CFont                m_Font;                 // 设置字体大小
	CBrush               m_Brush;                // 设置字体颜色

	HTuple               hv_WindowHandle;        // Halcon窗口显示
	HTuple               hv_Row1;                // 矩形框中心点的行
	HTuple               hv_Column1;             // 矩形框中心点的列
	HTuple               hv_Width;               // 矩形框的宽
	HTuple               hv_Height;              // 矩形框的高
	HTuple               hv_Phi1;                // 矩形框的弧度方向
	HObject              ho_Rectangle1;          // 带角度矩形区域

	HTuple               hv_Row2;                // 椭圆形框中心点的行
	HTuple               hv_Column2;             // 椭圆形框中心点的列
	HTuple               hv_Radius1;             // 椭圆形框的上半轴
	HTuple               hv_Radius2;             // 椭圆形框的下半轴
	HTuple               hv_Phi2;                // 椭圆形框的弧度方向
	HObject              ho_Ellipse;             // 椭圆形区域

	HTuple               hv_Row3;                // 矩形框三角形起始点的行
	HTuple               hv_Column3;             // 矩形框三角形起始点的列
	HTuple               hv_Row4;                // 矩形框三角形终止点的行
	HTuple               hv_Column4;             // 矩形框三角形终止点的列
	HObject              ho_Rectangle2;          // 矩形区域

	HTuple               hv_Row5;                // 圆形框标记五边形中心点的行
	HTuple               hv_Column5;             // 圆形框标记五边形中心点的列
	HTuple               hv_Radius;              // 圆形框标记五边形的半径
	HObject              ho_Circle;              // 圆形区域

	HObject              ho_Image1;              // 矩形图像对象
	HObject              ho_ImageReduced1;       // 将图像转换成矩形区域
	HObject              ho_Regions1;            // 矩形区域对象

	HObject              ho_Image2;              // 圆形图像对象
	HObject              ho_ImageReduced2;       // 将图像转换成圆形区域
	HObject              ho_Regions2;            // 圆形区域对象

	HObject              ho_Image3;              // 三角形图像对象
	HObject              ho_ImageReduced3;       // 将图像转换成三角形区域
	HObject              ho_Regions3;            // 三角形区域对象

	HObject              ho_Image4;              // 五边形图像对象
	HObject              ho_ImageReduced4;       // 将图像转换成五边形区域
	HObject              ho_Regions4;            // 五边形区域对象
};
