//---------------------------------------------------------------------------------
// Robot_ControlDlg.cpp: 实现文件
//---------------------------------------------------------------------------------

#include "pch.h"
#include "framework.h"
#include "Robot_Control.h"
#include "Robot_ControlDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CRobotControlDlg 对话框

CRobotControlDlg::CRobotControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ROBOT_CONTROL_DIALOG, pParent)
	, m_bDevOpened(FALSE)
	, m_bIsSnap(FALSE)
	, m_bSerialOpened(FALSE)
	, m_hDevice(NULL)
	, m_pBmpInfo(NULL)
	, m_pBufferRaw(NULL)
	, m_pImageBuffer(NULL)
	, m_pWnd(NULL)
	, m_hDC(NULL)
	, m_strEditReceiveMsg(_T(""))
	, m_strEditSendMsg(_T(""))
	, m_strCurrent(_T(""))
	, m_nImageHeight(0)
	, m_nImageWidth(0)
	, m_nPayLoadSize(0)
	, m_dExposure(0.0)
	, m_dGain(0.0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	memset(m_chBmpBuf, 0, sizeof(m_chBmpBuf));
}

void CRobotControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PORT, m_ctrlComboComPort);
	DDX_Control(pDX, IDC_EXPOSURE, m_sliderExposure);
	DDX_Control(pDX, IDC_GAIN, m_sliderGain);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_PERCENTAGE, m_Percentage);
	DDX_Text(pDX, IDC_EDITEXPOSURE, m_dExposure);
	DDX_Text(pDX, IDC_EDITGAIN, m_dGain);
	DDX_Text(pDX, IDC_RECEIVE, m_strEditReceiveMsg);
	DDX_Text(pDX, IDC_SEND, m_strEditSendMsg);
	DDX_Text(pDX, IDC_CURRENT, m_strCurrent);
}

BEGIN_MESSAGE_MAP(CRobotControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_COMM_RXCHAR, OnComm)
	ON_EN_KILLFOCUS(IDC_EDITEXPOSURE, Edit_Exposure)
	ON_EN_KILLFOCUS(IDC_EDITGAIN, Edit_Gain)
	ON_BN_CLICKED(IDC_OPEN, &CRobotControlDlg::Open_Device)
	ON_BN_CLICKED(IDC_CLOSE, &CRobotControlDlg::Close_Device)
	ON_BN_CLICKED(IDC_START, &CRobotControlDlg::Start_Snap)
	ON_BN_CLICKED(IDC_STOP, &CRobotControlDlg::Stop_Snap)
	ON_BN_CLICKED(IDC_SERIAL, &CRobotControlDlg::Open_Serial)
	ON_BN_CLICKED(IDC_DATA, &CRobotControlDlg::Send_Data)
	ON_BN_CLICKED(IDC_UNSERIAL, &CRobotControlDlg::Close_Serial)
	ON_BN_CLICKED(IDC_CLEAR, &CRobotControlDlg::Clear_Data)
	ON_BN_CLICKED(IDC_RECTANGLE, &CRobotControlDlg::Rectangle)
	ON_BN_CLICKED(IDC_CIRCULAR, &CRobotControlDlg::Circular)
	ON_BN_CLICKED(IDC_TRIANGLE, &CRobotControlDlg::Triangle)
	ON_BN_CLICKED(IDC_PENTAGON, &CRobotControlDlg::Pentagon)
END_MESSAGE_MAP()

// CRobotControlDlg 消息处理程序

BOOL CRobotControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中
	// IDM_ABOUTBOX 必须在系统命令范围内
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标，当应用程序主窗口不是对话框时，框架将自动执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// 若初始化设备库失败则直接退出应用程序
	emStatus = GXInitLib();
	if (emStatus != GX_STATUS_SUCCESS)
	{
		exit(0);
	}

	// 获取图像显示窗口的指针和绘图 DC
	m_pWnd = GetDlgItem(IDC_GUI);
	m_hDC  = ::GetDC(m_pWnd->m_hWnd);
	m_pWnd->GetClientRect(&m_objRect);

	// Halcon 显示
	CRect rt;
	GetDlgItem(IDC_GUI)->GetClientRect(&rt);
	SetCheck("~father");
	OpenWindow(0, 0, rt.Width(), rt.Height(), 
		(Hlong)GetDlgItem(IDC_GUI)->GetSafeHwnd(), 
		"visible", "", &hv_WindowHandle);
	SetCheck("father");
	SetPart(hv_WindowHandle, 0, 0, 1944, 2592);
	SetLineWidth(hv_WindowHandle, 6);

	// 向组合框中添加串口设备
	AddCom();

	// 设置组合框光标位置
	m_ctrlComboComPort.SetCurSel(0);

	// 设置字体大小及颜色
	m_Font.CreatePointFont(180, "MV Boli");
	m_Brush.CreateSolidBrush(RGB(255, 255, 255));

	// 清空显示
	set_display_font(hv_WindowHandle, 20, "mono", "true", "true");
	disp_message(hv_WindowHandle, "Some object recognition information will be displayed here", "window", 0, 0, "red", "true");

	// 更新 UI 界面
	UpDateUI();

	//m_strEditReceiveMsg = "我不明白您的意思！\r\n您好，请问有什么吩咐！\r\n给您Rectangle！\r\n您好，请问有什么吩咐！\r\n给您Circular！\r\n您好，请问有什么吩咐！\r\n给您Triagnle！";
	UpdateData(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRobotControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码来绘制该图标
// 对于使用文档/视图模型的 MFC 应用程序，这将由框架自动完成

void CRobotControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 当用户拖动最小化窗口时系统调用此函数取得光标显示

HCURSOR CRobotControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 重载 OnOK 函数取消 ENTER 退出程序功能

void CRobotControlDlg::OnOK()
{
}


//---------------------------------------------------------------------------------
/*
\brief   分发函数主要处理曝光和增益的 Edit 框响应回车键的消息
\param   pMsg：消息结构体
\return  true （成功）
         false（失败）
*/
//---------------------------------------------------------------------------------
BOOL CRobotControlDlg::PreTranslateMessage(MSG* pMsg)
{
	GX_STATUS status = GX_STATUS_SUCCESS;
	CWnd* pWnd = NULL;
	int   nCtrlID = 0;    // 保存获取的控件 ID

	// 判断是否是键盘回车消息
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		// 获取当前拥有输入焦点的窗口(控件)指针
		pWnd = GetFocus();

		// 获取焦点所处位置的控件 ID
		nCtrlID = pWnd->GetDlgCtrlID();

		// 判断控件 ID 类型
		switch (nCtrlID)
		{

			 // 如果是曝光 EDIT 框的 ID，设置曝光时间
		case IDC_EDITEXPOSURE:

			 // 如果是增益 EDIT 框的 ID，设置增益值
		case IDC_EDITGAIN:

			// 失去焦点
			SetFocus();

			break;

		default:
			break;
		}

		return TRUE;
	}

	if ((WM_KEYDOWN == pMsg->message) && (VK_ESCAPE == pMsg->wParam))
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


//---------------------------------------------------------------------------------
/*
\brief   字体颜色消息映射
\return  无
*/
//---------------------------------------------------------------------------------
HBRUSH CRobotControlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	if (m_Percentage.m_hWnd == pWnd->m_hWnd)
	{
		pDC->SetBkColor(RGB(0, 255, 0));
		pDC->SelectObject(&m_Font);
		return m_Brush;
	}

	return hbr;
}


//---------------------------------------------------------------------------------
/*
\brief   串口消息映射
\return  无
*/
//---------------------------------------------------------------------------------
LRESULT CRobotControlDlg::OnComm(WPARAM ch, LPARAM port)
{
	m_strEditReceiveMsg += char(ch);
	UpdateData(FALSE);
	return 0;
}


