
// Image_CalibrationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Image_Calibration.h"
#include "Image_CalibrationDlg.h"
#include "afxdialogex.h"
#include "iostream"
#include "CvvImage.h"
#include "vector"
#include "Strsafe.h"
#include "stdio.h"
#include "stdlib.h"
#include "iostream"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CImageCalibrationDlg* img_calib;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CImageCalibrationDlg dialog



CImageCalibrationDlg::CImageCalibrationDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_IMAGE_CALIBRATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageCalibrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_info_, info_);
	DDX_Control(pDX, IDC_camsel_, camsel_);
	DDX_Control(pDX, IDC_focus, focus);
	DDX_Control(pDX, IDC_shot, shot);
	DDX_Control(pDX, IDC_calc, calc);
}

BEGIN_MESSAGE_MAP(CImageCalibrationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_focus, &CImageCalibrationDlg::OnNMCustomdrawSlider1)
	ON_CBN_SELCHANGE(IDC_camsel_, &CImageCalibrationDlg::OnCbnSelchangecamsel)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_shot, &CImageCalibrationDlg::OnBnClickedshot)
	ON_BN_CLICKED(IDC_calc, &CImageCalibrationDlg::OnBnClickedcalc)
END_MESSAGE_MAP()


// CImageCalibrationDlg message handlers

BOOL CImageCalibrationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	img_calib = this;

	focus.SetRange(1, 255);
	focus.SetTicFreq(1);
	focus.SetPos(5);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CImageCalibrationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CImageCalibrationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CImageCalibrationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CImageCalibrationDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	cur_sel = focus.GetPos();
	if (cam_sgn)
	{
		capture.set(CAP_PROP_FOCUS, cur_sel);
	}
	else if (shot_sgn)
	{
		OnBnClickedcalc();
	}
	vector<Vec4i> hierarchy;

	*pResult = 0;
}

//camera sel
void CImageCalibrationDlg::OnCbnSelchangecamsel()
{
	// TODO: Add your control notification handler code here
	int index_ = camsel_.GetCurSel();
	capture.open(index_);
	capture.set(CAP_PROP_FRAME_WIDTH, 4096);//640
	capture.set(CAP_PROP_FRAME_HEIGHT, 2160);//360
	SetTimer(0, 50, NULL);
	cam_sgn = TRUE;
	shot_sgn = FALSE;
}

void CImageCalibrationDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case 0:
	{
		capture >> cap_frame;
		if (cap_frame.data)
		{
			disp_image(IDC_ori_image, cap_frame, img_calib);
		}
		break;
	}
	default:
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CImageCalibrationDlg::disp_image(UINT disp_ID, Mat dsp_img, CWnd* pt)
{
	IplImage cpy;
	CvvImage cimg;
	CDC* _pDC;
	HDC hDC;
	CRect rect;
	_pDC = pt->GetDlgItem(disp_ID)->GetDC();		// 获得显示控件的 DC
	hDC = _pDC->GetSafeHdc();				// 获取 HDC(设备句柄) 来进行绘图操作
	pt->GetDlgItem(disp_ID)->GetClientRect(&rect);	// 获取控件尺寸位置
	cpy = dsp_img;
	cimg.CopyOf(&cpy);						// 复制图片
	cimg.DrawToHDC(hDC, &rect);				// 将图片绘制到显示控件的指定区域内
	pt->ReleaseDC(_pDC);
}

//shot
void CImageCalibrationDlg::OnBnClickedshot()
{
	// TODO: Add your control notification handler code here
	capture >> cap_frame;
	if (cap_frame.data)
	{
		imwrite("..//temp.bmp", cap_frame);
		trs_img = cap_frame.clone();
		cam_sgn = FALSE;
		shot_sgn = TRUE;
	}

	KillTimer(0);
	camsel_.SetCurSel(-1);

}

//calculate
void CImageCalibrationDlg::OnBnClickedcalc()
{
	// TODO: Add your control notification handler code here
	Mat src;
	Mat dst;
	Mat tmp_;
	src = trs_img.clone();
	cvtColor(src, dst, COLOR_BGR2GRAY);
	threshold(dst, tmp_, cur_sel, 255, cv::THRESH_BINARY_INV );//_INV | cv::THRESH_OTSU
	imwrite("..//threshold.bmp", tmp_);
	//find contours
	Point2f vec_p[4];
	RotatedRect rot_rect;
	vector<vector<Point>> contour;
	findContours(tmp_, contour, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (int h = 0; h < contour.size(); h++)
	{
		if (contourArea(contour[h]) > 5500)
		{
			rot_rect = minAreaRect(contour[h]);
			rot_rect.points(vec_p);
			//-- Draw lines between the corners (the mapped object in the scene)
			for (int ind_ = 0; ind_ < 4; ind_++)
			{
				line(src, vec_p[ind_], vec_p[(ind_ + 1) % 4], Scalar(0, 0, 255), 10, 8);
			}
			drawContours(src, contour, h, Scalar(0, 255, 0), 4, 8);
		}
	}
	disp_image(IDC_ori_image, src, img_calib);
}