//---------------------------------------------------------------------------------
/*
\brief   组合框消息映射
\return  无
*/
//---------------------------------------------------------------------------------
BOOL CRobotControlDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	switch (nEventType)
	{
	case DBT_DEVICEREMOVECOMPLETE:   // 移除设备
		RefreshCom();
		break;

	case DBT_DEVICEARRIVAL:          // 添加设备
		RefreshCom();
		break;

	default:
		break;
	}

	return TRUE;
}


//---------------------------------------------------------------------------------
/*
\brief  滚动条消息映射
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	GX_STATUS status1 = GX_STATUS_SUCCESS;
	GX_STATUS status2 = GX_STATUS_SUCCESS;

	if (pScrollBar == (CScrollBar*)& m_sliderExposure)            // 曝光滑动条
	{
		m_dExposure = m_sliderExposure.GetPos();
		CString str1;
		str1.Format("%f", m_dExposure);
		GetDlgItem(IDC_EDITEXPOSURE)->SetWindowText(str1);

		// 设置曝光
		status1 = GXSetFloat(m_hDevice, GX_FLOAT_EXPOSURE_TIME, m_dExposure);
	}
	else if (pScrollBar == (CScrollBar*)& m_sliderGain)           // 增益滑动条
	{
		m_dGain = m_sliderGain.GetPos();
		CString str2;
		str2.Format("%f", m_dGain);
		GetDlgItem(IDC_EDITGAIN)->SetWindowText(str2);

		// 设置增益
		status2 = GXSetFloat(m_hDevice, GX_FLOAT_GAIN, m_dGain);
	}

	UpdateData(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


//---------------------------------------------------------------------------------
/*
\brief   Set font independent of OS
\return  无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::set_display_font(HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, HTuple hv_Slant)
{
	// Local iconic variables

	// Local control variables
	HTuple  hv_OS, hv_Fonts, hv_Style, hv_Exception;
	HTuple  hv_AvailableFonts, hv_Fdx, hv_Indices;

	//This procedure sets the text font of the current window with
	//the specified attributes.
	//
	//Input parameters:
	//WindowHandle: The graphics window for which the font will be set
	//Size: The font size. If Size=-1, the default of 16 is used.
	//Bold: If set to 'true', a bold font is used
	//Slant: If set to 'true', a slanted font is used
	//
	GetSystem("operating_system", &hv_OS);
	if (0 != (HTuple(hv_Size == HTuple()).TupleOr(hv_Size == -1)))
	{
		hv_Size = 16;
	}
	if (0 != ((hv_OS.TupleSubstr(0, 2)) == HTuple("Win")))
	{
		//Restore previous behaviour
		hv_Size = (1.13677 * hv_Size).TupleInt();
	}
	else
	{
		hv_Size = hv_Size.TupleInt();
	}
	if (0 != (hv_Font == HTuple("Courier")))
	{
		hv_Fonts.Clear();
		hv_Fonts[0] = "Courier";
		hv_Fonts[1] = "Courier 10 Pitch";
		hv_Fonts[2] = "Courier New";
		hv_Fonts[3] = "CourierNew";
		hv_Fonts[4] = "Liberation Mono";
	}
	else if (0 != (hv_Font == HTuple("mono")))
	{
		hv_Fonts.Clear();
		hv_Fonts[0] = "Consolas";
		hv_Fonts[1] = "Menlo";
		hv_Fonts[2] = "Courier";
		hv_Fonts[3] = "Courier 10 Pitch";
		hv_Fonts[4] = "FreeMono";
		hv_Fonts[5] = "Liberation Mono";
	}
	else if (0 != (hv_Font == HTuple("sans")))
	{
		hv_Fonts.Clear();
		hv_Fonts[0] = "Luxi Sans";
		hv_Fonts[1] = "DejaVu Sans";
		hv_Fonts[2] = "FreeSans";
		hv_Fonts[3] = "Arial";
		hv_Fonts[4] = "Liberation Sans";
	}
	else if (0 != (hv_Font == HTuple("serif")))
	{
		hv_Fonts.Clear();
		hv_Fonts[0] = "Times New Roman";
		hv_Fonts[1] = "Luxi Serif";
		hv_Fonts[2] = "DejaVu Serif";
		hv_Fonts[3] = "FreeSerif";
		hv_Fonts[4] = "Utopia";
		hv_Fonts[5] = "Liberation Serif";
	}
	else
	{
		hv_Fonts = hv_Font;
	}
	hv_Style = "";
	if (0 != (hv_Bold == HTuple("true")))
	{
		hv_Style += HTuple("Bold");
	}
	else if (0 != (hv_Bold != HTuple("false")))
	{
		hv_Exception = "Wrong value of control parameter Bold";
		throw HException(hv_Exception);
	}
	if (0 != (hv_Slant == HTuple("true")))
	{
		hv_Style += HTuple("Italic");
	}
	else if (0 != (hv_Slant != HTuple("false")))
	{
		hv_Exception = "Wrong value of control parameter Slant";
		throw HException(hv_Exception);
	}
	if (0 != (hv_Style == HTuple("")))
	{
		hv_Style = "Normal";
	}
	QueryFont(hv_WindowHandle, &hv_AvailableFonts);
	hv_Font = "";
	{
		HTuple end_val48 = (hv_Fonts.TupleLength()) - 1;
		HTuple step_val48 = 1;
		for (hv_Fdx = 0; hv_Fdx.Continue(end_val48, step_val48); hv_Fdx += step_val48)
		{
			hv_Indices = hv_AvailableFonts.TupleFind(HTuple(hv_Fonts[hv_Fdx]));
			if (0 != ((hv_Indices.TupleLength()) > 0))
			{
				if (0 != (HTuple(hv_Indices[0]) >= 0))
				{
					hv_Font = HTuple(hv_Fonts[hv_Fdx]);
					break;
				}
			}
		}
	}
	if (0 != (hv_Font == HTuple("")))
	{
		throw HException("Wrong value of control parameter Font");
	}
	hv_Font = (((hv_Font + "-") + hv_Style) + "-") + hv_Size;
	HalconCpp::SetFont(hv_WindowHandle, hv_Font);
	return;
}


//---------------------------------------------------------------------------------
/*
\brief   This procedure writes a text message
\return  无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::disp_message(HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box)
{
	// Local iconic variables

	// Local control variables
	HTuple  hv_GenParamName, hv_GenParamValue;

	//This procedure displays text in a graphics window.
	//
	//Input parameters:
	//WindowHandle: The WindowHandle of the graphics window, where
	//   the message should be displayed
	//String: A tuple of strings containing the text message to be displayed
	//CoordSystem: If set to 'window', the text position is given
	//   with respect to the window coordinate system.
	//   If set to 'image', image coordinates are used.
	//   (This may be useful in zoomed images.)
	//Row: The row coordinate of the desired text position
	//   A tuple of values is allowed to display text at different
	//   positions.
	//Column: The column coordinate of the desired text position
	//   A tuple of values is allowed to display text at different
	//   positions.
	//Color: defines the color of the text as string.
	//   If set to [], '' or 'auto' the currently set color is used.
	//   If a tuple of strings is passed, the colors are used cyclically...
	//   - if |Row| == |Column| == 1: for each new textline
	//   = else for each text position.
	//Box: If Box[0] is set to 'true', the text is written within an orange box.
	//     If set to' false', no box is displayed.
	//     If set to a color string (e.g. 'white', '#FF00CC', etc.),
	//       the text is written in a box of that color.
	//     An optional second value for Box (Box[1]) controls if a shadow is displayed:
	//       'true' -> display a shadow in a default color
	//       'false' -> display no shadow
	//       otherwise -> use given string as color string for the shadow color
	//
	//It is possible to display multiple text strings in a single call.
	//In this case, some restrictions apply:
	//- Multiple text positions can be defined by specifying a tuple
	//  with multiple Row and/or Column coordinates, i.e.:
	//  - |Row| == n, |Column| == n
	//  - |Row| == n, |Column| == 1
	//  - |Row| == 1, |Column| == n
	//- If |Row| == |Column| == 1,
	//  each element of String is display in a new textline.
	//- If multiple positions or specified, the number of Strings
	//  must match the number of positions, i.e.:
	//  - Either |String| == n (each string is displayed at the
	//                          corresponding position),
	//  - or     |String| == 1 (The string is displayed n times).
	//
	//
	//Convert the parameters for disp_text.
	if (0 != (HTuple(hv_Row == HTuple()).TupleOr(hv_Column == HTuple())))
	{
		return;
	}
	if (0 != (hv_Row == -1))
	{
		hv_Row = 12;
	}
	if (0 != (hv_Column == -1))
	{
		hv_Column = 12;
	}
	//
	//Convert the parameter Box to generic parameters.
	hv_GenParamName = HTuple();
	hv_GenParamValue = HTuple();
	if (0 != ((hv_Box.TupleLength()) > 0))
	{
		if (0 != (HTuple(hv_Box[0]) == HTuple("false")))
		{
			//Display no box
			hv_GenParamName = hv_GenParamName.TupleConcat("box");
			hv_GenParamValue = hv_GenParamValue.TupleConcat("false");
		}
		else if (0 != (HTuple(hv_Box[0]) != HTuple("true")))
		{
			//Set a color other than the default.
			hv_GenParamName = hv_GenParamName.TupleConcat("box_color");
			hv_GenParamValue = hv_GenParamValue.TupleConcat(HTuple(hv_Box[0]));
		}
	}
	if (0 != ((hv_Box.TupleLength()) > 1))
	{
		if (0 != (HTuple(hv_Box[1]) == HTuple("false")))
		{
			//Display no shadow.
			hv_GenParamName = hv_GenParamName.TupleConcat("shadow");
			hv_GenParamValue = hv_GenParamValue.TupleConcat("false");
		}
		else if (0 != (HTuple(hv_Box[1]) != HTuple("true")))
		{
			//Set a shadow color other than the default.
			hv_GenParamName = hv_GenParamName.TupleConcat("shadow_color");
			hv_GenParamValue = hv_GenParamValue.TupleConcat(HTuple(hv_Box[1]));
		}
	}
	//Restore default CoordSystem behavior.
	if (0 != (hv_CoordSystem != HTuple("window")))
	{
		hv_CoordSystem = "image";
	}
	//
	if (0 != (hv_Color == HTuple("")))
	{
		//disp_text does not accept an empty string for Color.
		hv_Color = HTuple();
	}
	//
	DispText(hv_WindowHandle, hv_String, hv_CoordSystem, hv_Row, hv_Column, hv_Color,
		hv_GenParamName, hv_GenParamValue);
	return;
}


//---------------------------------------------------------------------------------
/*
\brief   向组合框添加串口设备
\return  无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::AddCom(void)
{
	CUIntArray ports;
	CUIntArray portse;
	CUIntArray portsu;
	ports.RemoveAll();
	portse.RemoveAll();
	portsu.RemoveAll();

	// 因为至多有255个串口，所以依次检查各串口是否存在
	// 如果能打开某一串口，或打开串口不成功，但返回的是 ERROR_ACCESS_DENIED 错误信息
	// 都认为串口存在，只不过后者表明串口已经被占用，否则串口不存在
	for (int i = 1; i < 256; i++)
	{
		// Form the Raw device name
		CString sPort;
		sPort.Format(_T("\\\\.\\COM%d"), i);

		// Try to open the port
		BOOL bSuccess = FALSE;
		HANDLE hPort = new HANDLE;
		hPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

		if (hPort == INVALID_HANDLE_VALUE)
		{
			DWORD dwError = GetLastError();
			if (dwError == ERROR_ACCESS_DENIED)
			{
				bSuccess = TRUE;
				portsu.Add(i);       // 已占用的串口
			}
		}
		else
		{
			// The port was opened successfully
			bSuccess = TRUE;
			portse.Add(i);           // 可用的串口
		}

		// Don't forget to close the port, since we are going to do nothing with it anyway
		CloseHandle(hPort);

		// Add the port number to the array which will be returned
		if (bSuccess)
			ports.Add(i);            // 所有存在的串口
	}

	unsigned short uicounter;
	unsigned short uisetcom;
	CString str;

	// 获取存在的串口个数  
	uicounter = (unsigned short)(ports.GetSize());

	// 如果个数大于0  
	if (uicounter > 0)
	{
		// 初始化串口列表框
		for (int i = 0; i < uicounter; i++)
		{
			uisetcom = ports.ElementAt(i);
			str.Format(_T("COM%d "), uisetcom);
			m_ctrlComboComPort.AddString(str);
		}
	}
}


//---------------------------------------------------------------------------------
/*
\brief   刷新串口组合框
\return  无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::RefreshCom(void)
{
	int pos = m_strCurrent.Find('M');
	CString temp = m_strCurrent.Right(pos);           // 找到 COM 后的串口号数字
	int nPort = atoi(temp);
	int count = m_ctrlComboComPort.GetCount();        // 得到当前列表中的项数

	for (int i = 0; i < count; i++)
	{
		m_ctrlComboComPort.DeleteString(count - i - 1);
	}

	AddCom();
	m_ctrlComboComPort.SetCurSel(0);
}


//---------------------------------------------------------------------------------
/*
\brief   将字符转换为十六进制
\return  c：转换后的数据本身
*/
//---------------------------------------------------------------------------------
char CRobotControlDlg::HexChar(char c)
{
	if ((c >= '0') && (c <= '9'))
		return c - 0x30;
	else if ((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	else if ((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	else
		return 0x10;
}


//---------------------------------------------------------------------------------
/*
\brief   字符串转换为十六进制函数
\return  rlen：转换后的数据长度
*/
//---------------------------------------------------------------------------------
int CRobotControlDlg::Str2Hex(CString str, char* data)
{
	int t, t1;
	int rlen = 0, len = str.GetLength();

	for (int i = 0; i < len;)
	{
		char l, h = str[i];

		if (h == ' ')
		{
			i++;
			continue;
		}

		i++;

		if (i >= len)
			break;

		l  = str[i];
		t  = HexChar(h);      // 高位转换
		t1 = HexChar(l);      // 低位转换

		if ((t == 16) || (t1 == 16))
			break;
		else
			t = t * 16 + t1;

		i++;
		data[rlen] = (BYTE)t;
		rlen++;
	}
	return rlen;
}


//---------------------------------------------------------------------------------
/*
\brief   关闭应用程序函数
\return  无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::OnClose()
{
	if (MessageBox(_T("确定要退出程序吗？"), _T("退出提示"), MB_ICONINFORMATION | MB_YESNO) == IDNO)
		return;   // 注意无返回值

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// 若未停采则先停止采集
	if (m_bIsSnap)
	{
		// 发送停止采集命令
		emStatus  = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);

		// 注销回调
		emStatus  = GXUnregisterCaptureCallback(m_hDevice);

		m_bIsSnap = FALSE;

		// 释放为采集图像开辟的图像 Buffer
		UnPrepareForShowImg();
	}

	// 释放 pDC
	::ReleaseDC(m_pWnd->m_hWnd, m_hDC);

	// 未关闭设备则关闭设备
	if (m_bDevOpened)
	{
		// 关闭设备
		emStatus     = GXCloseDevice(m_hDevice);
		m_bDevOpened = FALSE;
		m_hDevice    = NULL;
	}

	// 关闭设备库
	emStatus = GXCloseLib();

	// 关闭串口
	m_SerialPort.ClosePort();

	//// 终止线程1
	//TerminateThread(m_Thread1, 0);

	//// 终止线程2
	//TerminateThread(m_Thread2, 0);

	//// 终止线程3
	//TerminateThread(m_Thread3, 0);

	//// 终止线程4
	//TerminateThread(m_Thread4, 0);

	//// 终止线程5
	//TerminateThread(m_Thread5, 0);

	//// 终止线程6
	//TerminateThread(m_Thread6, 0);

	//// 终止线程7
	//TerminateThread(m_Thread7, 0);

	//// 终止线程8
	//TerminateThread(m_Thread8, 0);

	//// 终止线程9
	//TerminateThread(m_Thread9, 0);

	//// 终止线程
	//ExitThread(0);

	//// 终止线程
	//_endthreadex(0);

	CDialogEx::OnClose();
}


//---------------------------------------------------------------------------------
/*
\brief   采集图像回调函数
\param   pFrame：回调参数
\return  无
*/
//---------------------------------------------------------------------------------
void __stdcall CRobotControlDlg::OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM* pFrame)
{
	CRobotControlDlg* pDlg = (CRobotControlDlg*)(pFrame->pUserParam);
	VxUint32 nImageHeight = (VxUint32)pDlg->m_nImageHeight;
	VxUint32 nImageWidth  = (VxUint32)pDlg->m_nImageWidth;

	// 当前帧图像的返回状态的判断
	if (pFrame->status != 0)
	{
		return;
	}

	if (pFrame->status == 0)
	{
		// 将回调中的图像数据拷贝出来
		memcpy(pDlg->m_pBufferRaw, pFrame->pImgBuf, pFrame->nImgSize);

		// 黑白相机需要翻转数据后显示
		for (VxUint32 i = 0; i < nImageHeight; i++)
		{
			memcpy(pDlg->m_pImageBuffer + i * nImageWidth, pDlg->m_pBufferRaw + (nImageHeight - i - 1) * nImageWidth, (size_t)nImageWidth);
		}

		// 显示图像
		pDlg->DrawImage(pDlg->m_pImageBuffer, nImageWidth, nImageHeight, pDlg->m_pBmpInfo);
	}
}


//---------------------------------------------------------------------------------
/**
\brief   将 m_pImageBuffer 中图像显示到界面
\return  无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::DrawImage(BYTE* pImageBuf, int nWidth, int nHeight, BITMAPINFO* pBmpInfo)
{
	// 显示图像的窗口大小                                      
	int nWndWidth  = m_objRect.right - m_objRect.left;      // 图像显示窗口宽度
	int nWndHeight = m_objRect.bottom - m_objRect.top;      // 图像显示窗口高度

	// 必须调用该语句，否则图像出现水纹
	::SetStretchBltMode(m_hDC, COLORONCOLOR);
	::StretchDIBits(m_hDC, 0, 0, nWndWidth, nWndHeight, 0, 0, (int)m_nImageWidth, (int)m_nImageHeight, m_pImageBuffer, m_pBmpInfo, DIB_RGB_COLORS, SRCCOPY);
}


//---------------------------------------------------------------------------------
/*
\brief   释放为图像显示准备的资源
\return  无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::UnPrepareForShowImg()
{
	if (m_pBufferRaw != NULL)
	{
		delete[]m_pBufferRaw;
		m_pBufferRaw = NULL;
	}

	if (m_pImageBuffer != NULL)
	{
		delete[]m_pImageBuffer;
		m_pImageBuffer = NULL;
	}
}


//---------------------------------------------------------------------------------
/*
\brief  刷新 UI 界面控件使能状态
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::UpDateUI()
{
	GetDlgItem(IDC_OPEN) ->EnableWindow(!m_bDevOpened);
	GetDlgItem(IDC_CLOSE)->EnableWindow(m_bDevOpened);
	GetDlgItem(IDC_START)->EnableWindow(m_bDevOpened && !m_bIsSnap);
	GetDlgItem(IDC_STOP) ->EnableWindow(m_bDevOpened && m_bIsSnap);
	GetDlgItem(IDC_EXPOSURE)->EnableWindow(m_bDevOpened && m_bIsSnap);
	GetDlgItem(IDC_GAIN)->EnableWindow(m_bDevOpened && m_bIsSnap);
	GetDlgItem(IDC_EDITEXPOSURE)->EnableWindow(m_bDevOpened && m_bIsSnap);
	GetDlgItem(IDC_EDITGAIN)->EnableWindow(m_bDevOpened && m_bIsSnap);

	GetDlgItem(IDC_SERIAL)->EnableWindow(!m_bSerialOpened);
	GetDlgItem(IDC_UNSERIAL)->EnableWindow(m_bSerialOpened);
	GetDlgItem(IDC_DATA)->EnableWindow(m_bSerialOpened);
	GetDlgItem(IDC_CLEAR)->EnableWindow(m_bSerialOpened);
	GetDlgItem(IDC_RECTANGLE)->EnableWindow(m_bSerialOpened);
	GetDlgItem(IDC_CIRCULAR)->EnableWindow(m_bSerialOpened);
	GetDlgItem(IDC_TRIANGLE)->EnableWindow(m_bSerialOpened);
	GetDlgItem(IDC_PENTAGON)->EnableWindow(m_bSerialOpened);
}


//---------------------------------------------------------------------------------
/*
\brief  初始化 UI 界面
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::InitUI()
{
	CEdit* pEditExposure = (CEdit*)GetDlgItem(IDC_EDITEXPOSURE);
	CEdit* pEditGain = (CEdit*)GetDlgItem(IDC_EDITGAIN);

	// 判断控件的有效性
	if ((pEditExposure == NULL) || (pEditGain == NULL))
	{
		return;
	}

	GX_STATUS emStatus1 = GX_STATUS_ERROR;
	GX_STATUS emStatus2 = GX_STATUS_ERROR;
	double   dExValue   = 0.0;
	double   dGainVal   = 0.0;

	// 获取当前曝光值
	emStatus1   = GXGetFloat(m_hDevice, GX_FLOAT_EXPOSURE_TIME, &dExValue);
	m_dExposure = dExValue;

	// 获取当前增益值
	emStatus2 = GXGetFloat(m_hDevice, GX_FLOAT_GAIN, &dGainVal);
	m_dGain   = dGainVal;

	// 初始化滑动条的范围
	m_sliderExposure.SetRange(0, 1000000, TRUE);
	m_sliderGain.SetRange(0, 17, TRUE);

	// 初始化设滑动条的位置
	m_sliderExposure.SetPos((int)m_dExposure);
	m_sliderGain.SetPos((int)m_dGain);

	// 将变量中的数据交换至控件中
	UpdateData(FALSE);
}


//---------------------------------------------------------------------------------
/*
\brief  曝光值 Edit 框失去焦点的响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Edit_Exposure()
{
	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		return;
	}

	GX_STATUS status = GX_STATUS_SUCCESS;

	// 将控件中的数据交换至变量中
	UpdateData(TRUE);

	// 判断输入值是否在曝光时间的范围内
	// 若大于最大值则将曝光值设为最大值
	if (m_dExposure > 1000000.00)
	{
		m_dExposure = 1000000.00;
	}

	// 若小于最小值将曝光值设为最小值
	if (m_dExposure < 36.00)
	{
		m_dExposure = 36.00;
	}

	// 设置曝光时间
	status = GXSetFloat(m_hDevice, GX_FLOAT_EXPOSURE_TIME, m_dExposure);

	// 将变量中的数据交换至控件中
	UpdateData(FALSE);

	// 设置曝光滑动条的位置
	m_sliderExposure.SetPos((int)m_dExposure);
}


//---------------------------------------------------------------------------------
/*
\brief  增益值 Edit 框失去焦点的响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Edit_Gain()
{
	// 判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		return;
	}

	GX_STATUS status = GX_STATUS_SUCCESS;

	// 将控件中的数据交换至变量中
	UpdateData(TRUE);

	// 判断输入值是否在增益值的范围内
	// 若输入的值大于最大值则将增益值设置成最大值
	if (m_dGain > 17.00)
	{
		m_dGain = 17.00;
	}

	// 若输入的值小于最小值则将增益的值设置成最小值
	if (m_dGain < 0.00)
	{
		m_dGain = 0.00;
	}

	// 设置增益
	status = GXSetFloat(m_hDevice, GX_FLOAT_GAIN, m_dGain);

	// 将变量中的数据交换至控件中
	UpdateData(FALSE);

	// 设置增益滑动条的位置
	m_sliderGain.SetPos((int)m_dGain);
}


//---------------------------------------------------------------------------------
/*
\brief  获取设备的宽高等属性信息
\return GX_STATUS_SUCCESS：全部获取成功
        其它状态码：未成功获取全部
*/
//---------------------------------------------------------------------------------
GX_STATUS CRobotControlDlg::InitDeviceParam()
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// 获取图像大小
	emStatus = GXGetInt(m_hDevice, GX_INT_PAYLOAD_SIZE, &m_nPayLoadSize);

	// 获取图像宽度
	emStatus = GXGetInt(m_hDevice, GX_INT_WIDTH, &m_nImageWidth);

	// 获取图像高度
	emStatus = GXGetInt(m_hDevice, GX_INT_HEIGHT, &m_nImageHeight);

	return emStatus;
}


//---------------------------------------------------------------------------------
/*
\brief  为保存图像分配 Buffer ,为图像显示准备资源
\return true  (图像显示准备资源成功)  
        false (准备资源失败)
*/
//---------------------------------------------------------------------------------
bool CRobotControlDlg::PrepareForShowImg()
{
	// 初始化 bitmap 头
	m_pBmpInfo                              = (BITMAPINFO*)m_chBmpBuf;
	m_pBmpInfo->bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
	m_pBmpInfo->bmiHeader.biWidth           = (LONG)m_nImageWidth;
	m_pBmpInfo->bmiHeader.biHeight          = (LONG)m_nImageHeight;

	m_pBmpInfo->bmiHeader.biPlanes          = 1;
	m_pBmpInfo->bmiHeader.biBitCount        = 8; // 黑白图像为8
	m_pBmpInfo->bmiHeader.biCompression     = BI_RGB;
	m_pBmpInfo->bmiHeader.biSizeImage       = 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter   = 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter   = 0;
	m_pBmpInfo->bmiHeader.biClrUsed         = 0;
	m_pBmpInfo->bmiHeader.biClrImportant    = 0;

	// 黑白图像需要初始化调色板
	for (int i = 0; i < 256; i++)
	{
		m_pBmpInfo->bmiColors[i].rgbBlue     = i;
		m_pBmpInfo->bmiColors[i].rgbGreen    = i;
		m_pBmpInfo->bmiColors[i].rgbRed      = i;
		m_pBmpInfo->bmiColors[i].rgbReserved = i;
	}

	// 为原始图像数据分配空间
	m_pBufferRaw = new BYTE[(size_t)m_nPayLoadSize];
	if (m_pBufferRaw == NULL)
	{
		return false;
	}

	// 为经过翻转后的图像数据分配空间
	m_pImageBuffer = new BYTE[(size_t)(m_nImageWidth * m_nImageHeight)];
	if (m_pImageBuffer == NULL)
	{
		delete[]m_pBufferRaw;
		m_pBufferRaw = NULL;

		return false;
	}

	return true;
}


//---------------------------------------------------------------------------------
/*
\brief  点击"打开设备"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Open_Device()
{
	GX_STATUS      emStatus = GX_STATUS_SUCCESS;
	uint32_t       nDevNum  = 0;
	GX_OPEN_PARAM  stOpenParam;

	// 初始化打开设备用参数,默认打开序号为1的设备
	stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
	stOpenParam.openMode   = GX_OPEN_INDEX;
	stOpenParam.pszContent = "1";

	// 枚举设备个数
	emStatus = GXUpdateDeviceList(&nDevNum, 1000);

	// 判断设备个数
	if (nDevNum <= 0)
	{
		AfxMessageBox("没有发现设备！");
		return;
	}

	// 如果设备已经打开则关闭,保证相机在初始化出错情况能再次打开
	if (m_hDevice != NULL)
	{
		emStatus  = GXCloseDevice(m_hDevice);
		m_hDevice = NULL;
	}

	// 打开设备
	emStatus = GXOpenDevice(&stOpenParam, &m_hDevice);
	m_bDevOpened = TRUE;
	AfxMessageBox("设备打开成功！");

	// 初始化参数
	emStatus = InitDeviceParam();

	// 初始化 UI 界面
	InitUI();

	// 更新 UI 界面
	UpDateUI();
}


//---------------------------------------------------------------------------------
/*
\brief  点击"关闭设备"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Close_Device()
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// 未停采时则停止采集
	if (m_bIsSnap)
	{
		// 发送停止采集命令
		emStatus  = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);

		// 注销回调
		emStatus  = GXUnregisterCaptureCallback(m_hDevice);

		m_bIsSnap = FALSE;

		// 释放为采集图像开辟的图像 Buffer
		UnPrepareForShowImg();
	}

	// 关闭设备
	emStatus     = GXCloseDevice(m_hDevice);

	m_bDevOpened = FALSE;
	m_hDevice    = NULL;
	AfxMessageBox("设备关闭成功！");

	// 更新 UI 界面
	UpDateUI();
}


//---------------------------------------------------------------------------------
/*
\brief  点击"开始采集"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Start_Snap()
{
	GX_STATUS emStatus = GX_STATUS_ERROR;

	// 为图像显示准备资源
	if (!PrepareForShowImg())
	{
		return;
	}

	// 注册回调
	emStatus = GXRegisterCaptureCallback(m_hDevice, this, OnFrameCallbackFun);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		UnPrepareForShowImg();
		return;
	}

	// 发开始采集命令
	emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_START);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		UnPrepareForShowImg();
		return;
	}

	m_bIsSnap = TRUE;
	AfxMessageBox("设备开始采集！");

	// 更新 UI 界面
	UpDateUI();
}


//---------------------------------------------------------------------------------
/*
\brief  点击"停止采集"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Stop_Snap()
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// 发送停止采集命令
	emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);

	// 注销回调
	emStatus = GXUnregisterCaptureCallback(m_hDevice);

	m_bIsSnap = FALSE;
	AfxMessageBox("设备停止采集！");

	// 释放为采集图像开辟的图像 Buffer
	UnPrepareForShowImg();

	// 更新 UI 界面
	UpDateUI();
}


//---------------------------------------------------------------------------------
/*
\brief  点击"打开串口"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Open_Serial()
{
	int n = m_ctrlComboComPort.GetCurSel();      // 得到当前所选项的索引

	if(n != -1)
	{
		CString str;
		m_ctrlComboComPort.GetLBText(n, str);    // 得到当前索引的内容
		int pos = str.Find('M');
		CString temp = str.Right(pos);           // 找到 COM 后的串口号数字
		int nPort = atoi(temp);

		if (m_SerialPort.InitPort(this, nPort, 9600, 'N', 8, 1, EV_RXFLAG | EV_RXCHAR, 512))
		{
			m_SerialPort.StartMonitoring();
			AfxMessageBox("串口打开成功！");
			m_bSerialOpened = TRUE;
			m_strCurrent = str;
			UpdateData(FALSE);
		}
		else
		{
			AfxMessageBox("没有发现此串口或者已经被占用！");
			m_bSerialOpened = FALSE;
		}
	}
	else
	{
		AfxMessageBox("没有发现此串口或者已经被占用！");
		m_bSerialOpened = FALSE;
	}

	// 更新 UI 界面
	UpDateUI();
}


//---------------------------------------------------------------------------------
/*
\brief  点击"发送数据"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Send_Data()
{
	if (!m_bSerialOpened)
		return;
	UpdateData(TRUE);
	int len = m_strEditSendMsg.GetLength();
	char* data = new char[len];
	len = Str2Hex(m_strEditSendMsg, data);
	m_SerialPort.WriteToPort(data, len);
	m_strEditSendMsg = "";
	UpdateData(FALSE);
	delete[] data;
}


//---------------------------------------------------------------------------------
/*
\brief  点击"关闭串口"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Close_Serial()
{
	m_SerialPort.ClosePort();
	AfxMessageBox("串口已经关闭！");
	m_bSerialOpened = FALSE;
	m_strCurrent = "";
	UpdateData(FALSE);

	// 更新 UI 界面
	UpDateUI();
}


//---------------------------------------------------------------------------------
/*
\brief  点击"清除数据"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Clear_Data()
{
	if (!m_bSerialOpened)
		return;
	m_strEditReceiveMsg = "";
	m_strEditSendMsg = "";
	AfxMessageBox("数据已经清除！");
	UpdateData(FALSE);
}


//---------------------------------------------------------------------------------
/*
\brief  点击"抓取矩形"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Rectangle()
{
	if (!m_bSerialOpened)
		return;

	if (m_bIsSnap == TRUE)
	{
		GX_STATUS emStatus = GX_STATUS_SUCCESS;

		// 发送停止采集命令
		emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);

		// 注销回调
		emStatus = GXUnregisterCaptureCallback(m_hDevice);

		m_bIsSnap = FALSE;

		// 释放为采集图像开辟的图像 Buffer
		UnPrepareForShowImg();

		// 更新 UI 界面
		UpDateUI();
	}

	// 读取图片
	ReadImage(&ho_Image1, "C:/Users/86136/Desktop/1.bmp");
	DispObj(ho_Image1, hv_WindowHandle);

	// 清空进度条
	m_Progress.SetPos(0);

	// 清空显示
	disp_message(hv_WindowHandle, "                                                          ", "window", 0, 0, "red", "true");

	// 在 Picture Control 中绘制矩形框
	SetColor(hv_WindowHandle, "red");
	DrawRectangle2(hv_WindowHandle, &hv_Row1, &hv_Column1, &hv_Phi1, &hv_Width, &hv_Height);

	// 显示正在处理
	disp_message(hv_WindowHandle, "        Object recognition program is in progress!        ", "window", 0, 0, "red", "true");

	// 创建线程1
	m_Thread1 = CreateThread(NULL, 0, ThreadProc1, this, 0, NULL);

	// 创建线程5
	m_Thread5 = CreateThread(NULL, 0, ThreadProc5, this, 0, NULL);

	// 关闭线程句柄1
	//CloseHandle(m_Thread1);

	// 关闭线程句柄5
	//CloseHandle(m_Thread5);
}


//---------------------------------------------------------------------------------
/*
\brief  线程函数1实现（抓取矩形）
\return 无
*/
//---------------------------------------------------------------------------------
DWORD WINAPI CRobotControlDlg::ThreadProc1(LPVOID lpParameter)
{
	CRobotControlDlg* pDlg = (CRobotControlDlg*)lpParameter;

	// 设置进度条参数
	pDlg->m_Progress.SetRange(0, 100);                   // 设置进度条的范围
	pDlg->m_Progress.SetBkColor(RGB(255, 255, 255));     // 设置进度条的背景色
	pDlg->m_Progress.SetBarColor(RGB(0, 255, 0));        // 设置进度条的颜色

	for (int i = 0; i <= 100; i++)
	{
		pDlg->m_Progress.SetPos(i);                      // 设置进度条的位置
		CString str;
		str.Format(_T("%d%%"), i);
		pDlg->m_Percentage.SetWindowText(str);           // 显示进度条的百分比
		if(i <= 20)
			Sleep(20);
		if (i > 20 && i <= 40)
			Sleep(80);
		if (i > 40 && i <= 60)
			Sleep(60);
		if (i > 60 && i <= 80)
			Sleep(40);
		if (i > 80 && i <= 100)
			Sleep(100);
	}

	// 在 Picture Control 中标注信息
	pDlg->str1.Format("This is a Rectangle! Center Point Row: %d, Column: %d       ", (int)pDlg->hv_Row1[0].D(), (int)pDlg->hv_Column1[0].D());
	pDlg->disp_message(pDlg->hv_WindowHandle, pDlg->str1.GetBuffer(), "window", 0, 0, "red", "true");

	// 向机械手发送指令控制其抓取矩形
	char* data = new char[10];
	int len = pDlg->Str2Hex("55 55 05 06 01 01 00", data);
	pDlg->m_SerialPort.WriteToPort(data, len);
	delete[] data;

	return 0;
}


//---------------------------------------------------------------------------------
/*
\brief  线程函数5实现（矩形识别）
\return 无
*/
//---------------------------------------------------------------------------------
DWORD WINAPI CRobotControlDlg::ThreadProc5(LPVOID lpParameter)
{
	CRobotControlDlg* pDlg = (CRobotControlDlg*)lpParameter;

	// 将绘制的矩形框提取出来
	GenRectangle2(&pDlg->ho_Rectangle1, pDlg->hv_Row1, pDlg->hv_Column1, pDlg->hv_Phi1, pDlg->hv_Width, pDlg->hv_Height);
	DispObj(pDlg->ho_Rectangle1, pDlg->hv_WindowHandle);
	Sleep(1500);

	// 将图像转换成区域
	ReduceDomain(pDlg->ho_Image1, pDlg->ho_Rectangle1, &pDlg->ho_ImageReduced1);
	DispObj(pDlg->ho_ImageReduced1, pDlg->hv_WindowHandle);
	Sleep(1500);

	// 阈值分割
	Threshold(pDlg->ho_ImageReduced1, &pDlg->ho_Regions1, 140, 255);
	DispObj(pDlg->ho_Regions1, pDlg->hv_WindowHandle);

	return 0;
}


//---------------------------------------------------------------------------------
/*
\brief  点击"抓取圆形"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Circular()
{
	if (!m_bSerialOpened)
		return;

	if (m_bIsSnap == TRUE)
	{
		GX_STATUS emStatus = GX_STATUS_SUCCESS;

		// 发送停止采集命令
		emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);

		// 注销回调
		emStatus = GXUnregisterCaptureCallback(m_hDevice);

		m_bIsSnap = FALSE;

		// 释放为采集图像开辟的图像 Buffer
		UnPrepareForShowImg();

		// 更新 UI 界面
		UpDateUI();
	}

	// 读取图片
	ReadImage(&ho_Image2, "C:/Users/86136/Desktop/3.bmp");
	DispObj(ho_Image2, hv_WindowHandle);

	// 清空进度条
	m_Progress.SetPos(0);

	// 清空显示
	disp_message(hv_WindowHandle, "                                                          ", "window", 0, 0, "blue", "true");

	// 在 Picture Control 中绘制圆形框
	SetColor(hv_WindowHandle, "blue");
	DrawEllipse(hv_WindowHandle, &hv_Row2, &hv_Column2, &hv_Phi2, &hv_Radius1, &hv_Radius2);

	// 显示正在处理
	disp_message(hv_WindowHandle, "        Object recognition program is in progress!        ", "window", 0, 0, "blue", "true");

	// 创建线程2
	m_Thread2 = CreateThread(NULL, 0, ThreadProc2, this, 0, NULL);

	// 创建线程6
	m_Thread6 = CreateThread(NULL, 0, ThreadProc6, this, 0, NULL);

	// 关闭线程句柄2
	//CloseHandle(m_Thread2);

	// 关闭线程句柄6
	//CloseHandle(m_Thread6);
}


//---------------------------------------------------------------------------------
/*
\brief  线程函数2实现（抓取圆形）
\return 无
*/
//---------------------------------------------------------------------------------
DWORD WINAPI CRobotControlDlg::ThreadProc2(LPVOID lpParameter)
{
	CRobotControlDlg* pDlg = (CRobotControlDlg*)lpParameter;

	// 设置进度条参数
	pDlg->m_Progress.SetRange(0, 100);                   // 设置进度条的范围
	pDlg->m_Progress.SetBkColor(RGB(255, 255, 255));     // 设置进度条的背景色
	pDlg->m_Progress.SetBarColor(RGB(0, 255, 0));        // 设置进度条的颜色

	for (int i = 0; i <= 100; i++)
	{
		pDlg->m_Progress.SetPos(i);                      // 设置进度条的位置
		CString str;
		str.Format(_T("%d%%"), i);
		pDlg->m_Percentage.SetWindowText(str);           // 显示进度条的百分比
		if (i <= 20)
			Sleep(60);
		if (i > 20 && i <= 40)
			Sleep(20);
		if (i > 40 && i <= 60)
			Sleep(100);
		if (i > 60 && i <= 80)
			Sleep(80);
		if (i > 80 && i <= 100)
			Sleep(40);
	}

	// 在 Picture Control 中标注信息
	pDlg->str1.Format("This is a Circle! Center Point Row: %d, Column: %d          ", (int)pDlg->hv_Row2[0].D(), (int)pDlg->hv_Column2[0].D());
	pDlg->disp_message(pDlg->hv_WindowHandle, pDlg->str1.GetBuffer(), "window", 0, 0, "blue", "true");

	// 向机械手发送指令控制其抓取圆形
	char* data = new char[10];
	int len = pDlg->Str2Hex("55 55 05 06 02 01 00", data);
	pDlg->m_SerialPort.WriteToPort(data, len);
	delete[] data;

	return 0;
}


//---------------------------------------------------------------------------------
/*
\brief  线程函数6实现（圆形识别）
\return 无
*/
//---------------------------------------------------------------------------------
DWORD WINAPI CRobotControlDlg::ThreadProc6(LPVOID lpParameter)
{
	CRobotControlDlg* pDlg = (CRobotControlDlg*)lpParameter;

	// 将绘制的圆形框提取出来
	GenEllipse(&pDlg->ho_Ellipse, pDlg->hv_Row2, pDlg->hv_Column2, pDlg->hv_Phi2, pDlg->hv_Radius1, pDlg->hv_Radius2);
	DispObj(pDlg->ho_Ellipse, pDlg->hv_WindowHandle);
	Sleep(1500);

	// 将图像转换成区域
	ReduceDomain(pDlg->ho_Image2, pDlg->ho_Ellipse, &pDlg->ho_ImageReduced2);
	DispObj(pDlg->ho_ImageReduced2, pDlg->hv_WindowHandle);
	Sleep(1500);

	// 阈值分割
	Threshold(pDlg->ho_ImageReduced2, &pDlg->ho_Regions2, 110, 255);
	DispObj(pDlg->ho_Regions2, pDlg->hv_WindowHandle);

	return 0;
}


//---------------------------------------------------------------------------------
/*
\brief  点击"抓取三角形"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Triangle()
{
	if (!m_bSerialOpened)
		return;

	if (m_bIsSnap == TRUE)
	{
		GX_STATUS emStatus = GX_STATUS_SUCCESS;

		// 发送停止采集命令
		emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);

		// 注销回调
		emStatus = GXUnregisterCaptureCallback(m_hDevice);

		m_bIsSnap = FALSE;

		// 释放为采集图像开辟的图像 Buffer
		UnPrepareForShowImg();

		// 更新 UI 界面
		UpDateUI();
	}

	// 读取图片
	ReadImage(&ho_Image3, "C:/Users/86136/Desktop/2.bmp");
	DispObj(ho_Image3, hv_WindowHandle);

	// 清空进度条
	m_Progress.SetPos(0);

	// 清空显示
	disp_message(hv_WindowHandle, "                                                          ", "window", 0, 0, "red", "true");

	// 在 Picture Control 中标记三角形
	SetColor(hv_WindowHandle, "red");
	DrawRectangle1(hv_WindowHandle, &hv_Row3, &hv_Column3, &hv_Row4, &hv_Column4);

	// 清空显示
	disp_message(hv_WindowHandle, "        Object recognition program is in progress!        ", "window", 0, 0, "red", "true");

	// 创建线程3
	m_Thread3 = CreateThread(NULL, 0, ThreadProc3, this, 0, NULL);

	// 创建线程7
	m_Thread7 = CreateThread(NULL, 0, ThreadProc7, this, 0, NULL);

	// 创建线程9
	m_Thread9 = CreateThread(NULL, 0, ThreadProc9, this, 0, NULL);

	// 关闭线程句柄3
	//CloseHandle(m_Thread3);

	// 关闭线程句柄7
	//CloseHandle(m_Thread7);
}


//---------------------------------------------------------------------------------
/*
\brief  线程函数3实现（抓取三角形）
\return 无
*/
//---------------------------------------------------------------------------------
DWORD WINAPI CRobotControlDlg::ThreadProc3(LPVOID lpParameter)
{
	CRobotControlDlg* pDlg = (CRobotControlDlg*)lpParameter;

	// 设置进度条参数
	pDlg->m_Progress.SetRange(0, 100);                   // 设置进度条的范围
	pDlg->m_Progress.SetBkColor(RGB(255, 255, 255));     // 设置进度条的背景色
	pDlg->m_Progress.SetBarColor(RGB(0, 255, 0));        // 设置进度条的颜色

	for (int i = 0; i <= 100; i++)
	{
		pDlg->m_Progress.SetPos(i);                      // 设置进度条的位置
		CString str;
		str.Format(_T("%d%%"), i);
		pDlg->m_Percentage.SetWindowText(str);           // 显示进度条的百分比
		if (i <= 20)
			Sleep(100);
		if (i > 20 && i <= 40)
			Sleep(60);
		if (i > 40 && i <= 60)
			Sleep(20);
		if (i > 60 && i <= 80)
			Sleep(40);
		if (i > 80 && i <= 100)
			Sleep(80);
	}

	// 在 Picture Control 中标注信息
	pDlg->str1.Format("This is a Triangle! Center Point Row: %d, Column: %d        ", ((int)pDlg->hv_Row3[0].D() + (int)pDlg->hv_Row4[0].D()) / 2, ((int)pDlg->hv_Column3[0].D() + (int)pDlg->hv_Column4[0].D()) / 2);
	pDlg->disp_message(pDlg->hv_WindowHandle, pDlg->str1.GetBuffer(), "window", 0, 0, "red", "true");

	// 向机械手发送指令控制其抓取三角形
	char* data = new char[10];
	int len = pDlg->Str2Hex("55 55 05 06 03 01 00", data);
	pDlg->m_SerialPort.WriteToPort(data, len);
	delete[] data;

	return 0;
}


//---------------------------------------------------------------------------------
/*
\brief  线程函数7实现（三角形识别）
\return 无
*/
//---------------------------------------------------------------------------------
DWORD WINAPI CRobotControlDlg::ThreadProc7(LPVOID lpParameter)
{
	CRobotControlDlg* pDlg = (CRobotControlDlg*)lpParameter;

	// 将绘制的三角形提取出来
	GenRectangle1(&pDlg->ho_Rectangle2, pDlg->hv_Row3, pDlg->hv_Column3, pDlg->hv_Row4, pDlg->hv_Column4);
	DispObj(pDlg->ho_Rectangle2, pDlg->hv_WindowHandle);
	Sleep(1500);

	// 将图像转换成区域
	ReduceDomain(pDlg->ho_Image3, pDlg->ho_Rectangle2, &pDlg->ho_ImageReduced3);
	DispObj(pDlg->ho_ImageReduced3, pDlg->hv_WindowHandle);
	Sleep(1500);

	// 阈值分割
	Threshold(pDlg->ho_ImageReduced3, &pDlg->ho_Regions3, 170, 255);
	DispObj(pDlg->ho_Regions3, pDlg->hv_WindowHandle);

	Sleep(30000);

	// 读取图片
	ReadImage(&pDlg->ho_Image4, "C:/Users/86136/Desktop/3.bmp");
	DispObj(pDlg->ho_Image4, pDlg->hv_WindowHandle);

	return 0;
}


//---------------------------------------------------------------------------------
/*
\brief  点击"抓取五边形"控件响应函数
\return 无
*/
//---------------------------------------------------------------------------------
void CRobotControlDlg::Pentagon()
{
	if (!m_bSerialOpened)
		return;

	if (m_bIsSnap == TRUE)
	{
		GX_STATUS emStatus = GX_STATUS_SUCCESS;

		// 发送停止采集命令
		emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);

		// 注销回调
		emStatus = GXUnregisterCaptureCallback(m_hDevice);

		m_bIsSnap = FALSE;

		// 释放为采集图像开辟的图像 Buffer
		UnPrepareForShowImg();

		// 更新 UI 界面
		UpDateUI();
	}

	// 读取图片
	ReadImage(&ho_Image4, "C:/Users/86136/Desktop/4.bmp");
	DispObj(ho_Image4, hv_WindowHandle);

	// 清空进度条
	m_Progress.SetPos(0);

	// 清空显示
	disp_message(hv_WindowHandle, "                                                          ", "window", 0, 0, "blue", "true");

	// 在 Picture Control 中标记五边形
	SetColor(hv_WindowHandle, "blue");
	DrawCircle(hv_WindowHandle, &hv_Row5, &hv_Column5, &hv_Radius);

	// 清空显示
	disp_message(hv_WindowHandle, "        Object recognition program is in progress!        ", "window", 0, 0, "blue", "true");

	// 创建线程4
	m_Thread4 = CreateThread(NULL, 0, ThreadProc4, this, 0, NULL);

	// 创建线程8
	m_Thread8 = CreateThread(NULL, 0, ThreadProc8, this, 0, NULL);

	// 关闭线程句柄4
	//CloseHandle(m_Thread4);

	// 关闭线程句柄8
	//CloseHandle(m_Thread8);
}


//---------------------------------------------------------------------------------
/*
\brief  线程函数4实现（抓取五边形）
\return 无
*/
//---------------------------------------------------------------------------------
DWORD WINAPI CRobotControlDlg::ThreadProc4(LPVOID lpParameter)
{
	CRobotControlDlg* pDlg = (CRobotControlDlg*)lpParameter;

	// 设置进度条参数
	pDlg->m_Progress.SetRange(0, 100);                   // 设置进度条的范围
	pDlg->m_Progress.SetBkColor(RGB(255, 255, 255));     // 设置进度条的背景色
	pDlg->m_Progress.SetBarColor(RGB(0, 255, 0));        // 设置进度条的颜色

	for (int i = 0; i <= 100; i++)
	{
		pDlg->m_Progress.SetPos(i);                      // 设置进度条的位置
		CString str;
		str.Format(_T("%d%%"), i);
		pDlg->m_Percentage.SetWindowText(str);           // 显示进度条的百分比
		if (i <= 20)
			Sleep(80);
		if (i > 20 && i <= 40)
			Sleep(20);
		if (i > 40 && i <= 60)
			Sleep(60);
		if (i > 60 && i <= 80)
			Sleep(100);
		if (i > 80 && i <= 100)
			Sleep(40);
	}

	// 在 Picture Control 中标注信息
	pDlg->str1.Format("This is a Pentagon! Center Point Row: %d, Column: %d        ", (int)pDlg->hv_Row5[0].D(), (int)pDlg->hv_Column5[0].D());
	pDlg->disp_message(pDlg->hv_WindowHandle, pDlg->str1.GetBuffer(), "window", 0, 0, "blue", "true");

	// 向机械手发送指令控制其抓取五边形
	char* data = new char[10];
	int len = pDlg->Str2Hex("55 55 05 06 04 01 00", data);
	pDlg->m_SerialPort.WriteToPort(data, len);
	delete[] data;

	return 0;
}


//---------------------------------------------------------------------------------
/*
\brief  线程函数8实现（五边形识别）
\return 无
*/
//---------------------------------------------------------------------------------
DWORD WINAPI CRobotControlDlg::ThreadProc8(LPVOID lpParameter)
{
	CRobotControlDlg* pDlg = (CRobotControlDlg*)lpParameter;

	// 将绘制的五边形提取出来
	GenCircle(&pDlg->ho_Circle, pDlg->hv_Row5, pDlg->hv_Column5, pDlg->hv_Radius);
	DispObj(pDlg->ho_Circle, pDlg->hv_WindowHandle);
	Sleep(1500);

	// 将图像转换成区域
	ReduceDomain(pDlg->ho_Image4, pDlg->ho_Circle, &pDlg->ho_ImageReduced4);
	DispObj(pDlg->ho_ImageReduced4, pDlg->hv_WindowHandle);
	Sleep(1500);

	// 阈值分割
	Threshold(pDlg->ho_ImageReduced4, &pDlg->ho_Regions4, 110, 255);
	DispObj(pDlg->ho_Regions4, pDlg->hv_WindowHandle);

	return 0;
}


//---------------------------------------------------------------------------------
/*
\brief  线程函数9实现（语音识别）
\return 无
*/
//---------------------------------------------------------------------------------
DWORD WINAPI CRobotControlDlg::ThreadProc9(LPVOID lpParameter)
{
	CRobotControlDlg* pDlg = (CRobotControlDlg*)lpParameter;

	while (1)
	{
		if (pDlg->m_strEditReceiveMsg == "您好！请问有什么吩咐？\r\n给您Rectangle！")
		{
			// 向机械手发送指令控制其抓取矩形
			char* data = new char[10];
			int len = pDlg->Str2Hex("55 55 05 06 01 01 00", data);
			pDlg->m_SerialPort.WriteToPort(data, len);
			delete[] data;
		}

		if (pDlg->m_strEditReceiveMsg == "您好！请问有什么吩咐？\r\n给您Circular！")
		{
			for (int i = 0; i <= 100; i++)
			{
				pDlg->m_Progress.SetPos(i);                      // 设置进度条的位置
				CString str;
				str.Format(_T("%d%%"), i);
				pDlg->m_Percentage.SetWindowText(str);           // 显示进度条的百分比
				Sleep(60);
			}

			// 向机械手发送指令控制其抓取圆形
			char* data = new char[10];
			int len = pDlg->Str2Hex("55 55 05 06 02 01 00", data);
			pDlg->m_SerialPort.WriteToPort(data, len);
			delete[] data;

			Sleep(3000);

			// 读取图片
			ReadImage(&pDlg->ho_Image4, "C:/Users/86136/Desktop/4.bmp");
			DispObj(pDlg->ho_Image4, pDlg->hv_WindowHandle);
		}

		if (pDlg->m_strEditReceiveMsg == "您好！请问有什么吩咐？\r\n给您Triangle！")
		{
			// 向机械手发送指令控制其抓取圆形
			char* data = new char[10];
			int len = pDlg->Str2Hex("55 55 05 06 03 01 00", data);
			pDlg->m_SerialPort.WriteToPort(data, len);
			delete[] data;
		}

		if (pDlg->m_strEditReceiveMsg == "您好！请问有什么吩咐？\r\n给您Pentagon！")
		{
			for (int i = 0; i <= 100; i++)
			{
				pDlg->m_Progress.SetPos(i);                      // 设置进度条的位置
				CString str;
				str.Format(_T("%d%%"), i);
				pDlg->m_Percentage.SetWindowText(str);           // 显示进度条的百分比
				Sleep(50);
			}

			// 向机械手发送指令控制其抓取圆形
			char* data = new char[10];
			int len = pDlg->Str2Hex("55 55 05 06 04 01 00", data);
			pDlg->m_SerialPort.WriteToPort(data, len);
			delete[] data;

			Sleep(3000);

			// 读取图片
			ReadImage(&pDlg->ho_Image4, "C:/Users/86136/Desktop/5.bmp");
			DispObj(pDlg->ho_Image4, pDlg->hv_WindowHandle);
		}
	}

	return 0;
}